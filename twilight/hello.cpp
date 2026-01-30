#include "handlers.h"

Json::Value HelloHandler(const Json::Value& requestData) {
	Json::Value response;
    response["code"] = (int)S_OK;
    response["data"] = requestData;
	return response;
}
