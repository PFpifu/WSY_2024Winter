#include "bmp.h"
#include <math.h>
#define pi 3.1415926

/*
Function: LoadImage.
Usage: load main content of input BMP image to a specific image pointer.
*/

Image* LoadImage(const char* path)  
{  
    Image* bmpImg;  
    FILE* fpbmp;  //the bmp file pointer.
    unsigned short fileType;  
    BiFileHeader bmpfileheader;  
    BiInfoHeader bmpinfoheader;  
    int channels = 1;  
    int width = 0;  
    int height = 0;  
    int step = 0;  //step means times needed to read and write image data.
    int offset = 0;  //help complete the 4-byte.
    unsigned char pixVal;  //pixel value,using unsigned char to represent 8-bit.
    RGBQuad* quad;  //the palette.
    int i, j, k;  
  
    bmpImg = (Image*)malloc(sizeof(Image));  
    fpbmp = fopen(path, "rb");  
    if (!fpbmp)  
    {  
        free(bmpImg);  
        return NULL;  
    }  //if the read failed,return.
  
    fread(&fileType, sizeof(unsigned short), 1, fpbmp);  //check whether the bfType is "BM".
    if (fileType == 0x4D42)  
    {  
        fread(&bmpfileheader, sizeof(BiFileHeader), 1, fpbmp);  
        fread(&bmpinfoheader, sizeof(BiInfoHeader), 1, fpbmp);  
   
        if (bmpinfoheader.biBitCount == 24)  //this is a real color image.
        {   
            channels = 3;  
            width = bmpinfoheader.biWidth;  
            height = bmpinfoheader.biHeight;  
  
            bmpImg->width = width;  
            bmpImg->height = height;  
            bmpImg->channels = 3;  //load contents.
            //bmpImg->imageData = (unsigned char*)malloc(sizeof(unsigned char)*width*3*height);  
            bmpImg->imageData = (unsigned char**)malloc(sizeof(unsigned char*)*height);
            for(i=0;i<height;i++) bmpImg->imageData[i] = (unsigned char*)malloc(sizeof(unsigned char)*width*3);
            //step = channels*width;  
  
            offset = (channels*width)%4;  
            if (offset != 0)  
            {  
                offset = 4 - offset;  
            }  //if the step is not the multiple of 4,complete it.
  
            for (i=0; i<height; i++)  
            {  
                for (j=0; j<width; j++)  
                {  
                    for (k=0; k<3; k++)  
                    {  
                        fread(&pixVal, sizeof(unsigned char), 1, fpbmp);  
                        bmpImg->imageData[height-1-i][j*3+k] = pixVal;  //read the image data to the array imageData[][].
                    }  
                }  
                if (offset != 0)  
                {  
                    for (j=0; j<offset; j++)  
                    {  
                        fread(&pixVal, sizeof(unsigned char), 1, fpbmp);  
                    }  
                }  
            }  
        }  
    }  
    //bmpImg->YUVflag = 0;  //set the change value to 0.
    return bmpImg;  
}  


/*
Function: SaveImage_color.
Usage: output a color image to the specific path.
*/

