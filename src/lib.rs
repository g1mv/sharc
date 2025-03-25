pub mod algorithm;
pub mod decoder;
pub mod encoder;
pub mod reader;
pub mod writer;

pub const HEADER: [u8; 5] = [b'S', b'H', b'A', b'R', b'C'];
pub const DEFAULT_BLOCK_SIZE_MB: u16 = 8;