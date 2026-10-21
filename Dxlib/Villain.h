#pragma once
#include "Character.h"
#include "VillainCollision.h"

class Villain : public Character
{
public:
    enum StateID { Idle, Run, Attack1, Damage1, MAX };

    void Init(const GameContext& context) override;
    void Update(const GameContext& context) override;
	void FrameLotate();
    void AttatchItems();
	void AttackMovement(const GameContext& context);
	void Draw() override;
	
    AABB GetHurtboxAABB() const override { return collision.GetHurtboxAABB(pos, angle); }
    bool GetCurrentAttackAABB(AABB& outAABB) const override { return collision.GetCurrentAttackAABB(*this, outAABB); }
    void OnTakeDamage() override;

private:
    VillainCollision collision;
    Weapon weapon1;
	Weapon weapon2;
};