int SaveImage_color(const char* path, Image* bmpImg)  
{  
    FILE *fpbmp;  
    unsigned short fileType;  
    BiFileHeader bmpfileheader;  
    BiInfoHeader bmpinfoheader;  
    int step;  
    int offset;  
    unsigned char pixVal = '\0';
    int i, j, k, a, b;  
    //RGBQuad* quad;  
  
    fpbmp = fopen(path, "wb");  
    if (!fpbmp)  
    {  
        printf("ERROR\n");
        return 0;  
    }  //the error handler.
  
    fileType = 0x4D42;  //"BM"
    fwrite(&fileType, sizeof(unsigned short), 1, fpbmp);  
  
    step = bmpImg->width * 3;  
    offset = step % 4;  

    bmpfileheader.bfSize = 54 + step * bmpImg->height ;  //the total bytes.
    bmpfileheader.bfReserved1 = 0;  
    bmpfileheader.bfReserved2 = 0;  
    bmpfileheader.bfOffBits = 54;  
    fwrite(&bmpfileheader, sizeof(BiFileHeader), 1, fpbmp);  
  
    bmpinfoheader.biSize = 40;  
    bmpinfoheader.biWidth = bmpImg->width;  
    bmpinfoheader.biHeight = bmpImg->height;  
    bmpinfoheader.biPlanes = 1;  
    bmpinfoheader.biBitCount = 24;  
    bmpinfoheader.biCompression = 0;  
    bmpinfoheader.biSizeImage = bmpImg->height*step;  
    bmpinfoheader.biXPelsPerMeter = 0;  
    bmpinfoheader.biYPelsPerMeter = 0;  
    bmpinfoheader.biClrUsed = 0;  //the gray intensity [0,255].
    bmpinfoheader.biClrImportant = 0;  
    fwrite(&bmpinfoheader, sizeof(BiInfoHeader), 1, fpbmp);  

    for(i=bmpImg->height-1; i>-1; i--)
    {
    	for(j=0; j<bmpImg->width; j++)
    	{
    		for(k=0; k<3; k++)
    		{
    			pixVal = bmpImg->imageData[i][j*3+k];
    			fwrite(&pixVal, sizeof(unsigned char), 1, fpbmp);
    		}
    	}
    	if(offset!=0)
    	{
    		for(j=0; j<4-offset; j++)
    		{
    			pixVal = '\0';
    			fwrite(&pixVal, sizeof(unsigned char), 1, fpbmp);  //complete image data of every row.
    		}
    	}
    }
    
    fclose(fpbmp);  
    return 1;  
}

/*
Function: Translation.
Usage: do translation operation to the specific color image, X' = X + x, Y' = Y + y.
*/

Image* Translation (Image* bmpImg, int x, int y)
{
	int i, j, k;
	Image* newImg;
	newImg = (Image*)malloc(sizeof(Image));
	newImg->channels = 3;
	newImg->height = bmpImg->height + abs(y);
	newImg->width = bmpImg->width + abs(x);  //initialization
	
	int tx = abs(x);
	int ty = abs(y);
	
	newImg->imageData = (unsigned char**)malloc(sizeof(unsigned char*) * newImg->height);
	for(i=0; i<newImg->height; i++) newImg->imageData[i] = (unsigned char*)malloc(sizeof(unsigned char)* newImg->width * 3);
	
	for(i=0; i<newImg->height; i++)
	{
		for(j=0; j<newImg->width; j++)
		{
			for(k=0;k<3;k++) newImg->imageData[i][j*3+k] = 0;  //initialize the new image
		}
	}

	if(x>=0)
	{
		if(y>=0)
		{
			for(i=0; i<bmpImg->height; i++)
			{
				for(j=0; j<bmpImg->width; j++)
				{
					for(k=0; k<3; k++) newImg->imageData[i+ty][(j+tx)*3+k] = bmpImg->imageData[i][j*3+k];
				}
			}
			
		} else {
			for(i=0; i<bmpImg->height; i++)
			{
				for(j=0; j<bmpImg->width; j++)
				{
					for(k=0; k<3; k++) newImg->imageData[i][(j+tx)*3+k] = bmpImg->imageData[i][j*3+k];
				}
			}
		}
	} else {
		if(y>=0)
		{
			for(i=0; i<bmpImg->height; i++)
			{
				for(j=0; j<bmpImg->width; j++)
				{
					for(k=0; k<3; k++) newImg->imageData[i+ty][j*3+k] = bmpImg->imageData[i][j*3+k];
				}
			}
		} else {
			for(i=0; i<bmpImg->height; i++)
			{
				for(j=0; j<bmpImg->width; j++)
				{
					for(k=0; k<3; k++) newImg->imageData[i][j*3+k] = bmpImg->imageData[i][j*3+k];  //do translation in 4 different situation.
				}
			}
		}
	}
	
	return newImg;
}

