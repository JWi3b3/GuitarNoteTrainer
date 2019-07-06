#include "SheetMusicPage.hpp"

#include <iostream>

SheetMusicPage::SheetMusicPage()
{
	srand(time(NULL));

	_elemNames = {
		"violinKey",
		"beat",
		"e1", "e1L", "e1N", "e1H",
		"f1", "f1L", "f1N", "f1H",
		"g1", "g1L", "g1N", "g1H",
		"a1", "a1L", "a1N", "a1H",
		"b1", "b1L", "b1N", "b1H",
		"c1", "c1L", "c1N", "c1H",
		"d1", "d1L", "d1N", "d1H",

		"e2", "e2L", "e2N", "e2H",
		"f2", "f2L", "f2N", "f2H",
		"g2", "g2L", "g2N", "g2H",
		"a2", "a2L", "a2N", "a2H",
		"b2", "b2L", "b2N", "b2H",
		"c2", "c2L", "c2N", "c2H",
		"d2", "d2L", "d2N", "d2H",

		"e3", "e3L", "e3N", "e3H",
		"f3", "f3L", "f3N", "f3H",
		"g3", "g3L", "g3N", "g3H",
		"a3", "a3L", "a3N", "a3H",
		"b3", "b3L", "b3N", "b3H",
		"c3", "c3L", "c3N", "c3H",
		"d3", "d3L", "d3N", "d3H",

		"e4", "e4L", "e4N", "e4H",
		"f4", "f4L", "f4N", "f4H",
		"g4", "g4L", "g4N", "g4H",
		"a4", "a4L", "a4N", "a4H",
		"b4", "b4L", "b4N", "b4H",
		"c4", "c4L", "c4N", "c4H",
		"d4", "d4L", "d4N", "d4H",

		"emptyLine"
	};

	_loadMusicElements();
}


SheetMusicPage::~SheetMusicPage()
{
}


void SheetMusicPage::_loadMusicElements()
{
	std::string pathToImg = "imgs/AllNotes.png";

	cv::Mat rawImg = cv::imread(pathToImg, cv::IMREAD_GRAYSCALE);
	if (rawImg.empty())
	{
		std::cout << "ERROR in SheetMusicPage::_loadMusicElements: Image " << pathToImg << " could not be found!" << std::endl;
		return;
	}

	auto elementPositions = _findElementsInRawImage(rawImg);
	
	_rImgHeight = rawImg.rows;

	int currentName = 0;

	for (auto const & elem : elementPositions)
	{
		_musicElements[_elemNames[currentName]] =
			rawImg(cv::Rect(elem[0], 0, elem[1], _rImgHeight)).clone();

		currentName++;
	}

	_musicElements["emptyLine"] =
		rawImg(cv::Rect(
			elementPositions[1][0] + elementPositions[1][1],
			0,
			elementPositions[2][0] - (elementPositions[1][0] + elementPositions[1][1]),
			_rImgHeight)).clone();
}

void SheetMusicPage::_drawMusicLine(cv::Mat& page, int height)
{
	int nextPosition = 0;
	_drawElement(page, "violinKey", _leftBorder, height, nextPosition);
	_drawEmptyLine(page, nextPosition, height, 50, nextPosition);

	std::string firstNote = _elemNames[_lowestNote];
	std::string lastNote = _elemNames[_highestNote];

	for (int i = 0; i < _notesPerLine; i++)
	{
		_drawElement(
			page,
			_getRandomNote(firstNote, lastNote, _useSharp, _useFlat, _useNatural),
			nextPosition,
			height,
			nextPosition,
			_noteOrTabs,
			!_noteOrTabs);
		_drawEmptyLine(page, nextPosition, height, _spaceBetweenNotes, nextPosition);
	}

	_drawEmptyLine(
		page,
		nextPosition,
		height,
		_pageSize.width - nextPosition - _leftBorder,
		nextPosition);
}

