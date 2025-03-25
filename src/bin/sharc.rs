use sharc::algorithm::Algorithm::Cheetah;
use sharc::commons::{parse_args, DEFAULT_BLOCK_SIZE_MB};
use sharc::encoder::encode;
use std::io::Result;

fn main() -> Result<()> {
    let file_name = parse_args();
    encode(&file_name, DEFAULT_BLOCK_SIZE_MB, Cheetah)
}