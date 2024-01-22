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
Main functions start at here
*/

/*
Function: WS
Usage: calculate the core of destination.
*/

double WS (int i, int j, int k, int l, double s_div)
{
	double index = (pow(i-k, 2)+pow(j-l, 2)) / (2*pow(s_div, 2)) * (-1.0);
	double res = exp(index);
	return res;
}

/*
Function: WR
Usage: calculate the core of pixel.
*/

double WR (int pix_src, int pix_near, double r_div)
{
	double index = pow((pix_src - pix_near), 2) / (2*pow(r_div, 2)) * (-1.0);
	double res = exp(index);
	return res;
}

/*
Function: Bilateral_Filter
Usage: use the bilateral filter formula to calculate the bilateral-filtered picture
*/

Image* Bilateral_Filter (Image* bmpImg, int N, double r_div)
{
	int i, j, k, m, n;
	Image* newImg;
	newImg = (Image*)malloc(sizeof(Image));
	newImg->channels = 3;
	newImg->height = bmpImg->height;
	newImg->width = bmpImg->width;
	newImg->imageData = (unsigned char**)malloc(sizeof(unsigned char*) * newImg->height);
	for(i=0; i<newImg->height; i++) newImg->imageData[i] = (unsigned char*)malloc(sizeof(unsigned char) * newImg->width * 3);
	
	double s_div = 0.02 * sqrt(pow(newImg->height, 2) + pow(newImg->width, 2));  //the standard deviation of destination
	
	for(i=0; i<bmpImg->height; i++)
	{
		for(j=0; j<bmpImg->width; j++)
		{
			double sum_W = 0, sum_r = 0, sum_g = 0, sum_b = 0;
			int pix_src = bmpImg->imageData[i][j*3] + bmpImg->imageData[i][j*3+1] + bmpImg->imageData[i][j*3+2];
			for(m=mymax(i-N, 0); m<=mymin(i+N, bmpImg->height-1); m++)
			{
				for(n=mymax(j-N, 0); n<=mymin(j+N, bmpImg->width-1); n++)
				{
					int pix_near = bmpImg->imageData[m][n*3] + bmpImg->imageData[m][n*3+1] + bmpImg->imageData[m][n*3+2];
					double W = WS(i, j, m, n, s_div) * WR(pix_src, pix_near, r_div);  //calculate the weight
					sum_W += W;
					sum_b += bmpImg->imageData[m][n*3] * W;
					sum_g += bmpImg->imageData[m][n*3+1] * W;
					sum_r += bmpImg->imageData[m][n*3+2] * W;
				}
			}
			newImg->imageData[i][j*3] = sum_b / sum_W;
			newImg->imageData[i][j*3+1] = sum_g / sum_W;
			newImg->imageData[i][j*3+2] = sum_r / sum_W;  //do the convolution
		}
	}
	
	return newImg;
}

int mymax(int x, int y)
{
	return (x>y) ? x : y;
}

int mymin (int x ,int y)
{
	return (x<y) ? x : y;
}

/*
Main functions end at here
*/

const char* fileName1 = "testimage.bmp";  //the tested image.
const char* fileName2 = "filterimage.bmp";  //output grayscale image.
const char* fileName3 = "Laplaceimage.bmp";  //output binary image without local window.
const char* fileName4 = "sumimage.bmp"; //output binary image using local window.

 
int main()  
{  
	Image* img = LoadImage(fileName1);  
    Image* bilateralImg = Bilateral_Filter(img, 20, 32);
    int flag = SaveImage_color(fileName2, bilateralImg);

	return 0;  
} 