std::string SheetMusicPage::_getRandomNote(
	std::string startNote,
	std::string endNote,
	bool useSharp,
	bool useFlat,
	bool useNatural)
{
	int startIndex = -1;
	int endIndex = -1;

	for (int i = 0; i < _elemNames.size(); i++)
	{
		if (_elemNames[i] == startNote)
			startIndex = i;

		if (_elemNames[i] == endNote)
			endIndex = i;
	}

	if (startIndex == -1 || endIndex == -1)
	{
		std::cout << "ERROR in _drawRandomNote: startNote or Endnote has not been found!" << std::endl;
		return "";
	}


	for (int i = 0; i < 1000; i++)
	{
		int randomNumber = rand() % static_cast<int>(endIndex - startIndex + 1);
		int randomNote = startIndex + randomNumber;
		
		bool noteIsOk = true;

		if (!useFlat && _elemNames[randomNote].find("L") != std::string::npos)
		{
			noteIsOk = false;
		}

		if (!useNatural && _elemNames[randomNote].find("N") != std::string::npos)
		{
			noteIsOk = false;
		}
		
		if (!useSharp && _elemNames[randomNote].find("H") != std::string::npos)
		{
			noteIsOk = false;
		}

		if (noteIsOk)
		{
			return _elemNames[randomNote];
		}
	}

	return "";
}


bool SheetMusicPage::_drawElement(
	cv::Mat& page,
	std::string element,
	int posX,
	int posY,
	int& nextPosition,
	bool notesEmpty,
	bool tabsEmpty)
{
	// element not found
	if (_musicElements.find(element) == _musicElements.end())
	{
		std::cout << "ERROR in _drawElement(): music element with name " << element << " not found!" << std::endl;
		return false;
	}
	
	cv::Mat elemImg = _musicElements[element].clone();

	return _drawElement(page, elemImg, posX, posY, nextPosition, notesEmpty, tabsEmpty);
}

bool SheetMusicPage::_drawElement(
	cv::Mat& page,
	cv::Mat& elemImg,
	int posX,
	int posY,
	int& nextPosition,
	bool notesEmpty,
	bool tabsEmpty)
{
	// position completely outside of page
	if (posX + elemImg.cols < 0 ||
		posX > page.cols - 1 ||
		posY + elemImg.rows < 0 ||
		posY > page.rows - 1)
	{
		std::cout << "ERROR in _drawElement(): element is outside of page!" << std::endl;
		return false;
	}

	int width = elemImg.cols;
	int height = elemImg.rows;

	// crop if element is partly outside of page
	if (posX + width > page.cols)
	{
		width = page.cols - posX;
	}
	if (posY + height > page.rows)
	{
		height = page.rows - posY;
	}

	if (posX < 0) posX = 0;
	if (posY < 0) posY = 0;

	cv::Mat pageRoi = cv::Mat(page, cv::Rect(posX, posY, width, height));
	elemImg.copyTo(pageRoi);

	pageRoi = 255 - pageRoi;

	if (notesEmpty)
	{
		_drawEmptyLine(page, posX, posY, width, nextPosition, true, false);
	}
	if (tabsEmpty)
	{
		_drawEmptyLine(page, posX, posY, width, nextPosition, false, true);
	}

	nextPosition = posX + width;

	return true;
}

void SheetMusicPage::_drawEmptyLine(
	cv::Mat& page,
	int posX,
	int posY,
	int length,
	int& nextPosition,
	bool drawTop,
	bool drawBottom)
{
	if (length < 1)
		length = 1;

	cv::Mat newImg;
	cv::resize(
		_musicElements["emptyLine"],
		newImg,
		cv::Size(length, _musicElements["emptyLine"].rows));

	cv::Mat emptyLineImg;

	if (drawTop && !drawBottom)
	{
		emptyLineImg = newImg(
			cv::Rect(
				0,
				0,
				newImg.cols,
				_rImgVerticalSeparation));
		_drawElement(page, emptyLineImg, posX, posY, nextPosition);
	}
	else if (!drawTop && drawBottom)
	{
		emptyLineImg = newImg(
			cv::Rect(
				0,
				_rImgVerticalSeparation,
				newImg.cols,
				newImg.rows - _rImgVerticalSeparation));

		_drawElement(page, emptyLineImg, posX, posY + _rImgVerticalSeparation, nextPosition);
	}
	else
	{
		emptyLineImg = newImg;
		_drawElement(page, emptyLineImg, posX, posY, nextPosition);
	}

}

