#pragma once
#include <json/json.h>
#include <windows.h>

constexpr auto WM_APP_FILE_OPEN = WM_APP + 1;

Json::Value HelloHandler(const Json::Value& requestData);
void FileOpenAsyncHandler(const std::string& requestId, const Json::Value& requestData);
Json::Value FileGetTextContentHandler(const Json::Value& requestData);
Json::Value FileWriteToPathHandler(const Json::Value& requestData);
