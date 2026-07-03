#pragma once
#include <string>
#include <iostream>

#include "PlatformKit/Structs.h"
#include "PlatformKit/WindowEvents.h"
#include "PlatformKit/IOEvents.h"
#include "PlatformKit/Mouse.h"
#include "PlatformKit/Keyboard.h"

//#include "Keyboard.h"
#include "CommonApi/Utilities/EventSystems/MultiEventSystem.h"
#include "CommonApi/Utilities/EventSystems/SingleCallbackEventSystem.h"

#define PLATFORM_WIN32_WINPROC_CALLBACK CALLBACK

namespace PlatformKit::Win32
{
    //windows are thread local, declare them thread_local for clarity
    class Window
    {
    private:
        static MessageResult PLATFORM_WIN32_WINPROC_CALLBACK windowProc(WindowHandle windowHandle, 
            uint32_t message, MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);

        class ClassRegistrator;

    public:
        using EventManager = Utils::MultiEventSystem<Utils::SingleCallbackEventSystem, WindowEventPolicy, IOEventPolicy>;

    private:
        WindowHandle m_windowHandle = nullptr;
        bool m_shouldClose = false;
        std::vector<uint8_t> m_rawInputBuffer;
        WindowAttributes m_attr;
        thread_local static inline size_t s_windowCounter = 0;
        Position m_ClientRelativeMousePosition = { 0, 0 };
        
    protected:
        EventManager m_windowEvents;

    public:
        Window() noexcept { Mouse::getInstance(); };
        Window(Extent extent, std::string_view title, WindowAttributes attr = WindowAttributes::defaultAtr());
        ~Window();

        Window(Window&& other) noexcept {
            m_windowHandle = std::exchange(other.m_windowHandle, nullptr);
            m_shouldClose = std::exchange(other.m_shouldClose, false);
            m_rawInputBuffer = std::exchange(other.m_rawInputBuffer, std::vector<uint8_t>());
            m_attr = std::exchange(other.m_attr, WindowAttributes());
            m_ClientRelativeMousePosition = std::exchange(other.m_ClientRelativeMousePosition, { 0, 0 });
            m_windowEvents = std::exchange(other.m_windowEvents, EventManager());
            if (m_windowHandle) {
                setThisPointer();
            }
        };

        Window& operator=(Window&& other) noexcept
        {
            if (this == &other)
                return *this;
            destroy();
            m_windowHandle = std::exchange(other.m_windowHandle, nullptr);
            m_shouldClose = std::exchange(other.m_shouldClose, false);
            m_rawInputBuffer = std::exchange(other.m_rawInputBuffer, std::vector<uint8_t>());
            m_attr = std::exchange(other.m_attr, WindowAttributes());
            m_ClientRelativeMousePosition = std::exchange(other.m_ClientRelativeMousePosition, { 0, 0 });
            m_windowEvents = std::exchange(other.m_windowEvents, EventManager());
            if (m_windowHandle) {
                setThisPointer();
            }
            return *this;
        };

        Window(const Window&) noexcept = delete;
        Window& operator=(const Window&) noexcept = delete;

        // returns a platform native handle
        WindowHandle getHandle() const { return m_windowHandle; }

        void pollEvents();
        static void pollEventsGlobal();
        void show();
        void hide();
        void minimise();
        void maximise();
        void restore();

        void destroy();
        void create(Extent extent, std::string_view title, WindowAttributes attr = WindowAttributes::defaultAtr());

        bool shouldClose() const;

        void setCursorPos(Position pos);
        void setResizable(bool resizable);
        void setVisible(bool visible);
        void setDecorated(bool decorated);
        void setFocused(bool focused);
        void setAutoIconify(bool autoIconify);
        void setFloating(bool floating);
        void setMaximized(bool maximized);
        void centerCursor(bool center);
        void setTransparentFramebuffer(bool transparent);
        void setFocusOnShow(bool focusOnShow);
        void setCursorMode(CursorMode mode);

