#pragma once
#include <string>
#include <vector>

#ifndef FILEUTILS_HPP
#define FILEUTILS_HPP

class FileUtils
{
public:
	FileUtils();

	static bool LoadBinaryFileToBuffer(const std::string& filepath, std::vector<unsigned char>& out_buffer);
	static bool SaveBufferToBinaryFile(const std::string& filepath, std::vector<unsigned char>& in_buffer);

	static bool LoadTextFileToBuffer(const std::string& filepath, std::string& out_buffer);

	bool createDirectoryA(std::string directory_name);

	static std::vector< std::string > EnumerateFilesInDirectory(const std::string& relativeDirectoryPath, const std::string& filePattern);
};

#endif FILEUTILS_HPP
