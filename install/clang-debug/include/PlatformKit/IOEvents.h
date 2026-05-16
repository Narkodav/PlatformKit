#pragma once
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <array>

#include "PlatformKit/Structs.h"

namespace PlatformKit
{
    // Mirrors HID usage codes, de facto native standard for modern keyboards, QWERTY layout
    enum class KeyboardKey : uint16_t
    {
        None = 0x00, // No key pressed / reserved

        // --- Letters (0x04–0x1D) ---
        A = 0x04, B = 0x05, C = 0x06, D = 0x07,
        E = 0x08, F = 0x09, G = 0x0A, H = 0x0B,
        I = 0x0C, J = 0x0D, K = 0x0E, L = 0x0F,
        M = 0x10, N = 0x11, O = 0x12, P = 0x13,
        Q = 0x14, R = 0x15, S = 0x16, T = 0x17,
        U = 0x18, V = 0x19, W = 0x1A, X = 0x1B,
        Y = 0x1C, Z = 0x1D,

        // --- Numbers (0x1E–0x27) ---
        Digit1 = 0x1E, Digit2 = 0x1F, Digit3 = 0x20,
        Digit4 = 0x21, Digit5 = 0x22, Digit6 = 0x23,
        Digit7 = 0x24, Digit8 = 0x25, Digit9 = 0x26,
        Digit0 = 0x27,

        // --- Control / whitespace (0x28–0x2C) ---
        Enter = 0x28,
        Escape = 0x29,
        Backspace = 0x2A,
        Tab = 0x2B,
        Space = 0x2C,

        // --- Symbols / punctuation (0x2D–0x38) ---
        Minus = 0x2D,
        Equal = 0x2E,
        LeftBracket = 0x2F,
        RightBracket = 0x30,
        Backslash = 0x31,
        NonUsHash = 0x32,       // ISO-specific
        Semicolon = 0x33,
        Apostrophe = 0x34,
        Grave = 0x35,
        Comma = 0x36,
        Period = 0x37,
        Slash = 0x38,

        // --- Lock keys (0x39) ---
        CapsLock = 0x39,

        // --- Function keys (0x3A–0x45) ---
        F1 = 0x3A,  F2 = 0x3B,  F3 = 0x3C,  F4 = 0x3D,
        F5 = 0x3E,  F6 = 0x3F,  F7 = 0x40,  F8 = 0x41,
        F9 = 0x42,  F10 = 0x43, F11 = 0x44, F12 = 0x45,

        // --- System / navigation keys (0x46–0x52) ---
        PrintScreen = 0x46,
        ScrollLock = 0x47,
        Pause = 0x48,
        Insert = 0x49,
        Home = 0x4A,
        PageUp = 0x4B,
        Delete = 0x4C,
        End = 0x4D,
        PageDown = 0x4E,
        Right = 0x4F,
        Left = 0x50,
        Down = 0x51,
        Up = 0x52,

        // --- Numpad / keypad keys (0x53–0x63) ---
        NumLock = 0x53,
        NumpadDivide = 0x54,
        NumpadMultiply = 0x55,
        NumpadMinus = 0x56,
        NumpadPlus = 0x57,
        NumpadEnter = 0x58,
        Numpad1 = 0x59, Numpad2 = 0x5A, Numpad3 = 0x5B,
        Numpad4 = 0x5C, Numpad5 = 0x5D, Numpad6 = 0x5E,
        Numpad7 = 0x5F, Numpad8 = 0x60, Numpad9 = 0x61,
        Numpad0 = 0x62,
        NumpadDecimal = 0x63,

