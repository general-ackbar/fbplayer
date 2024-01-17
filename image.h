//
//  Image.h
//  LMIReader
//
//  Created by Jonatan Yde on 06/12/2017.
//  Copyright © 2017 Codeninja. All rights reserved.
//



#ifndef Image_h
#define Image_h


#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;


struct Color {
  int red;
  int green;
  int blue;
  int alpha;
} ;


#define bytes_to_u16(MSB,LSB) (((unsigned int) ((unsigned char) MSB)) & 255)<<8 | (((unsigned char) LSB)&255)

class Image{
    char* data;
    char _header_length[1];
    char _width[4];
    char _height[4];
    char _framerate[1];
    char _identifier[3];
    unsigned int bitsPerPixel;
    unsigned int height;
    unsigned int width;
    unsigned int headerSize;
	unsigned int framerate;
    unsigned int totalframes;
    
private:
    char* readFileBytes(const char *name);
    
public:
	Image(){};
    Image(const char* filename);
    Color getColor565(int x, int y);
    Color getColor565FromFrame(int x, int y, int frame);
    Color getColorARGB(int x, int y);
    uint8_t* getFrameData();
    int getTotalFrames();
    int getFramerate();
    int getWidth();
    int getHeight();
    int getBitsPerColor();
};

#endif 
