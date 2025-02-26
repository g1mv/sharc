# sharc

Simple High-speed ARChiver

**sharc** is a very fast file archiver with one goal in mind : speed, while achieving the best ratio possible.
It is based on the [**density** compression library](http://github.com/g1mv/density).
            
### Build

### Usage

To compress a file named 'test' using the default algorithm (chameleon) :
> sharc test

To decompress 'test.sharc'
> sharc -d test.sharc

It is of course possible to choose the compression algorithm. The fastest (chameleon) can be specified like this :
> sharc -c1 test

There are also -c2 (cheetah) and -c3 (lion) options :
> sharc -c2 test

> sharc -c3 test

For more help and a full list of options :
> sharc -h