        // --- International / ISO keys (0x64–0x87) ---
        NonUsBackslash = 0x64,  // ISO-only
        Application = 0x65,     // Menu key
        Power = 0x66,
        NumpadEqual = 0x67,
        F13 = 0x68, F14 = 0x69, F15 = 0x6A, F16 = 0x6B,
        F17 = 0x6C, F18 = 0x6D, F19 = 0x6E, F20 = 0x6F,
        F21 = 0x70, F22 = 0x71, F23 = 0x72, F24 = 0x73,
        Execute = 0x74,
        Help = 0x75,
        Menu = 0x76,
        Select = 0x77,
        Stop = 0x78,
        Again = 0x79,
        Undo = 0x7A,
        Cut = 0x7B,
        Copy = 0x7C,
        Paste = 0x7D,
        Find = 0x7E,
        Mute = 0x7F,
        VolumeUp = 0x80,
        VolumeDown = 0x81,
        LockingCapsLock = 0x82,
        LockingNumLock = 0x83,
        LockingScrollLock = 0x84,
        NumpadComma = 0x85,
        NumpadEqualSign = 0x86,
        International1 = 0x87,  // ISO-specific
        International2 = 0x88,
        International3 = 0x89,
        International4 = 0x8A,
        International5 = 0x8B,
        International6 = 0x8C,
        International7 = 0x8D,
        International8 = 0x8E,
        International9 = 0x8F,

        // --- Language / Locale keys (0x90–0x98) ---
        LANG1 = 0x90, LANG2 = 0x91, LANG3 = 0x92, LANG4 = 0x93,
        LANG5 = 0x94, LANG6 = 0x95, LANG7 = 0x96, LANG8 = 0x97,
        LANG9 = 0x98,

        // --- Editing / system keys (0x99–0xA4) ---
        AlternateErase = 0x99,
        SysReq = 0x9A,
        Cancel = 0x9B,
        Clear = 0x9C,
        Prior = 0x9D,
        Return = 0x9E,
        Separator = 0x9F,
        Out = 0xA0,
        Oper = 0xA1,
        ClearAgain = 0xA2,
        CrSel = 0xA3,
        ExSel = 0xA4,

        // --- Modifiers (0xE0–0xE7) ---
        LeftControl = 0xE0,
        LeftShift   = 0xE1,
        LeftAlt     = 0xE2,
        LeftGui     = 0xE3,   // Windows / Command key
        RightControl= 0xE4,
        RightShift  = 0xE5,
        RightAlt    = 0xE6,
        RightGui    = 0xE7,   // Windows / Command key
        Count
    };

