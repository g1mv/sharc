SHARC
======

Simple Highspeed Archiver

SHARC is a very fast file archiver with one goal in mind : speed.
On an Intel Core i7-2600K @ 3.40 GHz it reaches compression speeds of <b>700 MB/s</b> and decompression speeds of <b>1 GB/s</b>, and that is <b>PER core</b> !
Compression ratio is typically at around 50-60 % with the fastest algorithm.

SHARC is actually a single-thread <b>command line interface</b> to the **<a href=http://github.com/centaurean/density>DENSITY compression library</a>**.

Benchmark
---------

File used : enwik8 (100 MB)

Platform : MacBook Pro, OSX 10.10.1, 2.3 GHz Intel Core i7, 8Go 1600 MHz DDR, SSD

Timing : using the *time* function, and taking the best *user* output after multiple runs

<sub>Program</sub> | <sub>Library</sub> | <sub>Compress</sub> | <sub>Decompress</sub> | <sub>Size</sub> | <sub>Ratio</sub> | <sub>Round trip</sub>
--- | --- | --- | --- | --- | --- | ---
<sub>sharc -c1</sub> | <sub>density 0.10.2</sub> | <sub>0,117s (854,70 MB/s)</sub> | <sub>0,096s (1041,67 MB/s)</sub> | <sub>61 525 266</sub> | <sub>61,53%</sub> | <sub>0,213s</sub>
<sub>sharc -c2</sub> | <sub>density 0.10.2</sub> | <sub>0,217s (460,83 MB/s)</sub> | <sub>0,231s (432,90 MB/s)</sub> | <sub>53 157 538</sub> | <sub>53,16%</sub> | <sub>0,448s</sub>
<sub>lz4 -1</sub> | <sub>lz4 r126</sub> | <sub>0,479s (208,77 MB/s)</sub> | <sub>0,091s (1098,90 MB/s)</sub> | <sub>56 995 497</sub> | <sub>57,00%</sub> | <sub>0,570s</sub>
<sub>lz4 -9</sub> | <sub>lz4 r126</sub> | <sub>3,925s (25,48 MB/s)</sub> | <sub>0,087s (1149,43 MB/s)</sub> | <sub>44 250 986</sub> | <sub>44,25%</sub> | <sub>4,012s</sub>
<sub>lzop -1</sub> | <sub>lzo 2.08</sub> | <sub>0,367s (272,48 MB/s)</sub> | <sub>0,309s (323,62 MB/s)</sub> | <sub>56 709 096</sub> | <sub>56,71%</sub> | <sub>0,676s</sub>
<sub>lzop -9</sub> | <sub>lzo 2.08</sub> | <sub>14,298s (6,99 MB/s)</sub> | <sub>0,315s 317,46 MB/s)</sub> | <sub>41 217 688</sub> | <sub>41,22%</sub> | <sub>14,613s</sub>

For a more exhaustive benchmark, [click here](http://quixdb.github.io/squash/benchmarks/core-i3-2105.html) to see <b>SHARC</b>'s fastest mode compared to other algorithms, on an Intel® Core™ i3-2105 (x86 64), Asus P8H61-H motherboard with Fedora 19. It is possible to run yours using [this project](https://github.com/quixdb/squash).

Usage
-----

To compress a file named 'test' :
> sharc test

To decompress 'test.sharc'
> sharc -d test.sharc

It is of course possible to choose the compression algorithm. The fastest (default) can be specified like this :
> sharc -c1 test

There is also a slightly slower algorithm (20 % slower), with a better compression ratio (5 to 25 % better) :
> sharc -c2 test

For more help and the full list of options :
> sharc -h
