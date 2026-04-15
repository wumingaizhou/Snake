#define UNICODE
#define _UNICODE

/*
 * 模块说明：
 * 这是一个纯 Win32 API 版本的启动器入口。
 * 它负责定位启动器所在目录，拼出 app\main.exe 的路径，
 * 然后在相邻 app 目录中启动真正的游戏程序。
 */

#include <windows.h>

#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>

enum
{
    // 路径和错误消息共用的临时缓冲区大小。
    kPathBufferSize = 4096
};

static int showLauncherError(const wchar_t *format, ...)
{
    // 用格式化字符串构造错误消息，再通过消息框提示用户。
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
    // 统一封装路径字符串拼接，顺便做长度检查。
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
    // 去掉可执行文件名，只保留启动器所在目录。
    wchar_t *lastSlash = wcsrchr(path, L'\\');
    if (lastSlash)
        *lastSlash = L'\0';
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand)
{
    // 这些参数当前没有使用，但保留标准 WinMain 入口签名。
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
        // 路径过长时提前报错，避免后续 CreateProcessW 失败但原因不明。
        return showLauncherError(L"Launcher path is too long.");
    }

    const DWORD attributes = GetFileAttributesW(gamePath);
    if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        // 启动器约定真正的游戏程序位于同级 app 目录下。
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
        // 如果启动失败，就把系统错误一并展示给用户。
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
