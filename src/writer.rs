use std::fs::File;
use std::io::Result;
use std::io::Write;

pub struct Writer {
    file: File,
}

impl Writer {
    pub fn new(file: File) -> Self {
        Writer {
            file,
        }
    }

    pub fn write_bytes(&mut self, buffer: &[u8]) -> Result<()> {
        self.file.write_all(&buffer)
    }

    pub fn write_encoded_block(&mut self, buffer: &[u8], hash: u64) -> Result<()> {
        let size_u64 = u64::try_from(buffer.len()).unwrap();
        self.write_bytes(&size_u64.to_le_bytes())?;
        self.write_bytes(buffer)?;
        self.write_bytes(&hash.to_le_bytes())
    }

    pub fn flush(&mut self) -> Result<()> {
        self.file.flush()
    }
}