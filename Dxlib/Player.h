#pragma once
#include "Character.h"
#include "PlayerCollision.h"
#include"Weapon.h"
class Player : public Character
{
public:

    void Init(const GameContext& context) override;
    void Update(const GameContext& context) override;

	void  AttatchItems();
	void FrameLotate();

	void Draw() override;
	void DrawDebug() const;
    AABB GetHurtboxAABB() const override { return playercollision.GetHurtboxAABB(pos, angle); }
    bool GetCurrentAttackAABB(AABB& outAABB) const override { return playercollision.GetCurrentAttackAABB(*this, outAABB); }
    void OnTakeDamage() override;
	
private:
    PlayerCollision playercollision;
	Weapon SwordItems;
	Weapon GunItemsRight;
	Weapon GunItemsLeft;


};