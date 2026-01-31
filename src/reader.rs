use crate::reader::Reader::{Buffer, MemoryMap};
use memmap2::Mmap;
use std::fs::File;
use std::io::{Read, Result};
use std::ops::{Index, Range};

const MEMORY_MAP_THRESHOLD: usize = 16 * 1024;

pub enum Reader {
    MemoryMap(Mmap, usize),
    Buffer(Vec<u8>, usize),
}

impl Reader {
    pub fn new(mut file: File) -> Result<Self> {
        let metadata = file.metadata()?;
        let file_size = usize::try_from(metadata.len()).unwrap();
        if file_size < MEMORY_MAP_THRESHOLD {
            let mut buffer = Vec::with_capacity(file_size);
            file.read_to_end(&mut buffer)?;
            Ok(Buffer(buffer, 0))
        } else {
            Ok(MemoryMap(unsafe { Mmap::map(&file)? }, 0))
        }
    }

    pub fn len(&self) -> usize {
        match self {
            MemoryMap(memory_map, _) => { memory_map.len() }
            Buffer(buffer, _) => { buffer.len() }
        }
    }

    pub fn bytes_at(&self, offset: usize, size: usize) -> &[u8] {
        match self {
            MemoryMap(memory_map, _) => { &memory_map[offset..offset + size] }
            Buffer(buffer, _) => { &buffer[offset..offset + size] }
        }
    }

    pub fn read_bytes(&mut self, size: usize) -> &[u8] {
        match self {
            MemoryMap(memory_map, cursor) => {
                let out = &memory_map[*cursor..*cursor + size];
                *cursor += size;
                out
            }
            Buffer(buffer, cursor) => {
                let out = &buffer[*cursor..*cursor + size];
                *cursor += size;
                out
            }
        }
    }

    pub fn get_cursor(&self) -> usize {
        match self {
            MemoryMap(_, cursor) => { *cursor }
            Buffer(_, cursor) => { *cursor }
        }
    }
}

impl Index<Range<usize>> for Reader {
    type Output = [u8];

    fn index(&self, range: Range<usize>) -> &Self::Output {
        match self {
            MemoryMap(memory_map, _) => { &memory_map[range] }
            Buffer(buffer, _) => { &buffer[range] }
        }
    }
}