std::vector<cv::Vec2i> SheetMusicPage::_findElementsInRawImage(cv::Mat& rawImg)
{
	std::vector<cv::Vec2i> elements;
	cv::Mat sumImg;

	cv::Mat copyImg = rawImg.clone();
	copyImg = 255 - copyImg;

	cv::reduce(copyImg, sumImg, 0, cv::REDUCE_SUM, CV_32S);

	int threshold = 3690;

	int32_t* ptrSumImg = sumImg.ptr<int32_t>();

	bool hightState = false;
	int elemSize = 0;
	bool change = false;
	for (int i = 0; i < sumImg.total(); i++)
	{
		change = false;

		if (ptrSumImg[i] > threshold)
		{
			if (!hightState)
				change = true;

			hightState = true;
			elemSize++;
		}
		else
		{
			if (hightState)
				change = true;

			hightState = false;
		}

		// element begins
		if (change && hightState)
		{
			elements.push_back(cv::Vec2i(i, 0));
		}

		// element ends
		else if ((change && !hightState) || (i == sumImg.total() - 1 && hightState))
		{
			elements.back()[1] = elemSize;
			elemSize = 0;
		}
	}

	// close small gaps
	int minGapSize = 10;

	while (true)
	{
		int startElementsSize = elements.size();
		for (int i = 0; i < elements.size() - 1; i++)
		{
			// small gap found
			if (elements[i + 1][0] - (elements[i][0] + elements[i][1]) < minGapSize)
			{
				elements[i][1] = elements[i + 1][0] + elements[i + 1][1] - elements[i][0];
				elements[i + 1] = cv::Vec2i();
			}
		}

		elements.erase(std::remove(elements.begin(), elements.end(), cv::Vec2i()), elements.end());

		if (elements.size() == startElementsSize)
			break;
	}
	return elements;
}



cv::Mat SheetMusicPage::createPage(
	int lowestNote,
	int highestNote,
	bool noteOrTabs,
	bool useSharp,
	bool useFlat,
	bool useNatural,
	int lineCount,
	int notesPerLine,
	int spaceBetweenNotes)
{
	cv::Mat page = cv::Mat::zeros(_pageSize, CV_8UC1);

	_lowestNote = lowestNote;
	_highestNote = highestNote;
	_noteOrTabs = noteOrTabs;
	_useSharp = useSharp;
	_useFlat = useFlat;
	_useNatural = useNatural;
	_lineCount = lineCount;
	_notesPerLine = notesPerLine;
	_spaceBetweenNotes = spaceBetweenNotes;

	if (_lowestNote < 2) _lowestNote = 2;
	else if (_lowestNote > _elemNames.size() - 2) _lowestNote = _elemNames.size() - 2;

	if (_highestNote < 2) _highestNote = 2;
	else if (_highestNote > _elemNames.size() - 2) _highestNote = _elemNames.size() - 2;

	if (_lowestNote > _highestNote - 1) _lowestNote = _highestNote - 1;

	if (_lineCount < 1) _lineCount = 1;
	if (_lineCount > 20) _lineCount = 20;

	if (_notesPerLine < 1) _notesPerLine = 1;
	if (_notesPerLine > 30) _notesPerLine = 30;

	if (_spaceBetweenNotes < 1) _spaceBetweenNotes = 1;

	for (int i = 0; i < _lineCount; i++)
	{
		_drawMusicLine(page, _topBorder + _systemDist * i);
	}

	page = 255 - page;

	cv::putText(
		page,
		"Note Exercise",
		_titlePosition,
		cv::FONT_HERSHEY_DUPLEX,
		3, 0, 2, cv::LINE_AA);

	return page;
}