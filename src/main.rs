#[macro_use]
extern crate quick_error;

use std::error;
use std::io::Write;

mod panda_db;

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
                    Err(e) => Err(ParseCommandError::ParseInt),
                    Ok(id) => Ok(Command::Select { id: id }),
                };
            }
        }
    }

    Ok(Command::Unknown)
}

#[derive(Debug)]
struct Tuple {
    id: u64,
    foo: String,
}

fn main() {
    println!("BTDB Version 0.1.0");
    let mut db = Vec::<Tuple>::new();
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
                db.push(Tuple { id: id, foo: foo });
                println!("Inserted");
            }
            Ok(Command::Select { id }) => match db.iter().find(|&tuple| tuple.id == id) {
                None => (),
                Some(row) => (),
            },
        }
    }
}
