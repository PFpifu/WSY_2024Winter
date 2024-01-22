#include "bmp_binary.h"  //the needed header file.
#include <math.h>

/*
Function: LoadImage.  
Usage:load main content of input BMP image to a specific image pointer
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
	
	unsigned char pb = '\0', pg = '\0', pr = '\0';
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
Function: SaveImage.  
Usage: output the binary image to the specific path.
*/

int SaveImage(const char* path, Image* bmpImg)  
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
Function: Otsu.  
Usage: Otsu algorithm is a algorithm used to change a grayscale BMP image into a bianry image.This algorithm output a threhold,and every pixel 
smaller than threshold will be set to black, every pixel bigger than threshold will be set to white.We assume w0 = the probability of foreground 
pixel, w1 = the probability of background pixel, u0 = the average grayscale of foreground pixel, u1 = the average of background pixel,then 
variance can be calculated by w0*w1*(u0-u1)^2.The threshold which has the biggest variance will be output.
*/

unsigned char Otsu (Image* bmpImg)
{
	int i, j, k, forecount, backcount;
	unsigned char grmax, grmin, threshold;
	unsigned char pb = '\0', pg = '\0', pr = '\0';
	int picount = bmpImg->height * bmpImg->width;
    unsigned char **gray;
    gray = (unsigned char**)malloc(sizeof(unsigned char*)*bmpImg->height);
    for(i=0;i<bmpImg->height;i++) gray[i] = (unsigned char*)malloc(sizeof(unsigned char)*bmpImg->width);
	int sumofgray, sumoffore, sumofback;
	double avegray, avefore, aveback;
	double forep, backp, maximum, tmpmax;  //initialize needed variables.
	
	for (i=bmpImg->height-1; i>-1; i--)
	{
		for (j=0; j<bmpImg->width; j++)
		{
			gray[i][j] = bmpImg->imageData[i][j];
		}
	}  //store the image data in another array.
	
	grmax = 0;
	grmin = 255;
	maximum = 0;
	
	for (i=bmpImg->height-1; i>-1; i--)
	{
		for(j=0; j<bmpImg->width; j++)
		{
			if(gray[i][j]>grmax) grmax = gray[i][j];
			if(gray[i][j]<grmin) grmin = gray[i][j];
		}
	}  //find the max and min grayscale.
	
	for(k=grmin; k<=grmax; k++)
	{
		forecount = backcount = 0;
	    sumofgray = sumoffore = sumofback = 0;
		for(i=0; i<bmpImg->height; i++)
		{
			for(j=0; j<bmpImg->width; j++)
			{
				if(gray[i][j]<k){
					forecount++;
					sumoffore += gray[i][j];
				}else{
					backcount++;
					sumofback += gray[i][j];
				}
				
				sumofgray += gray[i][j];
			}
		}  // calculate total grayscale of foreground and background.
		
		forep = (double)forecount/picount;  //  forep = w0.
		backp = (double)backcount/picount;  //  backp = w1.
		
		if(picount == 0) avegray = 0;
		else avegray = (double)sumofgray / picount;
		
		if(forecount == 0) avefore = 0;
		else avefore = (double)sumoffore / forecount;  // avefore = u0.
		
		if(backcount == 0) aveback = 0;
		else aveback = (double)sumofback / backcount;  //aveback = u1.
		
		tmpmax = (double)forep*pow((avefore-avegray), 2)+backp*pow((aveback-avegray), 2);  //tmpmax = variance of every threshold.
		
		if(tmpmax>=maximum){
			maximum = tmpmax;
			threshold = k;
		}  //do iteration to find needed threhold.
	}
	
	return threshold;
}

/*
Function: local_Otsu.
Usage: This function set a local window smaller than total image, and calculate every threshold in the local window. And we silde the 
local window to get a better binary image.
*/

