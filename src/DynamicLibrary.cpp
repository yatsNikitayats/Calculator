#include "DynamicLibrary.h"

#include <utility>

#include "Errors.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

DynamicLibrary::DynamicLibrary(const std::filesystem::path& path) {
    // Путь передаём как wide-строку, иначе не открываются папки с русскими буквами.
    HMODULE h = ::LoadLibraryW(path.wstring().c_str());
    if (h == nullptr)
        throw PluginError("cannot load library (WinAPI error " + std::to_string(::GetLastError()) +
                          ", file is not a valid DLL or has the wrong bitness)");
    handle_.reset(reinterpret_cast<void*>(h));
}

void DynamicLibrary::Unloader::operator()(void* handle) const noexcept {
    ::FreeLibrary(static_cast<HMODULE>(handle));
}

DynamicLibrary::~DynamicLibrary() = default;

DynamicLibrary::DynamicLibrary(DynamicLibrary&& other) noexcept = default;

DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& other) noexcept = default;

void* DynamicLibrary::symbol(const char* name) const {
    if (!handle_) return nullptr;
    return reinterpret_cast<void*>(::GetProcAddress(static_cast<HMODULE>(handle_.get()), name));
}


const char* libraryExtension() { return ".dll"; }

std::filesystem::path executableDir() {
    wchar_t buf[MAX_PATH];
    const DWORD len = ::GetModuleFileNameW(nullptr, buf, MAX_PATH);
    if (len > 0 && len < MAX_PATH) return std::filesystem::path(buf).parent_path();

    std::error_code ec;
    return std::filesystem::current_path(ec);  // если не получилось, берём текущую папку
}

