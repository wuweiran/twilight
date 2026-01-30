#include "handlers.h"
#include <fstream>
#include <filesystem>
#include <wil/com.h>

void FileOpenAsyncHandler(const std::string& requestId, const Json::Value& data) {
	// Pass requestId to the main thread for STA dialog
	std::string* id = new std::string(requestId); // allocated for WndProc
	PostMessage(GetActiveWindow(), WM_APP_FILE_OPEN, 0, reinterpret_cast<LPARAM>(id));
}

Json::Value FileGetTextContentHandler(const Json::Value& requestData) {
	Json::Value response;

	std::string path = requestData["path"].asString();

	if (path.empty()) {
		response["code"] = (int)E_FAIL;
		response["info"] = "Path is empty";
		return response;
	}

	std::ifstream file(std::filesystem::path(reinterpret_cast<const char8_t*>(path.c_str())),
		std::ios::in | std::ios::binary);
	if (!file) {
		response["code"] = (int)E_FAIL;
		response["info"] = "Failed to open file";
		return response;
	}

	std::ostringstream ss;
	ss << file.rdbuf();

	response["code"] = (int)S_OK;
	Json::Value data;
	data["mode"] = "text";
	data["encoding"] = "utf-8";
	data["content"] = ss.str();
	response["data"] = data;

	return response;
}

Json::Value FileWriteToPathHandler(const Json::Value& requestData)
{
	Json::Value result;

	std::string path = requestData.get("path", "").asString();
	std::string content = requestData.get("content", "").asString();

	if (path.empty()) {
		result["code"] = (int)E_INVALIDARG;
		result["info"] = "No path specified";
		return result;
	}

	std::ofstream ofs(path);
	if (!ofs) {
		result["code"] = (int)E_FAIL;
		result["info"] = "Failed to open file for writing";
		return result;
	}

	ofs.write(content.c_str(), content.size());
	ofs.close();

	result["code"] = (int)S_OK;
	result["data"] = path;
	return result;
}