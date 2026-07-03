#include "PlatformKit/Window.h"

namespace PlatformKit::Win32
{
    class Window::ClassRegistrator {
    private:
        WNDCLASSEX m_wc{};
        const wchar_t* m_nameW = L"Window";
        const char* m_name = "Window";

        ClassRegistrator() {
            m_wc.cbSize = sizeof(WNDCLASSEX);              // REQUIRED
            m_wc.style = CS_HREDRAW | CS_VREDRAW;
            m_wc.lpfnWndProc = reinterpret_cast<WNDPROC>(windowProc);
            m_wc.cbClsExtra = 0;
            m_wc.cbWndExtra = 0;
            m_wc.hInstance = GetModuleHandle(nullptr);
            m_wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
            m_wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
            m_wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            m_wc.lpszMenuName = nullptr;
            m_wc.lpszClassName = m_name;
            m_wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

            ATOM atom = RegisterClassEx(&m_wc);
            if (!atom)
            {
                DWORD error = GetLastError();
                printf("RegisterClassEx failed: %lu\n", error);
            }
        }

        ~ClassRegistrator() {
            UnregisterClass(m_name, m_wc.hInstance);
        }

        ClassRegistrator(const ClassRegistrator&) = delete;
        ClassRegistrator& operator=(const ClassRegistrator&) = delete;
        ClassRegistrator(ClassRegistrator&&) = delete;
        ClassRegistrator& operator=(ClassRegistrator&&) = delete;

    public:
        const wchar_t* getNameWide() const { return m_nameW; }
        const char* getName() const { return m_name; }
        const WNDCLASSEX& getHandle() const { return m_wc; }

        static ClassRegistrator& getInstance() {
            static ClassRegistrator instance;
            return instance;
        }
    };

    Window::Window(Extent extent, std::string_view title, WindowAttributes attr /*= WindowAttributes::defaultAtr()*/)
    {
        create(std::move(extent), std::move(title), std::move(attr));
    }

    Window::~Window()
    {
        destroy();
    }

