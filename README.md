SHARC
=======

Simple Highspeed Archiver

SHARC is a very fast file archiver with one goal in mind : speed.
On an Intel Core I7 it reaches compression speeds of 460 MB/s and decompression speeds of 770 MB/s, and that is PER core !
Compression ratio is typically at around 50-60 % with the fastest algorithm.

Usage
======

To compress a file named 'test' :
> sharc test

To decompress 'test.sharc'
> sharc -d test.sharc

Options
========

It is possible to choose the compression algorithm. The fastest (default) can be specified like this :
> sharc -c0 test

There is also a slightly slower algorithm (20% slower), with a better compression ratio (5 to 25% better) :
> sgarc -c1 test
