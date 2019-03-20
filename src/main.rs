#[macro_use]
extern crate prettytable;
#[macro_use]
extern crate quick_error;

use std::io::Write;

use sqlparser::dialect::PostgreSqlDialect;
use sqlparser::sqlparser::*;

use btdb;

// enum Command {
//     Unknown,
//     Quit,
//     Insert { id: u64, foo: String },
//     Select { id: u64 },
// }

// quick_error! {
//     #[derive(Debug)]
//     pub enum ParseCommandError {
//         // TODO: Take error as param.
//         ParseInt {}
//     }
// }

// // TODO: This function is a mess, but good enough for now. Should switch
// // over to a lib that can parse sql already for me.
// fn parse_command(line: &str) -> Result<Command, ParseCommandError> {
//     if line.starts_with("\\") {
//         return match line.as_ref() {
//             "\\q" => Ok(Command::Quit),
//             _ => Ok(Command::Unknown),
//         };
//     } else {
//         if line.starts_with("insert") {
//             let splt: Vec<&str> = line.split(" ").collect();
//             if splt.len() == 3 {
//                 return match splt[1].parse() {
//                     Err(e) => Err(ParseCommandError::ParseInt),
//                     Ok(id) => Ok(Command::Insert {
//                         id: id,
//                         foo: splt[2].to_string(),
//                     }),
//                 };
//             }
//         } else if line.starts_with("select") {
//             let splt: Vec<&str> = line.split(" ").collect();
//             if splt.len() == 2 {
//                 return match splt[1].parse() {
//                     Err(_) => Err(ParseCommandError::ParseInt),
//                     Ok(id) => Ok(Command::Select { id: id }),
//                 };
//             }
//         }
//     }

//     Ok(Command::Unknown)
// }

fn main() {
    println!("BTDB Version 0.1.0");
    loop {
        print!("btdb> ");
        std::io::stdout().flush().unwrap();

        let mut buffer = String::new();
        std::io::stdin().read_line(&mut buffer).unwrap();
        if buffer.trim() == "\\q" {
            break;
        }
        let ast = match Parser::parse_sql(&PostgreSqlDialect {}, buffer) {
            Err(err) => {
                println!("Failed to parse sql input: {:?}", err);
                continue;
            }
            Ok(ast) => ast,
        };
        println!("ast: {:?}", ast);
    }
}
