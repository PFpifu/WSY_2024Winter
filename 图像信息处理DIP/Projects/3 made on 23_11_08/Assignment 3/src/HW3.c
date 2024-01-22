#include "bmp.h"
#include <math.h>
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
Function: SaveImage_gray.
Usage: output a gray image to the specific path.
*/

int SaveImage_gray(const char* path, Image* bmpImg)  
{  
    FILE *fpbmp;  
    unsigned short fileType;  
    BiFileHeader bmpfileheader;  
    BiInfoHeader bmpinfoheader;  
    int step;  
    int offset;  
    unsigned char pixVal = '\0';
    int i, j, k, a, b;  
    RGBQuad* quad;  
  
    fpbmp = fopen(path, "wb");  
    if (!fpbmp)  
    {  
        printf("ERROR\n");
        return 0;  
    }  //the error handler.
  
    fileType = 0x4D42;  //"BM"
    fwrite(&fileType, sizeof(unsigned short), 1, fpbmp);  
  
    step = bmpImg->width;  
    offset = bmpImg->width%4;  

    bmpfileheader.bfSize = 54 + 256*4 + step ;  //the total bytes.
    bmpfileheader.bfReserved1 = 0;  
    bmpfileheader.bfReserved2 = 0;  
    bmpfileheader.bfOffBits = 54 + 256*4;  
    fwrite(&bmpfileheader, sizeof(BiFileHeader), 1, fpbmp);  
  
    bmpinfoheader.biSize = 40;  
    bmpinfoheader.biWidth = bmpImg->width;  
    bmpinfoheader.biHeight = bmpImg->height;  
    bmpinfoheader.biPlanes = 1;  
    bmpinfoheader.biBitCount = 8;  
    bmpinfoheader.biCompression = 0;  
    bmpinfoheader.biSizeImage = bmpImg->height*step;  
    bmpinfoheader.biXPelsPerMeter = 0;  
    bmpinfoheader.biYPelsPerMeter = 0;  
    bmpinfoheader.biClrUsed = 256;  //the gray intensity [0,255].
    bmpinfoheader.biClrImportant = 256;  
    fwrite(&bmpinfoheader, sizeof(BiInfoHeader), 1, fpbmp);  


    quad = (RGBQuad*)malloc(sizeof(RGBQuad)*256);  
    for (i=0; i<256; i++)  
    {  
        quad[i].rgbBlue = i;  
        quad[i].rgbGreen = i;  
        quad[i].rgbRed = i;  
        quad[i].rgbReserved = 0;  
    }  //define all gray colors.
    fwrite(quad, sizeof(RGBQuad), 256, fpbmp);  
    free(quad);  
    
    for(i=bmpImg->height-1; i>-1; i--)
    {
    	for(j=0; j<bmpImg->width; j++)
    	{
    		fwrite(&bmpImg->imageData[i][j], sizeof(unsigned char), 1, fpbmp);
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
Function: Log_Enhance
Usage: use the log enhancement formula to change the luminance of a color image and enhance the total image.
*/

Image* Log_Enhance (Image* bmpImg)
{
	int i, j;
	double lw, ld, lmax, temp;
	unsigned char ** gray;
	double ** gray_cal;
	double pb, pg, pr, pmax = 0, py, pu, pv;
	gray = (unsigned char**)malloc(sizeof(unsigned char*)*bmpImg->height);
    for(i=0;i<bmpImg->height;i++) gray[i] = (unsigned char*)malloc(sizeof(unsigned char)*bmpImg->width);  //store the luminance of pixels.
    gray_cal = (double**)malloc(sizeof(double*)*bmpImg->height);
    for(i=0;i<bmpImg->height;i++) gray_cal[i] = (double*)malloc(sizeof(double)*bmpImg->width);  //store the calculated luminance.
    for(i=0; i<bmpImg->height; i++)
    {
    	for(j=0; j<bmpImg->width; j++)
    	{
    		pb = bmpImg->imageData[i][j*3];
			pg = bmpImg->imageData[i][j*3+1];
			pr = bmpImg->imageData[i][j*3+2];
			gray[i][j] = 0.299*pr + 0.587*pg + 0.114*pb;
			if(gray[i][j]<0) gray[i][j] = 0;
			if(gray[i][j]>255) gray[i][j] = 255;  //calculate the grayscale of image.
    	}
    }
    
    for(i=0; i<bmpImg->height; i++)
    {
    	for(j=0; j<bmpImg->width; j++)
    	{
    		if(gray[i][j]>pmax) pmax = gray[i][j];  //find the max luminance of image.
    	}
    }
    
    lmax = (double)(pmax / 255.0);
    
    for(i=0; i<bmpImg->height; i++)
    {
    	for(j=0; j<bmpImg->width; j++)
    	{
    		lw = (double)(gray[i][j] / 255.0);
    		gray_cal[i][j] = (double)(log(lw+1.0) / log(lmax+1.0));  //the main formula.
    		gray[i][j] = (unsigned char)(gray_cal[i][j] * 255);  //change the grayscale.
    		pb = bmpImg->imageData[i][j*3];
			pg = bmpImg->imageData[i][j*3+1];
			pr = bmpImg->imageData[i][j*3+2];
			pu = -0.147*pr - 0.289*pg + 0.435*pb;
			pv = 0.615*pr - 0.515*pg - 0.100*pb;
			bmpImg->imageData[i][j*3] = gray[i][j] + 2.036*pu;
			if(bmpImg->imageData[i][j*3]<0) bmpImg->imageData[i][j*3] = 0;
			if(bmpImg->imageData[i][j*3]>255) bmpImg->imageData[i][j*3] = 255;
			bmpImg->imageData[i][j*3+1] = gray[i][j] - 0.3954*pu - 0.5805*pv;
			if(bmpImg->imageData[i][j*3+1]<0) bmpImg->imageData[i][j*3+1] = 0;
			if(bmpImg->imageData[i][j*3+1]>255) bmpImg->imageData[i][j*3+1] = 255;
			bmpImg->imageData[i][j*3+2] = gray[i][j] + 1.14*pv;
			if(bmpImg->imageData[i][j*3+2]<0) bmpImg->imageData[i][j*3+2] = 0;
			if(bmpImg->imageData[i][j*3+2]>255) bmpImg->imageData[i][j*3+2] = 255;  //change back to the color image.
    	}
    }
    return bmpImg;
}

/*
Function: Color_to_Gray.  
Usage:if the input BMP image is a colored image, this fuction can change it into grayscale image.
*/

Image* Color_to_Gray (Image* bmpImg)
{
	Image* grayImg;
	grayImg = (Image*)malloc(sizeof(Image));
	grayImg->channels = 1;
	grayImg->width = bmpImg->width;
	grayImg->height = bmpImg->height;  //initialize needed variables.
	
	double pb, pg, pr;
	int i, j, k;
	grayImg->imageData = (unsigned char**)malloc(sizeof(unsigned char*)*grayImg->height);
	for(i=0;i<grayImg->height;i++) grayImg->imageData[i] = (unsigned char*)malloc(sizeof(unsigned char)*grayImg->width);  //create new image data array.
	
	for(i=0; i<bmpImg->height; i++)
	{
		for(j=0; j<bmpImg->width; j++)
		{
			pb = bmpImg->imageData[i][j*3];
			pg = bmpImg->imageData[i][j*3+1];
			pr = bmpImg->imageData[i][j*3+2];
			grayImg->imageData[i][j] = 0.299*pr + 0.587*pg + 0.114*pb;  //data of every pixel in grayscale image is the luminance of colored image.
		}
	}
	
	return grayImg;
}

/*
Function: gray_Histogram.
Usage: calculate the histogram of image and return the array stored histogram.
*/

double* gray_Histogram (Image* bmpImg) //only for grayscale images!
{
	int count[256] = {0};
	double result[256] = {0};
	int i, j, k;
	for(i=0; i<bmpImg->height; i++)
	{
		for(j=0; j<bmpImg->width; j++)
		{
			for(k=0; k<256; k++)
			{
				if(bmpImg->imageData[i][j] == k)
				{
					count[k]++;  //count the number of gray pixels.
					break;  
				}
			}
		}
	}
	for(i=0; i<256; i++)
	{
		result[i] = (double) count[i] / (bmpImg->height*bmpImg->width) * 1.0;  //change to ratios.
	}
	
	double* result1 = result;  //define another pointer.
	return result1;
}

/*
Function: Histogram_Eq.
Usage: do histogram equation of the specific image.
*/

Image* Histogram_Eq (Image* bmpImg)
{
	int i, j;
	double* pk = gray_Histogram (bmpImg);  //get the histogram.
	double sk[256] = {0};
	unsigned char eq[256];
	sk[0] = pk[0];
	for(i=1; i<256; i++) sk[i] = sk[i-1] + pk[i];  //calculate the changing function.
	for(i=0; i<256; i++) eq[i] = round(sk[i] / (1.0/255.0));  //use the round function to get the new grayscale.
	for(i=0; i<bmpImg->height; i++)
	{
		for(j=0; j<bmpImg->width; j++)
		{
			bmpImg->imageData[i][j] = eq[bmpImg->imageData[i][j]];  //change the grayscale of image.
		}
	}
	
	return bmpImg;
}

/*
Function: Gray_to_Color.  
Usage:if the input BMP image is a grayscale image, this fuction can change it into color image.
*/

Image* Gray_to_Color (Image* bmpImg, Image* grayImg)
{
	Image* colorImg;
	colorImg = (Image*)malloc(sizeof(Image));
	colorImg->channels = 3;
	colorImg->width = bmpImg->width;
	colorImg->height = bmpImg->height;  //initialize needed variables.
	
	double pb, pg, pr, pu, pv, py;
	int i, j, k;
	colorImg->imageData = (unsigned char**)malloc(sizeof(unsigned char*)*colorImg->height);
	for(i=0;i<grayImg->height;i++) colorImg->imageData[i] = (unsigned char*)malloc(sizeof(unsigned char)*colorImg->width*3);  //create new image data array.
	
	for(i=0; i<bmpImg->height; i++)
	{
		for(j=0; j<bmpImg->width; j++)
		{
			pb = bmpImg->imageData[i][j*3];
			pg = bmpImg->imageData[i][j*3+1];
			pr = bmpImg->imageData[i][j*3+2];
			pu = -0.147*pr - 0.289*pg + 0.435*pb;
			pv = 0.615*pr - 0.515*pg - 0.100*pb;
            py = grayImg->imageData[i][j];
            pb = py + 2.036*pu;
            pb = check (pb);
            colorImg->imageData[i][j*3] = pb;
            pg = py - 0.3954*pu - 0.5805*pv;
			pg = check (pg);
			colorImg->imageData[i][j*3+1] = pg;
			pr = py + 1.14*pv;
			pr = check (pr);
            colorImg->imageData[i][j*3+2] = pr;
		}
	}
	
	return colorImg;
}

/*
Function: check.
Usage: check if x is set in [0, 255].
*/

double check (double x)
{
	if(x>255) x = 255.0;
	if(x<0) x = 0.0;
	
	return x;
}


const char* fileName1 = "testimage.bmp";  //the tested image.
const char* fileName2 = "Enhanceimage.bmp";  //output grayscale image.
const char* fileName3 = "Grayimage.bmp";  //output binary image without local window.
const char* fileName4 = "GrayEqimage.bmp"; //output binary image using local window.
const char* fileName5 = "ColorEqimage.bmp";  //output dilation image.
const char* fileName6 = "originimage.bmp";  //output erosion image.

 
int main()  
{  
	Image* img = LoadImage(fileName1);  
    img = Log_Enhance(img);
    int flag = SaveImage_color(fileName2, img);
    Image* grayImg = Color_to_Gray (img);
    //Image* originImg = Gray_to_Color (img, grayImg);
    //flag = SaveImage_color (fileName6, originImg);
    flag = SaveImage_gray(fileName3, grayImg);
    grayImg = Histogram_Eq (grayImg);
    flag = SaveImage_gray (fileName4, grayImg);
    Image* EqImg = Gray_to_Color (img, grayImg);
    flag = SaveImage_color (fileName5, EqImg);

	return 0;  
} 
