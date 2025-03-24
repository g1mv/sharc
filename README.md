# sharc

> [!CAUTION]
> Alpha version, do not use

Simple High-speed ARChiver

**sharc** is a very fast file archiver, using the [**density** compression library](http://github.com/g1mv/density).
It is ideal when speed is the main requirement, followed by compression ratio - although excellent ratios can be
achieved with certain settings and low entropy data.

### Build

### Usage

To compress a file named 'test' using the default algorithm (**density** cheetah):
> sharc test

To decompress 'test.sharc',
> sharc -d test.sharc

or:
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
