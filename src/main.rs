#[macro_use]
extern crate prettytable;
#[macro_use]
extern crate quick_error;

use std::io::Write;

enum Command {
    Unknown,
    Quit,
    Insert { id: u64, foo: String },
    Select { id: u64 },
}

quick_error! {
    #[derive(Debug)]
    pub enum ParseCommandError {
        // TODO: Take error as param.
        ParseInt {}
    }
}

fn parse_command(line: &str) -> Result<Command, ParseCommandError> {
    if line.starts_with("\\") {
        return match line.as_ref() {
            "\\q" => Ok(Command::Quit),
            _ => Ok(Command::Unknown),
        };
    } else {
        if line.starts_with("insert") {
            let splt: Vec<&str> = line.split(" ").collect();
            if splt.len() == 3 {
                return match splt[1].parse() {
                    Err(e) => Err(ParseCommandError::ParseInt),
                    Ok(id) => Ok(Command::Insert {
                        id: id,
                        foo: splt[2].to_string(),
                    }),
                };
            }
        } else if line.starts_with("select") {
            let splt: Vec<&str> = line.split(" ").collect();
            if splt.len() == 2 {
                return match splt[1].parse() {
                    Err(_) => Err(ParseCommandError::ParseInt),
                    Ok(id) => Ok(Command::Select { id: id }),
                };
            }
        }
    }

    Ok(Command::Unknown)
}

mod btdb {

    use bincode;
    use serde::{Deserialize, Serialize};

    use std::fs::File;

    #[derive(Debug, Clone, Serialize, Deserialize)]
    pub struct Tuple {
        pub id: u64,
        pub foo: String,
    }

    #[derive(Debug)]
    pub struct DB {
        storage: File,
    }

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

    impl DB {
        pub fn new() -> error::Result<DB> {
            let file = File::create("data/database.btdb")?;
            return Ok(DB { storage: file });
        }

        pub fn insert(&mut self, tpl: Tuple) -> error::Result<()> {
            bincode::serialize_into(&mut self.storage, &tpl)?;
            return Ok(());
        }

        pub fn select(&self, id: u64) -> error::Result<Vec<Tuple>> {
            panic!("select unimplemented for now");
            return Ok(Vec::new());
            //return self.storage.iter().filter(|tuple| tuple.id == id).cloned().collect();
        }
    }
}

fn main() {
    println!("BTDB Version 0.1.0");
    let mut db = btdb::DB::new().unwrap();
    let mut buffer = String::new();
    loop {
        print!("btdb> ");
        std::io::stdout().flush().unwrap();

        buffer.clear();
        match std::io::stdin().read_line(&mut buffer) {
            Err(error) => {
                println!("Error reading input: {}", error);
                continue;
            }
            _ => (),
        }

        let line = buffer.trim_end();
        match parse_command(line) {
            Err(err) => println!("Error: {}", err),
            Ok(Command::Unknown) => println!("Uknown command"),
            Ok(Command::Quit) => break,
            Ok(Command::Insert { id, foo }) => {
                match db.insert(btdb::Tuple { id: id, foo: foo }) {
                    Err(e) => println!("Error: {}", e),
                    Ok(_)  => println!("Inserted"),
                }
            }
            Ok(Command::Select { id }) => {
                let results: Vec<prettytable::Row> = db
                    .select(id)
                    .unwrap()
                    .iter()
                    .map(|tuple| row![tuple.id, tuple.foo])
                    .collect();
                let mut table = prettytable::Table::init(results);
                table.set_titles(row!["id", "foo"]);
                print!("{}", table);
                println!("({} rows)\n", table.len());
            }
        }
    }
}
