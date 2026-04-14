#define UNICODE
#define _UNICODE

#include <windows.h>

#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>

enum
{
    kPathBufferSize = 4096
};

static int showLauncherError(const wchar_t *format, ...)
{
    wchar_t message[kPathBufferSize] = {0};

    va_list args;
    va_start(args, format);
    _vsnwprintf(message, kPathBufferSize - 1, format, args);
    va_end(args);

    MessageBoxW(NULL, message, L"sfSnakePro Launcher", MB_ICONERROR | MB_OK);
    return 1;
}

static int buildPath(wchar_t *buffer, size_t bufferSize, const wchar_t *format, ...)
{
    va_list args;
    va_start(args, format);
    const int written = _vsnwprintf(buffer, bufferSize - 1, format, args);
    va_end(args);

    if (written < 0 || (size_t)written >= bufferSize)
    {
        if (bufferSize > 0)
            buffer[0] = L'\0';
        return 0;
    }

    buffer[written] = L'\0';
    return 1;
}

static void trimExecutableName(wchar_t *path)
{
    wchar_t *lastSlash = wcsrchr(path, L'\\');
    if (lastSlash)
        *lastSlash = L'\0';
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand)
{
    (void)instance;
    (void)previousInstance;
    (void)commandLine;
    (void)showCommand;

    wchar_t launcherDir[kPathBufferSize] = {0};
    const DWORD pathLength = GetModuleFileNameW(NULL, launcherDir, kPathBufferSize);
    if (pathLength == 0 || pathLength >= kPathBufferSize)
        return showLauncherError(L"Failed to resolve launcher path.");

    trimExecutableName(launcherDir);
    if (launcherDir[0] == L'\0')
        return showLauncherError(L"Failed to resolve launcher directory.");

    wchar_t gameDir[kPathBufferSize] = {0};
    wchar_t gamePath[kPathBufferSize] = {0};
    wchar_t commandLineBuffer[kPathBufferSize] = {0};

    if (!buildPath(gameDir, kPathBufferSize, L"%ls\\app", launcherDir) ||
        !buildPath(gamePath, kPathBufferSize, L"%ls\\main.exe", gameDir) ||
        !buildPath(commandLineBuffer, kPathBufferSize, L"\"%ls\"", gamePath))
    {
        return showLauncherError(L"Launcher path is too long.");
    }

    const DWORD attributes = GetFileAttributesW(gamePath);
    if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        return showLauncherError(L"Failed to find app\\main.exe.\nPlease keep the app folder next to the launcher.");
    }

    STARTUPINFOW startupInfo;
    PROCESS_INFORMATION processInfo;
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    ZeroMemory(&processInfo, sizeof(processInfo));
    startupInfo.cb = sizeof(startupInfo);

    if (!CreateProcessW(gamePath,
                        commandLineBuffer,
                        NULL,
                        NULL,
                        FALSE,
                        0,
                        NULL,
                        gameDir,
                        &startupInfo,
                        &processInfo))
    {
        const DWORD errorCode = GetLastError();
        wchar_t systemMessage[kPathBufferSize] = L"Unknown error.";
        const DWORD systemMessageLength =
            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                           NULL,
                           errorCode,
                           0,
                           systemMessage,
                           kPathBufferSize,
                           NULL);

        if (systemMessageLength == 0)
            wcscpy(systemMessage, L"Unknown error.");

        return showLauncherError(L"Failed to start the game.\n\n%ls", systemMessage);
    }

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);
    return 0;
}
