//
//  Lmi.h
//  LMIReader
//
//  Created by Jonatan Yde on 06/12/2017.
//  Copyright © 2017 Codeninja. All rights reserved.
//



#ifndef Lmi_h
#define Lmi_h


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

class Lmi{
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
	Lmi(){};
    Lmi(const char* filename);    
    Color getARGB8888FromFrame(int x, int y, int frame);
    Color getRGB888FromFrame(int x, int  y, int frame);
    Color getRGB565FromFrame(int x, int y, int frame);
    Color getRGB332FromFrame(int x, int  y, int frame);
    Color getBinaryFromFrame(int x, int  y, int frame, bool vertical);
    uint8_t* getFrameData();
    int getTotalFrames();
    int getFramerate();
    int getWidth();
    int getHeight();
    int getBitsPerColor();
};

#endif 
