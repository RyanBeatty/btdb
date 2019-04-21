#[macro_use]
extern crate quick_error;

// pub mod btdb;

use bincode;
use serde::{Deserialize, Serialize};
use sqlparser::{dialect::PostgreSqlDialect, sqlast::ASTNode, sqlast::Value, sqlparser as sqlp};

use std::collections::{HashMap, VecDeque};
use std::fs::File;
use std::io::{Read, Seek, SeekFrom, Write};

type PageId = u64;

const PAGE_SIZE: u16 = 4096;
const NUM_DISK_PAGES: u16 = 5;

struct DiskManager {}

impl DiskManager {
    fn new() -> error::Result<DiskManager> {
        let mut file = std::fs::OpenOptions::new()
            .read(true)
            .write(true)
            .create(true)
            .truncate(true)
            .open("data/btdb/database.btdb")?;
        // Initialize the empty pages.
        // TODO: Deal with allocating more pages later.
        file.set_len(From::from(PAGE_SIZE * NUM_DISK_PAGES))?;
        for _ in 0..NUM_DISK_PAGES {
            let page = Page::new();
            file.write_all(page.to_bytes().as_slice())?;
        }
        return Ok(DiskManager {});
    }

    fn get_page(&self, page_id: PageId) -> error::Result<Vec<u8>> {
        let mut file = std::fs::OpenOptions::new()
            .read(true)
            .open("data/btdb/database.btdb")?;
        file.seek(SeekFrom::Start(page_id * (PAGE_SIZE as u64)))?;
        let mut buffer = vec![0u8; From::from(PAGE_SIZE)];
        file.read_exact(&mut buffer)?;
        return Ok(buffer);
    }

    fn put_page(&self, page_id: PageId, buffer: &[u8]) -> error::Result<()> {
        let mut file = std::fs::OpenOptions::new()
            .write(true)
            .open("data/btdb/database.btdb")?;
        file.seek(SeekFrom::Start(page_id * (PAGE_SIZE as u64)))?;
        file.write_all(buffer)?;
        return Ok(());
    }
}

const NUM_FRAMES: u16 = 3;

struct BufferPool {
    // buffer: Vec<u8>,
    frames: Vec<Page>,
    page_table: HashMap<PageId, usize>,
    disk_manager: DiskManager,
    lru_replacer: LRUReplacer<PageId>,
}

impl BufferPool {
    fn new(disk_manager: DiskManager) -> error::Result<BufferPool> {
        let mut frames = Vec::new();
        for _ in 0..NUM_FRAMES {
            frames.push(Page::new());
        }
        return Ok(BufferPool {
            frames: frames,
            page_table: HashMap::new(),
            disk_manager: disk_manager,
            lru_replacer: LRUReplacer::new(),
        });
    }

    // TODO: Multiple borowwing won't work in multi-threaded env. Figure out how to do this. Rc<>?
    fn get_page(&mut self, page_id: PageId) -> Option<&mut Page> {
        match self.page_table.get(&page_id) {
            None => {
                // TODO: panic if theres a real error?
                let buffer = self.disk_manager.get_page(page_id).ok()?;
                for (index, page) in self.frames.iter_mut().enumerate() {
                    if page.get_page_id().is_none() {
                        page.replace(page_id, &buffer).unwrap();
                        page.pin();
                        self.page_table.insert(page_id, index);
                        return Some(&mut self.frames[index]);
                    }
                }
                while let Some(victim_pid) = self.lru_replacer.victim() {
                    let index = *self.page_table.get(&victim_pid).unwrap();
                    let mut page = &mut self.frames[index];
                    if page.is_pinned() {
                        continue;
                    }
                    if page.is_dirty() {
                        // TODO: remove unwrap.
                        self.disk_manager
                            .put_page(page.get_page_id().unwrap(), page.to_bytes())
                            .unwrap();
                        page.clean();
                    }
                    // TODO: Remove unwrap.
                    page.replace(page_id, &buffer).unwrap();
                    page.pin();
                    self.page_table.remove(&victim_pid);
                    self.page_table.insert(page_id, index);
                    return Some(&mut self.frames[index]);
                }

                return None;
            }
            Some(index) => {
                let mut page = &mut self.frames[*index];
                page.pin();
                return Some(&mut self.frames[*index]);
            }
        }
    }

