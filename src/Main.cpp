#include "SharcCipher.h"
#include "commons.h"
#include <fstream>

int main(int argc, char *argv[]) {
	if(argc <= 1)
		exit(0);
    
    Chrono* chrono = new Chrono();
    std::cout << "Chrono initialized" << std::endl;
    
    const unsigned int readBufferSize = PREFERRED_BUFFER_SIZE;  // Maximum depending on ENTRY.offset length
    std::cout << "Allocating " << readBufferSize << " as buffer read" << std::endl;
	byte* readArray = new byte[readBufferSize];
	byte* writeArray = new byte[readBufferSize];
    std::cout << "Read / write arrays initialized" << std::endl;
    
    //HashCipher* sharcCipher = new HashCipher();
    //LookupCipher* sharcCipher = new LookupCipher();
    SharcCipher* sharcCipher = new SharcCipher();
    
    for(int i = 1; i < argc; i ++) {
        unsigned int totalRead = 0;
        unsigned int totalWritten = 0;
        
		std::string inFileName = std::string(argv[i]);
		std::string outFileName = inFileName + ".sha";
		
        FILE* inFile = fopen(inFileName.c_str(), "rb");
        FILE* outFile = fopen(outFileName.c_str(), "wb+");
        
        unsigned int bytesRead;
        
        chrono->start();
		while((bytesRead = (unsigned int)fread(readArray, sizeof(byte), readBufferSize, inFile)) > 0) {
			totalRead += bytesRead;
            
            if(sharcCipher->encode(readArray, bytesRead, writeArray, bytesRead))
                totalWritten += fwrite(writeArray, sizeof(byte), sharcCipher->getOutPosition(), outFile);
            else
                totalWritten += fwrite(readArray, sizeof(byte), bytesRead, outFile);
		}
		chrono->stop();
        
		std::cout << "--------------------------------------------------------------------" << std::endl;
        
        double outBytes = totalWritten;
		double ratio = outBytes / totalRead;
		std::cout << "File " << argv[i] << ", " << totalRead << " bytes in, " << (unsigned int)outBytes << " bytes out" << std::endl;
        
		double outSpeed = (1000.0 * totalRead) / (chrono->getElapsedMillis() * 1024.0 * 1024.0);
		std::cout  << "Ratio out / in = " << ratio << ", time = " << chrono->getElapsedMillis() << " ms, Speed = " << outSpeed << " MB/s" << std::endl;
        
		std::cout << "COMBINED SCORE = " << outSpeed / ratio << std::endl << "------" << std::endl;
        
        fclose(inFile);
        fclose(outFile);
    }
    
	delete readArray;
	delete sharcCipher;
    delete chrono;
}