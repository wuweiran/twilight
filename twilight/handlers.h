#pragma once
#include <json/json.h>
#include <windows.h>

constexpr auto WM_APP_FILE_OPEN = WM_APP + 1;

Json::Value HelloHandler(const Json::Value& data);
void FileOpenHandler(const std::string& requestId, const Json::Value& data);
Json::Value FileGetContentHandler(const Json::Value& data);
Json::Value FileWriteToPathHandler(const Json::Value& data);
