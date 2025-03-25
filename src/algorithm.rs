use density_rs::algorithms::chameleon::chameleon::Chameleon;
use density_rs::algorithms::cheetah::cheetah::Cheetah;
use density_rs::algorithms::lion::lion::Lion;
use density_rs::codec::codec::Codec;
use density_rs::errors::decode_error::DecodeError;
use density_rs::errors::encode_error::EncodeError;

pub enum Algorithm {
    Chameleon,
    Cheetah,
    Lion,
}

impl Algorithm {
    pub fn from_code(code: u8) -> Self {
        match code {
            0 => Algorithm::Chameleon,
            1 => Algorithm::Cheetah,
            2 => Algorithm::Lion,
            _ => panic!("Algorithm error: code {} unknown", code)
        }
    }

    pub fn get_code(&self) -> u8 {
        match self {
            Algorithm::Chameleon => 0,
            Algorithm::Cheetah => 1,
            Algorithm::Lion => 2,
        }
    }

    pub fn safe_encode_buffer_size(&self, size: usize) -> usize {
        match self {
            Algorithm::Chameleon => Chameleon::safe_encode_buffer_size(size),
            Algorithm::Cheetah => Cheetah::safe_encode_buffer_size(size),
            Algorithm::Lion => Lion::safe_encode_buffer_size(size),
        }
    }

    pub fn encode(&self, input: &[u8], output: &mut [u8]) -> Result<usize, EncodeError> {
        match self {
            Algorithm::Chameleon => Chameleon::encode(input, output),
            Algorithm::Cheetah => Cheetah::encode(input, output),
            Algorithm::Lion => Lion::encode(input, output),
        }
    }

    pub fn decode(&self, input: &[u8], output: &mut [u8]) -> Result<usize, DecodeError> {
        match self {
            Algorithm::Chameleon => Chameleon::decode(input, output),
            Algorithm::Cheetah => Cheetah::decode(input, output),
            Algorithm::Lion => Lion::decode(input, output),
        }
    }
}