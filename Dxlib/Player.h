#pragma once
#include "Character.h"
#include "PlayerCollision.h"
#include"Weapon.h"
class Player : public Character
{
public:
    enum StateID { Idle, Run, Jump, Fall, Attack1, Evade, MAX };

    void Init(const GameContext& context) override;
    void Update(const GameContext& context) override;

    AABB GetHurtboxAABB() const override { return collision.GetHurtboxAABB(pos, angle); }
    bool GetCurrentAttackAABB(AABB& outAABB) const override { return collision.GetCurrentAttackAABB(*this, outAABB); }
    void OnTakeDamage() override;

private:
    PlayerCollision collision;
	Weapon SwordItems;
	Weapon GunItemsRight;
	Weapon GunItemsLeft;
};