    namespace Detail {
        static constexpr std::array<KeyboardKey, 172> s_keyCodeEnumValues = {
            KeyboardKey::A,                  // 0x04
            KeyboardKey::B,                  // 0x05
            KeyboardKey::C,                  // 0x06
            KeyboardKey::D,                  // 0x07
            KeyboardKey::E,                  // 0x08
            KeyboardKey::F,                  // 0x09
            KeyboardKey::G,                  // 0x0A
            KeyboardKey::H,                  // 0x0B
            KeyboardKey::I,                  // 0x0C
            KeyboardKey::J,                  // 0x0D
            KeyboardKey::K,                  // 0x0E
            KeyboardKey::L,                  // 0x0F
            KeyboardKey::M,                  // 0x10
            KeyboardKey::N,                  // 0x11
            KeyboardKey::O,                  // 0x12
            KeyboardKey::P,                  // 0x13
            KeyboardKey::Q,                  // 0x14
            KeyboardKey::R,                  // 0x15
            KeyboardKey::S,                  // 0x16
            KeyboardKey::T,                  // 0x17
            KeyboardKey::U,                  // 0x18
            KeyboardKey::V,                  // 0x19
            KeyboardKey::W,                  // 0x1A
            KeyboardKey::X,                  // 0x1B
            KeyboardKey::Y,                  // 0x1C
            KeyboardKey::Z,                  // 0x1D
            KeyboardKey::Digit1,             // 0x1E
            KeyboardKey::Digit2,             // 0x1F
            KeyboardKey::Digit3,             // 0x20
            KeyboardKey::Digit4,             // 0x21
            KeyboardKey::Digit5,             // 0x22
            KeyboardKey::Digit6,             // 0x23
            KeyboardKey::Digit7,             // 0x24
            KeyboardKey::Digit8,             // 0x25
            KeyboardKey::Digit9,             // 0x26
            KeyboardKey::Digit0,             // 0x27
            KeyboardKey::Enter,              // 0x28
            KeyboardKey::Escape,             // 0x29
            KeyboardKey::Backspace,          // 0x2A
            KeyboardKey::Tab,                // 0x2B
            KeyboardKey::Space,              // 0x2C
            KeyboardKey::Minus,              // 0x2D
            KeyboardKey::Equal,              // 0x2E
            KeyboardKey::LeftBracket,        // 0x2F
            KeyboardKey::RightBracket,       // 0x30
            KeyboardKey::Backslash,          // 0x31
            KeyboardKey::NonUsHash,          // 0x32
            KeyboardKey::Semicolon,          // 0x33
            KeyboardKey::Apostrophe,         // 0x34
            KeyboardKey::Grave,              // 0x35
            KeyboardKey::Comma,              // 0x36
            KeyboardKey::Period,             // 0x37
            KeyboardKey::Slash,              // 0x38
            KeyboardKey::CapsLock,           // 0x39
            KeyboardKey::F1,                 // 0x3A
            KeyboardKey::F2,                 // 0x3B
            KeyboardKey::F3,                 // 0x3C
            KeyboardKey::F4,                 // 0x3D
            KeyboardKey::F5,                 // 0x3E
            KeyboardKey::F6,                 // 0x3F
            KeyboardKey::F7,                 // 0x40
            KeyboardKey::F8,                 // 0x41
            KeyboardKey::F9,                 // 0x42
            KeyboardKey::F10,                // 0x43
            KeyboardKey::F11,                // 0x44
            KeyboardKey::F12,                // 0x45
            KeyboardKey::PrintScreen,        // 0x46
            KeyboardKey::ScrollLock,         // 0x47
            KeyboardKey::Pause,              // 0x48
            KeyboardKey::Insert,             // 0x49
            KeyboardKey::Home,               // 0x4A
            KeyboardKey::PageUp,             // 0x4B
            KeyboardKey::Delete,             // 0x4C
            KeyboardKey::End,                // 0x4D
            KeyboardKey::PageDown,           // 0x4E
            KeyboardKey::Right,              // 0x4F
            KeyboardKey::Left,               // 0x50
            KeyboardKey::Down,               // 0x51
            KeyboardKey::Up,                 // 0x52
            KeyboardKey::NumLock,            // 0x53
            KeyboardKey::NumpadDivide,       // 0x54
            KeyboardKey::NumpadMultiply,     // 0x55
            KeyboardKey::NumpadMinus,        // 0x56
            KeyboardKey::NumpadPlus,         // 0x57
            KeyboardKey::NumpadEnter,        // 0x58
            KeyboardKey::Numpad1,            // 0x59
            KeyboardKey::Numpad2,            // 0x5A
            KeyboardKey::Numpad3,            // 0x5B
            KeyboardKey::Numpad4,            // 0x5C
            KeyboardKey::Numpad5,            // 0x5D
            KeyboardKey::Numpad6,            // 0x5E
            KeyboardKey::Numpad7,            // 0x5F
            KeyboardKey::Numpad8,            // 0x60
            KeyboardKey::Numpad9,            // 0x61
            KeyboardKey::Numpad0,            // 0x62
            KeyboardKey::NumpadDecimal,      // 0x63
            KeyboardKey::NonUsBackslash,     // 0x64
            KeyboardKey::Application,        // 0x65
            KeyboardKey::Power,              // 0x66
            KeyboardKey::NumpadEqual,        // 0x67
            KeyboardKey::F13,                // 0x68
            KeyboardKey::F14,                // 0x69
            KeyboardKey::F15,                // 0x6A
            KeyboardKey::F16,                // 0x6B
            KeyboardKey::F17,                // 0x6C
            KeyboardKey::F18,                // 0x6D
            KeyboardKey::F19,                // 0x6E
            KeyboardKey::F20,                // 0x6F
            KeyboardKey::F21,                // 0x70
            KeyboardKey::F22,                // 0x71
            KeyboardKey::F23,                // 0x72
            KeyboardKey::F24,                // 0x73
            KeyboardKey::Execute,            // 0x74
            KeyboardKey::Help,               // 0x75
            KeyboardKey::Menu,               // 0x76
            KeyboardKey::Select,             // 0x77
            KeyboardKey::Stop,               // 0x78
            KeyboardKey::Again,              // 0x79
            KeyboardKey::Undo,               // 0x7A
            KeyboardKey::Cut,                // 0x7B
            KeyboardKey::Copy,               // 0x7C
            KeyboardKey::Paste,              // 0x7D
            KeyboardKey::Find,               // 0x7E
            KeyboardKey::Mute,               // 0x7F
            KeyboardKey::VolumeUp,           // 0x80
            KeyboardKey::VolumeDown,         // 0x81
            KeyboardKey::LockingCapsLock,    // 0x82
            KeyboardKey::LockingNumLock,     // 0x83
            KeyboardKey::LockingScrollLock,  // 0x84
            KeyboardKey::NumpadComma,        // 0x85
            KeyboardKey::NumpadEqualSign,    // 0x86
            KeyboardKey::International1,     // 0x87
            KeyboardKey::International2,     // 0x88
            KeyboardKey::International3,     // 0x89
            KeyboardKey::International4,     // 0x8A
            KeyboardKey::International5,     // 0x8B
            KeyboardKey::International6,     // 0x8C
            KeyboardKey::International7,     // 0x8D
            KeyboardKey::International8,     // 0x8E
            KeyboardKey::International9,     // 0x8F
            KeyboardKey::LANG1,              // 0x90
            KeyboardKey::LANG2,              // 0x91
            KeyboardKey::LANG3,              // 0x92
            KeyboardKey::LANG4,              // 0x93
            KeyboardKey::LANG5,              // 0x94
            KeyboardKey::LANG6,              // 0x95
            KeyboardKey::LANG7,              // 0x96
            KeyboardKey::LANG8,              // 0x97
            KeyboardKey::LANG9,              // 0x98
            KeyboardKey::AlternateErase,     // 0x99
            KeyboardKey::SysReq,             // 0x9A
            KeyboardKey::Cancel,             // 0x9B
            KeyboardKey::Clear,              // 0x9C
            KeyboardKey::Prior,              // 0x9D
            KeyboardKey::Return,             // 0x9E
            KeyboardKey::Separator,          // 0x9F
            KeyboardKey::Out,                // 0xA0
            KeyboardKey::Oper,               // 0xA1
            KeyboardKey::ClearAgain,         // 0xA2
            KeyboardKey::CrSel,              // 0xA3
            KeyboardKey::ExSel,              // 0xA4
            KeyboardKey::LeftControl,        // 0xE0
            KeyboardKey::LeftShift,          // 0xE1
            KeyboardKey::LeftAlt,            // 0xE2
            KeyboardKey::LeftGui,            // 0xE3
            KeyboardKey::RightControl,       // 0xE4
            KeyboardKey::RightShift,         // 0xE5
            KeyboardKey::RightAlt,           // 0xE6
            KeyboardKey::RightGui            // 0xE7
        };

