SHARC
======

Simple Highspeed Archiver<br/>www.centaurean.com/sharc

SHARC is a very fast file archiver with one goal in mind : speed.
On an Intel Core I7 it reaches compression speeds of <b>460 MB/s</b> and decompression speeds of <b>770 MB/s</b>, and that is <b>PER core</b> !
Compression ratio is typically at around 50-60 % with the fastest algorithm.

Usage
------

To compress a file named 'test' :
> sharc test

To decompress 'test.sharc'
> sharc -d test.sharc

Options
--------

It is possible to choose the compression algorithm. The fastest (default) can be specified like this :
> sharc -c0 test

There also is a slightly slower algorithm (20 % slower), with a better compression ratio (5 to 25 % better) :
> sharc -c1 test

SHARC is mainly licensed under the GPLv3. It is free for use, open source, and easy to intergrate in any other open source project.
For a more detailed options description, licensing information, etc. please refer to www.centaurean.com/sharc
