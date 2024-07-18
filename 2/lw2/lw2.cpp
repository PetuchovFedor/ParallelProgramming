#include "../../bitmap_image.hpp"
#include <windows.h>
#include <iostream>
#include <vector>
#include <chrono>
const size_t RADIUS_BLUR = 15;


struct Data
{
	bitmap_image& img;
	size_t leftTopX;
	size_t leftTopY;
	int widthMatrix;
	int heightMatrix;
	Data(bitmap_image& img, std::pair<size_t, size_t> leftTop, std::pair<int, int> size)
		: img(img)
		, leftTopX(leftTop.first)
		, leftTopY(leftTop.second)
		, widthMatrix(size.first)
		, heightMatrix(size.second)
	{}
};

struct Matrix
{
	std::pair<size_t, size_t> leftTop;
	std::pair<size_t, size_t> sizes;
	Matrix(std::pair<size_t, size_t> leftTop, std::pair<size_t, size_t> sizes)
		: leftTop(leftTop)
		, sizes(sizes)
	{}
};

int CalculateSideBlurMatrix(int widthImg, int heightImg, int countMatrix)
{
	double px = ceil(sqrt(countMatrix * widthImg / heightImg));
	double sx, sy;
	if (floor(px * heightImg / widthImg) * px < countMatrix)
		sx = heightImg / ceil(px * heightImg / widthImg);
	else
		sx = widthImg / px;
	double py = ceil(sqrt(countMatrix * heightImg / widthImg));
	if (floor(py * widthImg / heightImg) * py < countMatrix)
		sy = widthImg / ceil(widthImg * py / heightImg);
	else
		sy = heightImg / py;
	return (sx > sy ? sx : sy);
}

void AverageBlur(bitmap_image& img, size_t leftTopX, size_t leftTopY)
{
	int aveRed = 0;
	int aveGr = 0;
	int aveBlue = 0;
	int count = 0;
	for (size_t i = leftTopY; i < (leftTopY + RADIUS_BLUR); i++)
	{
		for (size_t j = leftTopX; j < (leftTopX + RADIUS_BLUR); j++)
		{
			if (i < img.height() && j < img.width())
			{
				aveRed += img.get_pixel(j, i).red;
				aveGr += img.get_pixel(j, i).green;
				aveBlue += img.get_pixel(j, i).blue;
				count++;
				
			}
		}
	}
	if ((leftTopY + RADIUS_BLUR / 2) < img.height() && (leftTopX + RADIUS_BLUR / 2) < img.width())
	{
		rgb_t color{};
		color.red = aveRed / count;
		color.blue = aveBlue / count;
		color.green = aveGr / count;
		img.set_pixel(leftTopX + RADIUS_BLUR / 2, leftTopY + RADIUS_BLUR / 2, color);
	}
}


void BlurMatrix(Data& data)
{
	for (size_t i = data.leftTopY; i < (data.leftTopY + data.heightMatrix); i++)
	{
		for (size_t j = data.leftTopX; j < (data.leftTopX + data.widthMatrix); j++)
		{
			if (j < data.img.width() && i < data.img.height())
			{
				AverageBlur(data.img, j, i);
			}
		}
	}
}

std::vector<Matrix> CreateArrayOfAllMatrix(bitmap_image& img, 
	std::pair<int, int> sides, int countMatrix)
{
	std::vector<Matrix> result;
	size_t x = 0;
	size_t y = 0;
	for (size_t i = 0; i < (countMatrix - sqrt(countMatrix)); i++)
	{
		result.push_back(Matrix({x, y}, {sides.first, sides.first}));
		x += sides.first;
		if (x >= img.width()) 
		{ 
			x = 0;
			y += sides.second;
		}
	}
	size_t tmpIdx = result.size() - 1;
	if ((result[tmpIdx].leftTop.first + result[tmpIdx].sizes.first) != img.width())
	{
		if ((result[tmpIdx].leftTop.first + result[tmpIdx].sizes.first) != 0)
		{
			result[tmpIdx].sizes.first = img.width() - result[tmpIdx].leftTop.first;
		}
	}
	int newHeight = img.height() - result[tmpIdx].leftTop.second;
	int newWidth = img.width() / sqrt(countMatrix);
	x = 0;
	y = result[tmpIdx].leftTop.second;
	for (size_t i = result.size(); i < countMatrix; i++)
	{
		result.push_back(Matrix({ x, y }, { newWidth, newHeight }));
		x += newWidth;
	}
	return result;
}

void Blur(std::vector<Data>* vector)
{
	for (auto& elem : *vector)
	{
		BlurMatrix(elem);
	}
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	//std::cout << "thread\n";
	std::vector<Data>* dataArr = (std::vector<Data>*)lpParam;
	Blur(dataArr);
	ExitThread(0);
}

int main(int argc, char* argv[])
{
	auto startTime = std::chrono::high_resolution_clock::now();
	if (argc != 5)
	{
		std::cout << "Error with arguments cmd" << std::endl;
		return 1;
	}
	std::string inputFileName(argv[1]);
	//std::cout << inputFileName << std::endl;
	std::string outputFileName(argv[2]);
	bitmap_image image(inputFileName);
	int numThreads = atoi(argv[3]);
	int numCores = atoi(argv[4]);
	HANDLE* handles = new HANDLE[numThreads];
	int side;
	std::pair<int, int> sizes;
	std::vector<Matrix> countMatrix;
	if (numThreads == 1)
	{
		sizes = std::make_pair(image.width(), image.height());
		countMatrix.push_back(Matrix({ 0, 0 }, sizes));
	}
	else
	{
		side = CalculateSideBlurMatrix(image.width(), image.height(), numThreads * numThreads);
		sizes = std::make_pair(side, side);
		countMatrix = CreateArrayOfAllMatrix(image, sizes, numThreads * numThreads);
	}
	int iter = 0;
	std::vector<std::vector<Data>> vectorDataVectors;
	for (size_t i = 0; i < (numThreads); i++)
	{
		std::vector<Data> ve;
		for (size_t j = 0; j < numThreads; j++)
		{
			auto& matrix = countMatrix[j + iter];
			Data data(image, matrix.leftTop, matrix.sizes);

			ve.push_back(data);
		}
		iter += numThreads;
		vectorDataVectors.push_back(ve);
	}
	for (size_t i = 0; i < numThreads; i++)
	{
		auto vePtr = &vectorDataVectors[i];
		handles[i] = CreateThread(NULL, 0, &ThreadProc, (LPVOID)vePtr, CREATE_SUSPENDED, NULL);

		if (handles[i] == NULL) continue;

		DWORD_PTR affinity_mask = (DWORD_PTR)1 << (i % numCores);
		SetThreadAffinityMask(handles[i], affinity_mask);
		SetThreadPriority(handles[i], THREAD_PRIORITY_HIGHEST);
		
	}
	for (int i = 0; i < numThreads; i++)
	{
		if (handles[i] == NULL)
		{
			std::cout << "Error when creating a thread " << i << std::endl;
			return 1;
		}
		ResumeThread(handles[i]);
	}
	WaitForMultipleObjects(numThreads, handles, true, INFINITE);
	image.save_image(outputFileName);
	auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - startTime).count();

	std::cout << "Time: " << duration << " ms" << std::endl;
	return 0;
}