        static constexpr std::array<KeyboardKey, 1024> s_ps2Set1ScanToKey = []{
            std::array<KeyboardKey, 1024> map{};
            map.fill(KeyboardKey::None);

            // --- Normal keys (PS/2 Set1) ---
            map[0x1E] = KeyboardKey::A;
            map[0x30] = KeyboardKey::B;
            map[0x2E] = KeyboardKey::C;
            map[0x20] = KeyboardKey::D;
            map[0x12] = KeyboardKey::E;
            map[0x21] = KeyboardKey::F;
            map[0x22] = KeyboardKey::G;
            map[0x23] = KeyboardKey::H;
            map[0x17] = KeyboardKey::I;
            map[0x24] = KeyboardKey::J;
            map[0x25] = KeyboardKey::K;
            map[0x26] = KeyboardKey::L;
            map[0x32] = KeyboardKey::M;
            map[0x31] = KeyboardKey::N;
            map[0x18] = KeyboardKey::O;
            map[0x19] = KeyboardKey::P;
            map[0x10] = KeyboardKey::Q;
            map[0x13] = KeyboardKey::R;
            map[0x1F] = KeyboardKey::S;
            map[0x14] = KeyboardKey::T;
            map[0x16] = KeyboardKey::U;
            map[0x2F] = KeyboardKey::V;
            map[0x11] = KeyboardKey::W;
            map[0x2D] = KeyboardKey::X;
            map[0x15] = KeyboardKey::Y;
            map[0x2C] = KeyboardKey::Z;

            map[0x02] = KeyboardKey::Digit1;
            map[0x03] = KeyboardKey::Digit2;
            map[0x04] = KeyboardKey::Digit3;
            map[0x05] = KeyboardKey::Digit4;
            map[0x06] = KeyboardKey::Digit5;
            map[0x07] = KeyboardKey::Digit6;
            map[0x08] = KeyboardKey::Digit7;
            map[0x09] = KeyboardKey::Digit8;
            map[0x0A] = KeyboardKey::Digit9;
            map[0x0B] = KeyboardKey::Digit0;

            map[0x1C] = KeyboardKey::Enter;
            map[0x01] = KeyboardKey::Escape;
            map[0x0E] = KeyboardKey::Backspace;
            map[0x0F] = KeyboardKey::Tab;
            map[0x39] = KeyboardKey::Space;

            map[0x0C] = KeyboardKey::Minus;
            map[0x0D] = KeyboardKey::Equal;
            map[0x1A] = KeyboardKey::LeftBracket;
            map[0x1B] = KeyboardKey::RightBracket;
            map[0x2B] = KeyboardKey::Backslash;
            map[0x27] = KeyboardKey::Semicolon;
            map[0x28] = KeyboardKey::Apostrophe;
            map[0x29] = KeyboardKey::Grave;
            map[0x33] = KeyboardKey::Comma;
            map[0x34] = KeyboardKey::Period;
            map[0x35] = KeyboardKey::Slash;

            // --- Modifiers ---
            map[0x2A] = KeyboardKey::LeftShift;
            map[0x36] = KeyboardKey::RightShift;
            map[0x1D] = KeyboardKey::LeftControl;
            map[0x38] = KeyboardKey::LeftAlt;
            map[0x3A] = KeyboardKey::CapsLock;
            
            // --- Function keys ---
            map[0x3B] = KeyboardKey::F1;
            map[0x3C] = KeyboardKey::F2;
            map[0x3D] = KeyboardKey::F3;
            map[0x3E] = KeyboardKey::F4;
            map[0x3F] = KeyboardKey::F5;
            map[0x40] = KeyboardKey::F6;
            map[0x41] = KeyboardKey::F7;
            map[0x42] = KeyboardKey::F8;
            map[0x43] = KeyboardKey::F9;
            map[0x44] = KeyboardKey::F10;
            map[0x57] = KeyboardKey::F11;
            map[0x58] = KeyboardKey::F12;

            // --- Extended keys (E0 prefix) ---
            constexpr uint16_t E0 = 0x100;

            map[E0 | 0x1D] = KeyboardKey::RightControl;
            map[E0 | 0x38] = KeyboardKey::RightAlt;
            map[E0 | 0x47] = KeyboardKey::Home;
            map[E0 | 0x48] = KeyboardKey::Up;
            map[E0 | 0x49] = KeyboardKey::PageUp;
            map[E0 | 0x4B] = KeyboardKey::Left;
            map[E0 | 0x4D] = KeyboardKey::Right;
            map[E0 | 0x4F] = KeyboardKey::End;
            map[E0 | 0x50] = KeyboardKey::Down;
            map[E0 | 0x51] = KeyboardKey::PageDown;
            map[E0 | 0x52] = KeyboardKey::Insert;
            map[E0 | 0x53] = KeyboardKey::Delete;
            map[E0 | 0x1C] = KeyboardKey::NumpadEnter;
            map[E0 | 0x35] = KeyboardKey::NumpadDivide;
            map[E0 | 0x37] = KeyboardKey::PrintScreen;

            // --- unique keys (E1 prefix) ---
            constexpr uint16_t E1 = 0x200;
            map[E1 | 0x45] = KeyboardKey::Pause;

            // --- ISO / international keys (if present on the keyboard) ---
            map[0x56] = KeyboardKey::NonUsHash;     // ISO # / ~ key
            map[0x5D] = KeyboardKey::Application;   // Menu key

            return map;
        }();

