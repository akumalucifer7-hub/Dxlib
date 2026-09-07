#pragma once
//各オブジェクトに共通する情報をまとめた構造体
class Field;
class InputValidation;
class Animator;
class Player;
class Villain;
class Camera;
struct GameContext
{
    Field* field;
    InputValidation* input;
	Animator* animator;
	Player* player; 
	Villain* villain;
	Camera* camera;

};