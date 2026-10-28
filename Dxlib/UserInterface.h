#pragma once
#include "DxLib.h"
#include <string>

struct GameContext;
class Character;

class GameUI
{
public:
    GameUI() = default;
    ~GameUI() = default;

    void Init();
    void Draw(const GameContext& context, bool isBossBattle = true);

private:
    // 各ゲージの描画補助関数
    void DrawPlayerHP(float currentHP, float maxHP, int level, int screenX, int screenY);
    void DrawBossHP(float currentHP, float maxHP, const std::string& bossName, int screenWidth, int screenHeight);
    void DrawMobHP(const Character& mob, int screenWidth, int screenHeight);

    // ゲージバーの共通描画処理（背景・中身・枠線）
    void DrawGaugeBar(int x, int y, int width, int height, float rate, unsigned int fillColor, unsigned int bgColor = GetColor(40, 40, 40));

    // カラー定義
    const unsigned int COLOR_WHITE = GetColor(255, 255, 255);
    const unsigned int COLOR_BLACK = GetColor(0, 0, 0);
    const unsigned int COLOR_PLAYER_HP = GetColor(50, 205, 50);   // ライムグリーン
    const unsigned int COLOR_BOSS_HP = GetColor(220, 20, 60);   // クリムゾンレッド
    const unsigned int COLOR_MOB_HP = GetColor(255, 140, 0);   // ダークオレンジ
    const unsigned int COLOR_BG_PANEL = GetColor(20, 20, 20);
};