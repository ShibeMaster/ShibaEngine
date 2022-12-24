#pragma once
#include <string>
class FileExtensions {
public:
	static bool OpenFileDialog(const std::string& initialDirectory, const std::string& fileType, std::string* outPath);
	static bool OpenFolderDialog(const std::string& initialDir, std::string* outPath);
	static bool SaveFileAsDialog(const std::string& startingDirectory, const std::string& fileType, std::string* outPath);
	static void CreateAndWriteToFile(const std::string& filePath, const std::string& text);
};