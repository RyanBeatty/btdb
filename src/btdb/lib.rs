#[macro_use]
extern crate quick_error;

// pub mod btdb;

use bincode;
use serde::{Deserialize, Serialize};

use std::fs::File;
use std::io::{Read, Seek, SeekFrom};

type PageId = u64;

pub mod storage {
    use std::collections::HashMap;

    const PAGE_SIZE: u16 = 4096;
    // Number of frames the buffer pool should contain.
    const BUFFER_POOL_SIZE: u16 = 3;

    struct BufferPool {
        buffer: Vec<u8>,
        page_table: HashMap<super::PageId, usize>
    }

    impl BufferPool {
        fn new() -> BufferPool {
            let capacity = (PAGE_SIZE * BUFFER_POOL_SIZE) as usize;
            let mut buffer = Vec::with_capacity(capacity);
            // Initialize memory. See https://users.rust-lang.org/t/how-to-allocate-huge-byte-array-safely/18284/36.
            for _ in 0..capacity {
                buffer.push(0);
            }

            let mut page_table = HashMap::<super::PageId, usize>::new();
            page_table.insert(0, 0);
            page_table.insert(1, From::from(PAGE_SIZE));
            // TODO: fix cast.
            page_table.insert(2, 2 * (PAGE_SIZE as usize));

            return BufferPool {
                buffer: buffer,
                page_table: page_table,
            }
        }

        // TODO: Multiple borowwing won't work in multi-threaded env. Figure out how to do this. Rc<>?
        fn get_page(&mut self, page_id: super::PageId) -> Option<&mut [u8]> {
            let offset = *self.page_table.get(&page_id)?;
            return Some(&mut self.buffer[offset..offset + (PAGE_SIZE as usize)])
        }
    }
}

// static PAGE_SIZE: u16 = 4096;

// #[derive(Debug, Clone, Serialize, Deserialize)]
// pub struct Tuple {
//     pub id: u64,
//     pub foo: String,
// }

// #[derive(Debug, Serialize, Deserialize)]
// struct BlockHeader {
//     //num_entries: u64,
//     free_lower: u16,
//     free_upper: u16,
//     //entries: Vec<(u16, u16)>,
// }

// impl BlockHeader {
//     fn new() -> BlockHeader {
//         return BlockHeader {
//             free_lower: PAGE_SIZE,
//             free_upper: std::mem::size_of::<BlockHeader>() as u16, // This downcast should always be safe.
//         };
//     }

//     // TODO: Implement free lower/upper collision.
//     fn insert_entry(&mut self, entry_size: u16) -> u16 {
//         let entry_loc = self.free_lower - entry_size;
//         self.free_lower = entry_loc;
//         return entry_loc;
//     }
// }

// #[derive(Debug)]
// pub struct DB {
//     storage: File,
//     header: BlockHeader,
// }

// impl DB {
//     pub fn open_db() -> error::Result<DB> {
//         let file = std::fs::OpenOptions::new()
//             .read(true)
//             .write(true)
//             .create(true)
//             .truncate(true)
//             .open("data/database.btdb")?;
//         // For now truncate the file then extend to page size.
//         // TODO: Deal with allocating more pages later.
//         file.set_len(PAGE_SIZE.into())?;
//         return Ok(DB {
//             storage: file,
//             header: BlockHeader::new(),
//         });
//     }
                        
//     pub fn insert(&mut self, tuple: Tuple) -> error::Result<()> {
//         let entry_size = bincode::serialized_size(&tuple)?;
//         if entry_size >= 4096 {
//             // TODO: do better here.
//             panic!("Tuple too large");
//         }
//         let entry_loc = self.header.insert_entry(entry_size as u16);
//         self.storage.seek(SeekFrom::Start(entry_loc.into()))?;
//         bincode::serialize_into(&mut self.storage, &tuple)?;
//         self.storage.seek(SeekFrom::Start(entry_loc.into()))?;
//         return Ok(());
//     }

//     pub fn select(&mut self, id: u64) -> error::Result<Vec<Tuple>> {
//         let original_loc = self.storage.seek(SeekFrom::Current(0))?;

//         let mut buffer = Vec::new();
//         self.storage.read_to_end(&mut buffer).or_else(|err| {
//             self.storage.seek(SeekFrom::Start(original_loc))?;
//             return Err(err);
//         })?;

//         let len = buffer.len();
//         let mut result = Vec::new();
//         let mut cursor = std::io::Cursor::new(buffer);
//         while (cursor.position() as usize) < len - 1 {
//             let tuple: Tuple = bincode::deserialize_from(&mut cursor).or_else(|err| {
//                 self.storage.seek(SeekFrom::Start(original_loc))?;
//                 return Err(err);
//             })?;
//             if tuple.id == id {
//                 result.push(tuple);
//             }
//         }

//         self.storage.seek(SeekFrom::Start(original_loc))?;
//         return Ok(result);
//     }

//     // TODO: Implement this.
//     pub fn delete(&mut self, id: u64) -> error::Result<u64> {
//         return Ok(0);
//     }
// }

pub mod error {
    pub type Result<T> = std::result::Result<T, Error>;

    quick_error! {
        #[derive(Debug)]
        pub enum Error {
            IO(err: std::io::Error) {}
            Serialize(err: bincode::Error) {}
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
}
