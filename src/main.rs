use std::error;
use std::io::Write;

mod panda_db;

enum Command {
    Unknown,
    Quit,
    Insert{id: u64, foo: String},
    Select{id: u64},
}

fn main() {
    println!("BTDB Version 0.1.0");
    let mut buffer = String::new();
    loop {
        print!("btdb> ");
        std::io::stdout().flush().unwrap();

        buffer.clear();
        match std::io::stdin().read_line(&mut buffer) {
            Err(error) => {
                println!("Error reading input: {}", error);
                continue;
            },
            _ => (),
        }

        let line = buffer.trim_end();

        let mut command = Command::Unknown;
        if line.starts_with("\\") {
            match line.as_ref() {
                "\\q" => command = Command::Quit,
                _     => (),
            }
        } else {
            if line.starts_with("insert") {
                let splt : Vec<&str> = line.split(" ").collect();
                if splt.len() == 3 {
                    match splt[1].parse() {
                        Err(e) => println!("Parse int error: {}", e),
                        Ok(id) => command = Command::Insert{id: id, foo: splt[2].to_string()},
                    }
                }
            } else if line.starts_with("select") {
                let splt : Vec<&str> = line.split(" ").collect();
                if splt.len() == 2 {
                    match splt[1].parse() {
                        Err(e) => println!("Parse int error: {}", e),
                        Ok(id) => command = Command::Select{id: id},
                    }
                }
            }
        }

        match command {
            Command::Unknown => println!("Unknown Command: {}", line),
            Command::Quit    => break,
            Command::Insert{id, foo}  => println!("Insert command id={} foo={}", id, foo),
            Command::Select{id} => println!("Select command id={}", id),
        }
    }
}
