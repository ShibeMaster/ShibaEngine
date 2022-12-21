#pragma once
#include <string>
class FileExtensions {
public:
	static bool OpenFileDialog(const std::string& initialDirectory, const std::string& fileType, std::string* outPath);
	static void CreateAndWriteToFile(const std::string& filePath, const std::string& text);
};