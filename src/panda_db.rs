    
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
