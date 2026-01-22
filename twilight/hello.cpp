#include "handlers.h"

Json::Value HelloHandler(const Json::Value& data) {
	Json::Value response;
    response["code"] = (int)S_OK;
    response["data"] = data;
	return response;
}
