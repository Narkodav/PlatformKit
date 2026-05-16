#pragma once
#include <cstdint>
#include <stdexcept>
#include <iostream>

#include "PlatformKit/Structs.h"

namespace PlatformKit
{
    enum class WindowEvents
    {
        WindowResized,
        WindowMoved,
        WindowFocused,
        WindowUnfocused,
        WindowMinimized,
        WindowMaximized,
        WindowClosed,
        WindowRefresh,
        WindowContentScaleChanged,

        Native, //called on all events, returns platform native data
        Count
    };

    namespace Win32
    {
        using WindowHandle = void*;
        using InstanceHandle = void*;
        using MessageResult = intptr_t;                     // matches LRESULT
        using MessageValueUnsigned = uintptr_t;             // matches WPARAM
        using MessageValueSigned = intptr_t;                // matches LPARAM
    }

    struct WindowEventPolicy {
        using Type = WindowEvents;

        template<Type T>
        struct Trait {};

        template <Type T>
        static void handleError(std::exception_ptr) {
            std::cerr << "[PlatformKit] Exception in window event callback: " << Trait<T>::s_name << std::endl;
        }
    };

#ifdef _WIN32
    template<>
    struct WindowEventPolicy::Trait<WindowEvents::Native> {
        using Signature = void(Win32::WindowHandle windowHandle, uint32_t message,
            Win32::MessageValueUnsigned auxiliaryData, Win32::MessageValueSigned payloadData);
        static constexpr const char* s_name = "Native";
    };
#else


#endif

    template<>
    struct WindowEventPolicy::Trait<WindowEvents::WindowResized> {
        using Signature = void(int width, int height);
        static constexpr const char* s_name = "WindowResized";
    };

    template<>
    struct WindowEventPolicy::Trait<WindowEvents::WindowMoved> {
        using Signature = void(int x, int y);
        static constexpr const char* s_name = "WindowMoved";
    };

    template<>
    struct WindowEventPolicy::Trait<WindowEvents::WindowFocused> {
        using Signature = void();
        static constexpr const char* s_name = "WindowFocused";
    };

    template<>
    struct WindowEventPolicy::Trait<WindowEvents::WindowUnfocused> {
        using Signature = void();
        static constexpr const char* s_name = "WindowUnfocused";
    };

    template<>
    struct WindowEventPolicy::Trait<WindowEvents::WindowMinimized> {
        using Signature = void(int width, int height);
        static constexpr const char* s_name = "WindowMinimized";
    };

    template<>
    struct WindowEventPolicy::Trait<WindowEvents::WindowMaximized> {
        using Signature = void(int width, int height);
        static constexpr const char* s_name = "WindowMaximized";
    };

    template<>
    struct WindowEventPolicy::Trait<WindowEvents::WindowClosed> {
        using Signature = void();
        static constexpr const char* s_name = "WindowClosed";
    };

    template<>
    struct WindowEventPolicy::Trait<WindowEvents::WindowRefresh> {
        using Signature = void();
        static constexpr const char* s_name = "WindowRefresh";
    };

    template<>
    struct WindowEventPolicy::Trait<WindowEvents::WindowContentScaleChanged> {
        using Signature = void(float xscale, float yscale);
        static constexpr const char* s_name = "WindowContentScaleChanged";
    };
}

