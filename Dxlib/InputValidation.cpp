#include"InputValidation.h"

void InputValidation::ValidateInputUpdate()
{
    previousKeyStates = currentKeyStates;
    previousMouseState = currentMouseState;
    previousJoypadState = currentJoypadState;

    GetHitKeyStateAll(currentKeyStates.data());

    currentMouseState = GetMouseInput();
    GetMousePoint(&mouseX, &mouseY);

    // コントローラー
    if (GetJoypadNum() > 0)
    {
        isControllerConnected = (GetJoypadXInputState(DX_INPUT_PAD1, &currentJoypadState) == double(0));
    }
    else
    {
        isControllerConnected = false;
        currentJoypadState = XINPUT_STATE{}; // 初期化
    }
}

bool InputValidation::IsKeyPressed(int keyCode) const
{
	return currentKeyStates[keyCode] != 0;
}
bool InputValidation::IsKeyTrigger(int keyCode) const
{
	return currentKeyStates[keyCode] != 0 && previousKeyStates[keyCode] == 0;
}
bool InputValidation::IsKeyReleased(int keyCode) const
{
	return currentKeyStates[keyCode] == 0 && previousKeyStates[keyCode] != 0;
}

bool InputValidation::IsMousePressed(Mouse button) const
{
	return (currentMouseState & static_cast<int>(button)) != 0;
}

bool InputValidation::IsMouseTrigger(Mouse button) const
{
	return (currentMouseState & static_cast<int>(button)) != 0 && (previousMouseState & static_cast<int>(button)) == 0;
}

bool InputValidation::IsMouseReleased(Mouse button) const
{
	return (currentMouseState &static_cast<int>(button)) == 0 && (previousMouseState & static_cast<int>(button)) != 0;
}
void InputValidation::GetMousePosition(int& x, int& y) const
{
	x = mouseX;
	y = mouseY;
}
bool InputValidation::IsButtonPressed(int xinputButton) const
{
	return currentJoypadState.Buttons[xinputButton]!=0;
}
bool InputValidation::IsButtonTrigger(int xinputButton) const
{
	return (currentJoypadState.Buttons[xinputButton]) != 0 && (previousJoypadState.Buttons [xinputButton]) == 0;
}
bool InputValidation::IsButtonReleased(int xinputButton) const
{
	return (currentJoypadState.Buttons [xinputButton]) == 0 && (previousJoypadState.Buttons [xinputButton]) != 0;
}
void InputValidation::GetLeftStick(float& x, float& y, float deadzone) const
{
	x = static_cast<float>(currentJoypadState.ThumbLX) / 32768.0f;
	y = static_cast<float>(currentJoypadState.ThumbLY) / 32768.0f;
	// デッドゾーンの適用
	if (std::abs(x) < deadzone) x = 0.0f;
	if (std::abs(y) < deadzone) y = 0.0f;
}

void InputValidation::GetRightStick(float& x, float& y, float deadzone) const
{
	x = static_cast<float>(currentJoypadState.ThumbRX) / 32768.0f;
	y = static_cast<float>(currentJoypadState.ThumbRY) / 32768.0f;
	// デッドゾーンの適用
	if (std::abs(x) < deadzone) x = 0.0f;
	if (std::abs(y) < deadzone) y = 0.0f;
}