void local_Otsu (Image* bmpImg, int n)  //n cannot be too big!
{
	Image* localImg; 
	int i, j;
	int w, h;
	localImg = (Image*)malloc(sizeof(Image));
	localImg->channels = 1;
	localImg->height = bmpImg->height / n;
	localImg->width = bmpImg->width / n;  //set the local window.
	localImg->imageData = (unsigned char**)malloc(sizeof(unsigned char*)*localImg->height);
	for(i=0; i<localImg->height; i++) localImg->imageData[i] = (unsigned char*)malloc(sizeof(unsigned char)*localImg->width);
	
	for(h=0; h<n; h++)
	{
		for(w=0; w<n; w++)
		{
			for(i=0; i<localImg->height; i++)
			{
				for(j=0; j<localImg->width; j++)
				{
					localImg->imageData[i][j] = bmpImg->imageData[h*localImg->height + i][w*localImg->width + j];  //copy data.
				}
			}
			Image* outImg = Gray_to_Binary (localImg);  //set threshold to every local window.
			for(i=0; i<localImg->height; i++)
			{
				for(j=0; j<localImg->width; j++)
				{
					bmpImg->imageData[h*localImg->height + i][w*localImg->width + j] = outImg->imageData[i][j];
				}
			}
		}
	}
}

/*
Function: Gray_to_Binary.
Usage: Change grayscale image to binary image using threshold given by Otsu algorithm.
*/

Image* Gray_to_Binary (Image* bmpImg)
{
	if(bmpImg->channels != 1) return NULL;
	
	int i, j;
	unsigned char threshold = Otsu(bmpImg);  //threshold is calculated by Otsu.
	for(i=0;i<bmpImg->height;i++)
	{
		for(j=0;j<bmpImg->width;j++)
		{
			if(bmpImg->imageData[i][j]<threshold) bmpImg->imageData[i][j] = 0;
			else bmpImg->imageData[i][j] = 255;
		}
	}
	
	return bmpImg;
}

/*
Function: Mask.
Usage: Mask do dilation or erosion operations with a binary image. The operation is determined by ope. If ope = 1,we will do dilation. If ope = 0,
we will do erosion. The structure element is a square whose side length is 2*n+1. Note:the square cannot be too big! 
*/

Image* Mask (Image* bmpImg, int n, int ope) 
{
    if(bmpImg->channels != 1) return NULL;
    int i, j, k;
	
    unsigned char** mask;
    mask = (unsigned char**)malloc(sizeof(unsigned char*)*(2*n+1));
    for(i=0;i<2*n+1;i++) mask[i] = (unsigned char*)malloc(sizeof(unsigned char)*(2*n+1));  
    
    for(i=0;i<2*n+1;i++)
    {
    	for(j=0;j<2*n+1;j++) mask[i][j] = '\0';
    }  //set structure element mask to all black.
	
	unsigned char** maskData;
	unsigned char** outData;

	maskData = (unsigned char**)malloc(sizeof(unsigned char*)*bmpImg->height);
	for(i=0;i<bmpImg->height;i++) maskData[i] = (unsigned char*)malloc(sizeof(unsigned char)*bmpImg->width);
	outData = (unsigned char**)malloc(sizeof(unsigned char*)*bmpImg->height);
	for(i=0;i<bmpImg->height;i++) outData[i] = (unsigned char*)malloc(sizeof(unsigned char)*bmpImg->width);
	
	for(i=0;i<bmpImg->height;i++)
	{
		for(j=0;j<bmpImg->width;j++)
		{
			maskData[i][j] = bmpImg->imageData[i][j];  //copy data.
			if(ope == 0)
			{
				if(maskData[i][j] == 0) maskData[i][j] = 255;
				else maskData[i][j] = 0;
			}  //if we do erosion, we negate total image and do dilation, then we do negation again to get erosion.
		}
	}
	
	int a, b, flag = 0;
	for(i=0;i<bmpImg->height;i++)
	{
		for(j=0;j<bmpImg->width;j++)
		{
			flag = 0;
			k=n;
			while(k>=0)
			{
				for(a=0;a<=k;a++)
				{
					if(!(maskData[Max(i-k, 0)][Max(j-k+a, 0)] || mask[n-k+a][n-k])) flag = 1;
					if(!(maskData[Max(i-k+a, 0)][Max(j-k, 0)] || mask[n-k][n-k+a])) flag = 1;
					if(!(maskData[Max(i-k, 0)][Min(j+k-a, bmpImg->width-1)] || mask[n+k-a][n-k])) flag = 1;
					if(!(maskData[Min(i+k-a,bmpImg->height-1)][Max(j-k, 0)] || mask[n+k][n-k+a])) flag = 1;
					if(!(maskData[Min(i+k-a, bmpImg->height-1)][Min(j+k, bmpImg->width-1)] || mask[n+k-a][n+k])) flag = 1;
					if(!(maskData[Min(i+k, bmpImg->height-1)][Min(j+k-a, bmpImg->width-1)] || mask[n+k][n+k-a])) flag = 1;
					if(!(maskData[Min(i+k, bmpImg->height-1)][Max(j-k+a, 0)] || mask[n-k+a][n+k])) flag = 1;
					if(!(maskData[Max(i-k+a, 0)][Min(j+k, bmpImg->width-1)] || mask[n-k][n+k-a])) flag =1;
				}  //check whether mask intersects the black pixel.
				if(flag == 1) break;
				k--;
			}
			
			if(flag == 1) outData[i][j] = 0;  //if they intersect, the midst point of mask should be set to black.
			else outData[i][j] = 255;
		}
	}
	
	for(i=0;i<bmpImg->height;i++)
	{
		for(j=0;j<bmpImg->width;j++)
		{
			if(ope == 0)
			{
				if(outData[i][j] == 0) outData[i][j] = 255;
				else outData[i][j] = 0;
			}  //if erosion, negate the image to get needed image.
		}
	} 
	
	Image* maskImg = (Image*)malloc(sizeof(Image));
	maskImg->channels = 1;
	maskImg->height = bmpImg->height;
	maskImg->width = bmpImg->width;
	maskImg->imageData = (unsigned char**)malloc(sizeof(unsigned char*)*maskImg->height);
	for(i=0;i<maskImg->height;i++) maskImg->imageData[i] = (unsigned char*)malloc(sizeof(unsigned char)*maskImg->width);
	for(i=0;i<maskImg->height;i++)
	{
		for(j=0;j<maskImg->width;j++) maskImg->imageData[i][j] = outData[i][j];
	}  //set another Image pointer and output.
		
	return maskImg;
}

