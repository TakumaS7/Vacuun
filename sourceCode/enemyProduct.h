//-------------------------------------------------------------
// enemyProduct.h
// 製作：楜澤旭渉　日付：2024/11/22
//-------------------------------------------------------------

//--------------------------概要-------------------------------
// 敵の生成を管理する
//-------------------------------------------------------------

#ifndef _ENEMYPRODUCT_H
#define _ENEMYPRODUCT_H

#include "bulletMaterial.h"
#include "coordinate.h"
#include "frameTime.h"
#include "enemyBase.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#define ENEMY_FOLDER "asset\\EnemyLD\\"
#define STRONG_BULLET_NUM 5

using namespace std;

//敵の情報を保持する構造体
struct enemyInfo {
	int num;	 //番号
	int frame;	 //生成フレーム
	int type;	 //種類
	int x0;		 //X座標0
	int x1;		 //X座標1(X座標x0～x1の範囲にランダム生成)
	int y0;		 //Y座標0
	int y1;		 //Y座標1(Y座標y0～y1の範囲にランダム生成)
	int behavior;//行動パターン
};

class Enemy;

class EnemyProduct {
private:
	int m_time = 0;	//経過時間

	const unsigned int m_UltEnemySpanTime = 5;
	unsigned int m_TimeRetention = 0;	//時間保持
	
	vector<enemyInfo> m_enemyInfoList; //CSVから読み込んだ敵の情報のリスト(生成前の敵)
	vector<Enemy*>* m_enemys;	//敵のインスタンス(生成後の敵)
	
	vector<string> m_stageFiles = { "EnemyLD_TUTORIAL.csv","EnemyLD_STAGE1.csv", "EnemyLD_STAGE2.csv", "EnemyLD_STAGE3.csv" };

	vector<vector<int>> m_currentEnemies; // 現在のステージの敵データ
	int m_currentStage = 0; // 現在のステージ番号
	bool isTimeStop = false;

	unsigned int m_HitStopTime = 0;
	bool m_IsHitStop = false;

	ID3D11ShaderResourceView* m_HitEffectTexture;
	std::vector<XMFLOAT2> m_HitEffectPosition = {};
	std::vector<float> m_HitEfffectTime = {};
	
public:
	

	EnemyProduct();
	~EnemyProduct();
	
	//CSVから敵の情報読み込み
	vector<vector<int>> LoadEnemyFromCSV(std::string filename);

	void LoadStage(int stageIndex);

    void Update();

	void CheckCollisionEnemies();

	//敵の生成
	void Create(int num, int type, int x, int y, int behavior);

	void Draw();

	void DrawUI();

	void CheckCollisionBullet();
	
	void SetKnockBack(Enemy* enemy, Enemy* bullet);

	void EnemyDeath(Enemy* enemy);

	void CheckCollisionEnemy(Enemy* enemyA);

	bool IsInvalidState(Enemy* enemy);

	void HitBossCollision(Enemy* bullet, Enemy* boss);

	void HitNormalEnemyCollision(Enemy* bullet, Enemy* enemy);

	void EnemyErase(Enemy* enemy);

	int CountIsBullet();

	void SetBulletDirection(Enemy* newEnemy, int num);

	vector<Enemy*>* GetEnemy() {return m_enemys; }

	float GetTime() { return m_time; }
	void SetTimeStop(bool b) { isTimeStop = b; }

	bool CheckAllEnemiesDeath();

	void AllDisableEnemies();

	void UseHitStop();

	void UseHitEffect(Enemy* enemyA, Enemy* enemyB);

	void DrawOuterHitEffect();

	void DeleteHitEffect();

};



#endif // !_ENEMY_H_