#include <windows.h>

#include <filesystem>
#include <string>

namespace
{
std::wstring getErrorMessage(DWORD errorCode)
{
    wchar_t *buffer = nullptr;
    const DWORD size = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        errorCode,
        0,
        reinterpret_cast<LPWSTR>(&buffer),
        0,
        nullptr);

    std::wstring message = size && buffer ? std::wstring(buffer, size) : L"Unknown error";
    if (buffer)
        LocalFree(buffer);
    return message;
}

int showLauncherError(const std::wstring &message)
{
    MessageBoxW(nullptr, message.c_str(), L"sfSnakePro Launcher", MB_ICONERROR | MB_OK);
    return 1;
}
} // namespace

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    wchar_t modulePathBuffer[MAX_PATH] = {};
    const DWORD modulePathLength = GetModuleFileNameW(nullptr, modulePathBuffer, MAX_PATH);
    if (modulePathLength == 0 || modulePathLength == MAX_PATH)
        return showLauncherError(L"Failed to resolve launcher path.");

    const std::filesystem::path launcherPath(modulePathBuffer);
    const std::filesystem::path launcherDir = launcherPath.parent_path();
    const std::filesystem::path gameDir = launcherDir / L"app";
    const std::filesystem::path gamePath = gameDir / L"main.exe";

    if (!std::filesystem::exists(gamePath))
        return showLauncherError(L"Failed to find app/main.exe.\nPlease keep the app folder next to the launcher.");

    std::wstring commandLine = L"\"" + gamePath.wstring() + L"\"";
    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);

    PROCESS_INFORMATION processInfo{};
    if (!CreateProcessW(gamePath.c_str(),
                        commandLine.data(),
                        nullptr,
                        nullptr,
                        FALSE,
                        0,
                        nullptr,
                        gameDir.c_str(),
                        &startupInfo,
                        &processInfo))
    {
        return showLauncherError(L"Failed to start the game.\n\n" + getErrorMessage(GetLastError()));
    }

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);
    return 0;
}
