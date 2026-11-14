#pragma once
#include "Winmain.h"

class InputValidation;

class GameOverScene
{
public:
    GameOverScene() = default;

    void Init();
    ModeState Update(const InputValidation& input);
    void Draw(int screenWidth, int screenHeight) const;

private:
    int selectedIndex = 0; // 0: リトライ(GameMode), 1: タイトルに戻る(TitleMode)
};