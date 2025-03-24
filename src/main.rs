use density_rs::algorithms::cheetah::cheetah::Cheetah;
use density_rs::codec::codec::Codec;
use memmap2::Mmap;
use std::env;
use std::fs::File;
use std::io::{Error, Write};

const EIGHT_MEGABYTES: usize = 8 * 1024 * 1024;

fn main() -> Result<(), Error> {
    let args: Vec<String> = env::args().collect();
    let file_name = args.get(1).unwrap();

    if let Ok(reader) = File::open(file_name) {
        let memory_map = unsafe { Mmap::map(&reader)? };
        dbg!(&memory_map.len());

        if let Ok(mut writer) = File::create_new(&format!("{}.sharc", file_name)) {
            let mut buffer = vec![0; Cheetah::new().safe_encode_buffer_size(EIGHT_MEGABYTES)];
            dbg!(&buffer.len());

            for i in (0..memory_map.len()).step_by(EIGHT_MEGABYTES) {
                let mut algorithm = Cheetah::new();
                let range = i..usize::min(i + EIGHT_MEGABYTES, memory_map.len());
                dbg!(&range);
                let hash = seahash::hash(&memory_map[range.start..range.end]);
                dbg!(&hash);
                // algorithm.clear_state();
                if let Ok(written) = algorithm.encode(&memory_map[range.start..range.end], &mut buffer) {
                    writer.write_all(&buffer[..written])?;
                    writer.write_all(&hash.to_le_bytes())?;
                }
            }
            writer.flush()?;
        }
    }

    Ok(())
}
