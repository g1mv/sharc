use sharc::DEFAULT_BLOCK_SIZE_MB;
use clap::Parser;
use sharc::algorithm::Algorithm;
use sharc::encoder::encode;
use std::io::Result;
use std::path::PathBuf;

#[derive(Parser, Debug)]
#[command(version, about, long_about = None)]
struct Args {
    /// Algorithm
    #[arg(short, long, default_value_t = String::from("cheetah"), value_parser = clap::builder::PossibleValuesParser::new(["chameleon", "cheetah", "lion"]))]
    algorithm: String,

    /// Block size in MB
    #[arg(short, long, default_value_t = DEFAULT_BLOCK_SIZE_MB)]
    block_size_mb: u16,

    /// File path
    #[arg()]
    file_path: PathBuf,
}

fn main() -> Result<()> {
    let args = Args::parse();
    let algorithm = match args.algorithm.as_str() {
        "chameleon" => { Algorithm::Chameleon }
        "cheetah" => { Algorithm::Cheetah }
        "lion" => { Algorithm::Lion }
        &_ => unreachable!()
    };
    encode(args.file_path, args.block_size_mb, algorithm)
}