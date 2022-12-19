#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <string.h>
#include <Windows.h>
class FileExtensions {
public:
	static bool OpenFileDialog(const std::string& initialDirectory, const std::string& fileType, std::string* outPath) {
		OPENFILENAME settings = { 0 };
		TCHAR fileBuffer[260] = { 0 };
		auto dir = std::wstring(initialDirectory.begin(), initialDirectory.end());
		HWND hwnd = NULL;
		settings.lStructSize = sizeof(settings);
		settings.hwndOwner = hwnd;
		settings.lpstrFile = fileBuffer;
		settings.nMaxFile = sizeof(fileBuffer);
		settings.lpstrFilter = NULL;
		settings.nFilterIndex = 0;
		settings.lpstrFileTitle = NULL;
		settings.nMaxFileTitle = 0;
		settings.lpstrInitialDir = dir.c_str();
		settings.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		if (GetOpenFileName(&settings) == TRUE) {
			std::wstring lpstr(settings.lpstrFile);
			std::string str(lpstr.begin(), lpstr.end());
			*outPath = str;
			std::filesystem::path path(str);
			return path.extension().string() == fileType;
		}
		else {
			return false;
		}
		
	}
};