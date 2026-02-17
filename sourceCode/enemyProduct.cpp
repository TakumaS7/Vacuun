//-------------------------------------------------------------
// enemyProduct.cpp
// 製作：楜澤旭渉　日付：2024/11/22
//-------------------------------------------------------------
#include "enemyProduct.h"
#include "enemyBase.h"
#include "enemyStone.h"
#include "enemyWeak.h"
#include "enemyWeakRegist.h"
#include "enemyUnique.h"
#include "enemyStrong.h"
#include "enemyBoss.h"
#include "enemyShotBullet.h"
#include "bullet.h"
#include "score.h"
#include "collision.h"
#include "coordinate.h"
#include "stage3.h"
#include <vector>
#include <iostream>
#include <random>
#include "player.h"
#include "sound.h"

static int g_CollisionSE;
static Bullet* g_Bullet = GetBullet();

EnemyProduct::EnemyProduct()
{
    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());

    m_enemys = nullptr;
    m_enemys = new vector<Enemy*>;
  
    m_HitEfffectTime.clear();
    isTimeStop = false;

    g_CollisionSE = LoadSound("asset\\SE\\EnemyDown.wav");

    m_HitEffectTexture = InitTexture(L"asset\\texture\\EFFECT_HIT_04.png");
}

EnemyProduct::~EnemyProduct()
{
    StopSound(g_CollisionSE);

    for (Enemy* enemy : *m_enemys) {
        delete enemy;
        enemy = nullptr;
    }
    delete m_enemys;
    m_enemys = nullptr;
}


//CSVから敵の情報を読み込む
vector<vector<int>> EnemyProduct::LoadEnemyFromCSV(std::string filename)
{
    vector<vector<int>> enemyData; //敵情報を格納するベクター
    ifstream file(filename);  //CSVファイルを開く
    string line;

    if (!file.is_open()) {
        cerr << "Failed to open CSV file." << endl;
        return enemyData;
    }

    while (getline(file, line)) {
        stringstream ss(line);
        vector<int> enemy;
        string value;

        //カンマで区切られた値を取得
        while (getline(ss, value, ',')) {
            enemy.push_back(stoi(value));
        }

        enemyData.push_back(enemy); //敵情報を追加
    }

    file.close();
    return enemyData;
}

void EnemyProduct::LoadStage(int stageIndex)
{
    if (stageIndex < 0 || stageIndex >= m_stageFiles.size()) {
        return; // 無効なステージ番号なら何もしない
    }

    // 敵リストをクリア
    m_enemyInfoList.clear();

    // ステージのCSVをロード
    string filepath = ENEMY_FOLDER + m_stageFiles[stageIndex];
    //m_currentEnemies = LoadEnemyFromCSV(filepath);
    vector<vector<int>> enemiesdata = LoadEnemyFromCSV(filepath);

    for (const auto& data : enemiesdata) {
        if (data.size() < 6) {
            cerr << "CSV内の敵データが無効です" << endl;
            continue;
        }

        enemyInfo info;
        info.num = data[0];
        info.frame = data[1];
        info.type = data[2];
        info.x0 = data[3];
        info.x1 = data[4];
        info.y0 = data[5];
        info.y1 = data[6];
        info.behavior = data[7];

        m_enemyInfoList.push_back(info);
    }

    m_currentStage = stageIndex; // 現在のステージを更新
}

