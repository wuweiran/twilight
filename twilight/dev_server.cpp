#include "dev_server.h"

#ifdef _DEBUG

// winsock2.h must precede windows.h to avoid the older winsock.h being pulled in.
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <pathcch.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Pathcch.lib")

namespace {

// The dev server port. Change this if 5174 conflicts with something on your
// machine; the app requires this exact port and fails fast if it is unavailable.
constexpr unsigned short kDevServerPort = 5174;

// Kept open for the process lifetime; closing it kills the dev server tree.
HANDLE g_devServerJob = nullptr;

// Returns true if a local TCP port can be bound (i.e. it is currently free).
bool IsPortFree(unsigned short port) {
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		return false;
	}
	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	InetPtonW(AF_INET, L"127.0.0.1", &addr.sin_addr);
	bool free = bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0;
	closesocket(sock);
	return free;
}

// Walks up from the executable to find the twilight-hammer project directory.
std::wstring FindWebProjectDir() {
	WCHAR exePath[MAX_PATH];
	if (GetModuleFileNameW(nullptr, exePath, MAX_PATH) == 0) {
		return {};
	}
	if (FAILED(PathCchRemoveFileSpec(exePath, MAX_PATH))) {
		return {};
	}

	std::wstring dir(exePath);
	for (;;) {
		std::wstring candidate = dir + L"\\twilight-hammer";
		std::wstring marker = candidate + L"\\package.json";
		if (GetFileAttributesW(marker.c_str()) != INVALID_FILE_ATTRIBUTES) {
			return candidate;
		}

		WCHAR parent[MAX_PATH];
		wcsncpy_s(parent, dir.c_str(), _TRUNCATE);
		if (FAILED(PathCchRemoveFileSpec(parent, MAX_PATH))) {
			return {};
		}
		if (wcscmp(parent, dir.c_str()) == 0) {
			return {};
		}
		dir = parent;
	}
}

} // namespace

bool WaitForDevServer(unsigned int timeoutMs) {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return false;
	}

	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(kDevServerPort);
	InetPtonW(AF_INET, L"127.0.0.1", &addr.sin_addr);

	const DWORD start = GetTickCount();
	bool connected = false;
	for (;;) {
		SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock != INVALID_SOCKET) {
			if (connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0) {
				connected = true;
			}
			closesocket(sock);
		}

		if (connected || timeoutMs == 0) {
			break;
		}
		if (GetTickCount() - start >= timeoutMs) {
			break;
		}
		Sleep(100);
	}

	WSACleanup();
	return connected;
}

bool StartDevServer() {
	std::wstring webDir = FindWebProjectDir();
	if (webDir.empty()) {
		OutputDebugStringW(L"[twilight] Could not locate twilight-hammer; dev server not started.\n");
		return false;
	}

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return false;
	}
	bool portFree = IsPortFree(kDevServerPort);
	WSACleanup();
	if (!portFree) {
		OutputDebugStringW(L"[twilight] Dev server port is already in use.\n");
		return false;
	}

	HANDLE job = CreateJobObjectW(nullptr, nullptr);
	if (job == nullptr) {
		return false;
	}

	JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits = {};
	limits.BasicLimitInformation.LimitFlags =
		JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE | JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION;
	if (!SetInformationJobObject(job, JobObjectExtendedLimitInformation, &limits, sizeof(limits))) {
		CloseHandle(job);
		return false;
	}

	// --strictPort makes Vite fail rather than silently pick a different port.
	std::wstring cmdLine =
		L"cmd.exe /c npm run dev -- --port " + std::to_wstring(kDevServerPort) + L" --strictPort";

	STARTUPINFOW si = {};
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = {};

	BOOL created = CreateProcessW(
		nullptr,
		&cmdLine[0],
		nullptr,
		nullptr,
		FALSE,
		CREATE_SUSPENDED | CREATE_NO_WINDOW | CREATE_NEW_PROCESS_GROUP,
		nullptr,
		webDir.c_str(),
		&si,
		&pi);

	if (!created) {
		CloseHandle(job);
		return false;
	}

	if (!AssignProcessToJobObject(job, pi.hProcess)) {
		OutputDebugStringW(L"[twilight] AssignProcessToJobObject failed; dev server won't auto-close.\n");
		CloseHandle(job);
		job = nullptr;
	}

	ResumeThread(pi.hThread);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	g_devServerJob = job;
	return true;
}

std::wstring GetDevServerUrl() {
	return L"http://localhost:" + std::to_wstring(kDevServerPort) + L"/";
}

#endif
