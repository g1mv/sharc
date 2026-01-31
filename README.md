# sharc

Simple High-speed ARChiver

**sharc** is a very fast file archiver, using the [**density**](http://github.com/g1mv/density) compression library.
It is ideal when speed is paramount, followed by compression ratio - although excellent ratios can be
achieved with certain settings on low entropy data.

**sharc** uses parallel processing to achieve maximum encode/decode performance, and is thus particularly
efficient with large files. Data integrity is verified using the fast, non-cryptographic [**seahash**](https://gitlab.redox-os.org/redox-os/seahash) hashing algorithm.

[![MIT licensed](https://img.shields.io/badge/License-MIT-blue.svg)](./LICENSE-MIT)
[![Apache-2.0 licensed](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](./LICENSE-APACHE)
[![Crates.io](https://img.shields.io/crates/v/sharc.svg)](https://crates.io/crates/density-rs)
[![Build Status](https://github.com/g1mv/sharc/actions/workflows/ci.yml/badge.svg)](https://github.com/g1mv/density/actions)

### Build

**sharc** can be built on rust-compatible platforms. First use [rustup](https://rustup.rs) to install
rust.

a) get the source code:

```shell
    git clone https://github.com/g1mv/sharc.git
    cd sharc
```

b) build and print help:

```shell
    RUSTFLAGS="-C target-cpu=native" cargo build --release
    target/release/sharc -h
```

### Output file format

The **.sharc** file format consists of a header, followed by a sequence of blocks.
The header has the following structure:

| Header  | Algorithm used | Block size MB | 
|---------|----------------|---------------|
| 5 bytes | 1 byte         | 2 bytes       |

The sequence of blocks has the following structure:

| Encoded block size (ebs) | Encoded block | Block hash |
|--------------------------|---------------|------------|
| 8 bytes                  | {ebs} bytes   | 8 bytes    |

### General usage

To compress a file named 'test.file' into 'test.file.sharc' using the default algorithm:
> sharc test.file

To decompress 'test.file.sharc',
> unsharc test.file.sharc

### Algorithm selection

Compression algorithm can be selected with the **-a** option.

| Algorithm | Speed rank | Ratio rank |
|-----------|------------|------------|
| chameleon | 1st        | 3rd        | 
| cheetah   | 2nd        | 2nd        |
| lion      | 3rd        | 1st        |

For example, to use the fastest available algorithm (**density** chameleon):
> sharc -a chameleon test.file

### Other options

For further information on all other options:
> sharc -h