    void Window::destroy() {
        if (!m_windowHandle)
                return;

        // Unregister raw input devices
        RAWINPUTDEVICE rid[2]{};

        // Keyboard
        rid[0].usUsagePage = 0x01;
        rid[0].usUsage     = 0x06;
        rid[0].dwFlags     = RIDEV_REMOVE;
        rid[0].hwndTarget  = nullptr;

        // Mouse
        rid[1].usUsagePage = 0x01;
        rid[1].usUsage     = 0x02;
        rid[1].dwFlags     = RIDEV_REMOVE;
        rid[1].hwndTarget  = nullptr;

        RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE));

        // Destroy the window
        DestroyWindow(reinterpret_cast<HWND>(m_windowHandle));

        // m_windowHandle will be cleared in WM_DESTROY (onDestroy)
    }

    void Window::create(Extent extent, std::string_view title, WindowAttributes attr /*= WindowAttributes::defaultAtr()*/) {
        destroy();
        m_attr = attr;

        auto& wc = ClassRegistrator::getInstance();

        // Convert WindowAttributes to Win32 window styles
        DWORD style = WS_OVERLAPPEDWINDOW;
        DWORD exStyle = 0;

        // Apply resizable attribute
        if (!m_attr.resizable) {
            style &= ~WS_THICKFRAME;
            style &= ~WS_MAXIMIZEBOX;
        }

        // Apply decorated attribute
        if (!m_attr.decorated) {
            style &= ~WS_CAPTION;
            style &= ~WS_SYSMENU;
            style &= ~WS_MINIMIZEBOX;
            style &= ~WS_MAXIMIZEBOX;

            // For borderless window, we might need to adjust
            if (m_attr.decorated) {
                style |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
                if (m_attr.resizable) {
                    style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
                }
            }
        }

        // Apply floating attribute (always on top)
        if (m_attr.floating) {
            exStyle |= WS_EX_TOPMOST;
        }

        // Apply transparent framebuffer
        if (m_attr.transparentFramebuffer) {
            exStyle |= WS_EX_LAYERED;
        }

        auto& mouse = Mouse::getInstance();
        auto& keyboard = Keyboard::getInstance();
        GetCursorPos(reinterpret_cast<POINT*>(&mouse.m_mousePos));
        
        // Create the window
        HWND hwnd = CreateWindowEx(
            exStyle,
            wc.getName(),
            title.data(),
            style,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            extent.width,
            extent.height,
            nullptr,
            nullptr,
            reinterpret_cast<HINSTANCE>(getInstanceHandle()),
            this
        );

        if(hwnd != m_windowHandle)
            throw std::runtime_error("Window handles dont match");

        if (!m_windowHandle) {
            DWORD error = GetLastError();
            LPSTR messageBuffer = nullptr;
            size_t size = FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr,
                error,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)&messageBuffer,
                0,
                nullptr
            );

            std::string message(messageBuffer, size);
            LocalFree(messageBuffer);

            std::cerr << "CreateWindowEx failed with error "
                    << error << ": " << message << std::endl;

            throw std::runtime_error("Failed to create window");
        }

        // Update floating (always on top) state
        SetWindowPos(reinterpret_cast<HWND>(m_windowHandle),
            m_attr.floating ? HWND_TOPMOST : HWND_NOTOPMOST,
            0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

        // Update transparency
        if (m_attr.transparentFramebuffer) {
            SetLayeredWindowAttributes(reinterpret_cast<HWND>(m_windowHandle), 0, 255, LWA_ALPHA);
        }

        // Show window with appropriate command based on maximized state
        int showCommand = attr.maximized ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL;
        if (!attr.visible) {
            showCommand = SW_HIDE;
        }
        ShowWindow(reinterpret_cast<HWND>(m_windowHandle), showCommand);

        // Apply focus if requested
        if (attr.focused) {
            SetFocus(reinterpret_cast<HWND>(m_windowHandle));
        }

        setCursorMode(attr.cursorMode);
        if (m_attr.centerCursor) {
            centerCursor();            
        }

        UpdateWindow(reinterpret_cast<HWND>(m_windowHandle));
        m_shouldClose = false;

        RAWINPUTDEVICE rid[2]{};

        // Keyboard
        rid[0].usUsagePage = 0x01;
        rid[0].usUsage     = 0x06;
        rid[0].dwFlags     = RIDEV_NOLEGACY;
        rid[0].hwndTarget  = reinterpret_cast<HWND>(m_windowHandle);

        // Mouse
        rid[1].usUsagePage = 0x01;
        rid[1].usUsage     = 0x02;
        rid[1].dwFlags     = RIDEV_INPUTSINK;
        rid[1].hwndTarget  = reinterpret_cast<HWND>(m_windowHandle);

        RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE));
    }

    void Window::pollEvents() {
        Mouse::getInstanceUnsafe().refreshState();
        Keyboard::getInstanceUnsafe().refreshState();
        MSG msg;
        while (PeekMessage(&msg, reinterpret_cast<HWND>(m_windowHandle), 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void Window::pollEventsGlobal() {
        Mouse::getInstanceUnsafe().refreshState();
        Keyboard::getInstanceUnsafe().refreshState();
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void Window::show() { ShowWindow(reinterpret_cast<HWND>(m_windowHandle), SW_SHOW); }
    void Window::hide() { ShowWindow(reinterpret_cast<HWND>(m_windowHandle), SW_HIDE); }
    void Window::minimise() {
        if (!m_windowHandle) return;
        ShowWindow(reinterpret_cast<HWND>(m_windowHandle), SW_MINIMIZE);
    }

    void Window::maximise() {
        if (!m_windowHandle) return;
        ShowWindow(reinterpret_cast<HWND>(m_windowHandle), SW_MINIMIZE);
    }

    void Window::restore() {
        if (!m_windowHandle) return;
        ShowWindow(reinterpret_cast<HWND>(m_windowHandle), SW_RESTORE);
    }

    InstanceHandle Window::getInstanceHandle() { return reinterpret_cast<InstanceHandle>(GetModuleHandle(nullptr)); };

    void Window::setThisPointer()
    {
        SetWindowLongPtr(reinterpret_cast<HWND>(m_windowHandle), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }

    Window* Window::getThisPointer(WindowHandle hwnd) {
        return reinterpret_cast<Window*>(GetWindowLongPtr(reinterpret_cast<HWND>(hwnd), GWLP_USERDATA));
    }
    
    LRESULT CALLBACK Window::windowProc(WindowHandle windowHandle, UINT message, WPARAM auxiliaryData, LPARAM payloadData)
    {
        switch (message) {
        case WM_NULL: return onNull(auxiliaryData, payloadData);
        case WM_CREATE: return onCreate(windowHandle, auxiliaryData, payloadData);
        case WM_DESTROY: return callEvent<&Window::onDestroy>(windowHandle, message, auxiliaryData, payloadData);
        
        // Window events
        case WM_SIZE: return callEvent<&Window::onWindowResized>(windowHandle, message, auxiliaryData, payloadData);

        case WM_ENTERSIZEMOVE: return callEvent<&Window::onWindowStartResizing>(windowHandle, message, auxiliaryData, payloadData);
        case WM_EXITSIZEMOVE: return callEvent<&Window::onWindowStopResizing>(windowHandle, message, auxiliaryData, payloadData);
        case WM_SIZING: return callEvent<&Window::onWindowResizing>(windowHandle, message, auxiliaryData, payloadData);

        case WM_MOVE: return callEvent<&Window::onWindowMoved>(windowHandle, message, auxiliaryData, payloadData);
        case WM_SETFOCUS: return callEvent<&Window::onWindowFocused>(windowHandle, message, auxiliaryData, payloadData);
        case WM_KILLFOCUS: return callEvent<&Window::onWindowUnfocused>(windowHandle, message, auxiliaryData, payloadData);
        case WM_CLOSE: return callEvent<&Window::onWindowClosed>(windowHandle, message, auxiliaryData, payloadData);
        case WM_PAINT: return callEvent<&Window::onWindowRefresh>(windowHandle, message, auxiliaryData, payloadData);
        case WM_DPICHANGED: return callEvent<&Window::onWindowContentScaleChanged>(windowHandle, message, auxiliaryData, payloadData);

        // Mouse events
        case WM_MOUSEMOVE: return callEvent<&Window::onMouseMoved>(windowHandle, message, auxiliaryData, payloadData);
        case WM_MOUSELEAVE: return callEvent<&Window::onMouseLeft>(windowHandle, message, auxiliaryData, payloadData);
        
        // Raw input
        case WM_INPUT: return callEvent<&Window::onRawInput>(windowHandle, message, auxiliaryData, payloadData);

        default: 
            Window* window = getThisPointer(windowHandle);
            if (window)
                window->m_windowEvents.emit<WindowEvents::Native>(windowHandle, message, auxiliaryData, payloadData);
            return DefWindowProc(reinterpret_cast<HWND>(windowHandle), message, auxiliaryData, payloadData);
        }
    }

    LRESULT Window::onNull(WPARAM, LPARAM)
    {
        return 0;
    }

    LRESULT Window::onCreate(WindowHandle hwnd, WPARAM, LPARAM lParam)
    {
        ++s_windowCounter;
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        Window* window = static_cast<Window*>(cs->lpCreateParams);
        window->m_windowHandle = reinterpret_cast<WindowHandle>(hwnd);
        window->setThisPointer();
        return 0;
    }

    LRESULT Window::onDestroy(WPARAM, LPARAM)
    {
        m_windowHandle = nullptr;
        --s_windowCounter;
        if(s_windowCounter == 0)
            PostQuitMessage(0);
        return 0;
    }

    LRESULT Window::onWindowResized(WPARAM wParam, LPARAM lParam)
    {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        switch (wParam)
        {
        case SIZE_RESTORED:
            m_windowEvents.emit<WindowEvents::WindowResized>(width, height); return 0;
        case SIZE_MINIMIZED:
            m_windowEvents.emit<WindowEvents::WindowMinimized>(width, height); return 0;
        case SIZE_MAXIMIZED:
            m_windowEvents.emit<WindowEvents::WindowMaximized>(width, height); return 0;
        default: return 0;
        }
        return 0;
    }

    LRESULT Window::onWindowMoved(WPARAM, LPARAM lParam)
    {
        int32_t x = static_cast<int16_t>(LOWORD(lParam));
        int32_t y = static_cast<int16_t>(HIWORD(lParam));
        m_windowEvents.emit<WindowEvents::WindowMoved>(x, y);
        return 0;
    }

    LRESULT Window::onWindowFocused(WPARAM, LPARAM)
    {
        m_windowEvents.emit<WindowEvents::WindowFocused>();
        return 0;
    }

    LRESULT Window::onWindowUnfocused(WPARAM, LPARAM)
    {
        m_windowEvents.emit<WindowEvents::WindowUnfocused>();
        return 0;
    }

    LRESULT Window::onWindowClosed(WPARAM, LPARAM)
    {
        m_shouldClose = true;
        m_windowEvents.emit<WindowEvents::WindowClosed>();
        return 0;
    }

    LRESULT Window::onWindowRefresh(WPARAM, LPARAM)
    {
        HDC hdc = GetDC(reinterpret_cast<HWND>(m_windowHandle));
        ReleaseDC(reinterpret_cast<HWND>(m_windowHandle), hdc);
        ValidateRect(reinterpret_cast<HWND>(m_windowHandle), nullptr);
        m_windowEvents.emit<WindowEvents::WindowRefresh>();
        return 0;
    }

    LRESULT Window::onWindowContentScaleChanged(WPARAM wParam, LPARAM)
    {
        float dpiX = HIWORD(wParam);
        float dpiY = LOWORD(wParam);
        //auto suggestedRect = reinterpret_cast<RECT*>(lParam);
        m_windowEvents.emit<WindowEvents::WindowContentScaleChanged>(dpiX, dpiY);
        return 0;
    }

    LRESULT Window::onMouseMoved(WPARAM, LPARAM payloadData) {
        auto& mouse = Mouse::getInstanceUnsafe();
        m_ClientRelativeMousePosition.x = GET_X_LPARAM(payloadData);
        m_ClientRelativeMousePosition.y = GET_Y_LPARAM(payloadData);
        mouse.m_mousePos = m_ClientRelativeMousePosition;
        ClientToScreen(reinterpret_cast<HWND>(m_windowHandle), reinterpret_cast<POINT*>(&mouse.m_mousePos));
        mouse.m_moved = true;
        m_windowEvents.emit<IOEvents::MouseMovedScreen>(m_ClientRelativeMousePosition);
        return 0;
    }
    
    LRESULT Window::onMouseLeft(WPARAM, LPARAM) {
        m_windowEvents.emit<IOEvents::MouseLeft>();
        return 0;
    }

    LRESULT Window::onRawInput(WPARAM, LPARAM payloadData) {
        UINT size = 0;
        GetRawInputData(
            reinterpret_cast<HRAWINPUT>(payloadData),
            RID_INPUT,
            nullptr,
            &size,
            sizeof(RAWINPUTHEADER));

        m_rawInputBuffer.resize(size);

        if (GetRawInputData(
                reinterpret_cast<HRAWINPUT>(payloadData),
                RID_INPUT,
                m_rawInputBuffer.data(),
                &size,
                sizeof(RAWINPUTHEADER)) != size)
        {
            return 0;
        }

        RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(m_rawInputBuffer.data());

        switch(raw->header.dwType) {
            case RIM_TYPEKEYBOARD: return onKeyboardInput(m_rawInputBuffer);
            case RIM_TYPEMOUSE: return onMouseInput(m_rawInputBuffer);
            default: return 0;
        }
        return 0;
    }

    LRESULT Window::onKeyboardInput(std::vector<uint8_t>& input) {
        RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(input.data());
        const RAWKEYBOARD& kb = raw->data.keyboard;

        bool isE0 = (kb.Flags & RI_KEY_E0) != 0;
        bool isE1 = (kb.Flags & RI_KEY_E1) != 0;
        bool isBreak = (kb.Flags & RI_KEY_BREAK) != 0;
        auto& keyboard = Keyboard::getInstanceUnsafe();
        auto keyCode = Detail::translatePS2Set1ToKey(isE1, isE0, static_cast<uint16_t>(kb.MakeCode));

        if (isBreak)
        {
            keyboard.m_keyStateChanged = true;
            keyboard.m_keyStates.setInputState<KeyboardKeyState::Pressed>(keyCode, false);
            keyboard.m_keyStates.setInputState<KeyboardKeyState::Changed>(keyCode, true);
            m_windowEvents.emit<IOEvents::KeyReleased>(keyCode);
        }
        else
        {
            if(!keyboard.keyPressed(keyCode)) {
                keyboard.m_keyStateChanged = true;
                keyboard.m_keyStates.setInputState<KeyboardKeyState::Pressed>(keyCode, true);
                keyboard.m_keyStates.setInputState<KeyboardKeyState::Changed>(keyCode, true);
                m_windowEvents.emit<IOEvents::KeyPressed>(
                    Detail::translatePS2Set1ToKey(isE1, isE0, static_cast<uint16_t>(kb.MakeCode)));
            }
            else m_windowEvents.emit<IOEvents::KeyRepeated>(
                Detail::translatePS2Set1ToKey(isE1, isE0, static_cast<uint16_t>(kb.MakeCode)));
        }
        return 0;
    }

    LRESULT Window::onMouseInput(std::vector<uint8_t>& input) {
        RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(input.data());
        const RAWMOUSE& mouse = raw->data.mouse;

        auto& mouseState = Mouse::getInstanceUnsafe();
                
        // Mouse movement
        if (mouse.usFlags & MOUSE_MOVE_ABSOLUTE) {
            RECT rect; 
            if (mouse.usFlags & MOUSE_VIRTUAL_DESKTOP) { 
                rect.left = GetSystemMetrics(SM_XVIRTUALSCREEN); 
                rect.top = GetSystemMetrics(SM_YVIRTUALSCREEN); 
                rect.right = GetSystemMetrics(SM_CXVIRTUALSCREEN); 
                rect.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN); 
            } else { 
                rect.left = 0; 
                rect.top = 0; 
                rect.right = GetSystemMetrics(SM_CXSCREEN); 
                rect.bottom = GetSystemMetrics(SM_CYSCREEN);
            }
            static thread_local uint16_t s_storedCursorPosX;
            static thread_local uint16_t s_storedCursorPosY;

            uint16_t newCursorPosX = MulDiv(mouse.lLastX, rect.right, USHRT_MAX) + rect.left; 
            uint16_t newCursorPosY = MulDiv(mouse.lLastY, rect.bottom, USHRT_MAX) + rect.top;
            if(newCursorPosX != s_storedCursorPosX || newCursorPosY != s_storedCursorPosY) {
                auto deltaX = newCursorPosX - s_storedCursorPosX;
                auto deltaY = newCursorPosY - s_storedCursorPosY;
                mouseState.m_mouseDeltaPos = { 
                    mouseState.m_mouseDeltaPos.x + deltaX, 
                    mouseState.m_mouseDeltaPos.y + deltaY};
                m_windowEvents.emit<IOEvents::MouseMovedDelta>(deltaX, deltaY);
                s_storedCursorPosX = newCursorPosX;
                s_storedCursorPosY = newCursorPosY;
            }
        } else if (mouse.lLastX != 0 || mouse.lLastY != 0){
            mouseState.m_mouseDeltaPos = { 
                mouseState.m_mouseDeltaPos.x + mouse.lLastX, 
                mouseState.m_mouseDeltaPos.y + mouse.lLastY};
            m_windowEvents.emit<IOEvents::MouseMovedDelta>(mouse.lLastX, mouse.lLastY);
        }

        // Mouse buttons
        checkMouseButton<IOEvents::MouseLeftButtonPressed, IOEvents::MouseLeftButtonReleased>
        (RI_MOUSE_LEFT_BUTTON_DOWN, RI_MOUSE_LEFT_BUTTON_UP, mouse.usButtonFlags, MouseButton::Lmb);

        checkMouseButton<IOEvents::MouseRightButtonPressed, IOEvents::MouseRightButtonReleased>
        (RI_MOUSE_RIGHT_BUTTON_DOWN, RI_MOUSE_RIGHT_BUTTON_UP, mouse.usButtonFlags, MouseButton::Rmb);

        checkMouseButton<IOEvents::MouseMiddleButtonPressed, IOEvents::MouseMiddleButtonReleased>
        (RI_MOUSE_MIDDLE_BUTTON_DOWN, RI_MOUSE_MIDDLE_BUTTON_UP, mouse.usButtonFlags, MouseButton::Mmb);

        checkMouseButton<IOEvents::MouseButton4Pressed, IOEvents::MouseButton4Released>
        (RI_MOUSE_BUTTON_4_UP, RI_MOUSE_BUTTON_4_DOWN, mouse.usButtonFlags, MouseButton::Button4);

        checkMouseButton<IOEvents::MouseButton5Pressed, IOEvents::MouseButton5Released>
        (RI_MOUSE_BUTTON_5_UP, RI_MOUSE_BUTTON_5_DOWN, mouse.usButtonFlags, MouseButton::Button5);

        // Mouse wheel
        if ((mouse.usButtonFlags & RI_MOUSE_WHEEL) || (mouse.usButtonFlags & RI_MOUSE_HWHEEL))
        {
            short wheelDelta = (short)mouse.usButtonData;
            float scrollDelta = (float)wheelDelta / WHEEL_DELTA;

            if (mouse.usButtonFlags & RI_MOUSE_HWHEEL) // Horizontal
            {
                unsigned long scrollChars = 1; // 1 is the default
                SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &scrollChars, 0);
                scrollDelta *= scrollChars;
                mouseState.m_scrolled = true;
                mouseState.m_scrollOffsets.x += scrollDelta;
                m_windowEvents.emit<IOEvents::MouseWhellScrolled>(scrollDelta, 0, mouseState.m_mousePos);
            }
            else // Vertical
            {
                unsigned long scrollLines = 3; // 3 is the default
                SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &scrollLines, 0);
                if (scrollLines != WHEEL_PAGESCROLL)
                    scrollDelta *= scrollLines;
                mouseState.m_scrollOffsets.y += scrollDelta;
                m_windowEvents.emit<IOEvents::MouseWhellScrolled>(0, scrollDelta, mouseState.m_mousePos);
            }
        }
        return 0;
    }

    LRESULT Window::onWindowStartResizing(WPARAM auxiliaryData, LPARAM payloadData) {
        m_windowEvents.emit<WindowEvents::WindowStartResizing>();
        return 0;
    }
    LRESULT Window::onWindowStopResizing(WPARAM auxiliaryData, LPARAM payloadData) {
        m_windowEvents.emit<WindowEvents::WindowStopResizing>();
        return 0;
    }
    LRESULT Window::onWindowResizing(WPARAM auxiliaryData, LPARAM payloadData) {
        auto resizeEdge = static_cast<ResizeEdge>(auxiliaryData);
        auto* rect = reinterpret_cast<Rectangle*>(payloadData);
        m_windowEvents.emit<WindowEvents::WindowBeingResized>(resizeEdge, rect);
        return 0;
    }

    bool Window::shouldClose() const {
        return m_shouldClose;
    }

    Position Window::getCursorPos() {
        if (GetCursorPos(reinterpret_cast<POINT*>(&m_ClientRelativeMousePosition))) {
            ScreenToClient(reinterpret_cast<HWND>(m_windowHandle), reinterpret_cast<POINT*>(&m_ClientRelativeMousePosition));
            return m_ClientRelativeMousePosition;
        }
        return m_ClientRelativeMousePosition;
    }

    Position Window::getGlobalCursorPos()
    {
        auto& mouse = Mouse::getInstanceUnsafe();
        GetCursorPos(reinterpret_cast<POINT*>(&mouse.m_mousePos));
        return mouse.m_mousePos;
    }

    void Window::setCursorPos(Position pos)
    {
        SetCursorPos(pos.x, pos.y);
        auto& mouse = Mouse::getInstanceUnsafe();
        mouse.m_mousePos = pos;
    }

    bool Window::isMinimised()
    {
        if (!m_windowHandle) return false;
        return IsIconic(reinterpret_cast<HWND>(m_windowHandle));
    }

    bool Window::isMaximised()
    {
        if (!m_windowHandle) return false;
        return IsZoomed(reinterpret_cast<HWND>(m_windowHandle)) != FALSE;
    }

    void Window::setResizable(bool resizable)
    {
        if (!m_windowHandle) return;
        LONG style = GetWindowLong(reinterpret_cast<HWND>(m_windowHandle), GWL_STYLE);
        if (resizable) {
            style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
        }
        else {
            style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
        }
        SetWindowLong(reinterpret_cast<HWND>(m_windowHandle), GWL_STYLE, style);
        // Force redraw
        SetWindowPos(reinterpret_cast<HWND>(m_windowHandle), nullptr, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }

    void Window::setVisible(bool visible)
    {
        if (!m_windowHandle) return;
        ShowWindow(reinterpret_cast<HWND>(m_windowHandle), visible ? SW_SHOW : SW_HIDE);
    }

    void Window::setDecorated(bool decorated)
    {
        if (!m_windowHandle) return;

        LONG style = GetWindowLong(reinterpret_cast<HWND>(m_windowHandle), GWL_STYLE);
        if (decorated) {
            style |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        }
        else {
            style &= ~(WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME | WS_MAXIMIZEBOX);
        }
        SetWindowLong(reinterpret_cast<HWND>(m_windowHandle), GWL_STYLE, style);

        SetWindowPos(reinterpret_cast<HWND>(m_windowHandle), nullptr, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }

    void Window::setFocused(bool focused)
    {
        if (!m_windowHandle) return;

        if (focused) {
            SetFocus(reinterpret_cast<HWND>(m_windowHandle));
            SetForegroundWindow(reinterpret_cast<HWND>(m_windowHandle));
        }
        // Note: There's no direct way to "unfocus" a window in Win32
        // The window loses focus when another window is activated
    }

    void Window::setAutoIconify(bool autoIconify)
    {
        (void)autoIconify;
        // Win32 automatically handles iconification on focus loss by default
        // This is mainly for API compatibility with GLFW
    }

    void Window::setFloating(bool floating)
    {
        if (!m_windowHandle) return;

        SetWindowPos(reinterpret_cast<HWND>(m_windowHandle),
            floating ? HWND_TOPMOST : HWND_NOTOPMOST,
            0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }

    void Window::setMaximized(bool maximized)
    {
        if (!m_windowHandle) return;
        ShowWindow(reinterpret_cast<HWND>(m_windowHandle), maximized ? SW_MAXIMIZE : SW_RESTORE);
    }

    void Window::centerCursor(bool center)
    {
        m_attr.centerCursor = center;
        if (m_attr.centerCursor) {
            centerCursor();
        }
    }

    void Window::setTransparentFramebuffer(bool transparent)
    {
        if (!m_windowHandle) return;

        if (transparent) {
            // Enable layered window for transparency
            SetWindowLong(reinterpret_cast<HWND>(m_windowHandle), GWL_EXSTYLE,
                GetWindowLong(reinterpret_cast<HWND>(m_windowHandle), GWL_EXSTYLE) | WS_EX_LAYERED);
            // Set to fully opaque by default
            SetLayeredWindowAttributes(reinterpret_cast<HWND>(m_windowHandle), 0, 255, LWA_ALPHA);
        }
        else {
            // Disable layered window
            SetWindowLong(reinterpret_cast<HWND>(m_windowHandle), GWL_EXSTYLE,
                GetWindowLong(reinterpret_cast<HWND>(m_windowHandle), GWL_EXSTYLE) & ~WS_EX_LAYERED);
        }
    }

    void Window::setFocusOnShow(bool focusOnShow)
    {
        m_attr.focusOnShow = focusOnShow;
        // This controls behavior when showing the window
        // We'll handle this in the showWindow/createWindow methods
    }

    void Window::setCursorMode(CursorMode mode)
    {
        m_attr.cursorMode = mode;
        updateCursorState();
    }

    void Window::updateCursorState()
    {
        if (!m_windowHandle) return;

        switch (m_attr.cursorMode) {
        case CursorMode::Normal:
            ShowCursor(TRUE);
            // Release cursor confinement
            ClipCursor(nullptr);
            break;

        case CursorMode::Hidden:
            ShowCursor(FALSE);
            ClipCursor(nullptr);
            break;

        case CursorMode::Disabled:
            ShowCursor(FALSE);
            // Confine cursor to window for raw motion input
            RECT rect;
            GetClientRect(reinterpret_cast<HWND>(m_windowHandle), &rect);
            MapWindowPoints(reinterpret_cast<HWND>(m_windowHandle), nullptr, (POINT*)(&rect), 2);
            ClipCursor(&rect);
            break;
        }
    }

    void Window::centerCursor()
    {
        if (!m_windowHandle) return;

        RECT rect;
        GetClientRect(reinterpret_cast<HWND>(m_windowHandle), &rect);

        POINT center;
        center.x = rect.left + (rect.right - rect.left) / 2;
        center.y = rect.top + (rect.bottom - rect.top) / 2;

        ClientToScreen(reinterpret_cast<HWND>(m_windowHandle), &center);
        SetCursorPos(center.x, center.y);
    }

    bool Window::isResizable() const
    {
        if (!m_windowHandle) return false;
        return (GetWindowLong(reinterpret_cast<HWND>(m_windowHandle), GWL_STYLE) & WS_THICKFRAME) != 0;
    }

    bool Window::isVisible() const
    {
        if (!m_windowHandle) return false;
        return IsWindowVisible(reinterpret_cast<HWND>(m_windowHandle)) != FALSE;
    }

    bool Window::isDecorated() const
    {
        if (!m_windowHandle) return false;
        return (GetWindowLong(reinterpret_cast<HWND>(m_windowHandle), GWL_STYLE) & WS_CAPTION) != 0;
    }

    bool Window::isFocused() const
    {
        if (!m_windowHandle) return false;
        return GetForegroundWindow() == reinterpret_cast<HWND>(m_windowHandle);
    }

    bool Window::isFloating() const
    {
        if (!m_windowHandle) return false;
        return (GetWindowLong(reinterpret_cast<HWND>(m_windowHandle), GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    }

    bool Window::isTransparentFramebuffer() const
    {
        if (!m_windowHandle) return false;
        return (GetWindowLong(reinterpret_cast<HWND>(m_windowHandle), GWL_EXSTYLE) & WS_EX_LAYERED) != 0;
    }

    Extent Window::getWindowExtent() const
    {
        if (!m_windowHandle) return { 0, 0 };

        RECT rect;
        if (GetWindowRect(reinterpret_cast<HWND>(m_windowHandle), &rect)) {
            return {
                rect.right - rect.left,  // Total window width
                rect.bottom - rect.top   // Total window height
            };
        }
        return { 0, 0 };
    }

    Extent Window::getClientExtent() const
    {
        if (!m_windowHandle) return { 0, 0 };

        RECT rect;
        if (GetClientRect(reinterpret_cast<HWND>(m_windowHandle), &rect)) {
            return {
                rect.right - rect.left,  // Client area width
                rect.bottom - rect.top   // Client area height
            };
        }
        return { 0, 0 };
    }

    Extent Window::getFrameBufferExtent() const
    {
        if (!m_windowHandle) return { 0, 0 };

        // For most cases, framebuffer size = client area size
        // But with DPI scaling, they might differ
        RECT rect;
        if (GetClientRect(reinterpret_cast<HWND>(m_windowHandle), &rect)) {
            // Convert to pixels accounting for DPI scaling
            float dpiScale = getDPIScale();
            return {
                static_cast<int32_t>((rect.right - rect.left) * dpiScale),
                static_cast<int32_t>((rect.bottom - rect.top) * dpiScale)
            };
        }
        return { 0, 0 };
    }

    Position Window::getWindowPosition() const
    {
        if (!m_windowHandle) return { 0, 0 };

        RECT rect;
        if (GetWindowRect(reinterpret_cast<HWND>(m_windowHandle), &rect)) {
            return { rect.left, rect.top };
        }
        return { 0, 0 };
    }

    float Window::getDPIScale() const
    {
        if (!m_windowHandle) return 1.0f;

        // Modern DPI awareness (Windows 10+)
        HMODULE user32 = LoadLibraryA("user32.dll");
        if (user32) {
            typedef UINT(WINAPI* GetDpiForWindowFn)(HWND);
            auto getDpiForWindow = reinterpret_cast<GetDpiForWindowFn>(reinterpret_cast<void*>(
                GetProcAddress(user32, "GetDpiForWindow")));

            if (getDpiForWindow) {
                UINT dpi = getDpiForWindow(reinterpret_cast<HWND>(m_windowHandle));
                FreeLibrary(user32);
                return dpi / 96.0f;  // 96 DPI = 100% scale
            }
            FreeLibrary(user32);
        }

        // Fallback for older Windows versions
        HDC hdc = GetDC(reinterpret_cast<HWND>(m_windowHandle));
        if (hdc) {
            int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
            ReleaseDC(reinterpret_cast<HWND>(m_windowHandle), hdc);
            return dpiX / 96.0f;
        }

        return 1.0f;
    }

}
