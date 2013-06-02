/*
 * Copyright (c) 2013, Centaurean
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Centaurean nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Centaurean BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * acceLZW
 *
 * 11/05/13 18:19
 * @author gpnuma
 */

#include "Sharcz.h"

Sharc::Sharc() {
    //dictionary = new ENTRY[1 << HASH_BITS];
	reset();
}

Sharc::~Sharc() {
    //delete[] dictionary;
}

FORCE_INLINE static void computeHash(unsigned int* hash, const unsigned int* value) {
    *hash = HASH_OFFSET_BASIS;
    *hash ^= *value;
    *hash *= HASH_PRIME;
    *hash = (*hash >> (32 - HASH_BITS)) ^ (*hash & 0xFFFF);
}

FORCE_INLINE static bool updateEntry(ENTRY* entry, const unsigned int* buffer, const unsigned int* index, SharcWriter* writer) {
	//entry->offset = *index;
	//entry->exists = true;
//#if __BYTE_ORDER == __LITTLE_ENDIAN
	//*(unsigned int*)entry = (*index | 1 << 24);
	/*entry->offset_0 = *index & 0xFFFF;
	entry->offset_1 = *index >> 16;
	entry->exists = true;*/
	*(unsigned int*)entry = (*index & 0xFFFFFF) | (1 << 24);
	//std::bitset<32> c(*(unsigned int*)entry);
	//std::bitset<32> i(*index);
    ////*(unsigned int*)entry = (*index << 1 | 1);
	////std::cout << sizeof(entry) << std::endl;
	//std::bitset<32> x(*index << 1 | 1);
	////std::bitset<32> y(*(unsigned int*)entry);
	//std::cout << sizeof(entry) /*<< ", " << c << ", " << i << " = " << x*/ << std::endl;
//#elif __BYTE_ORDER == __BIG_ENDIAN
//    *(unsigned int*)entry = (*index << 1 | 1);
//#elif
//#error
//#endif
	//writer->writeMode(false);
	return writer->writeChunk((unsigned int)*(buffer + *index));
}

FORCE_INLINE bool Sharc::compress(byte* byteBuffer, SharcWriter* writer) {
    //unsigned int outBits = 0;
    unsigned int hash;
    
    const unsigned int length = writer->getLimit() >> 2;
    const unsigned int* buffer = (const unsigned int*)byteBuffer;
    
    for(unsigned int i = 0; i < length; i ++) {
        computeHash(&hash, buffer + i);
        ENTRY* found = &dictionary[hash];
        if(found->exists) {
			if(buffer[i] ^ buffer[*(unsigned int*)found & 0xFFFFFF/*found->offset_0 + (found->offset_1 << 16)*/]) {
                if(!updateEntry(found, buffer, &i, writer))
                    return false;
            } else {
				//writer->writeMode(true);
                if(!writer->writeChunk((unsigned short)hash))
                    return false;
                //outBits += (HASH_BITS + 1);
			}
        } else {
            if(!updateEntry(found, buffer, &i, writer))
                return false;
        }
    }
    
	//return outBits;
    return true;
}

FORCE_INLINE bool Sharc::subs(byte* byteBuffer, SubsWriter* writer) {
    const unsigned int length = writer->getLimit() >> 2;
    const unsigned int* buffer = (const unsigned int*)byteBuffer;
    
    for(unsigned int i = 0; i < length; i ++) {
        //std::cout << (char)((buffer[i] >> 24) & 0xFFFF) << (char)((buffer[i] >> 16) & 0xFFFF) << (char)((buffer[i] >> 8) & 0xFFFF) << (char)((buffer[i]) & 0xFFFF) << ", " << i << std::endl;
        if(!writer->writeChunk(buffer[i]))
            return false;
    }
    
    return true;
}

void Sharc::reset() {
    for(unsigned int i = 0; i < (1 << HASH_BITS); i++)
        *(unsigned int*)(dictionary + i) = 0;
}

