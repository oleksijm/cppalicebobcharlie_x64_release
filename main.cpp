#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>




//last update on 13.08.2023 - trying to make it work for unsigned __int64 and the output in the win32 - IS WORKING if CSV has pure integers..
// ..and no decimals.
// rechecked on 09.08.2023 - works with max number of Excel rows. For larger numbers the program needs to be rewritten to
// accommodate larger data types for numbers:
// __int64 or
// unsigned __int64
// file name length limit as of now: 49 characters
// uses deprecation warning turn off for codecvt. 
//in this module I will try to learn to do calculations. Maybe play around with data structures?

//function prototype
std::wstring convert(const std::string& input);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND hMainWindow;
HWND hCanvasWindow;
HINSTANCE hMainInstance;

HWND hLblOutputFirstName;
HWND hLblOutputNumber;
HWND hTxtInput;
HWND hButton;
HWND hButtonDel;

WCHAR s_text_1[]{ L"Random Text.." };
int posX = 0;
int posY = 0;

std::vector<HWND> windows;

#define IDC_TEXTBOX 1000
#define IDC_BUTTON 1001
#define IDC_BUTTON_DEL 1002

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE PrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	MSG msg = { 0 };
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = TEXT("AliceBobCharlie");

	if (!RegisterClass(&wc))
		return 1;

	hMainWindow = CreateWindow(wc.lpszClassName, TEXT("AliceBobCharlie"), WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL | WS_VISIBLE,
		0, 0, 640, 480, 0, 0, hInstance, NULL);

	hMainInstance = wc.hInstance;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}



//************************************************************************FUNCTION DEFINITIONS*********************************************************************

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int offset = 0;
	std::wstring input;

	switch (message)
	{
	case WM_CREATE:
	{
		hMainWindow = hWnd;

		hCanvasWindow = CreateWindowEx(0, L"STATIC", L"bbb", WS_CHILD | WS_VISIBLE, posX, 0, 8000, 4800, hMainWindow,
			NULL, NULL, NULL);

		hTxtInput = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), s_text_1,
			WS_VISIBLE | WS_CHILD | ES_LEFT, 50, 50, 400, 25, hWnd, (HMENU)IDC_TEXTBOX, hMainInstance, NULL);

		hButton = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("Press me!"), WS_VISIBLE | WS_CHILD | WM_COPY | ES_LEFT,
			500, 50, 400, 25, hWnd, (HMENU)IDC_BUTTON, hMainInstance, NULL);

		hButtonDel = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("Delete all!"), WS_VISIBLE | WS_CHILD | WM_COPY | ES_LEFT,
			500, 100, 400, 25, hWnd, (HMENU)IDC_BUTTON_DEL, hMainInstance, NULL);

		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.nPos = 0;
		si.nMax = 800;
		si.nPage = 400;
		si.fMask = SIF_ALL;
		si.nMin = 0;
		SetScrollInfo(hCanvasWindow, SB_HORZ, &si, TRUE);

		//vertical scrolling set up
		SCROLLINFO sj;
		sj.cbSize = sizeof(sj);
		sj.nPos = 0;
		sj.nMax = 1200;
		sj.nPage = 400;
		sj.fMask = SIF_ALL;
		sj.nMin = 0;
		SetScrollInfo(hCanvasWindow, SB_VERT, &sj, TRUE);

		//break;

	}

	case WM_HSCROLL: {
		SCROLLINFO si;
		si.cbSize = sizeof(si);

		GetScrollInfo(hCanvasWindow, SB_HORZ, &si);
		switch (LOWORD(wParam)) {
		case SB_LINELEFT: {
			si.nPos -= 5;
		}break;
		case SB_LINERIGHT: {
			si.nPos += 5;
		}break;
		case SB_THUMBTRACK: {
			si.nPos = HIWORD(wParam);
		}break;
		};

		SetScrollInfo(hCanvasWindow, SB_HORZ, &si, TRUE);
		posX = -si.nPos;
		MoveWindow(
			hCanvasWindow,
			posX,
			posY,
			8000,
			4800,
			TRUE
		);
		break;
	}


	case WM_VSCROLL: {

		SCROLLINFO sj;
		sj.cbSize = sizeof(sj);

		GetScrollInfo(hCanvasWindow, SB_VERT, &sj);
		switch (LOWORD(wParam)) {
		case SB_LINEDOWN: {
			sj.nPos += 5;
		}break;
		case SB_LINEUP: {
			sj.nPos -= 5;
		}break;
		case SB_THUMBTRACK: {
			sj.nPos = HIWORD(wParam);
		}break;
		};

		SetScrollInfo(hCanvasWindow, SB_VERT, &sj, TRUE);
		posY = -sj.nPos;
		MoveWindow(
			hCanvasWindow,
			posX,
			posY,
			8000,
			4800,
			TRUE
		);
		break;
	}

	case WM_COMMAND: {

		if (LOWORD(wParam) == IDC_BUTTON)
		{
			input.resize(GetWindowTextLengthW(hTxtInput));
			GetWindowTextW(hTxtInput, input.data(), input.size() + 1);

			std::ifstream inputFile;
			inputFile.open(input.data());

			std::string line;

			//std::map<std::string, int> data;
			std::map<std::string, unsigned __int64> data;

			while (std::getline(inputFile, line))
			{
				std::stringstream inputString(line);
				std::string firstName;
				std::wstring WfirstName;
				//int Number;
				unsigned __int64 NumberL;
				std::string tempString;
				std::wstring WtempString;

				getline(inputString, firstName, ',');
				getline(inputString, tempString);
				//Number = atoi(tempString.c_str());
				//NumberL = _atoi64(tempString.c_str());
				NumberL = std::strtoull(tempString.c_str(),NULL, 0);

				if (data.count(firstName) == 0) {
					WfirstName = convert(firstName);
					//data[firstName] = Number;
					data[firstName] = NumberL;
				}
				else {
					//data[firstName] += Number;
					data[firstName] += NumberL;
				}




			}

			for (const auto& key : data) {

				std::wstring wkeyfirst = convert(key.first);
				std::string keysecond = std::to_string(key.second);
				std::wstring wkeysecond = convert(keysecond);

				hLblOutputFirstName = CreateWindowEx(WS_EX_STATICEDGE, TEXT("EDIT"), wkeyfirst.c_str(), WS_VISIBLE | WS_CHILD | ES_READONLY |
					ES_LEFT, 50, 200 + offset * 26, 100, 25, hCanvasWindow,
					(HMENU)IDC_TEXTBOX, hMainInstance, NULL);
				hLblOutputNumber = CreateWindowEx(WS_EX_STATICEDGE, TEXT("EDIT"), wkeysecond.c_str(), WS_VISIBLE | WS_CHILD | ES_READONLY |
					ES_LEFT, 250, 200 + offset * 26, 100, 25, hCanvasWindow,
					(HMENU)IDC_TEXTBOX, hMainInstance, NULL);
				++offset;
			}


		}

		else if (LOWORD(wParam) == IDC_BUTTON_DEL)
		{
			;
		}break;

	}default:
		return DefWindowProc(hWnd, message, wParam, lParam);

	}
	return 0;
}

std::wstring convert(const std::string& input)
{
	try
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.from_bytes(input);
	}
	catch (std::range_error& e)
	{
		size_t length = input.length();
		std::wstring result;
		result.reserve(length);
		for (size_t i = 0; i < length; i++)
		{
			result.push_back(input[i] & 0xFF);
		}
		return result;
	}
}
