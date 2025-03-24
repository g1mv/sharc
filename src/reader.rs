use crate::reader::Reader::{Buffer, MemoryMap};
use memmap2::Mmap;
use std::fs::File;
use std::io::{Read, Result};
use std::ops::{Index, Range};

const MEMORY_MAP_THRESHOLD: usize = 16 * 1024;

pub enum Reader {
    MemoryMap(Mmap),
    Buffer(Vec<u8>),
}

impl Reader {
    pub fn new(mut file: File) -> Result<Self> {
        let metadata = file.metadata()?;
        let file_size = usize::try_from(metadata.len()).unwrap();
        if file_size < MEMORY_MAP_THRESHOLD {
            let mut buffer = Vec::with_capacity(file_size);
            file.read_to_end(&mut buffer)?;
            Ok(Buffer(buffer))
        } else {
            Ok(MemoryMap(unsafe { Mmap::map(&file)? }))
        }
    }

    pub fn len(&self) -> usize {
        match self {
            MemoryMap(memory_map) => { memory_map.len() }
            Buffer(buffer) => { buffer.len() }
        }
    }
}

impl Index<Range<usize>> for Reader {
    type Output = [u8];

    fn index(&self, range: Range<usize>) -> &Self::Output {
        match self {
            MemoryMap(memory_map) => { &memory_map[range] }
            Buffer(buffer) => { &buffer[range] }
        }
    }
}