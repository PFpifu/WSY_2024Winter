#ifndef BMP_H  
#define BMP_H  //define the header file which contains necessary structures.
 
#include <stdio.h>  
#include <stdlib.h>  

typedef struct  
{  
	//unsigned short    bfType;  //must always be set to 'BM' to declare that this is a .bmp-file.
	unsigned long    bfSize;  //specifies the size of the file in bytes.
	unsigned short    bfReserved1;   // must always be set to zero.
	unsigned short    bfReserved2;  //must always be set to zero.
	unsigned long    bfOffBits;    //specifies the offset from the beginning of the file to the bitmap data.
	
} BiFileHeader;  
 
typedef struct  
{  
	unsigned long  biSize;   //Number of bytes to define BITMAPINFOHEADER structure.
	long   biWidth;   //Image width (number of pixels).
	long   biHeight;   //Image height (number of pixels),positive->inverted,negative->upright,usually be positive.
	unsigned short   biPlanes;   //Number of planes. Always be 1.
	unsigned short   biBitCount;  //Bits per pixel (Bits/pixel), which is 1, 4, 8, 16, 24 or 32.
	unsigned long  biCompression;   //Compression type. Only non-compression is discussed here: BI_RGB.(0)
	unsigned long  biSizeImage;   //Image size with bytes. When biCompression=BI_RGB, biSizeImage=0.
	long   biXPelsPerMeter;   //Horizontal resolution, pixels/meter.
	long   biYPelsPerMeter;   //Vertical resolution, pixels/meter
	unsigned long   biClrUsed;   //Number of color indices used in the bitmap (0->all the palette items are used).
	unsigned long   biClrImportant;   //Number of important color indices for image display. 0->all items are important.
	
} BiInfoHeader;  
 
typedef struct   
{  
	unsigned char rgbBlue; //blue part
	unsigned char rgbGreen; //green part 
	unsigned char rgbRed; //red part 
	unsigned char rgbReserved; //reserved,0
	 
} RGBQuad;  
 
typedef struct  
{  
	int width;  //specifies the width of target image.
	int height;  //specfies the height of target image.
	int channels;  //specifies biBitCount of target image.
	unsigned char** imageData;  //store image data in two demensions.
	
}Image;  
 
Image* LoadImage(const char* path);  //load target image to a specific Image structure.
int SaveImage_gray(const char* path, Image* bmpImg);  //output the image stored in the Image structure.
int SaveImage_color(const char* path, Image* bmpImg);
Image* Log_Enhance(Image* bmpImg);
Image* Color_to_Gray (Image* bmpImg);
double* gray_Histogram (Image* bmpImg);
Image* Histogram_Eq (Image* bmpImg);
Image* Gray_to_Color (Image* bmpImg, Image* grayImg);
double check (double x);



 
#endif
