
/* --------------------------------------------------------------------------------------------------
IMAGE RESTORATION IN FREQUENCY DOMAIN USING DISCRETE FOURIER TRANSFORMATION AND INVERSE FILTER
IMAGE RESTORATION IN FREQUENCY DOMAIN USING DISCRETE FOURIER TRANSFORMATION AND PSEUDO INVERSE FILTER
IMAGE RESTORATION IN FREQUENCY DOMAIN USING DISCRETE FOURIER TRANSFORMATION AND WEINER FILTER
IMAGE RESTORATION IN FREQUENCY DOMAIN USING DISCRETE FOURIER TRANSFORMATION AND LSE FILTER
IMAGE RESTORATION IN SPACIAL DOMAIN USING LUCY RICHARDSON FILTER (RUN FOR FIVE ITERATION)
Frequency Domain Image Enhancement Filter.cpp : Defines the entry point for the console application.
....................................................................................................*/

/*..................................... HEADER FILES ...............................................*/
// Include Necessary Header Files 
// [both in Std and Non Std Path]
#include "stdafx.h"
#include<stdio.h>
#include<math.h>
#include<time.h>
#include<opencv\cv.h>
#include<opencv\highgui.h>
#include<opencv2\imgproc\imgproc.hpp>
using namespace std;
using namespace cv;
/*................................... END HEADER FILES ............................................*/

/*.................................... INVERSE FILTER .............................................*/
// Inverse Restoration Filter Construction Function Defination
// Input Parameter: 2D FILTER , CUTOFF FREQUENCY
void RS_Inverse_Filter(Mat &RSFilter, int CUTOFF)
{
	// Temporary Mat Variable to Store Inverse Gaussian Coefficients
	// Calculate Mid Point of Mat Variable [RSFilter]
	Mat Filter = Mat(RSFilter.rows, RSFilter.cols, CV_32F);
	Point Mid = Point(RSFilter.rows / 2, RSFilter.cols / 2);
	float D, H;

	// Compute Point Wise Inverse Gaussian Coefficients and Store in Filter Variable
	for (int i = 0; i < RSFilter.rows; i++)
	{
		for (int j = 0; j < RSFilter.cols; j++)
		{
			D = pow(Mid.x - abs(i - Mid.x), 2.0) + pow(Mid.y - abs(j - Mid.y), 2.0);
			H = (float)exp(1 * D / (2 * pow(CUTOFF, 2)));
			Filter.at<float>(i, j) = H;
		}
	}

	// Construct A Filter of DFTTrans_Image[0] Size from Filter Variable
	Mat tmp[] = { Filter, Filter };
	merge(tmp, 2, RSFilter);
}
/*.................................. END INVERSE FILTER ..........................................*/

/*............................... PSEUDO INVERSE FILTER .........................................*/
// Pseudo Inverse Filter Construction Function Defination
// Input Parameter: 2D FILTER , CUTOFF FREQUENCY
void RS_PI_Filter(Mat &RSFilter, int CUTOFF)
{
	// Temporary Mat Variable to Store Pseudo Inverse Coefficients
	// Calculate Mid Point of Mat Variable [RSFilter]
	Mat Filter = Mat(RSFilter.rows, RSFilter.cols, CV_32F);
	Point Mid = Point(RSFilter.rows / 2, RSFilter.cols / 2);
	float D, H;

	// Compute Point Wise Pseudo Inverse Coefficients and Store in Filter Variable
	for (int i = 0; i < RSFilter.rows; i++)
	{
		for (int j = 0; j < RSFilter.cols; j++)
		{
			D = pow(Mid.x - abs(i - Mid.x), 2.0) + pow(Mid.y - abs(j - Mid.y), 2.0);
			H = (float)exp(-1 * D / (2 * pow(CUTOFF, 2)));
			Filter.at<float>(i, j) = (H > 0.05) ? 1.0 / H : 0.05;
		}
	}

	// Construct A Filter of DFTTrans_Image[0] Size from Filter Variable
	Mat tmp[] = { Filter, Filter };
	merge(tmp, 2, RSFilter);
}
/*............................... END PSEUDO INVERSE FILTER ......................................*/

/*.................................... WEINER FILTER .............................................*/
// Weiner Restoration Filter Construction Function Defination
// Input Parameter: 2D FILTER , CUTOFF FREQUENCY
void RS_Weiner_Filter(Mat &RSFilter, int CUTOFF)
{
	// Temporary Mat Variable to Store Weiner Coefficients
	// Calculate Mid Point of Mat Variable [RSFilter]
	Mat Filter = Mat(RSFilter.rows, RSFilter.cols, CV_32F);
	Point Mid = Point(RSFilter.rows / 2, RSFilter.cols / 2);
	float D, H;

	// Compute Point Wise Weiner Coefficients and Store in Filter Variable
	for (int i = 0; i < RSFilter.rows; i++)
	{
		for (int j = 0; j < RSFilter.cols; j++)
		{
			D = pow(Mid.x - abs(i - Mid.x), 2.0) + pow(Mid.y - abs(j - Mid.y), 2.0);
			H = (float)exp(-1 * D / (2 * pow(CUTOFF, 2)));
			Filter.at<float>(i, j) = H / (H*H + 0.005);
		}
	}

	// Construct A Filter of DFTTrans_Image[0] Size from Filter Variable
	Mat tmp[] = { Filter, Filter };
	merge(tmp, 2, RSFilter);
}
/*.................................. END WIENER FILTER............................................*/

