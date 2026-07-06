#pragma once

// Debug-only helpers that own the Vite dev server lifecycle. The server runs in
// a Win32 Job Object, so it is torn down automatically when this process exits.

#ifdef _DEBUG

#include <string>

// Starts the Vite dev server. Returns false on failure, including when the port
// is already in use.
bool StartDevServer();

// Waits until the dev server accepts a connection or the timeout elapses.
// Pass timeoutMs = 0 to probe once.
bool WaitForDevServer(unsigned int timeoutMs);

// Returns the URL to navigate to once the server is ready.
std::wstring GetDevServerUrl();

#endif