void EnemyProduct::Update()
{
    if (m_enemys == nullptr) {
        cerr << "m_enemys is nullptr" << endl; return;
    }

    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());


  
    float UltEnemyHeight = SCREEN_YCENTER + (-250.0f + rand() % 500);
    Player* g_pPlayer = GetPlayer();

    if (g_pPlayer->GetUltStart() == true) {
        /* ウルト時の敵生成 */
        if (m_TimeRetention + m_UltEnemySpanTime < m_time) {
            Create(
                0,
                0,
                SCREEN_WIDTH,
                UltEnemyHeight,
                1
            );

            m_TimeRetention = m_time;
        }
    }
  

    // 敵情報リストを確認して生成する
    for (int i = 0; i < m_enemyInfoList.size(); i++) {

        if (m_time < m_enemyInfoList[i].frame) { continue; }//生成時間による生成

        int enemyPosX = m_enemyInfoList[i].x0;
        int enemyPosY = m_enemyInfoList[i].y0;

        //x座標のランダム生成
        if (m_enemyInfoList[i].x0 <= m_enemyInfoList[i].x1) {
            std::uniform_int_distribution<int> distX(m_enemyInfoList[i].x0, m_enemyInfoList[i].x1);
            enemyPosX = distX(engine);
        }
        //y座標のランダム生成
        if (m_enemyInfoList[i].y0 <= m_enemyInfoList[i].y1) {
            std::uniform_int_distribution<int> distY(m_enemyInfoList[i].y0, m_enemyInfoList[i].y1);
            enemyPosY = distY(engine);
        }

        //敵を生成
        Create(
            m_enemyInfoList[i].num,
            m_enemyInfoList[i].type,
            enemyPosX,
            enemyPosY,
            m_enemyInfoList[i].behavior
        );

        //生成済みの敵情報を削除
        m_enemyInfoList.erase(m_enemyInfoList.begin() + i);
        i--;
        
    }

    // エネミーの処理
    for (size_t i = 0; i < m_enemys->size(); ++i) {
        Enemy* enemy = (*m_enemys)[i];

        if (enemy == nullptr) {
            m_enemys->erase(m_enemys->begin() + i);
            continue;
        }

        // Useフラグがfalseなら削除
        if (!enemy->GetUse()) {
            delete enemy;
            m_enemys->erase(m_enemys->begin() + i);
            continue;
        }

        
        enemy->Update();

       
        if (enemy->GetState() == ENEMYSTATE_ATTACK) {
            enemy->SetSpeed(enemy->GetOrgSpeed());
        }

  
        if (enemy->GetCanAbsorb()) {
            //衝突判定
            if (enemy->GetUse() && GetPlayer()->GetState() == PLAYERSTATE_ABSORB) {
                Coordinate hole;
                hole.m_Pos = { GetPlayer()->GetAimingCenter().x, GetPlayer()->GetAimingCenter().y, 0.0f };
                hole.m_Size = { abs(GetPlayer()->GetAimingRange().x),  abs(GetPlayer()->GetAimingRange().y) };

                if (CheckCircleCollider(enemy->GetCoordinate(), hole)) {
                    enemy->GetCoordinate().m_Pos = enemy->EnemyAbsorbed(enemy, 0.5f);
                }
             
            }
            enemy->BarrierBreakChecker();
            enemy->EnemyPostAbsorbed();
            CheckCollisionEnemy(enemy);
        }
        
        //範囲外の敵削除
        EnemyErase(enemy);
    }

    if (!isTimeStop) {
        //経過時間を加算
        m_time++;
    }

    /* ヒットストップ処理 **********************************/
    if (m_HitStopTime >= 5) {
        for (Enemy* enemy : *m_enemys) {
            enemy->SetCenterVelX(1.0f);
        }

        g_Bullet->SetBulletEffectHitStop(1.0f);

        m_HitStopTime = 0;
        m_IsHitStop = false;
    }

    if (m_IsHitStop) {
        m_HitStopTime++;
    }
    /*******************************************************/

    //ヒットエフェクトの削除
    DeleteHitEffect();

    //m_DamageTime++;

    for (Enemy* enemy : *m_enemys) {
        enemy->AddDamageTime();

        if (enemy->GetAbsorbedTime() >= ABSORBED_SPAN_TIME) {
            enemy->ToReturnAbsorbed();
            enemy->SetAbsorbedTime(0);
        }

        if (enemy->GetState() == ENEMYSTATE_ABSORBED && g_pPlayer->GetState() != PLAYERSTATE_ABSORB) {
            enemy->AddAbsorbedTime();
        }
    }
}


