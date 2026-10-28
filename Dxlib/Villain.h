#pragma once
#include "Character.h"
#include"weapon.h"
class Villain : public Character
{
public:
    virtual ~Villain();
    void Init(const GameContext& context) override;
    void Update(const GameContext& context) override;
	void Draw() override;
	void DrawDebug() override;
    void OnTakeDamage(const GameContext& context) override;

	CharacterStateID GetAttackAnimID(int comboStep) const override
	{
		if (comboStep == 1) return CharacterStateID::Attack1;
		if (comboStep == 2) return CharacterStateID::Attack2;
		if (comboStep == 3) return CharacterStateID::Attack3;
		if (comboStep == 4) return CharacterStateID::Attack4;
		return CharacterStateID::Attack1;
	}
	int GetMaxComboSteps() const override { return 4; } // Villainは最大4段コンボ
private:
    Weapon weapon1;
	Weapon weapon2;
};