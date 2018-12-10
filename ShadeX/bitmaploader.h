#ifndef SE_BITMAPLOADER_H
#define SE_BITMAPLOADER_H

#include "globals.h"

static SE_INLINE bool LoadBitmapFromFile(const char *filename, UINT &width, UINT &height, BYTE **bitmapImage)
{
	FILE *filePtr;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;

	INT error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	INT count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	width = (UINT)bitmapInfoHeader.biWidth;
	height = (UINT)bitmapInfoHeader.biHeight;

	// Calculate the size of the bitmap image data.
	INT imageSize = width * height * 3;

	// Allocate memory for the bitmap image data.
	*bitmapImage = new BYTE[imageSize];
	if (!*bitmapImage)
	{
		return false;
	}

	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
	count = fread(*bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	return true;
}

#endif // !SE_BITMAPLOADER_H