/*
Function: Mirror
Usage: do mirror operation to the specific image, axis is either 'x' or 'y'.
*/

Image* Mirror (Image* bmpImg, char axis)
{
	
	int i, j, k;
	Image* newImg;
	newImg = (Image*)malloc(sizeof(Image));
	newImg->channels = 3;
	if(axis == 'x')
	{
		newImg->height = bmpImg->height;
		newImg->width = bmpImg->width * 2;
		newImg->imageData = (unsigned char**)malloc(sizeof(unsigned char*) * newImg->height);
	    for(i=0; i<newImg->height; i++) newImg->imageData[i] = (unsigned char*)malloc(sizeof(unsigned char)* newImg->width * 3);
	
	    for(i=0; i<newImg->height; i++)
	    {
		    for(j=0; j<newImg->width; j++)
		    {
			    for(k=0;k<3;k++) newImg->imageData[i][j*3+k] = 0;  //initialize the new image.
		    }
	    }
	    for(i=0; i<bmpImg->height; i++)
	    {
	    	for(j=0; j<bmpImg->width; j++)
	    	{
	    		for(k=0; k<3; k++) newImg->imageData[i][j*3+k] = bmpImg->imageData[i][j*3+k];
	    		for(k=0; k<3; k++) newImg->imageData[i][(newImg->width-1-j)*3+k] = bmpImg->imageData[i][j*3+k];  //do x-axis mirror operation
	    	}
	    }
	} else if (axis == 'y')
	{
		newImg->height = bmpImg->height * 2;
		newImg->width = bmpImg->width;
		newImg->imageData = (unsigned char**)malloc(sizeof(unsigned char*) * newImg->height);
	    for(i=0; i<newImg->height; i++) newImg->imageData[i] = (unsigned char*)malloc(sizeof(unsigned char)* newImg->width * 3);
	
	    for(i=0; i<newImg->height; i++)
	    {
		    for(j=0; j<newImg->width; j++)
		    {
			    for(k=0;k<3;k++) newImg->imageData[i][j*3+k] = 0;
		    }
	    }
	    for(i=0; i<bmpImg->height; i++)
	    {
	    	for(j=0; j<bmpImg->width; j++)
	    	{
	    		for(k=0; k<3; k++) newImg->imageData[newImg->height-1-i][j*3+k] = newImg->imageData[i][j*3+k] = bmpImg->imageData[i][j*3+k];  //do y-axis mirror operation
	    	}
	    }
	}
	
	return newImg;
}

/*
Function: Scale.
Usage: do scale operation to the specific image, X' = X * ratiox, Y' = Y * ratioy.
*/

Image* Scale (Image* bmpImg, double ratiox, double ratioy)
{
	int i ,j;
	Image* newImg;
	newImg = (Image*)malloc(sizeof(Image));
	newImg->channels = 3;
	newImg->height = bmpImg->height * ratioy;
	newImg->width = bmpImg->width * ratiox;
	newImg->imageData = (unsigned char**)malloc(sizeof(unsigned char*) * newImg->height);
	for(i=0; i<newImg->height; i++) newImg->imageData[i] = (unsigned char*)malloc(sizeof(unsigned char)* newImg->width * 3);  //initialization
	
	double antiratiox = 1 / ratiox, antiratioy = 1 / ratioy;  //the inverse function of transformation function of scale operation
	double srcx, srcy;
	for(i=0; i<newImg->height; i++)
	{
		for(j=0; j<newImg->width; j++)
		{
			srcx = j * antiratiox * 1.0;
			srcy = i * antiratioy * 1.0;  //find the source coordinate
			bili_interpolation(bmpImg, newImg, j, i, srcx, srcy);  //do bilinear interpolation of target coordinate
		}
	}
	
	return newImg;
}

/*
Function: Rotation.
Usage: do rotation operation to the specific image. The rotate center is the center pixel of image. 
       X' = X*cos(angle/180*pi) - Y*sin(angle/180*pi), Y' = X*sin(angle/180*pi) + Y*cos(angle/180*pi).
*/

