SHARC
======

Simple Highspeed Archiver

SHARC is a very fast file archiver with one goal in mind : speed, while achieving the best ratio possible.
On an Intel Core i7-2600K @ 3.40 GHz it reaches compression speeds of **1.3 GB/s** and decompression speeds of **1.7 GB/s**, and that is **PER core** !
Compression ratio is typically at around 50-60 % with the fastest algorithm and can reach 10% or less with the stronger algorithms.

SHARC is actually a single-thread **command line interface** to the [DENSITY compression library](http://github.com/centaurean/density).

Benchmark
---------

**Quick bench**

File used : enwik8 (100 MB)

Platform : MacBook Pro, OSX 10.10.2, 2.3 GHz Intel Core i7, 8Go 1600 MHz DDR, SSD

Timing : using the *time* function, and taking the best *user* output after multiple runs

Note : *sharc -c1* uses density's chameleon algorithm, *sharc -c2* the cheetah algorithm, and *sharc -c3* the lion algorithm.

<sub>Program</sub> | <sub>Library</sub> | <sub>Compress</sub> | <sub>Decompress</sub> | <sub>Size</sub> | <sub>Ratio</sub> | <sub>Round trip</sub>
--- | --- | --- | --- | --- | --- | ---
<sub>**[sharc](https://github.com/centaurean/sharc)** -c1</sub> | <sub>**density** 0.12.0</sub> | <sub>0.111s (900 MB/s)</sub> | <sub>0.085s (1175 MB/s)</sub> | <sub>61 524 502</sub> | <sub>61,52%</sub> | <sub>0.196s</sub>
<sub>[lz4](https://github.com/Cyan4973/lz4) -1</sub> | <sub>lz4 r126</sub> | <sub>0.461s (217 MB/s)</sub> | <sub>0.091s (1099 MB/s)</sub> | <sub>56 995 497</sub> | <sub>57,00%</sub> | <sub>0.552s</sub>
<sub>[lzop](http://www.lzop.org) -1</sub> | <sub>lzo 2.08</sub> | <sub>0.367s (272 MB/s)</sub> | <sub>0.309s (324 MB/s)</sub> | <sub>56 709 096</sub> | <sub>56,71%</sub> | <sub>0.676s</sub>
<sub>**[sharc](https://github.com/centaurean/sharc)** -c2</sub> | <sub>**density** 0.12.0</sub> | <sub>0.212s (472 MB/s)</sub> | <sub>0.217s (460 MB/s)</sub> | <sub>53 156 782</sub> | <sub>53,16%</sub> | <sub>0.429s</sub>
<sub>**[sharc](https://github.com/centaurean/sharc)** -c3</sub> | <sub>**density** 0.12.0</sub> | <sub>0.361s (277 MB/s)</sub> | <sub>0.396s (253 MB/s)</sub> | <sub>47 991 605</sub> | <sub>47,99%</sub> | <sub>0.757s</sub>
<sub>[lz4](https://github.com/Cyan4973/lz4) -3</sub> | <sub>lz4 r126</sub> | <sub>1.520s (66 MB/s)</sub> | <sub>0.087s (1149 MB/s)</sub> | <sub>47 082 421</sub> | <sub>47,08%</sub> | <sub>1.607s</sub>
<sub>[lzop](http://www.lzop.org) -7</sub> | <sub>lzo 2.08</sub> | <sub>9.562s (10 MB/s)</sub> | <sub>0.319s (313 MB/s)</sub> | <sub>41 720 721</sub> | <sub>41,72%</sub> | <sub>9.881s</sub>

**Squash** (look for density)

Squash is an abstraction layer for compression algorithms, and has an extremely exhaustive set of benchmark results, including density's, [available here](https://quixdb.github.io/squash-benchmark/).
You can choose between system architecture and compressed file type. There are even ARM boards tested ! A great tool for selecting a compression library.

**FsBench** (look for density)

FsBench is a command line utility that enables real-time testing of compression algorithms, but also hashes and much more. A fork with the latest density releases is [available here](https://github.com/centaurean/fsbench-density) for easy access.
The original author's repository [can be found here](https://chiselapp.com/user/Justin_be_my_guide/repository/fsbench/). Very informative tool as well.

Here are the results of a couple of test runs on a MacBook Pro, OSX 10.10.2, 2.3 GHz Intel Core i7, 8Go 1600 MHz DDR, SSD :

*enwik8 (100,000,000 bytes)*

    Codec                                   version      args
    C.Size      (C.Ratio)        E.Speed   D.Speed      E.Eff. D.Eff.
    density::chameleon                      2015-03-22   
       61524474 (x 1.625)      903 MB/s 1248 MB/s       347e6  480e6
    density::cheetah                        2015-03-22   
       53156746 (x 1.881)      468 MB/s  482 MB/s       219e6  225e6
    density::lion                           2015-03-22   
       47991569 (x 2.084)      285 MB/s  271 MB/s       148e6  140e6
    LZ4                                     r127         
       56973103 (x 1.755)      258 MB/s 1613 MB/s       111e6  694e6
    LZF                                     3.6          very
       53945381 (x 1.854)      192 MB/s  370 MB/s        88e6  170e6
    LZO                                     2.08         1x1
       55792795 (x 1.792)      287 MB/s  371 MB/s       126e6  164e6
    QuickLZ                                 1.5.1b6      1
       52334371 (x 1.911)      281 MB/s  351 MB/s       134e6  167e6
    Snappy                                  1.1.0        
       56539845 (x 1.769)      244 MB/s  788 MB/s       106e6  342e6
    wfLZ                                    r10          
       63521804 (x 1.574)      150 MB/s  513 MB/s        54e6  187e6
       
*silesia (211,960,320 bytes)*

    Codec                                   version      args
    C.Size      (C.Ratio)        E.Speed   D.Speed      E.Eff. D.Eff.
    density::chameleon                      2015-03-22   
      133118910 (x 1.592)     1040 MB/s 1281 MB/s       386e6  476e6
    density::cheetah                        2015-03-22   
      101751474 (x 2.083)      531 MB/s  493 MB/s       276e6  256e6
    density::lion                           2015-03-22   
       89433997 (x 2.370)      304 MB/s  275 MB/s       175e6  159e6
    LZ4                                     r127         
      101634462 (x 2.086)      365 MB/s 1815 MB/s       189e6  944e6
    LZF                                     3.6          very
      102043866 (x 2.077)      254 MB/s  500 MB/s       131e6  259e6
    LZO                                     2.08         1x1
      100592662 (x 2.107)      429 MB/s  578 MB/s       225e6  303e6
    QuickLZ                                 1.5.1b6      1
       94727961 (x 2.238)      370 MB/s  432 MB/s       204e6  238e6
    Snappy                                  1.1.0        
      101385885 (x 2.091)      356 MB/s 1085 MB/s       185e6  565e6
    wfLZ                                    r10          
      109610020 (x 1.934)      196 MB/s  701 MB/s        94e6  338e6
      
      
Build
-----

SHARC is C99 compliant and easy to build on most platforms. You need a C compiler (gcc, clang ...), a *make* utility and *git* to checkout the DENSITY submodule.

Just *cd* into the sharc directory, then run the following command :
> make

And that's it !

Usage
-----

To compress a file named 'test' using the default algorithm (chameleon) :
> sharc test

To decompress 'test.sharc'
> sharc -d test.sharc

It is of course possible to choose the compression algorithm. The fastest (chameleon) can be specified like this :
> sharc -c1 test

There are also -c2 (cheetah) and -c3 (lion) options :
> sharc -c2 test

> sharc -c3 test

For more help and the full list of options :
> sharc -h
