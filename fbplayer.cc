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
#include <linux/fb.h>
#include <sys/ioctl.h>

#include "image.h"
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

	//Use fb0 as default output
	char *fbdev = "/dev/fb0";

	if(argc == 2)
		fbdev = argv[2;]


	int fbfd = open (fbdev, O_RDWR);


	if (fbfd >= 0)
  	{
  	  	  		
		char *image_path;

		if(argc > 1) {
			image_path = argv[1];	
		} else {
	    	printf("fbwriter v 1.0\n");
	    	printf("Usage: fbwriter <input>\n");
		return  1;
    	}

		//Initialize framebuffer
    	struct fb_var_screeninfo vinfo;

		ioctl (fbfd, FBIOGET_VSCREENINFO, &vinfo);

		int fb_width = vinfo.xres;
		int fb_height = vinfo.yres;
		int fb_bpp = vinfo.bits_per_pixel;
		int fb_bytes = fb_bpp / 8;
		
		printf("Framebuffer: %i x %i, %i bits per pixel\n", fb_width, fb_height, fb_bpp);
		
		int fb_data_size = fb_width * fb_height * fb_bytes;


		char *fbdata =static_cast<char*>( mmap (0, fb_data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, (off_t)0));
		
		//Clear screen
		//system ("CLS");
		memset (fbdata, 0, fb_data_size);

		if (ends_with(image_path, ".lmi")) {		
			//Read LMI data

			Image *image = new Image(image_path);	
			int width = image->getWidth();
			int height = image->getHeight();
			printf("Image dimensions: %i x %i\n", width, height);
			
			//Find center
			int offsetX = (fb_width - width)/2;
			int offsetY = (fb_height - height)/2;
			//int frame_size = width * height * 2;
		
			for( int frameIdx=0; frameIdx < image->getTotalFrames(); ++frameIdx )
			{
			//Traverse LMI data and write to screen
			for(int y = 0; y < height; y++)
			{
				for(int x = 0; x < width; x++)
				{

					Color c = image->getColor565FromFrame( x , y, frameIdx );
					int screen_offset = (( offsetY + y ) * fb_width + ( offsetX + x )) * 4;
					
					fbdata [screen_offset + 0] = c.blue;
					fbdata [screen_offset + 1] = c.green;
					fbdata [screen_offset + 2] = c.red;
					fbdata [screen_offset + 3] = 0;

				}
			}
			usleep(1000*1000 / image->getFramerate());
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
						fbdata [screen_offset + 3] = 255;
						
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
    		fbdata [screen_offset + 0] = 255;
			fbdata [screen_offset + 1] = 255;
			fbdata [screen_offset + 2] = 255;
			fbdata [screen_offset + 3] = 0;
    	}
  	
  		printf("\033[%d;%dH",10,ver);
		printf("Press <enter> to quit.\n");
		cin.get();
		//Clean up 
		memset (fbdata, 0, fb_data_size);
		munmap (fbdata, fb_data_size);
		close (fbfd);
		
	}

  	return ret;
}





    
