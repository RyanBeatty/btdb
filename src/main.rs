

mod panda_db;

fn main() {
    println!("Hello, world!");

    let mut db = panda_db::DB::new();
    db.set(String::from("hello"), String::from("World"));
    println!("Created the db: {:?}", db);
    println!("Get query: {:?}", db.get(String::from("hello")));
}
