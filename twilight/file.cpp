#include "handlers.h"
#include <fstream>
#include <filesystem>
#include <wil/com.h>

static const char* BASE64_CHARS =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

std::string Base64Encode(const std::vector<unsigned char>& data) {
    std::string out;
    int val = 0, valb = -6;
    for (unsigned char c : data) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(BASE64_CHARS[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back(BASE64_CHARS[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}

void FileOpenHandler(const std::string& requestId, const Json::Value& data) {
    // Pass requestId to the main thread for STA dialog
    std::string* id = new std::string(requestId); // allocated for WndProc
    PostMessage(GetActiveWindow(), WM_APP_FILE_OPEN, 0, reinterpret_cast<LPARAM>(id));
}

Json::Value FileGetContentHandler(const Json::Value& data) {
    Json::Value response;

    std::string path = data["path"].asString();
    std::string mode = data.get("mode", "text").asString();

    if (path.empty()) {
        response["code"] = (int)E_FAIL;
        response["info"] = "Path is empty";
        return response;
    }


    if (mode == "text") {
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
    }
    else if (mode == "binary") {
        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file) {
            response["info"] = "Failed to open file";
            return response;
        }

        std::vector<unsigned char> buffer(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
        );

        response["code"] = (int)S_OK;
        Json::Value data;
        response["mode"] = "binary";
        response["content"] = Base64Encode(buffer);
        response["data"] = data;
    }
    else {
        response["code"] = (int)E_FAIL;
        response["info"] = "Unknown mode";
    }

    return response;
}

Json::Value FileWriteToPathHandler(const Json::Value& data)
{
    Json::Value result;

    // Extract target path and content
    std::string path = data.get("path", "").asString();
    std::string content = data.get("content", "").asString();

    if (path.empty()) {
        result["code"] = (int)E_INVALIDARG;
        result["info"] = "No path specified";
        return result;
    }

    // Write string to file
    std::ofstream ofs(path);
    if (!ofs) {
        result["code"] = (int)E_FAIL;
        result["info"] = "Failed to open file for writing";
        return result;
    }

    ofs.write(content.c_str(), content.size());
    ofs.close();

    result["code"] = (int)S_OK;
    result["data"] = path; // return the file path for confirmation
    return result;
}