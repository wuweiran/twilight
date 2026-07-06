# Twilight Hammer

A Windows desktop app bootstrapper that pairs a native **C++ / Win32 + WebView2** host
with a modern **Vite + React + TypeScript** frontend. Use it as a starting template for
building lightweight desktop apps whose UI is web tech but whose shell and OS integration
are native C++ — without bundling a whole browser like Electron.

---

## 1. Architecture at a glance

| Project | Tech | Role |
| --- | --- | --- |
| `twilight/` | C++ / Win32 | Native host window that embeds WebView2 and exposes a JSON message bridge |
| `twilight-hammer/` | Vite + React + TypeScript | The web UI rendered inside the WebView |

- **Debug**: the C++ host starts the Vite dev server itself and navigates to `http://localhost:5174/`. The dev server is owned by the host process (via a Win32 Job Object), so it shuts down automatically when you stop debugging.
- **Release**: the frontend is built to a `web/` folder next to the `.exe`, and the host loads it from `file:///.../web/index.html`.
- **Bridge**: the UI calls `call<Req, Res>(uri, data)` (see `twilight-hammer/src/containers/api.ts`); the host dispatches by `uri` to handlers registered in `twilight.cpp`.

---

## 2. Requirements

Install before opening the solution:

- **Visual Studio 2022/2026** with the **Desktop development with C++** workload.
- **Node.js** (includes npm) — v18+ recommended (developed with Node 24 / npm 9+). `npm` must be on your `PATH`.
- **WebView2 Runtime** — preinstalled on current Windows 10/11. If missing, install the Evergreen runtime from Microsoft.
- **Inno Setup 6** — only needed to build the installer (`publish.ps1`). Expected at `C:\Program Files (x86)\Inno Setup 6\ISCC.exe`.

NuGet packages (WebView2, JsonCpp, WIL) are restored automatically on first build.

---

## 3. First-time setup (after opening in Visual Studio)

1. **Open** `twilight.sln`.
2. **Set the startup project** to **`twilight`** (the C++ project):
   right-click it in Solution Explorer → **Set as Startup Project**.
   The C++ host now launches the dev server itself, so you do **not** need to start `twilight-hammer` separately.
3. **Select a configuration**: `Debug` + a platform (`x64` recommended).
4. Restore happens automatically; if the frontend ever looks stale, run `npm install` once inside `twilight-hammer/`.

> First build triggers `npm install` and a Vite build via the project dependency, so it can take a bit longer than later builds.

---

## 4. Running & debugging

- Press **F5** (or Ctrl+F5). The host builds, starts Vite on port **5174**, waits until it's ready, then shows the window.
- **Debugging the C++ side**: normal Visual Studio breakpoints.
- **Debugging the TypeScript/React side**: press **F12** in the running app window to open the WebView DevTools (enabled in Debug builds).
- **Stop debugging**: this also terminates the Vite dev server automatically — no orphaned terminal.

If port 5174 is already in use, startup fails fast with a message box (by design). Change the port in `twilight/dev_server.cpp` (see placeholders below) if you need a different one.

---

## 5. Building

- **Debug build**: just build/run in the IDE as above.
- **Release build (no installer)**: switch to `Release` and build. This produces `twilight.exe`, the required DLLs, and a `web/` folder in the output directory — but does **not** create an installer.

---

## 6. Publishing the installer

Publishing is a **separate, opt-in step** so normal Release builds stay fast.

```powershell
.\publish.ps1                 # Release | x64 (default)
.\publish.ps1 -Platform ARM64
.\publish.ps1 -Platform Win32
```

- The script does a clean Release rebuild for the chosen platform and runs Inno Setup.
- Output: `dist\twilight_setup_<platform>.exe`.
- Equivalent raw command: `msbuild twilight.sln -p:Configuration=Release -p:Platform=x64 -p:Publish=true`.
- Requires Inno Setup 6 (otherwise the build warns and skips the installer step).

---

## 7. Renaming the app — what to change vs. what to leave

The name "twilight" appears in two very different roles. **You only need to change the
user-facing branding.** The structural/internal names can stay as-is; renaming them is
optional and higher-effort.

### 7a. Must change — user-visible branding

These control what your users actually see (window title, exe name, installer, About box):