        static inline KeyboardKey translatePS2Set1ToKey(bool isE1, bool isE0, uint16_t scancode) {
            scancode |= static_cast<uint16_t>(isE0) << 8;
            scancode |= static_cast<uint16_t>(isE1) << 9;
            return s_ps2Set1ScanToKey[scancode];
        }
    }

    enum class MouseButton
    {
        Lmb,           // Left mouse button
        Rmb,           // Right mouse button
        Mmb,           // Middle mouse button
        Button4,       // Usually Browser Back button
        Button5,       // Usually Browser Forward button

        Button6,       // Additional buttons some mice have, currently not supported
        Button7,
        Button8,

        Unknown,
        Count
    };

    enum class MouseButtonState
    {
        Pressed,
        Changed,
        Count
    };

    enum class KeyboardKeyState
    {
        Pressed,
        Repeated,
        Changed,
        Count
    };

    enum class IOEvents
    {
        KeyPressed,
        KeyReleased,
        KeyRepeated,
        CharInput,

        MouseButtonPressed,
        MouseLeftButtonPressed,
        MouseRightButtonPressed,
        MouseMiddleButtonPressed,

        MouseButton4Pressed,
        MouseButton5Pressed,
        MouseButton6Pressed,
        MouseButton7Pressed,
        MouseButton8Pressed,

