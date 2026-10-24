#pragma once
#include "Character.h"
#include"Weapon.h"
class Player : public Character
{
public:

    void Init(const GameContext& context) override;
    void Update(const GameContext& context) override;

	void  AttatchItems() override;

	void Draw() override;
	void DrawDebug() override;


    void OnTakeDamage(const GameContext& context) override;
	
private:
 
	Weapon SwordItems;
	Weapon GunItemsRight;
	Weapon GunItemsLeft;


};