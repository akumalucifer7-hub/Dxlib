#pragma once
#include"Dxlib.h"
#include "Gamecontext.h"
class UserInterface
{
public:
	void InitUI(const GameContext& context);
	void UpdateUI(const GameContext& context);
	void DrawUI(const GameContext& context);
private:
	
};