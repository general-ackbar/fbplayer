//
//  Eva.h
//  LMIReader
//
//  Created by Jonatan Yde on 06/12/2017.
//  Copyright © 2017 Codeninja. All rights reserved.
//



#ifndef Eva_h
#define Eva_h

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdint.h>

using namespace std;

#define XSIZE 96
#define YSIZE 92
#define FRAMESIZE XSIZE/4 * YSIZE

#define bytes_to_u16(MSB,LSB) (((unsigned int) ((unsigned char) MSB)) & 255)<<8 | (((unsigned char) LSB)&255)



class Eva{
    char* data;    
    char _identifier[4];
    char _comment[1];    
    char _framecount[2];
    char _framerate[2];    

    unsigned int height;
    unsigned int width;
    unsigned int fps;
    unsigned int totalframes;
    unsigned int headerSize;
    unsigned int version;
    
    
private:
    char* readFileBytes(const char *name);
    
public:
	Eva(){};
    Eva(const char* filename);    
    void decodeFrame(uint8_t* frameBytes);
    int getWidth();
    int getHeight();
	int getTotalFrames();
    int getVersion();
	int getFramerate();
    uint8_t* getFrameData();
};

#endif 