        MouseButtonReleased,
        MouseLeftButtonReleased,
        MouseRightButtonReleased,
        MouseMiddleButtonReleased,

        MouseButton4Released,
        MouseButton5Released,
        MouseButton6Released,
        MouseButton7Released,
        MouseButton8Released,

        MouseMovedScreen,   // for when the mouse visible moves on the screen
        MouseMovedDelta,    // for any, even invisible, mouse movement 
        MouseWhellScrolled,
        //MouseEntered,
        MouseLeft,
        Count
    };

    struct IOEventPolicy {
        using Type = IOEvents;

        template<Type T>
        struct Trait {};

        template <Type T>
        static void handleError(std::exception_ptr) {
            std::cerr << "[PlatformKit] Exception in IO event callback: " << Trait<T>::s_name << std::endl;
        }
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::KeyPressed> {
        using Signature = void(KeyboardKey key);
        static inline const std::string s_name = "KeyPressed";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::KeyReleased> {
        using Signature = void(KeyboardKey key);
        static inline const std::string s_name = "KeyReleased";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::KeyRepeated> {
        using Signature = void(KeyboardKey key);
        static inline const std::string s_name = "KeyRepeated";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::CharInput> {
        using Signature = void(unsigned int codepoint, bool isExtendedKey, bool isAltPressed, bool wasDownBefore);
        static inline const std::string s_name = "CharInput";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseButtonPressed> {
        using Signature = void(MouseButton button, Position mousePosition);
        static inline const std::string s_name = "MouseButtonPressed";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseLeftButtonPressed> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseLeftButtonPressed";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseRightButtonPressed> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseRightButtonPressed";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseMiddleButtonPressed> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseMiddleButtonPressed";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseButton4Pressed> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseButton4Pressed";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseButton5Pressed> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseButton5Pressed";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseButton6Pressed> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseButton6Pressed";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseButton7Pressed> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseButton7Pressed";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseButton8Pressed> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseButton8Pressed";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseButtonReleased> {
        using Signature = void(MouseButton button, Position mousePosition);
        static inline const std::string s_name = "MouseButtonReleased";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseLeftButtonReleased> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseLeftButtonReleased";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseRightButtonReleased> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseRightButtonReleased";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseMiddleButtonReleased> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseMiddleButtonReleased";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseButton4Released> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseButton4Released";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseButton5Released> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseButton5Released";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseButton6Released> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseButton6Released";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseButton7Released> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseButton7Released";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseButton8Released> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseButton8Released";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseMovedScreen> {
        using Signature = void(Position mousePosition);
        static inline const std::string s_name = "MouseMovedScreen";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseMovedDelta> {
        using Signature = void(int32_t deltaX, int32_t deltaY);
        static inline const std::string s_name = "MouseMovedDelta";
    };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseWhellScrolled> {
        using Signature = void(int32_t xoffset, int32_t yoffset, Position mousePosition);
        static inline const std::string s_name = "MouseWhellScrolled";
    };

    // template<>
    // struct IOEventPolicy::Trait<IOEvents::MouseEntered> {
    //     using Signature = void(Position mousePosition);
    //     static inline const std::string s_name = "MouseEntered";
    // };

    template<>
    struct IOEventPolicy::Trait<IOEvents::MouseLeft> {
        using Signature = void();
        static inline const std::string s_name = "MouseLeft";
    };
}

