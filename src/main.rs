use std::io::Write;

use btdb;

fn main() -> btdb::error::Result<()> {
    let mut db = btdb::DB::new()?;
    println!("BTDB Version 0.1.0");
    loop {
        print!("btdb> ");
        std::io::stdout().flush().unwrap();

        let mut buffer = String::new();
        std::io::stdin().read_line(&mut buffer).unwrap();
        let trimmed = buffer.trim();
        if trimmed.starts_with("\\") {
            match trimmed {
                "\\q" => break,
                _ => println!("Unknown meta command!"),
            }
        } else {
            let result = db.query(trimmed.to_string())?;
            println!("{}", result.to_string());
        }
    }

    db.close()?;
    return Ok(());
}
