use clap::Parser;
use sharc::decoder::decode;
use std::io::Result;
use std::path::PathBuf;

#[derive(Parser, Debug)]
#[command(version, about, long_about = None)]
struct Args {
    /// File path
    #[arg()]
    file_path: PathBuf,
}

fn main() -> Result<()> {
    let args = Args::parse();
    decode(args.file_path)
}