/*.................................... LSE FILTER ................................................*/
// Least Square Error Filter Construction Function Defination
// Input Parameter: 2D FILTER , CUTOFF FREQUENCY
void RS_LSE_Filter(Mat &RSFilter, Mat &Lap, int CUTOFF)
{
	// Temporary Mat Variable to Store LSE Filter Coefficients
	// Calculate Mid Point of Mat Variable [RSFilter]
	Mat Filter = Mat(RSFilter.rows, RSFilter.cols, CV_32F);
	Point Mid = Point(RSFilter.rows / 2, RSFilter.cols / 2);
	int Freq = pow(CUTOFF, 2);
	float D, H;

	// Compute Point Wise Least Square Error Coefficients and Store in Filter Variable
	for (int i = 0; i < RSFilter.rows; i++)
	{
		for (int j = 0; j < RSFilter.cols; j++)
		{
			D = pow(Mid.x - abs(i - Mid.x), 2.0) + pow(Mid.y - abs(j - Mid.y), 2.0);
			H = (float)exp(-1 * D / (2 * pow(CUTOFF, 2)));
			Filter.at<float>(i, j) = H / (H*H + 0.005*pow(Lap.at<float>(i, j), 2));
		}
	}

	// Construct A Filter of DFTTrans_Image[0] Size from Filter Variable
	Mat tmp[] = { Filter, Filter };
	merge(tmp, 2, RSFilter);
}
/*.................................. END LSE FILTER................................................*/

/*............................... LUCY RICHARDSON FILTER ..........................................*/
// Lucy Richardson Filter Construction Function Defination
// Input Parameter: Distorted Image , Temprary Image,  Low Pass Filter, Iterator
Mat RS_LR_Filter(Mat &Image, Mat &Temp, Mat &RSFilter, int itr)
{
	// Convolve of Previous Restored Image with The Low Pass Filterand Divide Distorted Image
	// Convolve the Resulted Value with Low Pass Filter [Flipped] and Multiply with The Previous Image.
	Mat Denom, Numer;
	do {
		filter2D(Temp, Denom, -1, RSFilter, Point(-1, -1), 0, BORDER_DEFAULT);
		Numer = Image / Denom;
		filter2D(Numer, Denom, -1, RSFilter, Point(-1, -1), 0, BORDER_DEFAULT);
		Temp = Temp.mul(Denom);
		itr--;
	} while (itr > 0);
	return Temp;
}
/*.............................. END LUCKY RICHARDSON FILTER ......................................*/