void EnemyProduct::Create(int num, int type, int x, int y, int behavior) {
    if (m_enemys == nullptr) {
        cerr << "m_enemys is nullptr" << endl;
        return;
    }

    Enemy* newEnemy = nullptr;


  

    switch (type) {
    case ENEMYTYPE_STONE:
        newEnemy = new EnemyStone(static_cast<float>(x), static_cast<float>(y), behavior);
        newEnemy->SetEnemyType(ENEMYTYPE_STONE);
        break;
    case ENEMYTYPE_WEAK:
        newEnemy = new EnemyWeak(static_cast<float>(x), static_cast<float>(y), behavior);
        newEnemy->SetEnemyType(ENEMYTYPE_WEAK);
        break;
    case ENEMYTYPE_WEAK_REGIST:
        newEnemy = new EnemyWeakRegist(static_cast<float>(x), static_cast<float>(y), behavior);
        newEnemy->SetEnemyType(ENEMYTYPE_WEAK_REGIST);
        break;
    case ENEMYTYPE_UNIQUE:
        newEnemy = new EnemyUnique(static_cast<float>(x), static_cast<float>(y), behavior);
        newEnemy->SetEnemyType(ENEMYTYPE_UNIQUE);
        break;
    case ENEMYTYPE_STRONG:
        newEnemy = new EnemyStrong(static_cast<float>(x), static_cast<float>(y), behavior);
        newEnemy->SetEnemyType(ENEMYTYPE_STRONG);
        break;
    case ENEMYTYPE_BULLET:
        newEnemy = new EnemyShotBullet(static_cast<float>(x), static_cast<float>(y), behavior);
        newEnemy->SetEnemyType(ENEMYTYPE_BULLET);
        SetBulletDirection(newEnemy,num);
        break;
    case ENEMYTYPE_BOSS:
        newEnemy = new EnemyBoss(static_cast<float>(x), static_cast<float>(y), behavior);
        newEnemy->SetEnemyType(ENEMYTYPE_BOSS);
        break;
    default:
        return;
    }

    if (newEnemy == nullptr) {
        cerr << "Failed to create new enemy." << endl;
        return;
    }


    Player* g_pPlayer = GetPlayer();

    // 新しい敵を使用可能に設定
    newEnemy->SetUse(true);
    if (newEnemy->GetEnemyType() == ENEMYTYPE_WEAK_REGIST) {
        newEnemy->SetState(ENEMYSTATE_BARRIER);
    }
    else {
        newEnemy->SetState(ENEMYSTATE_ATTACK);
    }

    m_enemys->push_back(newEnemy);
}


void EnemyProduct::Draw()
{
    if (m_enemys == nullptr) {
        cerr << "m_enemys is nullptr" << endl;
        return;
    }
    for (Enemy* enemy : *m_enemys) {
        if (enemy == nullptr || !enemy->GetUse()) {
            continue; //無効なエネミーはスキップ
        }
        if (enemy != nullptr && enemy->GetUse()) {
            enemy->Draw();
        }
    }


}

void EnemyProduct::DrawUI()
{

    DrawOuterHitEffect();

    for (Enemy* enemy : *m_enemys) {
        if (enemy == nullptr || !enemy->GetUse()) { continue; } //無効なエネミーはスキップ 
        if (enemy != nullptr && enemy->GetUse()) {
            enemy->DrawUI();
        }
    }
}



//衝突チェック関数
void EnemyProduct::CheckCollisionBullet()
{
    for (auto it = m_enemys->begin(); it != m_enemys->end(); ++it)  {

        Enemy* enemy = *it;
        Bullet* bullet = GetBullet();

        if (!enemy->GetUse() || !bullet->GetUse()) { continue; } // 敵が使用中の場合のみチェック

        for (Enemy* enemy : *m_enemys)
        {
            if (enemy == nullptr) {
                return;
            }

            //衝突判定 (円形判定を使用)
            if (CheckCircleCollider(enemy->GetCoordinate(), bullet->m_Coordinate) && !enemy->GetState() == ENEMYSTATE_BULLET)
            {
                EnemyDeath(enemy);   //敵を死亡させる
                //bullet->SetUse(false); //弾を無効化
                break;
            }

        }

    }
}


void EnemyProduct::CheckCollisionEnemy(Enemy* enemyA)
{
    
    if (!enemyA || enemyA->GetState() != ENEMYSTATE_BULLET ||GetIsClear()) return;

    for (Enemy* enemyB : *m_enemys) {
        if (!enemyB) continue;

        //当たり判定の調整
        Coordinate enemy1 = enemyA->GetCoordinate();
        Coordinate enemy2 = enemyB->GetCoordinate();
        enemy1.m_Size = { enemy1.m_Size.x * 1.3f,enemy1.m_Size.y * 1.2f };
        enemy2.m_Size = { enemy2.m_Size.x * 1.3f,enemy2.m_Size.y * 1.2f };

        //衝突判定
        if (!CheckCircleCollider(enemy1, enemy2)) continue;
      
      

        //ボスとの衝突処理
        if (enemyB->GetEnemyType() == ENEMYTYPE_BOSS) {
            HitBossCollision(enemyA, enemyB);
            continue;
        }

        //無効な状態の敵はスキップ
        if (IsInvalidState(enemyB)) continue;

        //通常敵との衝突処理
        HitNormalEnemyCollision(enemyA, enemyB);
    }
}


//敵が無効かどうか判定
bool EnemyProduct::IsInvalidState(Enemy* enemy)
{
    return (enemy->GetState() == ENEMYSTATE_BULLET ||
        enemy->GetState() == ENEMYSTATE_KNOCKOUT ||
        enemy->GetState() == ENEMYSTATE_ABSORBED);
}


