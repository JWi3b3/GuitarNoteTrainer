#pragma once
#include <opencv2/opencv.hpp>
#include <unordered_map>
#include <string>

class SheetMusicPage
{
public:
	SheetMusicPage();
	~SheetMusicPage();

	cv::Mat createPage(
		int lowestNote,
		int highestNote,
		bool noteOrTabs,
		bool useSharp,
		bool useFlat,
		bool useNatural,
		int lineCount,
		int notesPerLine,
		int spaceBetweenNotes);

private:
	// variables
	cv::Size _pageSize = cv::Size(2100, 2970);
	//cv::Size _pageSize = cv::Size(1800, 1000);

	int _lowestNote = 0;
	int _highestNote = 10;
	bool _noteOrTabs = false;
	bool _useSharp = false;
	bool _useFlat = false;
	bool _useNatural = false;

	int _singleLineDist = 14;
	int _leftBorder = 100;
	int _topBorder = 250;
	int _rightBorder = 100;
	int _systemDist = 600;
	int _lineThickness = 1;
	int _helpLineLength = 15;
	int _spaceBetweenNotes = 50;
	int _notesPerLine = 15;
	int _lineCount;

	cv::Point2i _titlePosition = cv::Point2i(_leftBorder, 130);

	// parameters to get the music elements from the raw image
	int _rImgLineStartWidth = 134;
	int _rImgNoteWidth = 138;
	int _rImgHeight = 100;
	int _rImgVerticalSeparation = 320;

	cv::Size _noteSize = cv::Size(9, 6);

	std::unordered_map<std::string, cv::Mat> _musicElements;
	
	// functions
	void _loadMusicElements();
	void _drawMusicLine(cv::Mat& page, int height);

	std::string _getRandomNote(
		std::string startNote,
		std::string endNote,
		bool useSharp,
		bool useFlat,
		bool useNatural);

	bool _drawElement(
		cv::Mat& page,
		std::string element,
		int posX, int posY,
		int& nextPosition,
		bool notesEmpty = false,
		bool tabsEmpty = false);

	bool _drawElement(
		cv::Mat& page,
		cv::Mat& element,
		int posX, int posY,
		int& nextPosition,
		bool notesEmpty = false,
		bool tabsEmpty = false);

	void _drawEmptyLine(
		cv::Mat& page,
		int posX,
		int posY,
		int length,
		int& nextPosition,
		bool drawTop = true,
		bool drawBottom = true);

	std::vector<cv::Vec2i> _findElementsInRawImage(cv::Mat& rawImg);

	std::vector<std::string> _elemNames;

};