    fn unpin_page(&mut self, page_id: PageId, is_dirty: bool) {
        let offset = *self.page_table.get(&page_id).unwrap();
        let mut page = &mut self.frames[offset];
        page.unpin(is_dirty);
        if !page.is_pinned() {
            self.lru_replacer.insert(page_id);
        }
    }

    fn flush_pages(&mut self) -> error::Result<()> {
        for (page_id, index) in self.page_table.iter() {
            let mut page = self
                .frames
                .get_mut(*index)
                .ok_or(error::Error::BTDB(format!(
                    "Could not find frame for page_id={} index={}",
                    page_id, index
                )))?;
            self.disk_manager
                .put_page(page.get_page_id().unwrap(), page.to_bytes())?;
            page.clean();
            self.lru_replacer.erase(*page_id);
        }
        return Ok(());
    }
}

// TODO: Not the fastest implementation, but focus on functionality for now.
struct LRUReplacer<T: PartialEq> {
    queue: VecDeque<T>,
}

impl<T: PartialEq> LRUReplacer<T> {
    fn new() -> LRUReplacer<T> {
        return LRUReplacer {
            queue: VecDeque::new(),
        };
    }

    fn insert(&mut self, data: T) {
        if self.queue.contains(&data) {
            let mut index = 0;
            for (i, elem) in self.queue.iter().enumerate() {
                if *elem == data {
                    index = i;
                }
            }

            self.queue.remove(index);
            self.queue.push_back(data);
        } else {
            self.queue.push_back(data);
        }
    }

    fn victim(&mut self) -> Option<T> {
        return self.queue.pop_front();
    }

    // TODO: Some duplication here with insert, but fix later.
    fn erase(&mut self, data: T) -> bool {
        if self.queue.contains(&data) {
            let mut index = 0;
            for (i, elem) in self.queue.iter().enumerate() {
                if *elem == data {
                    index = i;
                }
            }

            self.queue.remove(index);
            return true;
        } else {
            return false;
        }
    }

    fn size(&self) -> usize {
        return self.queue.len();
    }
}

struct Page {
    page_id: Option<PageId>,
    dirty: bool,
    pinned: u64,
    buffer: Vec<u8>,
    header: PageHeader,
}

impl Page {
    fn new() -> Page {
        let capacity: usize = From::from(PAGE_SIZE);
        let mut buffer = Vec::with_capacity(capacity);
        // Initialize memory. See https://users.rust-lang.org/t/how-to-allocate-huge-byte-array-safely/18284/36.
        for _ in 0..capacity {
            buffer.push(0);
        }
        let header = PageHeader::new();
        for (i, byte) in header.to_bytes().iter().enumerate() {
            buffer[i] = *byte;
        }
        return Page {
            page_id: None,
            pinned: 0,
            dirty: false,
            buffer: buffer,
            header: header,
        };
    }

    fn get_page_id(&self) -> Option<PageId> {
        return self.page_id;
    }

    fn is_dirty(&self) -> bool {
        return self.dirty;
    }

    fn is_pinned(&self) -> bool {
        return self.pinned != 0;
    }

    fn pin(&mut self) {
        self.pinned += 1;
    }

    fn unpin(&mut self, dirty: bool) -> u64 {
        if self.pinned == 0 {
            panic!(format!(
                "Tried to unpin on Page page_id={:?} which is already not pinned by any thread",
                self.page_id
            ));
        }

        self.pinned -= 1;
        self.dirty |= dirty;
        return self.pinned;
    }

    fn clean(&mut self) {
        self.dirty = false;
    }

    fn replace(&mut self, page_id: PageId, buffer: &[u8]) -> error::Result<()> {
        if buffer.len() != From::from(PAGE_SIZE) {
            return Err(error::Error::BTDB(format!(
                "Invalid page size buffer: {}",
                buffer.len()
            )));
        }

        self.page_id = Some(page_id);
        for (i, byte) in buffer.iter().enumerate() {
            self.buffer[i] = *byte;
        }
        self.header = PageHeader::from_bytes(&self.buffer).ok_or(error::Error::BTDB(format!(
            "Failed to parse page header for page_id={}",
            page_id
        )))?;
        return Ok(());
    }

