#include "FileExtensions.h"
#include <fstream>
#include <filesystem>
#include <Windows.h>
#include <ShlObj.h>
#include <atlbase.h>
bool FileExtensions::OpenFileDialog(const std::string& initialDirectory, const std::string& fileType, std::string* outPath) {
	CoInitializeEx(nullptr, COINIT::COINIT_MULTITHREADED);
	IFileOpenDialog* fileDialog;
	auto result = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&fileDialog));
	if (SUCCEEDED(result)) {
		DWORD options;
		fileDialog->SetOptions(FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST);
		fileDialog->AddRef();
		result = fileDialog->Show(NULL);
		if (SUCCEEDED(result)) {
			IShellItem* item;
			result = fileDialog->GetResult(&item);
			if (SUCCEEDED(result)) {
				PWSTR pwstrFilePath;
				result = item->GetDisplayName(SIGDN_FILESYSPATH, &pwstrFilePath);
				if (SUCCEEDED(result)) {

					std::wstring lpstr(pwstrFilePath);
					std::string str(lpstr.begin(), lpstr.end());
					std::filesystem::path path(str);

					if (path.extension().string() == fileType) {

						*outPath = str;
						CoTaskMemFree(pwstrFilePath);
						item->Release();
						fileDialog->Release();
						CoUninitialize();
						return true;
					}
					CoTaskMemFree(pwstrFilePath);
					item->Release();
					fileDialog->Release();
					CoUninitialize();
				}
				item->Release();
			}
		}
		fileDialog->Release();
	}

	CoUninitialize();
	return false;
}
bool FileExtensions::SaveFileAsDialog(const std::string& initialDir, const std::string& filetype, std::string* outpath) {
	auto result = CoInitializeEx(nullptr, COINIT::COINIT_MULTITHREADED);
	if (SUCCEEDED(result)) {
		IFileSaveDialog* dialog;
		result = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&dialog));
		if (SUCCEEDED(result)) {
			std::wstring str = std::wstring(filetype.begin(), filetype.end());
			LPCWSTR extension = str.c_str();
			const COMDLG_FILTERSPEC rgspec[] = { extension, extension };
			dialog->SetFileTypes(ARRAYSIZE(rgspec), rgspec);
			if (SUCCEEDED(dialog)) {
				result = dialog->Show(NULL);
				if (SUCCEEDED(result)) {
					IShellItem* item;
					result = dialog->GetResult(&item);
					if (SUCCEEDED(result)) {
						PWSTR path;
						result = item->GetDisplayName(SIGDN_FILESYSPATH, &path);
						if (SUCCEEDED(result)) {
							std::wstring lpstr(path);
							std::string str(lpstr.begin(), lpstr.end());
							*outpath = str;
							CoTaskMemFree(path);
							item->Release();
							dialog->Release();
							CoUninitialize();
							return true;
						}
					}
					item->Release();
				}
			}
		}
		CoUninitialize();
	}
	return false;
}
bool FileExtensions::OpenFolderDialog(const std::string& initialDir, std::string* outPath) {
	CoInitializeEx(nullptr, COINIT::COINIT_MULTITHREADED);
	IFileOpenDialog* folderDialog;
	auto result = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&folderDialog));
	if (SUCCEEDED(result)) {
		DWORD options;
		folderDialog->SetOptions(FOS_PICKFOLDERS | FOS_PATHMUSTEXIST);
		folderDialog->AddRef();
		result = folderDialog->Show(NULL);
		if (SUCCEEDED(result)) {
			IShellItem* item;
			result = folderDialog->GetResult(&item);
			if (SUCCEEDED(result)) {
				PWSTR pwstrFilePath;
				result = item->GetDisplayName(SIGDN_FILESYSPATH, &pwstrFilePath);
				if (SUCCEEDED(result)) {

					std::wstring lpstr(pwstrFilePath);
					std::string str(lpstr.begin(), lpstr.end());
					*outPath = str;
					CoTaskMemFree(pwstrFilePath);
					item->Release();
					folderDialog->Release();
					CoUninitialize();
					return true;
				}
				item->Release();
			}
		}
		folderDialog->Release();
	}

	CoUninitialize();
	return false;

}
void FileExtensions::CreateAndWriteToFile(const std::string& filePath, const std::string& text) {
	std::ofstream file(filePath, std::ofstream::out | std::ofstream::trunc);
	file << text;
	file.close();
}