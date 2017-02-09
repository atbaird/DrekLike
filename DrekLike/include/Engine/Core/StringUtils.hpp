#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int& maxLength, const char* format, ... );

//-----------------------------------------------------------------------------------------------
const std::vector<std::string> ParseString(const std::string& str, const char& turnToSpaces = ' ');
const std::string ReplaceCharInString(const std::string& original, const char& toReplace, const char& replaceBy);
const std::string ReplaceStringWithString(const std::string& original, const std::string& toReplace, const std::string& replaceBy);

//------------------------------------------------------------------------------------------------
const std::string MakeLower(const std::string& str);

//------------------------------------------------------------------------------------------------
const std::string ToString(const int& intVal);
const bool SimpleStrCmp(const std::string& str1, const std::string& str2);

