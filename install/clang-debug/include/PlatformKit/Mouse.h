#pragma once
#include "PlatformKit/Structs.h"
#include "PlatformKit/IOEvents.h"
#include "PlatformKit/WindowEvents.h"
#include "PlatformKit/InputStateTracker.h"

 // Mouse is a mayers singleton, can only be obtained through a window
namespace PlatformKit {
#ifdef _WIN32
	namespace Win32 {
		class Window;
	}
	class Mouse
	{
	friend class Win32::Window;
#else
	
#endif
	private:
		using ButtonStates = PlatformKit::InputStateTracker<MouseButton, static_cast<size_t>(MouseButton::Count),
			MouseButtonState, static_cast<size_t>(MouseButtonState::Count)>;

		ButtonStates m_buttonStates;
		Position m_mousePos = { 0, 0 };
		Position m_scrollOffsets = { 0, 0 };
		Position m_mouseDeltaPos = { 0, 0 };

		bool m_moved = false;
		bool m_scrolled = false;
		bool m_buttonStateChanged = false;

		static inline Mouse* thisPointer;
		
		Mouse() {
			thisPointer = this;
		}
		~Mouse() = default;

		Mouse(const Mouse&) = delete;
		Mouse(Mouse&&) = delete;

		Mouse& operator=(const Mouse&) = delete;
		Mouse& operator=(Mouse&&) = delete;

		static Mouse& getInstance() {
			static Mouse mouse;
			return mouse;
		}

		static Mouse& getInstanceUnsafe() {
			return *thisPointer;
		}
		
		void refreshState()
		{
			m_moved = false;
			m_scrolled = false;
			m_buttonStateChanged = false;
			m_scrollOffsets = { 0, 0 };
			m_mouseDeltaPos = { 0, 0 };
			for (size_t i = 0; i < static_cast<size_t>(MouseButton::Count); ++i)
				m_buttonStates.setInputState<MouseButtonState::Changed>(static_cast<MouseButton>(i), false);
		}

	public:	
		
		template<MouseButton E>
		bool buttonJustPressed() const
		{
			return m_buttonStates.getInputState<E, MouseButtonState::Pressed>() && 
			m_buttonStates.getInputState<E, MouseButtonState::Changed>();
		}

		bool buttonJustPressed(MouseButton Button) const
		{
			return m_buttonStates.getInputState<MouseButtonState::Pressed>(Button) && 
				m_buttonStates.getInputState<MouseButtonState::Changed>(Button);
		}

		template<MouseButton E>
		bool buttonJustReleased() const
		{
			return !m_buttonStates.getInputState<E, MouseButtonState::Pressed>() && 
				m_buttonStates.getInputState<E, MouseButtonState::Changed>();
		}

		bool buttonJustReleased(MouseButton Button) const
		{
			return !m_buttonStates.getInputState<MouseButtonState::Pressed>(Button) && 
				m_buttonStates.getInputState<MouseButtonState::Changed>(Button);
		}

		template<MouseButton E>
		bool buttonPressed() const
		{
			return m_buttonStates.getInputState<E, MouseButtonState::Pressed>();
		}

		bool buttonPressed(MouseButton Button) const
		{
			return m_buttonStates.getInputState<MouseButtonState::Pressed>(Button);
		}

		template<MouseButton E>
		bool buttonReleased() const
		{
			return !m_buttonStates.getInputState<E, MouseButtonState::Pressed>();
		}

		bool buttonReleased(MouseButton Button) const
		{
			return !m_buttonStates.getInputState<MouseButtonState::Pressed>(Button);
		}

		const Position& getMousePos() const { return m_mousePos; };
		const Position& getMouseDeltaPos() const { return m_mouseDeltaPos; };
		const Position& getScrollOffsets() const { return m_scrollOffsets; };

		bool moved() const { return m_moved; };
	};
}