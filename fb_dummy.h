

#ifndef fb_dummy_h
#define fb_dummy_h

#define FBIOGET_VSCREENINFO 1
#define XRES 800
#define YRES 640
#define BPP 32
#define FBSIZE XRES*YRES* (BPP/8)


struct fb_var_screeninfo
{
    int xres = XRES;
	int yres = YRES;
	int bits_per_pixel = BPP;
};

#endif