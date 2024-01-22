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

Image* filtering (Image* bmpImg, int size)  //size > 0 && size % 2 == 1
{
	int i ,j, h, w;
	Image* newImg = (Image*)malloc(sizeof(Image));
	newImg->channels = 3;
	newImg->height = bmpImg->height;
	newImg->width = bmpImg->width;
	newImg->imageData = (unsigned char**)malloc(sizeof(unsigned char*)*newImg->height);
	for(i=0; i<newImg->height; i++) newImg->imageData[i] = (unsigned char*)malloc(sizeof(unsigned char)*newImg->width*3);
	
	
	int half = size / 2;

	int sumr, sumg, sumb, count;
	
	for(i=0; i<newImg->height; i++)
	{
		for(j=0; j<newImg->width; j++)
		{
			sumr = sumg = sumb = count = 0;
			for(h=i-half; h<=i+half; h++)
			{
				for(w=j-half; w<=j+half; w++)
				{
					if(h>=0 && h<newImg->height && w>=0 && w<newImg->width)
					{
						count++;
						sumb += bmpImg->imageData[h][w*3];
						sumg += bmpImg->imageData[h][w*3+1];
						sumr += bmpImg->imageData[h][w*3+2];
					}
				}
			}
			newImg->imageData[i][j*3] = round(sumb/(1.0*count));
			newImg->imageData[i][j*3+1] = round(sumg/(1.0*count));
			newImg->imageData[i][j*3+2] = round(sumr/(1.0*count));
		}
	}
	 return newImg;
}


Image* Laplace (Image* bmpImg)  //the size of window is usually 3
{
	int i ,j, h, w;
	Image* newImg = (Image*)malloc(sizeof(Image));
	newImg->channels = 3;
	newImg->height = bmpImg->height;
	newImg->width = bmpImg->width;
	newImg->imageData = (unsigned char**)malloc(sizeof(unsigned char*)*newImg->height);
	for(i=0; i<newImg->height; i++) newImg->imageData[i] = (unsigned char*)malloc(sizeof(unsigned char)*newImg->width*3);
	
	int half = 1, size = 3;  //size = 3
	
	int sumr, sumg, sumb, count;
	
	for(i=0; i<newImg->height; i++)
	{
		for(j=0; j<newImg->width; j++)
		{
			sumr = sumg = sumb = count = 0;
			for(h=i-half; h<=i+half; h++)
			{
				for(w=j-half; w<=j+half; w++)
				{
					if(h>=0 && h<newImg->height && w>=0 && w<newImg->width)
					{
						count++;
						sumb += bmpImg->imageData[h][w*3];
						sumg += bmpImg->imageData[h][w*3+1];
						sumr += bmpImg->imageData[h][w*3+2];
					}
				}
			}
			newImg->imageData[i][j*3] = 0.5*check(1.0*count*bmpImg->imageData[i][j*3] - sumb);
			newImg->imageData[i][j*3+1] = 0.5*check(1.0*count*bmpImg->imageData[i][j*3+1] - sumg);
			newImg->imageData[i][j*3+2] = 0.5*check(1.0*count*bmpImg->imageData[i][j*3+2] - sumr);
		}
	}
	
	return newImg;
}

Image* AddImage (Image* oldImg, Image* newImg)
{
	int i ,j, k, h, w;
	Image* sumImg = (Image*)malloc(sizeof(Image));
	sumImg->channels = 3;
	sumImg->height = oldImg->height;
	sumImg->width = oldImg->width;
	sumImg->imageData = (unsigned char**)malloc(sizeof(unsigned char*)*sumImg->height);
	for(i=0; i<sumImg->height; i++) sumImg->imageData[i] = (unsigned char*)malloc(sizeof(unsigned char)*sumImg->width*3);
	
	for(i=0; i<sumImg->height; i++)
	{
		for(j=0; j<sumImg->width; j++)
		{
			for(k=0; k<3; k++)
				sumImg->imageData[i][j*3+k] = check(oldImg->imageData[i][j*3+k] + newImg->imageData[i][j*3+k]);
		}
	}
	
	return sumImg;
}



double Max (double x, double y)
{
	if(x>y) return x;
	else return y;
}

double Min (double x, double y)
{
	if(x<y) return x;
	else return y;
} 

double check (double x)
{
	if(x>255.0) x = 255.0;
	else if(x<0) x = 0;
	
	return x;
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
    Image* filterImg = filtering (img, 15);
    int flag = SaveImage_color(fileName2, filterImg);
    Image* LaImg = Laplace (img);
    flag = SaveImage_color(fileName3, LaImg);
    Image* sumImg = AddImage(img, LaImg);
    flag = SaveImage_color(fileName4, sumImg);

	return 0;  
} 