int main(int argc, char *argv[]) {
	if(argc <= 1)
		exit(0);
    
    Chrono* chrono = new Chrono();
    std::cout << "Chrono initialized" << std::endl;
    
    Sharc* sharc = new Sharc();
    std::cout << "Sharc initialized" << std::endl;
    
    const unsigned int readBufferSize = 1 << 24;  // Maximum depending on ENTRY.offset length
    std::cout << "Allocating " << readBufferSize << " as buffer read" << std::endl;
	byte* readArray = new byte[readBufferSize];
	byte* writeArray = new byte[readBufferSize];
    std::cout << "Read / write arrays initialized" << std::endl;
    
    for(int i = 1; i < argc; i ++) {
        //unsigned int outBits = 0;
        unsigned int totalRead = 0;
        unsigned int totalWritten = 0;
        
		std::string inFileName = std::string(argv[i]);
		std::string outFileName = inFileName + ".sha";
		
        //std::ifstream inFile (inFileName, std::ios::in | std::ios::binary);
		//std::ofstream outFile (outFileName, std::ios::out | std::ios::binary);
        
        FILE* inFile = fopen(inFileName.c_str(), "rb");
        FILE* outFile = fopen(outFileName.c_str(), "wb+");
        
		SubsWriter* swriter = new SubsWriter(writeArray/*readArray*/, readBufferSize);
        std::cout << "SubsWriter initialized" << std::endl;
        
		SharcWriter* writer = new SharcWriter(readArray/*writeArray*/, readBufferSize);
        std::cout << "SharcWriter initialized" << std::endl;
        
        /*writer->writeChunk((unsigned int)65266236);
        writer->flush();
        std::cout << "ok " << writer->getPosition() << std::endl;*/
        unsigned int bytesRead;
        
        chrono->start();
		while((bytesRead = (unsigned int)fread(readArray, sizeof(byte), readBufferSize, inFile)) > 0) {
			//inFile.read ((char*)(readArray), readBufferSize);
			//unsigned int bytesRead = (unsigned int);//(unsigned int)inFile.gcount();
			totalRead += bytesRead;
            
            swriter->setLimit(bytesRead/*writer->getPosition()*/);
            
            if(sharc->subs(readArray, swriter)) {
                //std::cout << "Subs : compress" << std::endl;
                swriter->flush();
            } else {
                std::cout << "Subs : no compress" << std::endl;
            }
            //std::cout << bytesRead/*writer->getPosition()*/ << " > " << swriter->getPosition() << std::endl;
            
            writer->setLimit(swriter->getPosition()/*bytesRead*/);
             
             if(sharc->compress(readArray, writer)) {
                 writer->flush(); // todo manage remaining bytes
                 totalWritten += fwrite(readArray, sizeof(byte), writer->getPosition(), outFile);
             } else {
                 std::cout << "Sharc : No compress" << std::endl;
                 totalWritten += fwrite(writeArray, sizeof(byte), bytesRead, outFile);
             }
             //std::cout << swriter->getPosition()/*bytesRead*/ << " > " << writer->getPosition() << std::endl << "-----" << std::endl;
            
            
            sharc->reset();
            swriter->resetModes();
            swriter->resetBuffer();
            writer->resetModes();
            writer->resetBuffer();
		}
		chrono->stop();
                
        //outFile.flush();

		std::cout << "--------------------------------------------------------------------" << std::endl;
        
        double outBytes = totalWritten;
		double ratio = outBytes / totalRead;
		std::cout << "File " << argv[i] << ", " << totalRead << " bytes in, " << (unsigned int)outBytes << " bytes out" << std::endl;
        
		double outSpeed = (1000.0 * totalRead) / (chrono->getElapsedMillis() * 1024.0 * 1024.0);
		std::cout  << "Ratio out / in = " << ratio << ", time = " << chrono->getElapsedMillis() << " ms, Speed = " << outSpeed << " MB/s" << std::endl;
        
		std::cout << "COMBINED SCORE = " << outSpeed / ratio << std::endl << "------" << std::endl;
        
        fclose(inFile);
        fclose(outFile);
		//inFile.close();
		//outFile.close();
		//delete bitOutputStream;
		//delete inFile;
		//delete outFile;
    }
    
	delete readArray;
	delete sharc;
    delete chrono;
}
