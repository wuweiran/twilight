// twilight.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "twilight.h"
#include <pathcch.h>
#include <WebView2EnvironmentOptions.h>
#include <string>
#include <json/json.h>
#include <sstream>
#include <unordered_map>
#include <functional>

using namespace Microsoft::WRL;

constexpr auto MAX_LOADSTRING = 100;

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

// Pointer to WebViewController
static wil::com_ptr<ICoreWebView2Controller> webviewController;

// Pointer to WebView window
static wil::com_ptr<ICoreWebView2> webview;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_TWILIGHT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex = {
	.cbSize = sizeof(WNDCLASSEX),
	.style = CS_HREDRAW | CS_VREDRAW,
	.lpfnWndProc = WndProc,
	.cbClsExtra = 0,
	.cbWndExtra = 0,
	.hInstance = hInstance,
	.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TWILIGHT)),
	.hCursor = LoadCursor(nullptr, IDC_ARROW),
	.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
	.lpszMenuName = NULL,
	.lpszClassName = szWindowClass,
	.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL))
	};

	return RegisterClassExW(&wcex);
}

std::string ConvertPWSTRToString(PWSTR pwsz) {
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, pwsz, -1, NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, pwsz, -1, &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

PWSTR ConvertStringToPWSTR(const std::string& str) {
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	PWSTR pwsz = new WCHAR[size_needed];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, pwsz, size_needed);
	return pwsz;
}

std::unordered_map<std::string, std::function<Json::Value(const Json::Value&)>> requestHandlers;

Json::Value HelloHandler(const Json::Value& data) {
	Json::Value response = data;
	return response;
}

void RegisterHandlers() {
	requestHandlers["/hello"] = HelloHandler;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	RegisterHandlers();

	ComPtr<CoreWebView2EnvironmentOptions> options = Make<CoreWebView2EnvironmentOptions>();
	options->put_AdditionalBrowserArguments(L"--disable-web-security");
	CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, options.Get(),
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[hWnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {

				// Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
				env->CreateCoreWebView2Controller(hWnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
					[hWnd](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
						if (controller != nullptr) {
							webviewController = controller;
							webviewController->get_CoreWebView2(&webview);
						}

						wil::com_ptr<ICoreWebView2Settings> settings;
						webview->get_Settings(&settings);
						settings->put_IsScriptEnabled(TRUE);
						settings->put_AreDefaultScriptDialogsEnabled(TRUE);
						settings->put_IsWebMessageEnabled(TRUE);
#ifndef _DEBUG
						settings->put_AreDevToolsEnabled(FALSE);
						settings->put_AreDefaultContextMenusEnabled(FALSE);
						settings->put_IsZoomControlEnabled(FALSE);
#endif
						RECT bounds;
						GetClientRect(hWnd, &bounds);
						webviewController->put_Bounds(bounds);

#ifdef _DEBUG
						webview->Navigate(L"http://localhost:5174/");
#else
						WCHAR path[MAX_PATH];
						GetModuleFileName(NULL, path, MAX_PATH);
						PathCchRemoveFileSpec(path, MAX_PATH);
						PathCchAppend(path, MAX_PATH, L"index.html");
						webview->Navigate(path);
#endif

						EventRegistrationToken token;

						// Communication between host and web content
						// Set an event handler for the host to return received message back to the web content
						webview->add_WebMessageReceived(Callback<ICoreWebView2WebMessageReceivedEventHandler>(
							[](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
								wil::unique_cotaskmem_string message;
								HRESULT result = args->get_WebMessageAsJson(&message);
								if (result != S_OK) {
									return E_FAIL;
								}

								PWSTR payload = message.get();
								Json::CharReaderBuilder readerBuilder;
								Json::Value root;
								std::string errs;

								std::string jsonString = ConvertPWSTRToString(payload);
								std::istringstream s(jsonString);
								if (!Json::parseFromStream(readerBuilder, s, &root, &errs)) {
									return E_FAIL;
								}

								std::string uri = root["uri"].asString();
								std::string requestId = root["requestId"].asString();
								Json::Value data = root["data"];
								if (uri.empty() || requestId.empty() || data.isNull()) return E_FAIL;

								Json::Value apiResponse;
								apiResponse["requestId"] = requestId;
								apiResponse["code"] = (int)S_OK;

								// Dispatch to registered handler
								auto handler = requestHandlers.find(uri);
								if (handler != requestHandlers.end()) {
									apiResponse["data"] = handler->second(data);
								}
								else {
									apiResponse["code"] = (int)E_FAIL;
									apiResponse["info"] = "Unknown URI";
								}

								Json::StreamWriterBuilder wbuilder;
								wbuilder["indentation"] = "";
								PWSTR apiResponseJsonString = ConvertStringToPWSTR(Json::writeString(wbuilder, apiResponse));
								webview->PostWebMessageAsJson(apiResponseJsonString);
								delete[] apiResponseJsonString;

								return S_OK;
							}).Get(), &token);

						return S_OK;
					}).Get());
				return S_OK;
			}).Get());

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		if (webviewController != nullptr) {
			RECT bounds;
			GetClientRect(hWnd, &bounds);
			webviewController->put_Bounds(bounds);
		};
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