        bool isResizable() const;
        bool isVisible() const;
        bool isDecorated() const;
        bool isFocused() const;
        bool isAutoIconify() const { return true; } //always true on windows
        bool isFloating() const;
        bool isMinimised();
        bool isMaximised();
        bool isCursorCentered() const { return m_attr.centerCursor; }
        bool isTransparentFramebuffer() const;
        bool isFocusOnShow() const { return m_attr.focusOnShow; }
        CursorMode getCursorMode() const { return m_attr.cursorMode; }
        Position getCursorPos();
        static Position getGlobalCursorPos();
        void centerCursor();

        Extent getWindowExtent() const;
        Extent getClientExtent() const;
        Extent getFrameBufferExtent() const;
        Position getWindowPosition() const;
        float getDPIScale() const;
        float getAspectRatio() const { 
            auto extent = getWindowExtent();
            return static_cast<float>(extent.width) / static_cast<float>(extent.height); 
        };

        Mouse& getMouse() const { return Mouse::getInstanceUnsafe(); }
        Keyboard& getKeyboard() const { return Keyboard::getInstanceUnsafe(); }

        static InstanceHandle getInstanceHandle();

    private:
        void setThisPointer();
        static Window* getThisPointer(WindowHandle windowHandle);

        template <MessageResult(Window::* func)(MessageValueUnsigned, MessageValueSigned)>
        static inline MessageResult callEvent(WindowHandle windowHandle, 
            uint32_t message, MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData) {
            Window* window = getThisPointer(windowHandle);
            if (window)
            {
                window->m_windowEvents.emit<WindowEvents::Native>(windowHandle, message, auxiliaryData, payloadData);
                return (window->*func)(auxiliaryData, payloadData);
            }
            return 0;
        }

        static MessageResult onNull(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        static MessageResult onCreate(WindowHandle WindowHandle, MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        MessageResult onDestroy(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);

        MessageResult onWindowResized(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        MessageResult onWindowMoved(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        MessageResult onWindowFocused(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        MessageResult onWindowUnfocused(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        MessageResult onWindowClosed(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        MessageResult onWindowRefresh(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        MessageResult onWindowContentScaleChanged(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);

        // MessageResult onLeftMouseButtonPressed(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        // MessageResult onRightMouseButtonPressed(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        // MessageResult onMiddleMouseButtonPressed(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        // MessageResult onExtraMouseButtonPressed(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);

        // MessageResult onLeftMouseButtonReleased(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        // MessageResult onRightMouseButtonReleased(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        // MessageResult onMiddleMouseButtonReleased(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        // MessageResult onExtraButtonReleased(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);

        MessageResult onMouseMoved(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        //MessageResult onMouseScrolled(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        MessageResult onMouseLeft(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);

        MessageResult onRawInput(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);

        MessageResult onKeyboardInput(std::vector<uint8_t>& input);
        MessageResult onMouseInput(std::vector<uint8_t>& input);

        MessageResult onWindowStartResizing(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        MessageResult onWindowStopResizing(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);
        MessageResult onWindowResizing(MessageValueUnsigned auxiliaryData, MessageValueSigned payloadData);

        template<IOEvents eventPress, IOEvents eventRelease>
        void checkMouseButton(uint16_t buttonDownFlag, uint16_t buttonUpFlag, uint16_t flags, MouseButton button) {
            auto& mouse = Mouse::getInstanceUnsafe();
            if (flags & buttonDownFlag) {
                mouse.m_buttonStateChanged = true;
                mouse.m_buttonStates.setInputState<MouseButtonState::Pressed>(button, true);
                mouse.m_buttonStates.setInputState<MouseButtonState::Changed>(button, true);
                m_windowEvents.emit<IOEvents::MouseButtonPressed>(button, mouse.m_mousePos);
                m_windowEvents.emit<eventPress>(mouse.m_mousePos);
            }
            else if (flags & buttonUpFlag) {
                mouse.m_buttonStateChanged = true;
                mouse.m_buttonStates.setInputState<MouseButtonState::Pressed>(button, false);
                mouse.m_buttonStates.setInputState<MouseButtonState::Changed>(button, true);
                m_windowEvents.emit<IOEvents::MouseButtonReleased>(button, mouse.m_mousePos);
                m_windowEvents.emit<eventRelease>(mouse.m_mousePos);
            }
        }

    protected:
        void updateCursorState();        
    };
}
    