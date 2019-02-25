use panda_db;

#[test]
fn get_and_set_works() {
    let mut db = panda_db::DB::new();
    let input = vec![
        (String::from("hello"), String::from("world")),
        (String::from("1234"), String::from("4321")),
        (String::from("the quick"), String::from("brown fox")),
    ];

    for (key, val) in input.iter() {
        db.set(key.to_string(), val.to_string());
    }

    for (key, val) in input.iter() {
        assert_eq!(Some(val), db.get(key.to_string()));
    }
}

#[test]
fn set_set_works() {
    let mut db = panda_db::DB::new();
    db.set(String::from("foo"), String::from("bar"));
    assert_eq!(Some(&String::from("bar")), db.get(String::from("foo")));
    db.set(String::from("foo"), String::from("baz"));
    assert_eq!(Some(&String::from("baz")), db.get(String::from("foo")));
}

#[test]
fn get_not_found() {
    let mut db = panda_db::DB::new();
    assert_eq!(None, db.get(String::from("foo")));
}
