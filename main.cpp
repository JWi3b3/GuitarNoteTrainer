#include "SheetMusicPage.hpp"

#include "opencv2/opencv.hpp"

#include <iostream>
#include <chrono>
#include <thread>
#include <ctime> 

bool update = true;

std::string toStringLeadingZeros(const int value, const unsigned precision)
{
	std::ostringstream oss;
	oss << std::setw(precision) << std::setfill('0') << value;
	return oss.str();
}

void onChange(int sliderValue, void* userData)
{
	update = true;
}


void main()
{
	std::cout << "Random Sheet Music Generator" << std::endl;
	
	int scaleFactor = 30;
	int noteEmpty = 0;
	int lowestNote = 0;
	int highestNote = 87;
	int useSharp = 0;
	int useFlat = 0;
	int useNatural = 0;
	int lineCount = 4;
	int notesPerLine = 10;
	int spaceBetweenNotes = 125;
	int saveToImage = 0;

	cv::namedWindow("Slider");
	cv::imshow("Slider", cv::Mat::zeros(100, 400, CV_8UC1));
	cv::createTrackbar("scale", "Slider", &scaleFactor, 200, onChange);
	cv::createTrackbar("NoteOrTab", "Slider", &noteEmpty, 1, onChange);
	cv::createTrackbar("highestNote", "Slider", &highestNote, 100, onChange);
	cv::createTrackbar("lowestNote", "Slider", &lowestNote, 100, onChange);
	cv::createTrackbar("useSharp", "Slider", &useSharp, 1, onChange);
	cv::createTrackbar("useFlat", "Slider", &useFlat, 1, onChange);
	cv::createTrackbar("useNatural", "Slider", &useNatural, 1, onChange);

	cv::createTrackbar("lineCount", "Slider", &lineCount, 20, onChange);
	cv::createTrackbar("notesPerLine", "Slider", &notesPerLine, 30, onChange);
	cv::createTrackbar("spaceBetweenNotes", "Slider", &spaceBetweenNotes, 500, onChange);

	cv::createTrackbar("saveToImage", "Slider", &saveToImage, 1, onChange);
	SheetMusicPage pageCreator;
	cv::waitKey(1);

	while (true)
	{
		if (!update)
		{
			if (cv::waitKey(20) == 27)
				break;

			continue;
		}
		else
			update = false;

		cv::Mat page = pageCreator.createPage(
			lowestNote,
			highestNote,
			noteEmpty,
			useSharp,
			useFlat,
			useNatural,
			lineCount,
			notesPerLine,
			spaceBetweenNotes);


		cv::Mat dispImg;
		double scaleDouble = 1. * scaleFactor / 100.;
		cv::resize(page, dispImg, cv::Size(), scaleDouble, scaleDouble);
		cv::imshow("page", dispImg);

		if (saveToImage == 1)
		{
			cv::setTrackbarPos("saveToImage", "Slider", 0);

			std::time_t t = std::time(0);   // get time now
			std::tm* now = std::localtime(&t);

			std::string fileName =
				toStringLeadingZeros(now->tm_year + 1900, 4) + "-" +
				toStringLeadingZeros(now->tm_mon + 1, 2) + "-" +
				toStringLeadingZeros(now->tm_mday, 2) + "-" +
				toStringLeadingZeros(now->tm_hour, 2) + "-" +
				toStringLeadingZeros(now->tm_min, 2) + "-" +
				toStringLeadingZeros(now->tm_sec, 2) + ".png";
			cv::imwrite(fileName, page);
		}

		if (cv::waitKey(1) == 27)
			break;
	}
}