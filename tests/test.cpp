#include "CommonApi/Utilities/EventSystems/MultiEventSystem.h"
#include "CommonApi/Utilities/EventSystems/SingleCallbackEventSystem.h"

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
    using LongResult = long long;               // matches LongResult (long long on 64-bit)
    using UnsignedInt = unsigned int;           // matches UnsignedInt  
    using WideParameter = unsigned long long;   // matches WideParameter (unsigned long long on 64-bit)
    using LongParameter = long long;            // matches LongParameter (long long on 64-bit)
}

struct WindowEventPolicy {
    using Type = WindowEvents;

    template<Type T>
    struct Trait {};

    template <Type T>
    static void handleError(std::exception_ptr) {
    }

};

#ifdef _WIN32
template<>
struct WindowEventPolicy::Trait<WindowEvents::Native> {
    using Signature = void(Win32::WindowHandle windowHandle, Win32::UnsignedInt unsignedMessage,
        Win32::WideParameter wideParameter, Win32::LongParameter longParameter);
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

#include <iostream>

void callback(float xscale, float yscale) {
    (void)xscale; (void)yscale;
    std::cout << "Static" << std::endl;
}

class Callback {
public:
    void callbackMem(float xscale, float yscale) {
        (void)xscale; (void)yscale;
        std::cout << "Internal" << std::endl;
    }
};

#include "CommonApi/MultiThreading/ThreadPools/MinimalThreadPool.h"
#include "CommonApi/PlatformAbstractions/Console.h"
#include "CommonApi/PlatformAbstractions/Thread.h"

#include "PlatformKit/Window.h"

static inline auto& console = MT::Console::getInstance();

int main() {
    PlatformKit::Window window;

    window.create({ 800, 600 }, "app", PlatformKit::WindowAttributes::firstPersonGameMaximisedAtr());

	window.registerCallback<PlatformKit::IOEvents::MouseMovedScreen>([&](PlatformKit::Position pos){
        std::cout << pos.x << " " << pos.y << std::endl;
	});

	while (!window.shouldClose()) {		
		try {
			window.pollEvents();
		}
		catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}
	window.destroy();
}