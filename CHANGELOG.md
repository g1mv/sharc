0.9.7 beta
----------
<i>August 8, 2013</i>
* Improved binary mode setting for stdin/stdout on Windows
* Added support for big endian CPU systems
* Date/time display now using preprocessor macros
* Namespacing of macros, structures, types and functions names*
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
