#include <windows.h>

/*
 * 模块说明：
 * 这是使用 C++17 std::filesystem 实现的启动器入口。
 * 它完成与 C 版本相同的职责：找到 app\main.exe 并启动游戏。
 */

#include <filesystem>
#include <string>

namespace
{
std::wstring getErrorMessage(DWORD errorCode)
{
    // 将 Win32 错误码翻译为人类可读的系统消息文本。
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
    // 所有启动器错误统一通过消息框反馈给用户。
    MessageBoxW(nullptr, message.c_str(), L"sfSnakePro Launcher", MB_ICONERROR | MB_OK);
    return 1;
}
} // namespace

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // 先拿到启动器自身路径，再逐级拼出目标程序路径。
    wchar_t modulePathBuffer[MAX_PATH] = {};
    const DWORD modulePathLength = GetModuleFileNameW(nullptr, modulePathBuffer, MAX_PATH);
    if (modulePathLength == 0 || modulePathLength == MAX_PATH)
        return showLauncherError(L"Failed to resolve launcher path.");

    const std::filesystem::path launcherPath(modulePathBuffer);
    const std::filesystem::path launcherDir = launcherPath.parent_path();
    const std::filesystem::path gameDir = launcherDir / L"app";
    const std::filesystem::path gamePath = gameDir / L"main.exe";

    if (!std::filesystem::exists(gamePath))
        // 启动器和 app 目录需要一起分发，否则无法找到真正的游戏可执行文件。
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
        // 拼接系统错误消息，帮助用户判断是权限、路径还是依赖问题。
        return showLauncherError(L"Failed to start the game.\n\n" + getErrorMessage(GetLastError()));
    }

    // 启动成功后关闭句柄，避免资源泄漏。
    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);
    return 0;
}
