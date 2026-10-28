#pragma once

class Field;
class InputValidation;
class Animator;
class Player;
class Villain;
class Camera;

struct GameContext
{
    Field* field = nullptr;
    InputValidation* input = nullptr;
    Animator* animator = nullptr;
    Player* player = nullptr;
    Villain* villain = nullptr;
    Camera* camera = nullptr;

    // フレーム共通のデルタタイム（秒）
    float deltaTime = 0.01666f; // デフォルト約60FPS相当で初期化
};