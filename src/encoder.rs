use crate::algorithm::Algorithm;
use crate::reader::Reader;
use crate::writer::Writer;
use crate::HEADER;
use rayon::iter::IntoParallelIterator;
use rayon::iter::ParallelIterator;
use std::collections::BTreeMap;
use std::fs::File;
use std::io::Result;
use std::path::PathBuf;
use std::sync::mpsc::channel;

pub fn encode(mut file_path: PathBuf, block_size_mb: u16, algorithm: Algorithm) -> Result<()> {
    let reader = Reader::new(File::open(&file_path)?)?;
    file_path.set_extension(format!("{}{}", file_path.extension().unwrap().to_str().unwrap(), ".sharc")); // Unstable add_extension()
    let mut writer = Writer::new(File::create_new(&file_path)?);

    // Parallel block encoding
    let block_size = block_size_mb as usize * 1024 * 1024;
    let (sender, receiver) = channel();
    (0..1 + reader.len() / block_size).into_par_iter().for_each_with(sender, |out, block_number| {
        let block_range = block_number * block_size..usize::min((block_number + 1) * block_size, reader.len());
        let mut buffer = vec![0; algorithm.safe_encode_buffer_size(block_range.end - block_range.start)];
        let block_bytes = &reader[block_range.start..block_range.end];
        let hash = seahash::hash(block_bytes);
        match algorithm.encode(block_bytes, &mut buffer) {
            Ok(written) => { out.send((block_number, buffer, written, hash)).unwrap(); }
            Err(error) => { panic!("Failed to encode block: {:?}", error); }
        }
    });

    // Write header
    writer.write_bytes(&HEADER)?;
    writer.write_bytes(&vec![algorithm.get_code()])?;
    writer.write_bytes(&block_size_mb.to_le_bytes())?;

    // Write blocks (sequencer)
    let mut current_block_number = 0;
    let mut sequencer = BTreeMap::new();
    for (block_number, buffer, size, hash) in receiver.iter() {
        if block_number == current_block_number {
            writer.write_encoded_block(&buffer[..size], hash)?;
            current_block_number += 1;
        } else {
            sequencer.insert(block_number, (buffer, size, hash));

            // Consume from sequencer to free up memory
            if let Some((buffer, size, hash)) = sequencer.remove(&current_block_number) {
                writer.write_encoded_block(&buffer[..size], hash)?;
                current_block_number += 1;
            }
        }
    }
    for (_block_number, (buffer, size, hash)) in sequencer {
        writer.write_encoded_block(&buffer[..size], hash)?;
    }
    writer.flush()?;

    Ok(())
}