    fn to_bytes(&self) -> &Vec<u8> {
        return &self.buffer;
    }

    fn add_tuple(&mut self, tuple: &[u8]) -> Option<()> {
        let offset = self.header.add_entry(tuple.len() as u16)?;
        for (i, byte) in tuple.iter().enumerate() {
            self.buffer[(offset + (i as u16)) as usize] = *byte;
        }

        // Don't forget to write updated page header.
        for (i, byte) in self.header.to_bytes().iter().enumerate() {
            self.buffer[i] = *byte;
        }

        return Some(());
    }

    fn get_tuple(&self, tuple_id: u16) -> Option<&[u8]> {
        let (offset, size) = self.header.get_entry(tuple_id)?;
        return Some(&self.buffer[From::from(*offset)..From::from(offset + size)]);
    }
}

const ENTRY_METADATA_SIZE: usize = std::mem::size_of::<u16>() * 2;

struct PageHeader {
    free_start: u16,
    free_end: u16,
    entries: Vec<(u16, u16)>,
}

impl PageHeader {
    fn new() -> PageHeader {
        return PageHeader {
            free_start: PAGE_SIZE,
            free_end: 6,
            entries: Vec::new(),
        };
    }

    fn from_bytes(buffer: &[u8]) -> Option<PageHeader> {
        if buffer.len() != From::from(PAGE_SIZE) {
            return None;
        }

        // TODO: Probably a better way then direct indexing.
        let free_start = u16::from_le_bytes([buffer[0], buffer[1]]);
        let free_end = u16::from_le_bytes([buffer[2], buffer[3]]);

        let num_entries = u16::from_le_bytes([buffer[4], buffer[5]]);
        let mut entries = Vec::new();
        let entry_list_offset: usize = 6;
        let mut count: usize = 0;
        while count < From::from(num_entries) {
            let entry_offset = u16::from_le_bytes([
                buffer[entry_list_offset + count],
                buffer[entry_list_offset + count + 1],
            ]);
            let entry_size = u16::from_le_bytes([
                buffer[entry_list_offset + count + 2],
                buffer[entry_list_offset + count + 3],
            ]);
            entries.push((entry_offset, entry_size));
            count += 1;
        }
        return Some(PageHeader {
            free_start: free_start,
            free_end: free_end,
            entries: entries,
        });
    }

    fn to_bytes(&self) -> Vec<u8> {
        let mut buffer = Vec::new();
        buffer.extend_from_slice(&self.free_start.to_le_bytes());
        buffer.extend_from_slice(&self.free_end.to_le_bytes());
        let num_entries = self.entries.len() as u16;
        buffer.extend_from_slice(&num_entries.to_le_bytes());
        for (offset, size) in self.entries.iter() {
            buffer.extend_from_slice(&offset.to_le_bytes());
            buffer.extend_from_slice(&size.to_le_bytes());
        }
        return buffer;
    }

    fn add_entry(&mut self, entry_size: u16) -> Option<u16> {
        // prevents underflow.
        if entry_size >= self.free_start {
            return None;
        }
        // I don't think overflow is possible here.
        let new_free_end = self.free_end + (ENTRY_METADATA_SIZE as u16);
        let new_free_start = self.free_start - entry_size;
        if new_free_start <= new_free_end {
            return None;
        }

        self.free_end = new_free_end;
        self.free_start = new_free_start;
        self.entries.push((new_free_start, entry_size));
        return Some(new_free_start);
    }

    fn get_entry(&self, entry_id: u16) -> Option<&(u16, u16)> {
        return self.entries.get(entry_id as usize);
    }

    fn num_entries(&self) -> u16 {
        return self.entries.len() as u16;
    }
}

struct Cursor<'a> {
    buffer_pool: &'a mut BufferPool,
    cur_page_id: PageId,
    cur_tuple_id: u16,
}

