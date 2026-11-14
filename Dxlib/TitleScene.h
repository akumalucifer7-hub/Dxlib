#pragma once
#include "Winmain.h"

class InputValidation;

class TitleScene
{
public:
    TitleScene() = default;

    void Init(); // ★追加: カーソル位置のリセット用
    ModeState Update(const InputValidation& input);
    void Draw(int screenWidth, int screenHeight) const;

private:
    int selectedIndex = 0; // ★追加: 0ならスタート, 1なら終了
};