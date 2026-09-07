#pragma once
#include"GameObject.h"

class Weapon : public GameObject
{
	void Init(const GameContext& context)override;
	void Update(const GameContext& context)override;
	void Draw()override;
	~Weapon() {};
};