impl<'a> Cursor<'a> {
    fn new(buffer_pool: &'a mut BufferPool) -> Cursor<'a> {
        return Cursor {
            buffer_pool: buffer_pool,
            cur_page_id: 0,
            cur_tuple_id: 0,
        };
    }

    fn add_tuple(&mut self, tuple: &Tuple) -> Option<()> {
        let tuple_bytes = tuple.to_bytes();
        for page_id in 0.. {
            let cur_page = self.buffer_pool.get_page(page_id)?;
            if cur_page.add_tuple(&tuple_bytes).is_some() {
                self.buffer_pool.unpin_page(page_id, true);
                return Some(());
            }
            self.buffer_pool.unpin_page(page_id, false);
        }
        return None;
    }
}

impl<'a> Iterator for Cursor<'a> {
    type Item = Vec<u8>;

    fn next(&mut self) -> Option<Self::Item> {
        let mut cur_page = self.buffer_pool.get_page(self.cur_page_id)?;
        match cur_page.get_tuple(self.cur_tuple_id) {
            None => {
                self.buffer_pool.unpin_page(self.cur_page_id, false);
                self.cur_page_id += 1;
                self.cur_tuple_id = 0;
                return self.next();
            }
            Some(next_tuple) => {
                self.cur_tuple_id += 1;
                return Some(next_tuple.to_vec());
            }
        }
    }
}

struct QueryExecutor<'a> {
    buffer_pool: &'a mut BufferPool,
}

impl<'a> QueryExecutor<'a> {
    fn new(buffer_pool: &'a mut BufferPool) -> QueryExecutor<'a> {
        return QueryExecutor {
            buffer_pool: buffer_pool,
        };
    }

    fn insert(&mut self, tuple: &Tuple) -> error::Result<()> {
        let mut cursor = Cursor::new(&mut self.buffer_pool);
        cursor
            .add_tuple(tuple)
            .ok_or(error::Error::BTDB(String::from("Failed to insert tuple")))?;
        return Ok(());
    }

    fn select(&mut self) -> error::Result<Option<Vec<Tuple>>> {
        let mut results = Vec::new();
        let mut cursor = Cursor::new(&mut self.buffer_pool);
        for raw_tuple in cursor {
            let tuple = Tuple::from_bytes(&mut raw_tuple.as_slice())?;
            results.push(tuple);
        }

        return Ok(Some(results));
    }
}

pub struct DB {
    buffer_pool: BufferPool,
}

impl DB {
    pub fn new() -> error::Result<DB> {
        let mut disk_manager = DiskManager::new()?;
        let mut buffer_pool = BufferPool::new(disk_manager)?;
        return Ok(DB {
            buffer_pool: buffer_pool,
        });
    }

    pub fn close(&mut self) -> error::Result<()> {
        return self.buffer_pool.flush_pages();
    }

