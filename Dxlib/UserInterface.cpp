#include "UserInterface.h"
#include "GameContext.h"
#include "Character.h"
#include <algorithm>
#include"Villain.h"
#include"Player.h"
void GameUI::Init()
{
    // 初期化処理（必要に応じてフォントハンドルの生成など）
}

void GameUI::Draw(const GameContext& context, bool isBossBattle)
{
    int screenWidth = 1920;
    int screenHeight = 1080;
    GetScreenState(&screenWidth, &screenHeight, NULL);

    // 1. ① プレイヤーの体力 (画面左上)
    if (context.player)
    {
        // ※仮の値（CharacterクラスにGetHP/GetMaxHP/GetLevelがある前提）
        float currentHp = context.player->GetHP();
        float maxHp = context.player->GetMaxHP();
        int level = context.player->GetLevel();

        DrawPlayerHP(currentHp, maxHp, level, 30, 30);
    }

    // 2. ② BOSSの体力 (画面中央下)
    if (isBossBattle && context.villain)
    {
        float bossHp = context.villain->GetHP();
        float bossMaxHp = context.villain->GetMaxHP();

        DrawBossHP(bossHp, bossMaxHp, "BOSS", screenWidth, screenHeight);
    }
    // 3. ③ 雑魚敵の体力 (※ボス戦以外の場合のみ敵の頭上に表示)
    else if (!isBossBattle && context.villain)
    {
        DrawMobHP(*context.villain, screenWidth, screenHeight);
    }
}

// -----------------------------------------------------------------------------
// ① プレイヤーの体力描画 (画面左上)
// -----------------------------------------------------------------------------
void GameUI::DrawPlayerHP(float currentHP, float maxHP, int level, int screenX, int screenY)
{
    float rate = std::clamp(currentHP / maxHP, 0.0f, 1.0f);

    // 半透明背景パネル
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
    DrawBox(screenX - 10, screenY - 10, screenX + 280, screenY + 65, COLOR_BG_PANEL, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // レベル表示 (例: Lv.1)
    DrawFormatString(screenX, screenY, COLOR_WHITE, "Lv.%d", level);

    // 体力バー
    DrawGaugeBar(screenX, screenY + 25, 250, 18, rate, COLOR_PLAYER_HP);

    // 数値表示 (例: 100 / 100)
    DrawFormatString(screenX + 160, screenY + 46, COLOR_WHITE, "%3.0f / %3.0f", currentHP, maxHP);
}

// -----------------------------------------------------------------------------
// ② BOSSの体力描画 (画面中央下)
// -----------------------------------------------------------------------------
void GameUI::DrawBossHP(float currentHP, float maxHP, const std::string& bossName, int screenWidth, int screenHeight)
{
    float rate = std::clamp(currentHP / maxHP, 0.0f, 1.0f);

    int barWidth = 700;
    int barHeight = 22;
    int posX = (screenWidth - barWidth) / 2;
    int posY = screenHeight - 80;

    // 半透明背景パネル
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
    DrawBox(posX - 15, posY - 30, posX + barWidth + 15, posY + barHeight + 15, COLOR_BG_PANEL, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // ボス名
    DrawString(posX, posY - 24, bossName.c_str(), COLOR_WHITE);

    // 体力バー
    DrawGaugeBar(posX, posY, barWidth, barHeight, rate, COLOR_BOSS_HP);

    // パーセンテージ表示
    DrawFormatString(posX + barWidth - 60, posY - 24, COLOR_WHITE, "%.0f%%", rate * 100.0f);
}

// ③ 雑魚敵の体力描画 (3D世界座標から2D画面座標へ変換して頭上に表示)
void GameUI::DrawMobHP(const Character& mob, int screenWidth, int screenHeight)
{
    // 敵の頭上位置（Y座標 + 22.0f）を取得
    VECTOR worldPos = mob.GetPos();
    worldPos.y += 22.0f;

    // 3D空間の座標を画面上の2D座標へ変換
    VECTOR screenPos = ConvWorldPosToScreenPos(worldPos);

    // カメラの画角内（画面上）に存在する場合のみ描画
    if (screenPos.z >= 0.0f && screenPos.z <= 1.0f)
    {
        float currentHP = 50.0f;
        float maxHP = 100.0f;
        float rate = std::clamp(currentHP / maxHP, 0.0f, 1.0f);

        int barWidth = 80;
        int barHeight = 8;
        int posX = static_cast<int>(screenPos.x) - barWidth / 2;
        int posY = static_cast<int>(screenPos.y);

        // 頭上HPゲージ描画
        DrawGaugeBar(posX, posY, barWidth, barHeight, rate, COLOR_MOB_HP);
    }
}

// -----------------------------------------------------------------------------
// ゲージバー描画共通関数
// -----------------------------------------------------------------------------
void GameUI::DrawGaugeBar(int x, int y, int width, int height, float rate, unsigned int fillColor, unsigned int bgColor)
{
    // 1. ゲージ背景
    DrawBox(x, y, x + width, y + height, bgColor, TRUE);

    // 2. ゲージ中身 (HP残量)
    int fillWidth = static_cast<int>(width * rate);
    if (fillWidth > 0)
    {
        DrawBox(x, y, x + fillWidth, y + height, fillColor, TRUE);
    }

    // 3. 枠線 (黒・白の二重枠)
    DrawBox(x - 1, y - 1, x + width + 1, y + height + 1, GetColor(0, 0, 0), FALSE);
    DrawBox(x, y, x + width, y + height, GetColor(200, 200, 200), FALSE);
}