Image* Rotation (Image* bmpImg, double angle)  
{
	int i ,j;
	Image* newImg;
	newImg = (Image*)malloc(sizeof(Image));
	newImg->channels = 3;
	newImg->height = floor(abs(bmpImg->width*sin(angle/180.0*pi))+abs(bmpImg->height*cos(angle/180.0*pi)));
	newImg->width = floor(abs(bmpImg->width*cos(angle/180.0*pi))+abs(bmpImg->height*sin(angle/180.0*pi)));
	newImg->imageData = (unsigned char**)malloc(sizeof(unsigned char*) * newImg->height);
	for(i=0; i<newImg->height; i++) newImg->imageData[i] = (unsigned char*)malloc(sizeof(unsigned char)* newImg->width * 3);  //initialization
	
	double srcx, srcy;
	for(i=0; i<newImg->height; i++)
	{
		for(j=0; j<newImg->width; j++)
		{
			srcy = (i-newImg->height/2.0)*cos(angle/180.0*pi) + (j-newImg->width/2.0)*sin(angle/180.0*pi) + bmpImg->height/2.0;
			srcx = (i-newImg->height/2.0)*sin(-angle/180.0*pi) + (j-newImg->width/2.0)*cos(angle/180.0*pi) + bmpImg->width/2.0;  //the inverse function of transformation function of rotation operation
			if(srcx>=0&&srcx<bmpImg->width&&srcy>=0&&srcy<bmpImg->height)
			{
				bili_interpolation(bmpImg, newImg, j, i, srcx, srcy);  //do bilinear interpolation of target coordinate
			}
		}
    }
	return newImg;
}

/*
Function: Shear.
Usage: do shear operation to the specific image. The shear center is the center pixel of image. 
       X' = X + dx * Y, Y' = dy * X + Y.
*/

Image* Shear (Image* bmpImg, double dx, double dy)
{
	int i , j;
	Image* newImg;
	newImg = (Image*)malloc(sizeof(Image));
	newImg->channels = 3;
	newImg->height = bmpImg->height + bmpImg->width * dy;
	newImg->width = bmpImg->width + bmpImg->height * dx;
	newImg->imageData = (unsigned char**)malloc(sizeof(unsigned char*) * newImg->height);
	for(i=0; i<newImg->height; i++) newImg->imageData[i] = (unsigned char*)malloc(sizeof(unsigned char)* newImg->width * 3);  //initialization
	
	double srcx, srcy;
	for(i=0; i<newImg->height; i++)
	{
		for(j=0; j<newImg->width; j++)
		{
			srcy = 1.0/(1 - dx*dy)*((i-newImg->height/2.0) - dy*(j-newImg->width/2.0)) + bmpImg->height/2.0;
			srcx = 1.0/(1 - dx*dy)*((j-newImg->width/2.0) - dx*(i-newImg->height/2.0)) + bmpImg->width/2.0;  //the inverse function of transformation function of shear operation
			if(srcx>=0&&srcx<bmpImg->width&&srcy>=0&&srcy<bmpImg->height)
			{
				bili_interpolation(bmpImg, newImg, j, i, srcx, srcy);  //do bilinear interpolation of target coordinate
			}
		}
    }
	return newImg;
}

/*
Function: bili_interpolation.
Usage: do bilinear interpolation to the given target coordinate (dstx, dsty). The function choose 4 corner coordinate 
(srcx-1, srcy-1), (srcx-1, srcy+1), (srcx+1, srcy-1), (srcx+1, srcy+1) and do average operation of 4 coordinates' color
to be the color of target coordinate (dstx, dsty).
*/

