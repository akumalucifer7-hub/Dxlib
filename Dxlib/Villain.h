#pragma once
#include "Character.h"
#include"weapon.h"
class Villain : public Character
{
public:
    void Init(const GameContext& context) override;
    void Update(const GameContext& context) override;
    void AttatchItems() override;
	void AttackMovement(const GameContext& context);
	void Draw() override;
	void DrawDebug() override;
    void OnTakeDamage(const GameContext& context) override;

private:
    Weapon weapon1;
	Weapon weapon2;
};