/*..................................... MAIN PROGRAM ..............................................*/
int main(void)
{
	// 'Select' Variables for selection of Filter and Filter 'RSFilter' Variable to Store 2D Filter's Coefficients.
	// CUTOFF Frequency to Retain Amount of Overall Details in An Image
	Mat RSFilter; int Select, CUTOFF, Itr;

	// Initialize Clock Variable to compute Time Taken in millisecs
	clock_t start, end;
	double Time_Used;

	// Create Variables to Store Actual and Its DFT Transfomed Values of [R, G, B] Cheannels.
	//Create Variables to Store Actual and Transformed Values of Laplacian Matrix.
	Mat RGB_Image[3], DFTTrans_Image[3];
	Mat Lap_Image, Trans_Lap_Image;

	//Input The Values for Cutoff Frequency and Selection of Filter.
	printf("Selection of Filter [1]Inverse [2]Pseudo Inverse [3]Weiner [4]LSE [5]Lucy Richardson :\n");
	scanf("%d", &Select);

	// Enter the Cutoff Frequency for Ideal Filter to LSE Filter or No. of Iterations of Lucy Filter.
	if (Select <= 4) {
		printf("Enter The Cutoff Frequency of The Filter: \n");
		scanf("%d", &CUTOFF);
	}
	else {
		printf("Enter The No. of Iteration for Lucy Richardson Filter: \n");
		scanf("%d", &Itr);
	}

	// Read Image Data from Image File
	Mat Image = imread("Blur_Turbulence.bmp", CV_LOAD_IMAGE_COLOR);

	// Check, Whether Program Open Image File
	if (!Image.data)
	{
		printf("COULDN'T OPEN OR READ IMAGE FILE");
		return -1;
	}

	// Display The Distorted Image
	namedWindow("DISTORTED IMAGE", WINDOW_NORMAL);
	imshow("DISTORTED IMAGE", Image);

	// Laplacian sharp filter kernel (3 x 3)
	Mat Lap = (Mat_<float>(3, 3) << -1.0, -1.0, -1.0, -1.0, 8.0, -1.0, -1.0, -1.0, -1.0);

	// Gaussian smoothing filter kernel (5 x 5)
	Mat LPFilter = (Mat_<float>(5, 5) << 0.00391, 0.015625, 0.023437, 0.015625, 0.00391,
		0.015625, 0.0625, 0.09375, 0.0625, 0.015625,
		0.023437, 0.09375, 0.140625, 0.09375, 0.023437,
		0.015625, 0.0625, 0.09375, 0.0625, 0.015625,
		0.00391, 0.015625, 0.023437, 0.015625, 0.00391);

	// Fill laplacian Filter value to empty image
	Lap_Image.create(Image.rows, Image.cols, CV_32F);
	copyMakeBorder(Lap, Lap_Image, 0, Image.rows - 3, 0, Image.cols - 3, BORDER_CONSTANT, 0.0);

	// Discreate Fourier Transformation of Laplacian Matrix and Compute Its absolute Values.
	dft(Lap_Image, Trans_Lap_Image, DFT_COMPLEX_OUTPUT);
	for (int i = 0; i < Image.rows * Image.cols; i++)
		Lap_Image.at<float>(i / Image.cols, i % Image.cols) = abs(Trans_Lap_Image.at<complex<float>>(i / Image.cols, i % Image.cols));

	// Start The Timer
	start = clock();

	// Copy Image.data values to RGB_Image for Extracting All Three Frames [R, G, B] 
	split(Image, RGB_Image);

	// Convert Each Channel Pixel Values from 8U to 32F
	// Discreate Fourier Transformation of Each Channel
	for (int i = 0; i < 3; i++)
	{
		RGB_Image[i].convertTo(RGB_Image[i], CV_32F);
		dft(RGB_Image[i], DFTTrans_Image[i], DFT_COMPLEX_OUTPUT);
	}

	// Make Filter Variable Same Size of DFTTrans_Image.
	RSFilter = DFTTrans_Image[0].clone();

	// Selection of a Restoration Filter
	switch (Select)
	{
	case 1: printf("You Selected Inverse Filter \n");
		RS_Inverse_Filter(RSFilter, CUTOFF);
		break;
	case 2: printf("You Selected Pseudo Inverse Filter \n");
		RS_PI_Filter(RSFilter, CUTOFF);
		break;
	case 3:	printf("You Selected Weiner Filter \n");
		RS_Weiner_Filter(RSFilter, CUTOFF);
		break;
	case 4: printf("You Selected LSE Filter \n");
		RS_LSE_Filter(RSFilter, Lap_Image, CUTOFF);
		break;
	case 5: printf("You Selected Lucy Richardson Filter \n");
		Image.convertTo(Image, CV_32F);
		Image = RS_LR_Filter(Image, Image, LPFilter, Itr);
		Image.convertTo(Image, CV_8U);
		break;
	default: printf("Wrong Selection \n");
		break;
	}

	// Enhance The Image Using Restoration Filter By Applying It on Channels Separately
	// Inverse Discrete Fourier Transformation of Each Channel
	// Convert Each Chanel Pixel Values from 32F to 8U
	if (Select <= 4) {
		for (int i = 0; i < 3; i++)
		{
			Mat chan[2];
			mulSpectrums(DFTTrans_Image[i], RSFilter, DFTTrans_Image[i], 0);
			idft(DFTTrans_Image[i], DFTTrans_Image[i], DFT_SCALE);
			split(DFTTrans_Image[i], chan);
			chan[0].convertTo(RGB_Image[i], CV_8U);
		}

		// Merge All Three Channels to Construct Final Image
		merge(RGB_Image, 3, Image);
	}

	// Stop the Timer
	end = clock();

	// Calculate Time Taken for Image Restoration by the Filter in sec.
	Time_Used = (float)(end - start) / CLOCKS_PER_SEC;
	printf("Time Taken for Image smoothing : %.3f", Time_Used);

	// Display Final Enhanced Image By The FIlter
	namedWindow("ENHANCED IMAGE", WINDOW_NORMAL);
	imshow("ENHANCED IMAGE", Image);

	// Write 2D Array to Image Fil (.bmp)
	imwrite("Tur_In_95.bmp", Image);

	// Release All Mat Variables
	Trans_Lap_Image.release();
	DFTTrans_Image->release();
	RGB_Image->release();
	LPFilter.release();
	Lap.release();
	Image.release();

	waitKey(0);
	return 0;
}
/*..................................... END MAIN PROGRAM ..........................................*/
