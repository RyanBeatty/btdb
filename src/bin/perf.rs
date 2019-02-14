use rand::prelude::*;

use panda_db;

fn main() -> std::io::Result<()> {
    println!("Starting perf run!");
    const CHARSET: &[u8] =  b"ABCDEFGHIJKLMNOPQRSTUVWXYZ\
    abcdefghijklmnopqrstuvwxyz\
    0123456789)(*&^%$#@!~";

    let mut rng = rand::thread_rng();
    let mut words = Vec::new();
    for _i in 1..10000000 {
        let s: Option<String> = (0..30)
        .map(|_| Some(*CHARSET.choose(&mut rng)? as char))
        .collect();
        words.push(s.unwrap());
    }

    let start = std::time::SystemTime::now();
    let mut db = panda_db::DB::new();
    for entry in words {
        db.set(entry.to_string(), entry.to_string());
    }

    let elapsed = start.elapsed().unwrap();
    println!("Finished perf run: {}", elapsed.as_secs());
    return Ok(());
}
