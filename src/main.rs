

mod panda_db {
    
    #[derive(Debug)]
    pub struct DB {
        table: std::collections::HashMap<String, String>
    }

    impl DB {
        pub fn new() -> DB {
            return DB {
                table: std::collections::HashMap::new()
            }
        }

        pub fn set(&mut self, key : String, val : String) {
            self.table.insert(key, val);
        }

        pub fn get(&self, key : String) -> std::option::Option<&String> {
            return self.table.get(&key);
        }

    }
}

fn main() {
    println!("Hello, world!");

    let mut db = panda_db::DB::new();
    db.set(String::from("hello"), String::from("World"));
    println!("Created the db: {:?}", db);
    println!("Get query: {:?}", db.get(String::from("hello")));
}
