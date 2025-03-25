use crate::algorithm::Algorithm;
use crate::commons::HEADER;
use crate::reader::Reader;
use crate::writer::Writer;
use rayon::iter::IntoParallelIterator;
use rayon::iter::ParallelIterator;
use std::collections::BTreeMap;
use std::fs::File;
use std::io::Result;
use std::sync::mpsc::channel;

pub fn encode(file_name: &str, block_size_mb: u16, algorithm: Algorithm) -> Result<()> {
    let reader = Reader::new(File::open(file_name)?)?;
    let mut writer = Writer::new(File::create_new(&format!("{}.sharc", file_name))?);

    // Parallel block encoding
    let block_size = block_size_mb as usize * 1024 * 1024;
    let (sender, receiver) = channel();
    (0..1 + reader.len() / block_size).into_par_iter().for_each_with(sender, |out, block| {
        let block_range = block * block_size..usize::min((block + 1) * block_size, reader.len());
        let mut buffer = vec![0; algorithm.safe_encode_buffer_size(block_range.end - block_range.start)];
        match algorithm.encode(&reader[block_range.start..block_range.end], &mut buffer) {
            Ok(written) => { out.send((block, buffer, written)).unwrap(); }
            Err(error) => { panic!("Failed to encode block: {:?}", error); }
        }
    });

    // Write header
    writer.write_bytes(&HEADER)?;
    writer.write_bytes(&vec![algorithm.get_code()])?;
    writer.write_bytes(&block_size_mb.to_le_bytes())?;

    // Write blocks (sequencer)
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
