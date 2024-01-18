//
//  Lmi.cpp
//  LMIReader
//
//  Created by Jonatan Yde on 06/12/2017.
//  Copyright © 2017 Codeninja. All rights reserved.
//

#include "Lmi.h"


Lmi::Lmi(const char *filename){
   
    
    ifstream myfile (filename, ios::binary);
    
    
    myfile.read(this->_identifier, 4);    
    myfile.read(this->_header_length, 1);
    myfile.read(this->_width, 2);
    myfile.read(this->_height, 2);

    
    
	this->headerSize = (int)this->_header_length[0];
    this->width = bytes_to_u16(_width[0], _width[1]);
    this->height = bytes_to_u16(_height[0], _height[1]);
    this->bitsPerPixel = (int)this->_identifier[3];
    this->totalframes = 1;
    this->framerate = 1;
    
    if(this->headerSize > 9)
    {
    	myfile.read(this->_framerate, 1);
    	this->framerate = (int)this->_framerate[0];
    	
    }
    
    myfile.close();
    
    this->data = this->readFileBytes(filename);
    
}

char* Lmi::readFileBytes(const char *name)
{
    ifstream fl(name);
    fl.seekg( 0, ios::end );
    size_t len = fl.tellg();
    len = len-this->headerSize;
    char *ret = new char[len];
    fl.seekg(this->headerSize, ios::beg);
    fl.read(ret, len);
    fl.close();
    
    if(len > this->width*2*this->height*2)
    {
    	this->totalframes = len / (this->width*2*this->height*2);
    }
    
    return ret;
}

Color Lmi::getRGB565FromFrame(int x, int y, int frame) {  
    int pos = (this->width * 2 * y + (x*2)) + ((this->width * 2 * this->height * 2) * frame);
    //printf("pos: %i, ", pos);
    Color c;
    
    uint16_t rgb565 = ((uint8_t)this->data[pos] << 8) | (uint8_t)this->data[pos+1];          

    c.red = (rgb565 & 0b1111100000000000) >> 8;
    c.green = (rgb565 & 0b0000011111100000) >> 3;
    c.blue = (rgb565 & 0b0000000000011111) << 3;

    return c;  
}

Color Lmi::getRGB332FromFrame(int x, int y, int frame) {  
    
    int pos = (this->width * y + x) + ((this->width * this->height) * frame);
    
    uint8_t rgb332 = (uint8_t)this->data[pos]; 

    Color c;          
    c.red = (this->data[pos] >> 5) * 32;
    c.green = ((this->data[pos] & 28) >> 2) * 32;
    c.blue = (this->data[pos] & 3) *64;    
    return c;  
}

Color Lmi::getRGB888FromFrame(int x, int y, int frame) {  
    

    int pos = (this->width * 3 * y + (x*3)) + ((this->width * 3 * this->height * 3) * frame);
    
    Color c;    
    c.red = (unsigned char)this->data[pos];
    c.blue = (unsigned char)this->data[pos+1];
    c.green = (unsigned char) this->data[pos+2];
    return c;  
}



Color Lmi::getARGB8888FromFrame(int x, int y, int frame) {  
    int pos = (this->width * 4 * y + (x*2)) + ((this->width * 4 * this->height * 4) * frame);
    
    Color c;
    c.red = (unsigned char)this->data[pos];
    c.blue = (unsigned char)this->data[pos+1];
    c.green = (unsigned char) this->data[pos+2];
    c.alpha = (unsigned char) this->data[pos+3];
    
    return c;  
}

Color Lmi::getBinaryFromFrame(int x, int y, int frame, bool vertical=false)
{}

int Lmi::getWidth() {return this->width; }
int Lmi::getHeight() {return this->height; }
int Lmi::getTotalFrames() {return this->totalframes; }
int Lmi::getFramerate() {return this->framerate; }
uint8_t* Lmi::getFrameData() {return (uint8_t*)this->data; }
int Lmi::getBitsPerColor() {return this->bitsPerPixel; }

