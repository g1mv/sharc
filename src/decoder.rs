use crate::algorithm::Algorithm;
use crate::reader::Reader;
use crate::writer::Writer;
use crate::HEADER;
use rayon::iter::{IntoParallelRefIterator, ParallelIterator};
use std::collections::{BTreeMap, LinkedList};
use std::fs::File;
use std::io::Result;
use std::path::PathBuf;
use std::sync::mpsc::channel;

pub fn decode(file_path: PathBuf) -> Result<()> {
    let mut reader = Reader::new(File::open(&file_path)?)?;
    let mut writer = Writer::new(File::create_new(file_path.parent().unwrap().join(file_path.file_stem().unwrap()))?);

    let header = reader.read_bytes(HEADER.len());
    assert_eq!(header, HEADER, "Incorrect header!");
    let algorithm = Algorithm::from_code(reader.read_bytes(1)[0]);
    let decoded_block_size = u16::from_le_bytes(<&[u8] as TryInto<[u8; size_of::<u16>()]>>::try_into(reader.read_bytes(size_of::<u16>())).unwrap()) as usize * 1024 * 1024;

    // Blocks mapping
    let mut offset = reader.get_cursor();
    let mut block_number = 0;
    let mut blocks = LinkedList::new();
    while offset < reader.len() {
        let encoded_block_size = usize::try_from(u64::from_le_bytes(<&[u8] as TryInto<[u8; size_of::<u64>()]>>::try_into(reader.bytes_at(offset, size_of::<u64>())).unwrap())).unwrap();
        offset += size_of::<u64>();
        blocks.push_back((block_number, offset, encoded_block_size));
        offset += encoded_block_size + size_of::<u64>();
        block_number += 1;
    }

    // Parallel block decoding
    let (sender, receiver) = channel();
    blocks.par_iter().for_each_with(sender, |out, &(block_number, offset, encoded_block_size)| {
        let mut buffer = vec![0; decoded_block_size];
        match algorithm.decode(&reader[offset..offset + encoded_block_size], &mut buffer) {
            Ok(written) => {
                let hash = u64::from_le_bytes(<&[u8] as TryInto<[u8; size_of::<u64>()]>>::try_into(reader.bytes_at(offset + encoded_block_size, size_of::<u64>())).unwrap());
                assert_eq!(hash, seahash::hash(&buffer[..written]), "Failed hash verification!");
                out.send((block_number, buffer, written)).unwrap();
            }
            Err(error) => { panic!("Failed to decode block: {:?}", error); }
        }
    });

    // Sequence blocks
    let mut current_block = 0;
    let mut sequencer = BTreeMap::new();
    for (block, buffer, size) in receiver.iter() {
        if block == current_block {
            writer.write_bytes(&buffer[..size])?;
            current_block += 1;
        } else {
            sequencer.insert(block, (buffer, size));

            // Consume from sequencer to free up memory
            if let Some((buffer, size)) = sequencer.remove(&current_block) {
                writer.write_bytes(&buffer[..size])?;
                current_block += 1;
            }
        }
    }
    for (_block, (buffer, size)) in sequencer {
        writer.write_bytes(&buffer[..size])?;
    }
    writer.flush()?;

    Ok(())
}