// DynamicLibrary.h — обёртка над загруженной dll.
//
// В конструкторе dll загружается, в деструкторе выгружается, так что даже при
// исключении ничего не остаётся открытым. Копировать объект нельзя, иначе dll
// выгрузилась бы дважды. Перемещать можно.
//
// Всё, что связано с WinAPI, спрятано в .cpp.
#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <type_traits>

class DynamicLibrary {
public:
    explicit DynamicLibrary(const std::filesystem::path& path);  // бросает PluginError
    ~DynamicLibrary();

    DynamicLibrary(const DynamicLibrary&) = delete;
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;

    DynamicLibrary(DynamicLibrary&& other) noexcept;
    DynamicLibrary& operator=(DynamicLibrary&& other) noexcept;

    // Адрес функции из dll по имени, или nullptr, если такой нет.
    // Fn — тип указателя на функцию; неверный тип ловится на этапе компиляции.
    template <typename Fn>
    Fn symbol(const char* name) const {
        static_assert(std::is_pointer_v<Fn> && std::is_function_v<std::remove_pointer_t<Fn>>,
                      "symbol<Fn>: Fn должен быть указателем на функцию");
        return reinterpret_cast<Fn>(rawSymbol(name));
    }

private:
    void* rawSymbol(const char* name) const;

    struct Unloader {
        void operator()(void* handle) const noexcept;
    };

    std::unique_ptr<void, Unloader> handle_;
};


// Расширение динамических библиотек: ".dll".
const char* libraryExtension();

// Папка, в которой лежит исполняемый файл (calc.exe).
std::filesystem::path executableDir();

