// twilight.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "twilight.h"
#include "pathcch.h"
#include "WebView2EnvironmentOptions.h"
#include <string>

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

    if (!InitInstance (hInstance, nCmdShow))
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

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TWILIGHT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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
							   args->TryGetWebMessageAsString(&message);
							   // processMessage(&message);
							   webview->PostWebMessageAsString(message.get());
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
