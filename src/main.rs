use std::io::Write;

mod panda_db;

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

        match line {
            "\\q" => break,
            _ => println!("Entered: {}", line),
        }

    }
}
