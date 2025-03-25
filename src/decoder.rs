use crate::algorithm::Algorithm;
use crate::commons::HEADER;
use crate::reader::Reader;
use crate::writer::Writer;
use density_rs::codec::codec::Codec;
use rayon::iter::{IntoParallelIterator, IntoParallelRefIterator, ParallelIterator};
use std::collections::BTreeMap;
use std::fs::File;
use std::io::Result;
use std::sync::mpsc::{channel, Sender};

fn process_block(index: usize, reader: Reader, offset: usize, sender: Sender<(usize, Reader, usize)>) {}

pub fn decode(file_name: &str) -> Result<()> {
    let mut reader = Reader::new(File::open(file_name)?)?;
    let mut writer = Writer::new(File::create_new(file_name.rsplit_once(".").unwrap().0)?);

    let header = reader.read_bytes(HEADER.len());
    assert_eq!(header, HEADER, "Incorrect header!");
    let algorithm = Algorithm::from_code(reader.read_bytes(1)[0]);
    let block_size = u16::from_le_bytes(<&[u8] as TryInto<[u8; size_of::<u16>()]>>::try_into(reader.read_bytes(size_of::<u16>())).unwrap()) as usize * 1024 * 1024;

    // Parallel block decoding
    let (sender, receiver) = channel();
    let mut offset = reader.get_index();
    let mut block = 0;
    while offset < reader.len() {
        let block_size = usize::try_from(u64::from_le_bytes(<&[u8] as TryInto<[u8; size_of::<u64>()]>>::try_into(reader.bytes_at(offset, size_of::<u64>())).unwrap())).unwrap();
        let hash = u64::from_le_bytes(<&[u8] as TryInto<[u8; size_of::<u64>()]>>::try_into(reader.bytes_at(offset + size_of::<u64>() + block_size, size_of::<u64>())).unwrap());
        sender.send((block, offset + size_of::<u64>(), block_size, hash)).unwrap();
        offset += size_of::<u64>() + block_size + size_of::<u64>();
    }

    receiver.into_par_iter().for_each(|(block, offset, block_size, hash)| {

    });


    (0..1 + reader.len() / block_size).into_par_iter().for_each_with(sender, |out, block| {
        let block_range = block * block_size..usize::min((block + 1) * block_size, reader.len());
        let mut buffer = vec![0; block_size];
        match algorithm.decode(&reader[block_range.start..block_range.end], &mut buffer) {
            Ok(written) => { out.send((block, buffer, written)).unwrap(); }
            Err(error) => { panic!("Failed to encode block: {:?}", error); }
        }
    });

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

    Ok(())
}