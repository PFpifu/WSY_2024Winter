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
int SaveImage(const char* path, Image* bmpImg);  //output the image stored in the Image structure.
unsigned char Otsu (Image* bmpImg);  //the Otsu algorithm.
Image* Color_to_Gray (Image* bmpImg);  //change the input colored image to grayscale image.
Image* Gray_to_Binary (Image* bmpImg);  //use the Otsu algorithm to the total image to generate binary image.
Image* Mask (Image* bmpImg, int n, int ope);  //do dilation or erosion operation.
Image* Closing (Image* bmpImg, int n);  //do closing operation.
Image* Opening (Image* bmpImg, int n);  //do opening operation.
void local_Otsu (Image* bmpImg, int n);  //use Otsu algorithm to a specific local window of the image and slide it.
int Max (int a, int b);  //return the maximum of a and b.
int Min (int a, int b);  //return the minimum of a and b.


 
#endif