| What | File | Line / setting |
| --- | --- | --- |
| **Window title** | `twilight/twilight.rc` | `IDS_APP_TITLE "twilight"` |
| **App exe name** (`twilight.exe`) | `twilight/twilight.vcxproj` | add `<TargetName>YourApp</TargetName>` (defaults to the project name today) |
| **Error dialog captions** | `twilight/twilight.cpp` | the `L"Twilight"` caption in each `MessageBoxW` |
| **HTML tab title** | `twilight-hammer/index.html` | `<title>Twilight Hammer</title>` |
| **About screen title + copyright** | `twilight-hammer/src/App.tsx` | `<Title1>Twilight Hammer</Title1>`, `©…M1Knight Technology` |
| **Home demo heading** | `twilight-hammer/src/App.tsx` | `Vite + React + WebView2 + C++` (optional) |
| **Sample message text** | `twilight-hammer/src/components/SendMessage.tsx` | `"Hello Twilight Hammer!"` (optional) |

**Installer branding** — `setup_script.iss`:

| Setting | Purpose |
| --- | --- |
| `AppId={{...}}` | **Generate a NEW GUID** for your app (Tools → Create GUID). Do not reuse. |
| `AppName`, `AppVersion`, `AppPublisher`, `App*URL` | Shown in the installer/Programs list |
| `DefaultDirName={autopf}\Twilight` | Install folder |
| `DefaultGroupName=Twilight` | Start-menu group |
| `OutputBaseFilename=twilight_setup_{#VSPlatform}` | Installer file name |
| `UninstallDisplayIcon`, `[Icons]`, `[Run]` entries | Reference the exe — update if you change the exe name |

> If you rename the exe (`<TargetName>`), also update the `twilight.exe` references in `setup_script.iss` (`[Files]`, `[Icons]`, `[Run]`, `UninstallDisplayIcon`) and the installer name in `publish.ps1`.

### 7b. Leave alone — structural / internal names

These are **not** shown to users. Changing them is optional refactoring and risks breaking build wiring, so it's fine to keep them:

- **Source folder names** `twilight/` and `twilight-hammer/` — the folder name `twilight-hammer` is **hardcoded** in `twilight/dev_server.cpp` and in the `.vcxproj` PreBuildEvents (`cd $(SolutionDir)twilight-hammer`). Renaming requires updating all of those, so leave it unless you have a reason.
- **C++ file names** `twilight.cpp`, `twilight.h`, `twilight.rc`, `resource.h`.
- **`<RootNamespace>twilight</RootNamespace>`** in the `.vcxproj`.
- **Resource IDs** `IDS_APP_TITLE`, `IDC_TWILIGHT`, `IDI_TWILIGHT`, etc. in `resource.h` — these are identifiers, not text.
- **`name` in `package.json`** (`twilight-hammer`) — internal npm package name.
- **`[twilight]` debug log prefix** in `dev_server.cpp` — only visible in the debugger Output window.
- **Solution/project GUIDs** in `twilight.sln`.

> Rule of thumb: change **strings the user reads**; keep **identifiers the build relies on**.

### 7c. Replace the icons

Swap `twilight/twilight.ico` and `twilight/small.ico` with your own (keep the same file names, or update the `ICON` references in `twilight.rc`).

---

## 8. Project layout

```
twilight.sln                     Solution
publish.ps1                      One-click installer build (opt-in)
setup_script.iss                 Inno Setup script (installer definition)
twilight/                        C++ / Win32 + WebView2 host
  twilight.cpp                   Entry point, window, WebView2 setup, bridge dispatch
  dev_server.{h,cpp}             Debug-only Vite dev server lifecycle (Job Object)
  handlers.h, hello.cpp, file.cpp  Bridge request handlers
  twilight.rc, resource.h        Win32 resources (title, icons)
twilight-hammer/                 Vite + React + TypeScript frontend
  src/containers/api.ts          Frontend side of the JSON bridge (call<Req,Res>)
  src/App.tsx                    UI (Home / Example / About)
```

---

## 9. Extending the native bridge

1. Add a handler in the C++ host (see `hello.cpp` / `file.cpp` for the pattern) and register it in `RegisterHandlers()` in `twilight.cpp` under a new `uri`.
2. Call it from the frontend: `call<RequestType, ResponseType>("/your/uri", payload)`.
3. Synchronous handlers return immediately; asynchronous ones post their result back later (see the file-open example).