//ボスとの衝突処理
void EnemyProduct::HitBossCollision(Enemy* bullet, Enemy* boss)
{
    PlaySound(g_CollisionSE, false);

    // ヒットストップ処理 
    UseHitStop();
    //ヒットエフェクト
    UseHitEffect(bullet, boss);
    PlaySound(g_CollisionSE, false);
    boss->SetIsHit(true);
    boss->DecreaseHp(5);
    bullet->SetUse(false);
    bullet->SetIsHit(true);
    bullet->SetEffectTime(0.0f);
}


//通常敵との衝突処理
void EnemyProduct::HitNormalEnemyCollision(Enemy* bullet, Enemy* enemy)
{
    PlaySound(g_CollisionSE, false);

    //一定時間内のダメージ制限
    //if (m_DamageTimeRetention + m_DamageSpanTime >= m_DamageTime) return;
    if (enemy->GetDamageTimeRetention() + ENEMY_DAMAGE_SPAN_TIME >= enemy->GetDamageTime()) return;

    //ヒットストップ処理 
    UseHitStop();
    //ヒットエフェクト
    UseHitEffect(bullet, enemy);

    if (enemy->GetState() == ENEMYSTATE_BARRIER) {
        enemy->SetState(ENEMYSTATE_ATTACK);
        //m_DamageTimeRetention = m_DamageTime;
        enemy->SetDamageTimeRetention(enemy->GetDamageTime());
        return;
    };

    //スコア加算
    if (enemy->GetEnemyType() >= ENEMYTYPE_STRONG || enemy->GetEnemyType() == ENEMYTYPE_BOSS) {
        if (enemy->GetState() == ENEMYSTATE_ATTACK) {
            AddScore(1);
        }
    }

    //敵をノックアウト状態に
    enemy->SetState(ENEMYSTATE_KNOCKOUT);
    enemy->SetIsBullet(false);
    enemy->SetAngle(0.0f);
    enemy->SetRegistPoint(10);
    enemy->SetIsHit(true);

    //弾のエフェクト処理
    bullet->SetIsHit(true);
    bullet->SetEffectTime(0.0f);
 
    AbsorbedOnce();
    if (enemy->GetEnemyType() == ENEMYTYPE_BULLET) { DecMakingBulletExp(); }

    //衝突方向を計算してノックバック
    SetKnockBack(enemy, bullet);

  

    //弾なら削除
    if (bullet->GetEnemyType() == ENEMYTYPE_BULLET && enemy->GetEnemyType() != ENEMYTYPE_BULLET) {
        bullet->SetUse(false);
    }
}

//敵をノックバックさせる処理
void EnemyProduct::SetKnockBack(Enemy* enemy, Enemy* bullet)
{
    XMFLOAT3 enemyPos = enemy->GetCoordinate().m_Pos;
    XMFLOAT3 bulletPos = { GetPlayer()->GetAimingCenter().x - 10, GetPlayer()->GetAimingCenter().y, 0.0f };

    XMVECTOR enemyVec = XMLoadFloat3(&enemyPos);
    XMVECTOR bulletVec = XMLoadFloat3(&bulletPos);
    XMVECTOR direction = XMVectorSubtract(enemyVec, bulletVec);

    // 正規化
    direction = XMVector3Normalize(direction);

    // ノックバックの移動ベクトルをセット
    enemy->SetMoveVector(direction * 0.5f);
}


//敵を死亡させる
void EnemyProduct::EnemyDeath(Enemy* enemy)
{
    enemy->SetUse(false); // 敵を無効化
    
}

//範囲外の敵を殺す
void EnemyProduct::EnemyErase(Enemy* enemy) 
{

    if (enemy->GetEnemyType() == ENEMYTYPE_BOSS) { return; }

    XMFLOAT3 pos = enemy->GetCoordinate().m_Pos;

    if (pos.x < -100 )  {
        enemy->SetUse(false);
    }

    //画面外の判定
    bool isOutOfScreen = (pos.x < -100 || pos.x > SCREEN_WIDTH + 100 ||
        pos.y < -100 || pos.y > SCREEN_HEIGHT + 100);

    //ENEMYSTATE_KNOCKOUT または ENEMYSTATE_BULLET の場合、画面外に出たら削除
    if ((enemy->GetState() == ENEMYSTATE_KNOCKOUT || enemy->GetState() == ENEMYSTATE_BULLET) && isOutOfScreen) {
       enemy->SetUse(false);
    }
}

