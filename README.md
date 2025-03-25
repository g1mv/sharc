# sharc

> [!CAUTION]
> Alpha version, do not use

Simple High-speed ARChiver

**sharc** is a very fast file archiver, using the [**density**](http://github.com/g1mv/density) compression library.
It is ideal when speed is paramount, followed by compression ratio - although excellent ratios can be
achieved with certain settings and low entropy data.

**sharc** uses parallel processing to achieve maximum encode/decode performance. It is especially efficient with large files.

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

b) build and print commands list:

```shell
    RUSTFLAGS="-C target-cpu=native" cargo build --release
    target/release/sharc -h
```

### Usage

To compress a file named 'test' into 'test.sharc' using the default algorithm (**density** cheetah):
> sharc test

To decompress 'test.sharc',
> unsharc test.sharc

Compression algorithm can be selected with the -c option.
Fastest algorithm (**density** chameleon):
> sharc -c1 test

Well-balanced default (**density** cheetah)
> sharc -c2 test

Slower but still very fast, with better compression ratio (**density** lion):
> sharc -c3 test

For help and a full list of options:
> sharc -h
