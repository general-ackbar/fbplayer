#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <iostream>
#include <fcntl.h>  
#include <unistd.h> 

#include <errno.h>
extern int errno ;
//#define __DUMMY__

#ifndef __DUMMY__
	#include <linux/fb.h>
#else
	#include "fb_dummy.h"
#endif

#include <sys/ioctl.h>

#include "Lmi.h"
#include "Eva.h"

bool ends_with(char const *str, char const *suffix)
{
    size_t l0 = strlen(str);
    size_t l1 = strlen(suffix);

    if (l0 < l1)
        return false;

    return strcmp(str + l0 - l1, suffix) == 0;
}

int main (int argc, char *argv[])
{
	int ret = 0; // Exit value
	char *image_path;
	
	if(argc > 1) {
		image_path = argv[1];	
	} else {
	    printf("fbwriter v 1.0\n");
	    printf("Usage: fbwriter <input>\n");
		return  1;
    }

	//Use fb0 as default output
	char *fbdev = "/dev/fb0";

	if(argc == 3)
		fbdev = argv[2];

#ifndef __DUMMY__
	int fbfd = open (fbdev, O_RDWR);
#else //create dummy framebuffer 
	int fbfd = open (fbdev, O_RDWR | O_CREAT | O_TRUNC, 0644);	
	lseek(fbfd, FBSIZE-1, SEEK_SET );
	write(fbfd, 0, 1);
	lseek(fbfd, 0, SEEK_SET);
#endif
	
	if (fbfd >= 0)
  	{
		//Initialize framebuffer
    	struct fb_var_screeninfo vinfo;

		ioctl (fbfd, FBIOGET_VSCREENINFO, &vinfo);

		int fb_width = vinfo.xres;
		int fb_height = vinfo.yres;
		int fb_bpp = vinfo.bits_per_pixel;
		int fb_bytes = fb_bpp / 8;
		int fb_data_size = fb_width * fb_height * fb_bytes; 

		printf("Framebuffer: %i x %i, %i bits per pixel (%i bytes) \n", fb_width, fb_height, fb_bpp, fb_data_size);
		

		/*
		int errnum;
		
		char *dst;
		if((dst = (char*)mmap (NULL, fb_data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, (off_t)0)) == (caddr_t) -1)
		{
			errnum = errno;
      		fprintf(stderr, "Value of errno: %d\n", errno);
      		perror("Error printed by perror");
      		fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));
		}
		*/


		char *fbdata =static_cast<char*>( mmap (NULL, fb_data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, (off_t)0) );
		//Clear screen
		memset (fbdata, 0, fb_data_size);

		if (ends_with(image_path, ".lmi")) {		
			//Read LMI data

			Lmi *lmi = new Lmi(image_path);	
			int width = lmi->getWidth();
			int height = lmi->getHeight();
			printf("Image dimensions: %i x %i\n", width, height);
			if(lmi->getBitsPerColor() != 16 )
			{
				printf("This viewer only support images encoded with the rgb565 colorspace.\n");
				return 1;
			}
			
			//Find center
			int offsetX = (fb_width - width)/2;
			int offsetY = (fb_height - height)/2;
			//int frame_size = width * height * 2;
		
			for( int frameIdx=0; frameIdx < lmi->getTotalFrames(); ++frameIdx )
			{
			//Traverse LMI data and write to screen
			for(int y = 0; y < height; y++)
			{
				if(y > fb_height) continue;
				for(int x = 0; x < width; x++)
				{
					if(x > fb_width) continue;
					
					Color c = lmi->getRGB565FromFrame( x , y, frameIdx );
					int screen_offset = (( offsetY + y ) * fb_width + ( offsetX + x )) * 4;
					
					//printf("(X:%i, y:%i) -> (r:%i, g:%i, b:%i, a:%i)\n", x, y, c.red, c.green, c.blue, c.alpha);
					fbdata [screen_offset + 0] = c.blue;
					fbdata [screen_offset + 1] = c.green;
					fbdata [screen_offset + 2] = c.red;
					fbdata [screen_offset + 3] = 0;

				}
			}
			usleep(1000*1000 / lmi->getFramerate());
			}
			
		}
		else if(ends_with(image_path, ".eva")) {
			
			
			//Read EVA data
			Eva *eva = new Eva(image_path);	
			int width = eva->getWidth();
			int height = eva->getHeight();
			
		
			//Find center
			int offsetX = (fb_width - width)/2;
			int offsetY = (fb_height - height)/2;
		
    		for( int frameIdx=0; frameIdx < eva->getTotalFrames(); ++frameIdx )
    		{
				int offset = frameIdx * FRAMESIZE;
				uint8_t display_byte;
				int x = 0, y = 0;
			
				for (int pos = offset; pos < offset+FRAMESIZE; pos++)
				{
					display_byte = eva->getFrameData()[pos];
								
					for(int b = 0; b < 8; b+=2)
					{
						int level = (int)((display_byte & (0x03 << (6-b))) >> (6-b));                
						int gray = level * 63;

						int screen_offset = (( offsetY + y ) * fb_width + ( offsetX + x )) * 4;
						
						fbdata [screen_offset + 0] = gray;
						fbdata [screen_offset + 1] = gray;
						fbdata [screen_offset + 2] = gray;
						fbdata [screen_offset + 3] = (uint8_t)255;
						
						x++;
					}
				
					//Time to move to next row?
					if(x > 0 && (x % XSIZE ==0 ))
					{
						y++;
						x=0;
					}
				}
        		usleep(1000*1000 / eva->getFramerate());
    		}
		}


    	int ver = fb_height - 25;
    	for(int hor = 0; hor < fb_width; hor++)
    	{
    		int screen_offset = (ver * fb_width + hor ) * 4;
    		fbdata [screen_offset + 0] = (uint8_t)255;
			fbdata [screen_offset + 1] = (uint8_t)255;
			fbdata [screen_offset + 2] = (uint8_t)255;
			fbdata [screen_offset + 3] = 0;
    	}
		
  		printf("\033[%d;%dH",10,ver);
		printf("Press <enter> to quit.\n");
		cin.get();
		//Clean up 

		#ifdef __DUMMY__
		memset (fbdata, 0, fb_data_size);
		munmap (fbdata, fb_data_size);		
		#endif

		close (fbfd);
		
	}

  	return ret;
}





    
