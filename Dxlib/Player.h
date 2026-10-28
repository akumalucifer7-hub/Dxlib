#pragma once
#include "Character.h"
#include"Weapon.h"
class Player : public Character
{
public:
	virtual ~Player();
    void Init(const GameContext& context) override;
    void Update(const GameContext& context) override;

	void  AttatchItems() override;

	void Draw() override;
	void DrawDebug() override;


    void OnTakeDamage(const GameContext& context) override;

	int GetMaxComboSteps() const override { return 4; }
	CharacterStateID GetAttackAnimID(int comboStep) const override
	{
		if (comboStep == 1) return CharacterStateID::Attack1;
		if (comboStep == 2) return CharacterStateID::Attack2;
		if (comboStep == 3) return CharacterStateID::Attack3;
		if (comboStep == 4) return CharacterStateID::Attack4;
		return CharacterStateID::Attack1;
	}
	void SetParryTimer(float time) { parryTimer = time; }
	float GetParryTimer() const { return parryTimer; }
	void ClearParryTimer() { parryTimer = 0.0f; }

private:
 
	Weapon SwordItems;
	Weapon GunItemsRight;
	Weapon GunItemsLeft;

	float parryTimer = 0.5f; // パリィ受付タイマー
};