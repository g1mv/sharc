1.2.1
-----
*March 25, 2015*

* Updated user timings for windows compatibility

1.2.0
-----
*March 24, 2015*

* Updated Density to 0.12.0
* Improved time display and sampling by measuring user time which is more realistic
* Corrected incorrect import in chrono.h
* Improved compilation switches

1.1.1
-----
*February 10, 2015*

* Updated Density to 0.11.4
* Removed unnecessary makefile, now using a single makefile

1.1.0
-----
*February 5, 2015*

* Added integrity checks
* Updated to Density 0.11.3

1.0.0
-----
*January 19, 2015*

* Now using Density 0.11.1 with the new streams API
* Improved README, added benchmark

0.9.11 beta
-----------
*December 12, 2013*

* Client code and compression library are now distinct projects
* Updated compression library to [Density 0.9.12 beta](https://github.com/centaurean/density.git)
* Makefile improvements
* File structure update
* Compression modes corrected
* Corrected help output
* Improved memory management of text contents
* Updated license back to standard GPLv3

0.9.10 beta
-----------
*September 14, 2013*

* Added a stream API with flushing support and custom memory allocation
* Added a buffers API on top of the the stream API layer
* Added api.h documentation
* Changed -O3 to -O4 (maximum clang optimizations)
* Reorganized SHARC hash algorithm members to enable better compiler optimization
* Added block footer structures
* Added mode reversion markers
* Block encoding and global encoding are now processed distinctly
* Improved dictionary data format with compiler-required braces
* Corrected printf format in client version display
* Updated compressed default dictionary (little and big endian)
* Added compilation flag -Wall when WARNINGS is defined, as well as a DEBUG option
* Reintroduced full C99 compatibility 

0.9.9 beta
----------
*August 26, 2013*

* Added maximum dictionary reset cycle to ensure possible parallelization
* Improved XOR mask
* Reorganized global variables
* Added buffer size and dictionary max cycles in stream header
* Reduced dictionary sizes to optimize fitting in processor caches
* Removed "cipher" references in file names, functions, and preprocessor macro guard names
* Added dictionary preloading to optimize encoding/decoding startup efficiency

0.9.8 beta
----------
*August 19, 2013*

* Added preprocessor guards on large file #defines
* Modified dictionary indexing process to enable reuse of long distance previously-read file data (major performance boost)
* Namespaced dictionaries
* Added the specify output path (-p) option
* Fixed problem with some strings not being initialized in client.c
* Added parenthesis protection on preprocessor macro SHARC_PREFERRED_BUFFER_SIZE
* Added reset dictionary information in block header
* Modified -std=c99 to -std=gnu99 in Makefile to remove the ftello implicit declaration warning

0.9.7 beta
----------
*August 8, 2013*

* Improved binary mode setting for stdin/stdout on Windows
* Added support for big endian CPU systems
* Date/time display now using preprocessor macros
* Namespacing of macros, structures, types and functions names
* Modified platform detection compiler check strings
* Client now displaying endianness
* Modification of the file format to include the type of media (stream, file...)
* Changed file type identification check to a magic number
* Improved header functions signatures
* Cleared remaining compilation warnings
* Modified XOR hash mask

0.9.6 beta
----------
*August 1, 2013*

* Target architecture definition option re-added in makefile
* Improved makefile and program displays
* Now properly using stdin/stdout as binary streams on Windows
* Added support for very large files (> 4GB)
* Platform detection now using gcc preprocessor macros
* Fixed problem with -c1 compression not properly working with certain files

0.9.5 beta
----------
*July 20, 2013*

* Improved makefile and added build information on version display
* Added stddef.h to globals.h for standard macro definitions

0.9.4 beta
----------
*July 5, 2013*

* Added no prompting for overwrites option
* Created client interface to differentiate file management and stream compression
* Added stdin reading / stdout writing support
* Added help option to display usage

0.9.3 beta
----------
*July 2, 2013*

* Corrected out file name problem
* Corrected problem with intermediate buffer pointer swap in case of an encoding mode reversion
* Automatic generation of objects from sources list in makefile
* Time is now measured and displayed in seconds
* Modified timing function to provide better precision on Linux platforms

0.9.2 beta
----------
*June 29, 2013*

* Improved makefile
* Added usage
* Added check for file overwrites
* Corrected dual pass compression / decompression
* Updated license
* Added restoring of original file attributes
* Corrected problems with intermediate buffering

0.9.1 beta
----------
*June 24, 2013*

* Initial beta version
