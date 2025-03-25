use sharc::commons::parse_args;
use sharc::decoder::decode;
use std::io::Result;

fn main() -> Result<()> {
    let file_name = parse_args();
    decode(&file_name)
}