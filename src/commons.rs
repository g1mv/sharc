use std::env;

pub const HEADER: [u8; 5] = [b'S', b'H', b'A', b'R', b'C'];
pub const DEFAULT_BLOCK_SIZE_MB: u16 = 8;

pub fn parse_args() -> String {
    let args: Vec<String> = env::args().collect();
    let file_name = args.get(1).unwrap();
    file_name.to_owned()
}