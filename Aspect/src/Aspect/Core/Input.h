#pragma once

#include "Aspect/Core/KeyCode.h"
#include "Aspect/Core/Base/Base.h"


namespace Aspect
{

	struct ControllerButtonData
	{
		int Button;
		Key::KeyState State = Key::KeyState::None;
		Key::KeyState OldState = Key::KeyState::None;
	};

	struct Controller
	{
		int ID;
		std::string Name;
		std::map<int, bool> ButtonDown;
		std::map<int, ControllerButtonData> ButtonStates;
		std::map<int, float> AxisStates;
		std::map<int, float> DeadZones;
		std::map<int, uint8_t> HatStates;
	};

	struct KeyData
	{
		KeyCode Key;
		Key::KeyState State = Key::KeyState::None;
		Key::KeyState OldState = Key::KeyState::None;
	};

	struct ButtonData
	{
		Key::MouseButton Button;
		Key::KeyState State = Key::KeyState::None;
		Key::KeyState OldState = Key::KeyState::None;
	};

	class Input
	{
	public:
		static void Update();
		// 内联函数需要在类内定义,这里删除inline
		static bool IsKeyPressed(int keycode);
		static bool IsKeyHeld(KeyCode keycode);
		static bool IsKeyDown(KeyCode keycode);
		static bool IsKeyReleased(KeyCode keycode);
		
		static bool IsMouseButtonPressed(int button);// { return s_Instance->IsMouseButtonPressedImpl(button); }
		static bool IsMouseButtonHeld(Key::MouseButton button);
		static bool IsMouseButtonDown(Key::MouseButton button);
		static bool IsMouseButtonReleased(Key::MouseButton button);
		
		static std::pair<float, float> GetMousePosition();// { return s_Instance->GetMousePositionImpl(); }
		static float GetMouseX();// { return s_Instance->GetMouseXImpl(); }
		static float GetMouseY();// { return s_Instance->GetMouseYImpl(); }
	/*
	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;

		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
	private:
		static Input* s_Instance;*/

	private:
		inline static std::map<KeyCode, KeyData> s_KeyData;
		inline static std::map<Key::MouseButton, ButtonData> s_MouseData;
		inline static std::map<int, Controller> s_Controllers;
		
	};
}