int EnemyProduct::CountIsBullet() 
{
    if (!m_enemys) return 0; //敵リストが存在しない場合は 0 を返す
    int bulletCount = 0; //弾状態の敵のカウント用変数
    for (Enemy* enemy : *m_enemys) { // 敵リストをループ
        if (enemy && enemy->GetState() == ENEMYSTATE_BULLET) { //弾状態ならカウント
            bulletCount++;
        }
    }
    return bulletCount; //カウント結果を返す
}

void EnemyProduct::SetBulletDirection(Enemy* newEnemy,int num)
{
    switch (num)
    {
    case 0:
        newEnemy->SetMoveVector({1.0f,0.0f,0.0f});
        break;
    case 1:
        newEnemy->SetMoveVector({1.0f,1.0f,0.0f });
        break;
    case 2:
        newEnemy->SetMoveVector({ 0.0f,-1.5f,0.0f });
        break;
    case 3:
        newEnemy->SetMoveVector({ 1.0f,0.0f,0.0f });
        newEnemy->SetCanAbsorb(false);
        break;
    case 4:
        newEnemy->SetMoveVector({ 1.0f,1.0f,0.0f });
        newEnemy->SetCanAbsorb(false);
        break;
    case 5:
        newEnemy->SetMoveVector({ 0.0f,-1.5f,0.0f });
        newEnemy->SetCanAbsorb(false);
        break;
    default:
        break;
    }
    return;
}

//全ての敵が死んだか確認
bool EnemyProduct::CheckAllEnemiesDeath()
{
    Player* pPlayer = GetPlayer();

    //プレイヤーが死んだら下に通らない
    if (pPlayer->GetState() == PLAYERSTATE_DEATH) {
        return false;
    }

    //生成前の敵リストが空であることを確認
    if (!m_enemyInfoList.empty()) {
        return false;
    }

    //生成後の敵リストが空であるか倒されたことを確認
    for (Enemy* enemy : *m_enemys) {
        if (enemy && enemy->GetUse()) {
            return false;
        }
    }

    return true; 
}

// ボス以外の敵を削除
void EnemyProduct::AllDisableEnemies()
{
    for (Enemy* enemy : *m_enemys) {
        if (enemy && enemy->GetEnemyType() != ENEMYTYPE_BOSS) {
            enemy->SetUse(false); // 使用フラグを無効化
        }
    }
}

//ヒットストップ
void EnemyProduct::UseHitStop()
{
    g_Bullet->SetBulletEffectHitStop(0.0f);

    for (Enemy* enemy : *m_enemys) {
        enemy->SetCenterVelX(0.0f);
        m_HitStopTime = 0;
        m_IsHitStop = true;
    }
}

//ヒットエフェクト設定
void EnemyProduct::UseHitEffect(Enemy* enemyA, Enemy* enemyB)
{
    if (enemyB->GetState() == ENEMYSTATE_KNOCKOUT) { return; }
    // ヒットエフェクトの追加
    XMFLOAT2 effectPos = {
        (enemyA->GetCoordinate().m_Pos.x + enemyB->GetCoordinate().m_Pos.x) / 2.0f,
        (enemyA->GetCoordinate().m_Pos.y + enemyB->GetCoordinate().m_Pos.y) / 2.0f,
    };
    // ヒットエフェクトの位置と残り時間を記録
    m_HitEffectPosition.push_back(effectPos);
    m_HitEfffectTime.push_back(10.0f);

}

//ヒットエフェクト描画
void EnemyProduct::DrawOuterHitEffect()
{
    if (m_HitEffectPosition.empty()) return;
    GetDeviceContext()->PSSetShaderResources(0, 1, &m_HitEffectTexture); // ヒットエフェクト用テクスチャを設定
    for (size_t i = 0; i < m_HitEffectPosition.size(); i++)
    {
        DrawSpriteAnim(m_HitEffectPosition[i], 0.0f, { 200.0f, 200.0f }, 8, 2, static_cast<int>(m_HitEfffectTime[i]));
    }
}

//ヒットエフェクト削除
void EnemyProduct::DeleteHitEffect()
{
    for (size_t i = 0; i < m_HitEfffectTime.size(); ++i)
    {
        if (m_HitEfffectTime[i] > 0)
        {
            m_HitEfffectTime[i] -= 0.5f; 
        }
    }
    //時間が0になったエフェクトを削除
    for (size_t i = 0; i < m_HitEfffectTime.size();)
    {
        if (m_HitEfffectTime[i] <= 0)
        {
            m_HitEfffectTime.erase(m_HitEfffectTime.begin() + i);
            m_HitEffectPosition.erase(m_HitEffectPosition.begin() + i);
        }
        else
        {
            i++;
        }
    }
}
