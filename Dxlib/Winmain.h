#pragma once

#include "DxLib.h"
#include <cmath>
#include <algorithm>
#include"GameScene.h"
#include"InputValidation.h"
GameScene gameScene;
InputValidation& inputValidation = InputValidation::Instance();

enum class ModeState
{
	GameMode,
	TitleMode,
	GameOverMode
};

ModeState modestate = ModeState::GameMode;

InputValidation input;