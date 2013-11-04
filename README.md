SHARC <a href="http://www.centaurean.com"><img src="http://www.centaurean.com/images/centaurean.png" width="140" align="right" title="Centaurean"/></a>
======

Simple Highspeed Archiver<br/>www.centaurean.com/sharc

SHARC is a very fast file archiver with one goal in mind : speed.
On an Intel Core i7-2600K @ 3.40 GHz it reaches compression speeds of <b>700 MB/s</b> and decompression speeds of <b>1 GB/s</b>, and that is <b>PER core</b> !
Compression ratio is typically at around 50-60 % with the fastest algorithm.

SHARC is actually a single-thread <b>command line interface</b> to the **<a href=http://github.com/centaurean/libssc>libssc library</a>**.

Benchmark
---------
Here is a benchmark of <b>libssc</b>'s fastest mode compared to other libraries, on an Intel® Core™ i3-2105	(x86 64), Asus P8H61-H motherboard with Fedora 19. It is possible to run yours using <a href=https://github.com/quixdb/squash>this project</a>.
<table><tr><td><img src=http://www.libssc.net/images/ratio.png /></td><td><img src=http://www.libssc.net/images/i3.png /></td></tr></table>

Usage
-----

To compress a file named 'test' :
> sharc test

To decompress 'test.sharc'
> sharc -d test.sharc

It is of course possible to choose the compression algorithm. The fastest (default) can be specified like this :
> sharc -c0 test

There is also a slightly slower algorithm (20 % slower), with a better compression ratio (5 to 25 % better) :
> sharc -c1 test

For more help and the full list of options :
> sharc -h

More information
-----------------
For more information please refer to <a href="www.centaurean.com/sharc">www.centaurean.com/sharc</a>
