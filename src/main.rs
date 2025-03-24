mod reader;
mod writer;

use crate::reader::Reader;
use crate::writer::Writer;
use density_rs::algorithms::lion::lion::Lion;
use density_rs::codec::codec::Codec;
use rayon::iter::IntoParallelIterator;
use rayon::iter::ParallelIterator;
use std::collections::BTreeMap;
use std::env;
use std::fs::File;
use std::io::Result;
use std::sync::mpsc::channel;

const HEADER: [u8; 5] = [b'S', b'H', b'A', b'R', b'C'];
const DEFAULT_BLOCK_SIZE_MB: u16 = 8;

fn main() -> Result<()> {
    let args: Vec<String> = env::args().collect();
    let file_name = args.get(1).unwrap();
    let reader = Reader::new(File::open(file_name)?)?;
    let mut writer = Writer::new(File::create_new(&format!("{}.sharc", file_name))?);

    // Parallel block encoding
    let block_size = DEFAULT_BLOCK_SIZE_MB as usize * 1024 * 1024;
    let (sender, receiver) = channel();
    (0..1 + reader.len() / block_size).into_par_iter().for_each_with(sender, |out, block| {
        let block_range = block * block_size..usize::min((block + 1) * block_size, reader.len());
        let mut buffer = vec![0; Lion::safe_encode_buffer_size(block_range.end - block_range.start)];
        match Lion::encode(&reader[block_range.start..block_range.end], &mut buffer) {
            Ok(written) => { out.send((block, buffer, written)).unwrap(); }
            Err(error) => { panic!("Failed to encode block: {:?}", error); }
        }
    });

    // Write header
    writer.write_bytes(&HEADER)?;
    writer.write_bytes(&vec![0])?;
    writer.write_bytes(&DEFAULT_BLOCK_SIZE_MB.to_le_bytes())?;

    // Write blocks
    let mut current_block = 0;
    let mut storage = BTreeMap::new();
    for (block, buffer, size) in receiver.iter() {
        if block == current_block {
            writer.write_encoded_block(&buffer[..size])?;
            current_block += 1;
        } else {
            storage.insert(block, (buffer, size));

            // Consume from storage to free up memory
            if let Some((buffer, size)) = storage.remove(&current_block) {
                writer.write_encoded_block(&buffer[..size])?;
                current_block += 1;
            }
        }
    }
    for (_block, (buffer, size)) in storage {
        writer.write_encoded_block(&buffer[..size])?;
    }
    writer.flush()?;

    Ok(())
}