void bili_interpolation (Image* oriImg, Image* newImg, int dstx, int dsty, double srcx, double srcy)
{
    double src1x = floor(srcx);
    double src1y = floor(srcy);  //get a integer coordinate
    if(src1x<0) src1x = 0;
    if(src1x>oriImg->width-1) src1x = oriImg->width-1;
    if(src1y<0) src1y = 0;
    if(src1y>oriImg->height-1) src1y = oriImg->height-1;  //range limitation
    
    int srcax = src1x - 1, srcay = src1y - 1;
    int srcbx = src1x + 1, srcby = src1y - 1;
    int srccx = src1x - 1, srccy = src1y + 1;
    int srcdx = src1x + 1, srcdy = src1y + 1;  //define 4 corner coordinate
    if(srcax<0) srcax = 0;
    if(srcax>oriImg->width-1) srcax = oriImg->width-1;
    if(srcay<0) srcay = 0;
    if(srcay>oriImg->height-1) srcay = oriImg->height-1;
	if(srcbx<0) srcbx = 0;
    if(srcbx>oriImg->width-1) srcbx = oriImg->width-1;
    if(srcby<0) srcby = 0;
    if(srcby>oriImg->height-1) srcby = oriImg->height-1;
	if(srccx<0) srccx = 0;
    if(srccx>oriImg->width-1) srccx = oriImg->width-1;
    if(srccy<0) srccy = 0;
    if(srccy>oriImg->height-1) srccy = oriImg->height-1;
	if(srcdx<0) srcdx = 0;
    if(srcdx>oriImg->width-1) srcdx = oriImg->width-1;
    if(srcdy<0) srcdy = 0;
    if(srcdy>oriImg->height-1) srcdy = oriImg->height-1;  //range limitation
    
    double ave0 = ((oriImg->imageData[srcay][srcax*3] + oriImg->imageData[srcby][srcbx*3])/2 + (oriImg->imageData[srccy][srccx*3] + oriImg->imageData[srcdy][srcdx*3])/2)/2;
    double ave1 = ((oriImg->imageData[srcay][srcax*3+1] + oriImg->imageData[srcby][srcbx*3+1])/2 + (oriImg->imageData[srccy][srccx*3+1] + oriImg->imageData[srcdy][srcdx*3+1])/2)/2;
    double ave2 = ((oriImg->imageData[srcay][srcax*3+2] + oriImg->imageData[srcby][srcbx*3+2])/2 + (oriImg->imageData[srccy][srccx*3+2] + oriImg->imageData[srcdy][srcdx*3+2])/2)/2;
    ave0 = check(ave0);
    ave1 = check(ave1);
    ave2 = check(ave2);  //do average and check the overflow
    newImg->imageData[dsty][dstx*3] = ave0;
    newImg->imageData[dsty][dstx*3+1] = ave1;
    newImg->imageData[dsty][dstx*3+2] = ave2;
    
    return ;
}

/*
Function: check.
Usage: check if x is in the range of [0, 255].
*/

double check (double x)
{
	if(x>255) x = 255.0;
	if(x<0) x = 0.0;
	
	return x;
}

const char* fileName1 = "testimage.bmp";  //the tested image.
const char* fileName2 = "translationimage.bmp";  //output grayscale image.
const char* fileName3 = "mirrorimage.bmp";  //output binary image without local window.
const char* fileName4 = "scaleimage.bmp"; //output binary image using local window.
const char* fileName5 = "rotatedimage.bmp";  //output dilation image.
const char* fileName6 = "shearimage.bmp";  //output erosion image.

 
int main()  
{  
	Image* img = LoadImage(fileName1);  
    Image* tranImg = Translation (img, -200, -200);
    int flag = SaveImage_color(fileName2, tranImg);
    Image* mirrorImg = Mirror(img, 'x');
    flag = SaveImage_color(fileName3, mirrorImg);
    Image* scaleImg = Scale(img, 2.0, 1.5);
    flag = SaveImage_color(fileName4, scaleImg);
    Image* rotatedImg =  Rotation(img, 45);
    flag = SaveImage_color(fileName5, rotatedImg);
    Image* shearImg = Shear(img, 0.5, 0.0);
    flag = SaveImage_color(fileName6, shearImg);

	return 0;  
} 

