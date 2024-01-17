//
//  Eva.cc
//  LMIReader
//
//  Created by Jonatan Yde on 06/12/2017.
//  Copyright © 2017 Codeninja. All rights reserved.
//

#include "Eva.h"


Eva::Eva(const char *filename){
   
    
    ifstream myfile (filename, ios::binary);
    
    
    myfile.read(this->_identifier, 4);
	myfile.read(this->_comment, 1);
    myfile.read(this->_framecount, 2);
    myfile.read(this->_framerate, 2);
    
    
    
	/*
	
	if(this->headerSize < 9)
	{
		assert("Error. Non valid header\n");
		return;
	}
    */

    this->width = XSIZE;
    this->height = YSIZE;
    this->fps = bytes_to_u16(_framerate[1], _framerate[0]);
    this->totalframes = bytes_to_u16(_framecount[1], _framecount[0]);
    this->version = _identifier[3] - '0';
    this->headerSize = 9;
    
    myfile.close();
    
    if(version > 3)
    {
        printf("For now only EVA3 format is supported\n");
    } 
    else
        this->data = this->readFileBytes(filename);
        
}

char* Eva::readFileBytes(const char *name)
{
    ifstream fl(name);
    fl.seekg( 0, ios::end );
    size_t len = fl.tellg();
    len = len-this->headerSize;
    char *ret = new char[len];
    fl.seekg(headerSize, ios::beg);
    fl.read(ret, len);

/*
    for(int frameIdx = 0; frameIdx < this->totalframes; frameIdx++)
    {
        int offset = frameIdx * FRAMESIZE + this->headerSize;

        fl.seekg(offset, ios::beg);
        fl.read(ret, FRAMESIZE);
        uint8_t * frame = decodeFrame((uint8_t*) ret);
    
    }
*/

    fl.close();
    return ret;
}


void Eva::decodeFrame(uint8_t* frameBytes)
{
    uint8_t image[ XSIZE * YSIZE * 4 ];


    uint8_t display_byte;
    int x = 0, y = 0;
    
    for (int i = 0; i < FRAMESIZE; i++)
    {
        display_byte = frameBytes[i];
        
        
        for(int b = 0; b < 8; b+=2)
        {
            int level = (int)((display_byte & (0x03 << (6-b))) >> (6-b));
            
            int gray = level * 63;

            uint8_t* pixel = &image[(y*width+x)*4];
            
            pixel[0] = gray;
            pixel[1] = gray;
            pixel[2] = gray;
            pixel[3] = 255; 

            
            x++;

        }
        
        //if(i > 0 && (i % ROWSIZE ==0 ))
        if(x > 0 && (x % XSIZE ==0 ))
        {
            y++;
            x=0;
        }
    }
    
    //return image;
}




int Eva::getWidth() {return this->width; }
int Eva::getHeight() {return this->height; }
int Eva::getTotalFrames() {return this->totalframes; }
int Eva::getFramerate() {return this->fps; }
int Eva::getVersion() {return this->version; }
uint8_t* Eva::getFrameData() {return (uint8_t*)this->data; }
