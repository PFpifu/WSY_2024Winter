#include "bmp.h"  //the needed header file.
  
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
            bmpImg->imageData = (unsigned char*)malloc(sizeof(unsigned char)*width*3*height);  
            step = channels*width;  
  
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
                        bmpImg->imageData[(height-1-i)*step+j*3+k] = pixVal;  //read the image data to the array imageData[].
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
    bmpImg->YUVflag = 0;  //set the change value to 0.
    return bmpImg;  
}  
  
int SaveImage(const char* path, Image* bmpImg)  
{  
    FILE *fpbmp;  
    unsigned short fileType;  
    BiFileHeader bmpfileheader;  
    BiInfoHeader bmpinfoheader;  
    int step;  
    int offset;  
    unsigned char pixVal = '\0';
	unsigned char pb = '\0', pg = '\0', pr = '\0', py = '\0';
	char pu = '\0', pv = '\0';  //RGB and YUV temp variables.
    int i, j;  
    RGBQuad* quad;  
  
    fpbmp = fopen(path, "wb");  
    if (!fpbmp)  
    {  
        printf("ERROR\n");
        return 0;  
    }  //the error handler.
  
    fileType = 0x4D42;  //"BM"
    fwrite(&fileType, sizeof(unsigned short), 1, fpbmp);  
  
    if (bmpImg->channels == 3)  //24bytes, which means the color image.
    {  
        step = bmpImg->channels*bmpImg->width;  
        offset = step%4;  
        if (offset != 4)  
        {  
            step += 4-offset;  
        }  //complement handler.
  
        bmpfileheader.bfSize = bmpImg->height*step + 54;  
        bmpfileheader.bfReserved1 = 0;  
        bmpfileheader.bfReserved2 = 0;  
        bmpfileheader.bfOffBits = 54;  //load contents.
        fwrite(&bmpfileheader, sizeof(BiFileHeader), 1, fpbmp);  
  
        bmpinfoheader.biSize = 40;  //total bytes.
        bmpinfoheader.biWidth = bmpImg->width;  
        bmpinfoheader.biHeight = bmpImg->height;  
        bmpinfoheader.biPlanes = 1;  
        bmpinfoheader.biBitCount = 24;  
        bmpinfoheader.biCompression = 0;  
        bmpinfoheader.biSizeImage = bmpImg->height*step;  
        bmpinfoheader.biXPelsPerMeter = 0;  
        bmpinfoheader.biYPelsPerMeter = 0;  
        bmpinfoheader.biClrUsed = 0;  
        bmpinfoheader.biClrImportant = 0;  
        fwrite(&bmpinfoheader, sizeof(BiInfoHeader), 1, fpbmp);  
  
        if(bmpImg->YUVflag = 0)  //the common output.
        {
        	for (i=bmpImg->height-1; i>-1; i--)  
            {  
                 for (j=0; j<bmpImg->width; j++)  
                 {  
                      pixVal = bmpImg->imageData[i*bmpImg->width*3+j*3];  
                      fwrite(&pixVal, sizeof(unsigned char), 1, fpbmp);  
                      pixVal = bmpImg->imageData[i*bmpImg->width*3+j*3+1];  
                      fwrite(&pixVal, sizeof(unsigned char), 1, fpbmp);  
                      pixVal = bmpImg->imageData[i*bmpImg->width*3+j*3+2];  
                      fwrite(&pixVal, sizeof(unsigned char), 1, fpbmp);  //sequentially output the pixel values to the image data.
                }  
                if (offset!=0)  
                {  
                    for (j=0; j<4-offset; j++)  
                    {  
                        pixVal = 0;  
                        fwrite(&pixVal, sizeof(unsigned char), 1, fpbmp);  
                    }  
                }  
            }  
        }else if(bmpImg->YUVflag = 1)  //the RGB->YUV and YUV->RGB changer.
        {
        	for (i=bmpImg->height-1; i>-1; i--)  
            {  
                 for (j=0; j<bmpImg->width; j++)  
                 {  
                      pb = bmpImg->imageData[i*bmpImg->width*3+j*3];  
                      pg = bmpImg->imageData[i*bmpImg->width*3+j*3+1];    
                      pr = bmpImg->imageData[i*bmpImg->width*3+j*3+2]; 
					   
                      py = 0.299*pr + 0.5869*pg + 0.114*pb;
                      if(py<0) py = 0;
                      if(py>255) py = 255;
					  pu = -0.147*pr - 0.289*pg + 0.435*pb ;
					  pv = 0.615*pr - 0.515*pg - 0.100*pb ;
					   
					  py /= 2;  //my unique luminance value Y changing formula, you can change whatever you want.
					  
					  pixVal = py + 2.036*pu;
					  fwrite(&pixVal, sizeof(unsigned char), 1, fpbmp);
					  pixVal = py - 0.3954*pu - 0.5805*pv;
					  fwrite(&pixVal, sizeof(unsigned char), 1, fpbmp);
					  pixVal = py + 1.14*pv;
					  fwrite(&pixVal, sizeof(unsigned char), 1, fpbmp);
                 }  
                if (offset!=0)  
                {  
                    for (j=0; j<4-offset; j++)  
                    {  
                         pixVal = 0;  
                         fwrite(&pixVal, sizeof(unsigned char), 1, fpbmp);  
                    }  
                }  
            }  
        }
    }  
    else if (bmpImg->channels == 1)  //8bytes ,the gray image.
    {  
        step = bmpImg->width;  
        offset = step%4;  
        if (offset != 4)  
        {  
            step += 4-offset;  
        }  
  
        bmpfileheader.bfSize = 54 + 256*4 + step;  //the total bytes.
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
  
        for (i=bmpImg->height-1; i>-1; i--)  
        {  
            for (j=0; j<bmpImg->width; j++)  
            {  

				pb = bmpImg->imageData[i*bmpImg->width*3+j*3];
				pg = bmpImg->imageData[i*bmpImg->width*3+j*3+1];
				pr = bmpImg->imageData[i*bmpImg->width*3+j*3+2];
				pixVal = 0.299*pr + 0.587*pg + 0.114*pb;  //compute the Y value to be the gray value of the new image.
                fwrite(&pixVal, sizeof(unsigned char), 1, fpbmp);  //3 in 1, 24bytes to 8 bytes.
            }  
            if (offset!=0)  
            {  
                for (j=0; j<4-offset; j++)  
                {  
                    pixVal = 0;  
                    fwrite(&pixVal, sizeof(unsigned char), 1, fpbmp);  
                }  
            }  
        }  
    }  
    fclose(fpbmp);  
  
    return 1;  
}

const char* fileName1 = "testimage.bmp";  //the tested image.
const char* fileName2 = "outimage1.bmp";  //the gray image.
const char* fileName3 = "outimage2.bmp";  //the Y-changed image. 
 
int main()  
{  
	Image* img = LoadImage(fileName1);  
	img->channels = 1;  //output the gray image.
	int flag = SaveImage(fileName2, img); 
	//img->YUVflag = 1;
	img->channels = 3;  //output the Y-changed image.
	flag = SaveImage(fileName3, img); 

	return 0;  
} 