    pub fn query(&mut self, query: String) -> error::Result<QueryResult> {
        let ast = sqlp::Parser::parse_sql(&PostgreSqlDialect {}, query)?;
        match ast {
            ASTNode::SQLInsert {
                table_name: _,
                columns,
                values,
            } => {
                let id_pos = columns
                    .iter()
                    .position(|c| c == "id")
                    .ok_or(error::Error::BTDB(String::from("id column not found")))?;
                let foo_pos = columns
                    .iter()
                    .position(|c| c == "foo")
                    .ok_or(error::Error::BTDB(String::from("foo column not found")))?;
                let bar_pos = columns
                    .iter()
                    .position(|c| c == "bar")
                    .ok_or(error::Error::BTDB(String::from("bar column not found")))?;
                let mut executor = QueryExecutor::new(&mut self.buffer_pool);
                let mut count: u64 = 0;
                for val in values.iter() {
                    let id = val
                        .get(id_pos)
                        .ok_or(error::Error::BTDB(String::from(
                            "id value not found in row to insert",
                        )))
                        .and_then(|x| match x {
                            ASTNode::SQLValue(Value::Long(n)) => Ok(n),
                            _ => Err(error::Error::BTDB(String::from("Invalid sql ast"))),
                        })?;
                    let foo = val
                        .get(foo_pos)
                        .ok_or(error::Error::BTDB(String::from(
                            "foo value not found in row to insert",
                        )))
                        .and_then(|x| match x {
                            ASTNode::SQLValue(Value::SingleQuotedString(n)) => Ok(n),
                            _ => Err(error::Error::BTDB(String::from("Invalid sql ast"))),
                        })?;
                    let bar = val
                        .get(bar_pos)
                        .ok_or(error::Error::BTDB(String::from(
                            "bar value not found in row to insert",
                        )))
                        .and_then(|x| match x {
                            ASTNode::SQLValue(Value::Long(n)) => Ok(n),
                            _ => Err(error::Error::BTDB(String::from("Invalid sql ast"))),
                        })?;
                    let tuple = Tuple::new(*id as u64, foo.to_string(), *bar as i32);
                    executor.insert(&tuple)?;
                    count += 1;
                }

                return Ok(QueryResult::InsertResult {
                    num_inserted: count,
                });
            }
            ASTNode::SQLSelect {
                projection: _,
                relation: _,
                joins: _,
                selection: _,
                order_by: _,
                group_by: _,
                having: _,
                limit: _,
            } => {
                let mut executor = QueryExecutor::new(&mut self.buffer_pool);
                let rows = executor.select()?;
                return Ok(QueryResult::SelectResult {
                    rows: rows.unwrap(),
                });
            }
            _ => {
                return Err(error::Error::BTDB(format!(
                    "Unimplimented query method: {:?}",
                    ast
                )));
            }
        };
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Tuple {
    pub id: u64,
    pub foo: String,
    pub bar: i32,
}

impl Tuple {
    fn new(id: u64, foo: String, bar: i32) -> Tuple {
        return Tuple {
            id: id,
            foo: foo,
            bar: bar,
        };
    }

    fn to_bytes(&self) -> Vec<u8> {
        let mut buffer = Vec::new();
        buffer.extend_from_slice(&self.id.to_le_bytes());
        buffer.extend_from_slice(&self.foo.len().to_le_bytes());
        buffer.extend_from_slice(self.foo.as_bytes());
        buffer.extend_from_slice(&self.bar.to_le_bytes());
        return buffer;
    }

    fn from_bytes<R: Read>(buffer: &mut R) -> error::Result<Tuple> {
        let mut id_buffer = [0; std::mem::size_of::<u64>()];
        buffer.read_exact(&mut id_buffer)?;
        let id = u64::from_le_bytes(id_buffer);
        let mut foo_len_buffer = [0; std::mem::size_of::<usize>()];
        buffer.read_exact(&mut foo_len_buffer)?;
        let mut foo_buffer = vec![0; usize::from_le_bytes(foo_len_buffer)];
        buffer.read_exact(&mut foo_buffer);
        let foo = String::from_utf8(foo_buffer)?;
        let mut bar_buffer = [0; std::mem::size_of::<i32>()];
        buffer.read_exact(&mut bar_buffer)?;
        let bar = i32::from_le_bytes(bar_buffer);
        return Ok(Tuple::new(id, foo, bar));
    }
}

pub enum QueryResult {
    InsertResult { num_inserted: u64 },
    SelectResult { rows: Vec<Tuple> },
}

impl ToString for QueryResult {
    fn to_string(&self) -> String {
        return match self {
            QueryResult::InsertResult { num_inserted } => format!("INSERT 0 {}", num_inserted),
            QueryResult::SelectResult { rows } => format!("{:?}", rows),
        };
    }
}

pub mod error {
    use sqlparser::sqlparser::ParserError;

    pub type Result<T> = std::result::Result<T, Error>;

    quick_error! {
        #[derive(Debug)]
        pub enum Error {
            IO(err: std::io::Error) {}
            Serialize(err: bincode::Error) {}
            FromUtf8Error(err: std::string::FromUtf8Error) {}
            SQLParser(err: ParserError) {}
            BTDB(msg: String) {}
        }
    }

    impl From<std::io::Error> for Error {
        fn from(error: std::io::Error) -> Self {
            return Error::IO(error);
        }
    }

    impl From<bincode::Error> for Error {
        fn from(error: bincode::Error) -> Self {
            return Error::Serialize(error);
        }
    }

    impl From<std::string::FromUtf8Error> for Error {
        fn from(error: std::string::FromUtf8Error) -> Self {
            return Error::FromUtf8Error(error);
        }
    }

    impl From<ParserError> for Error {
        fn from(error: ParserError) -> Self {
            return Error::SQLParser(error);
        }
    }
}
