#pragma once
#include "PlatformKit/Structs.h"
#include "PlatformKit/IOEvents.h"
#include "PlatformKit/WindowEvents.h"
#include "PlatformKit/InputStateTracker.h"

// Keyboard is a mayers singleton, can only be obtained through a window
namespace PlatformKit {
#ifdef _WIN32
	namespace Win32 {
		class Window;
	}
	class Keyboard
	{
	friend class Win32::Window;
#else
	
#endif
	private:
		using KeyStates = PlatformKit::InputStateTracker<KeyboardKey, static_cast<size_t>(KeyboardKey::Count),
			KeyboardKeyState, static_cast<size_t>(KeyboardKeyState::Count)>;

		KeyStates m_keyStates;
		bool m_keyStateChanged = false;

		static inline Keyboard* thisPointer;
		
		Keyboard() {
			thisPointer = this;
		}
		~Keyboard() = default;

		Keyboard(const Keyboard&) = delete;
		Keyboard(Keyboard&&) = delete;

		Keyboard& operator=(const Keyboard&) = delete;
		Keyboard& operator=(Keyboard&&) = delete;

		static Keyboard& getInstance() {
			static Keyboard keyboard;
			return keyboard;
		}

		static Keyboard& getInstanceUnsafe() {
			return *thisPointer;
		}
		
		void refreshState()
		{
			m_keyStateChanged = false;
			for (size_t i = 0; i < Detail::s_keyCodeEnumValues.size(); ++i)
				m_keyStates.setInputState<KeyboardKeyState::Changed>(static_cast<KeyboardKey>(Detail::s_keyCodeEnumValues[i]), false);
		}

	public:	
		
		template<KeyboardKey E>
		bool keyJustPressed() const
		{
			return m_keyStates.getInputState<E, KeyboardKeyState::Pressed>() && 
			m_keyStates.getInputState<E, KeyboardKeyState::Changed>();
		}

		bool keyJustPressed(KeyboardKey key) const
		{
			return m_keyStates.getInputState<KeyboardKeyState::Pressed>(key) && 
				m_keyStates.getInputState<KeyboardKeyState::Changed>(key);
		}

		template<KeyboardKey E>
		bool keyJustReleased() const
		{
			return !m_keyStates.getInputState<E, KeyboardKeyState::Pressed>() && 
				m_keyStates.getInputState<E, KeyboardKeyState::Changed>();
		}

		bool keyJustReleased(KeyboardKey key) const
		{
			return !m_keyStates.getInputState<KeyboardKeyState::Pressed>(key) && 
				m_keyStates.getInputState<KeyboardKeyState::Changed>(key);
		}

		template<KeyboardKey E>
		bool keyPressed() const
		{
			return m_keyStates.getInputState<E, KeyboardKeyState::Pressed>();
		}

		bool keyPressed(KeyboardKey key) const
		{
			return m_keyStates.getInputState<KeyboardKeyState::Pressed>(key);
		}

		template<KeyboardKey E>
		bool keyReleased() const
		{
			return !m_keyStates.getInputState<E, KeyboardKeyState::Pressed>();
		}

		bool keyReleased(KeyboardKey key) const
		{
			return !m_keyStates.getInputState<KeyboardKeyState::Pressed>(key);
		}
	};
}