

fn main() {

    for i in 1..30 {
        std::thread::sleep(std::time::Duration::from_secs(1));
    }

    println!("helo, foo!");
}
