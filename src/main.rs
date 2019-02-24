use std::error;
use std::io::Write;

mod panda_db;

enum Command {
    Unknown,
    Quit,
    Insert{ key: String, val: String},
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
                    command = Command::Insert{key: splt[1].to_string(), val: splt[2].to_string()};
                }
            }
        }

        match command {
            Command::Unknown => println!("Unknown Command: {}", line),
            Command::Quit    => break,
            Command::Insert{key, val}  => println!("Insert command key={} val={}", key, val),
        }
    }
}
