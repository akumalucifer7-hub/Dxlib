#pragma once

#include "DxLib.h"
#include "GameScene.h"
#include "InputValidation.h"

// シーン（モード）状態の定義
enum class ModeState
{
    TitleMode,
    GameMode,
    GameOverMode
};