/*
Function: Closing.
Usage: Closing uses function Mask to do dilation firstly, do erosion secondly. 
*/

Image* Closing (Image* bmpImg, int n)
{
	bmpImg = Mask(bmpImg, n, 1);
	bmpImg = Mask(bmpImg, n, 0);
	return bmpImg;
}

/*
Function: Opening.
Usage: Opening uses function Mask to do erosion firstly, do dilation secondly.
*/

Image* Opening (Image* bmpImg, int n)
{
	bmpImg = Mask(bmpImg, n, 0);
	bmpImg = Mask(bmpImg, n, 1);
	return bmpImg;
}

/*
Function: Max.
Usage: Max return the maximum of a and b.
*/

int Max (int a, int b)
{
	if(a>=b) return a;
	else return b;
}

/*
Function: Min.
Usage: Min return the minimum of a and b.
*/

int Min (int a, int b)
{
	if(a<=b) return a;
	else return b;
}

const char* fileName1 = "testimage.bmp";  //the tested image.
const char* fileName2 = "Grayimage.bmp";  //output grayscale image.
const char* fileName3 = "Binaryimage.bmp";  //output binary image without local window.
const char* fileName4 = "LocalWindowimage.bmp"; //output binary image using local window.
const char* fileName5 = "Dilation.bmp";  //output dilation image.
const char* fileName6 = "Erosion.bmp";  //output erosion image.
const char* fileName7 = "Opening.bmp";  //output opening image.
const char* fileName8 = "Closing.bmp";  //output closing image.
 
int main()  
{  
	Image* img = LoadImage(fileName1);  
    img = Color_to_Gray(img);
    int flag = SaveImage(fileName2, img);
    Image* BiImg = Gray_to_Binary(img);
    //flag = SaveImage(fileName3, BiImg);
    local_Otsu(img, 4);
    BiImg = img;
    flag = SaveImage(fileName3, BiImg);
    Image* Eimg = Mask(BiImg, 1, 0); //erosion.
    Image* Dimg = Mask(BiImg, 1, 1); //dilation.
    flag = SaveImage(fileName5, Dimg);
    flag = SaveImage(fileName6, Eimg);
    Image* Oimg = Opening(BiImg, 1);
    Image* Cimg = Closing(BiImg, 1);
    flag = SaveImage(fileName7, Oimg);
    flag = SaveImage(fileName8, Cimg);


	return 0;  
} 
