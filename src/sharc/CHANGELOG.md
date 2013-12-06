0.9.10 beta
-----------
<i>September 14, 2013</i>

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
<i>August 26, 2013</i>

* Added maximum dictionary reset cycle to ensure possible parallelization
* Improved XOR mask
* Reorganized global variables
* Added buffer size and dictionary max cycles in stream header
* Reduced dictionary sizes to optimize fitting in processor caches
* Removed "cipher" references in file names, functions, and preprocessor macro guard names
* Added dictionary preloading to optimize encoding/decoding startup efficiency

0.9.8 beta
----------
<i>August 19, 2013</i>

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
<i>August 8, 2013</i>

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
<i>August 1, 2013</i>

* Target architecture definition option re-added in makefile
* Improved makefile and program displays
* Now properly using stdin/stdout as binary streams on Windows
* Added support for very large files (> 4GB)
* Platform detection now using gcc preprocessor macros
* Fixed problem with -c1 compression not properly working with certain files

0.9.5 beta
----------
<i>July 20, 2013</i>

* Improved makefile and added build information on version display
* Added stddef.h to globals.h for standard macro definitions

0.9.4 beta
----------
<i>July 5, 2013</i>

* Added no prompting for overwrites option
* Created client interface to differentiate file management and stream compression
* Added stdin reading / stdout writing support
* Added help option to display usage

0.9.3 beta
----------
<i>July 2, 2013</i>

* Corrected out file name problem
* Corrected problem with intermediate buffer pointer swap in case of an encoding mode reversion
* Automatic generation of objects from sources list in makefile
* Time is now measured and displayed in seconds
* Modified timing function to provide better precision on Linux platforms

0.9.2 beta
----------
<i>June 29, 2013</i>

* Improved makefile
* Added usage
* Added check for file overwrites
* Corrected dual pass compression / decompression
* Updated license
* Added restoring of original file attributes
* Corrected problems with intermediate buffering

0.9.1 beta
----------
<i>June 24, 2013</i>
 
* Initial beta version
