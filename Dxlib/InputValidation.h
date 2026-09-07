#pragma once
#include"Dxlib.h"
#include <array>

class InputValidation
{
public:
    enum class Mouse
    {
        Left = MOUSE_INPUT_LEFT,
        Right = MOUSE_INPUT_RIGHT,
        Middle = MOUSE_INPUT_MIDDLE
    };
    static InputValidation& Instance()
    {
        static InputValidation instance;
        return instance;
    }
    void ValidateInputUpdate();

    // --- キーボード入力 ---
    bool IsKeyPressed(int keyCode) const;     // 押しっぱなし
    bool IsKeyTrigger(int keyCode) const;    // 押された瞬間
    bool IsKeyReleased(int keyCode) const;   // 離された瞬間

    // --- マウス入力 ---
    bool IsMousePressed(Mouse button) const;   // 押しっぱなし
    bool IsMouseTrigger(Mouse button) const;  // 押された瞬間
    bool IsMouseReleased(Mouse button) const; // 離された瞬間
	void GetMousePosition(int& x, int& y) const;// マウス座標を取得

    // --- コントローラー入力 (XInput) ---
    bool IsButtonPressed(int xinputButton) const;   // 押しっぱなし
    bool IsButtonTrigger(int xinputButton) const;  // 押された瞬間
    bool IsButtonReleased(int xinputButton) const; // 離された瞬間
	// スティックの値を取得（デッドゾーンを考慮）
	void GetLeftStick(float& x, float& y, float deadzone = 0.2f) const;// デッドゾーンを考慮して左スティックの値を取得
	void GetRightStick(float& x, float& y, float deadzone = 0.2f) const;// デッドゾーンを考慮して右スティックの値を取得
    
private:
    // キーボード状態 (DxLibは256要素のchar配列を使用)
    std::array<char, 256> currentKeyStates{};
    std::array<char, 256> previousKeyStates{};

    // マウス状態
    int currentMouseState = 0;
    int previousMouseState = 0;
    int mouseX = 0;
    int mouseY = 0;

    // コントローラー状態
    XINPUT_STATE currentJoypadState{};
    XINPUT_STATE previousJoypadState{};
    bool isControllerConnected = false;
};