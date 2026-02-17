//-------------------------------------------------------------
// enemy.h
// 製作：笠原凜　日付：2024/11/06
//-------------------------------------------------------------

//--------------------------概要-------------------------------
// 敵の基底クラス。
// 吸収可能な弾の素材にもなるが、攻撃とは別の数値を使用。
// すべての敵はこのクラスを継承する。
//-------------------------------------------------------------

#ifndef _ENEMY_H
#define _ENEMY_H

#include "bulletMaterial.h"
#include "coordinate.h"
#include "player.h"
#include "frameTime.h"
#include "texture.h"
#include <cmath>

//-------------------------------------------------------------
// **定数**
//-------------------------------------------------------------
#define STRONG_BULLET_NUM 5  // 強弾の最大数
#define ABSORBED_SPAN_TIME 120 //プレイヤーが吸い込んでないとき抵抗状態を解除するまでの時間
#define ENEMY_DAMAGE_SPAN_TIME 30 //一定時間無敵になる時間

// 敵の種類
enum ENEMYTYPE {
    ENEMYTYPE_STONE = 0,
    ENEMYTYPE_WEAK,
    ENEMYTYPE_WEAK_REGIST,
    ENEMYTYPE_UNIQUE,
    ENEMYTYPE_STRONG,
    ENEMYTYPE_BULLET,
    ENEMYTYPE_BOSS,
    ENEMYTYPE_MAX
};

// 敵の状態
enum ENEMYSTATE {
    ENEMYSTATE_NORMAL = 0,                  // 通常
    ENEMYSTATE_ATTACK,                      // 攻撃
    ENEMYSTATE_DAMEGED,                     // 被ダメージ
    ENEMYSTATE_KNOCKOUT,                    // 気絶
    ENEMYSTATE_ABSORBED,                    // 吸収中
    ENEMYSTATE_BULLET,                      // 弾として存在
    ENEMYSTATE_BARRIER,                     // バリア発動中
    ENEMYSTATE_ULT = ENEMYSTATE_BARRIER,    //ウルト状態の敵（Stoneのみ変更）
    ENEMYSTATE_ATTACK_PREPARE,              // 攻撃準備         
    ENEMYSTATE_ATTACK_02,
    ENEMYSTATE_MAX
};

// 敵の移動パターン
enum ENEMYMOVE {
    ENEMYMOVE_NONE = 0,
    ENEMYMOVE_STRAIGHT,
    ENEMYMOVE_CHASE,
    ENEMYMOVE_UP_DOWN,
    ENEMYMOVE_ROTATION,
    ENEMYMOVE_STRONG,
    ENEMYMOVE_BOSS,
    ENEMYSTATE_KNOCKBACK,
    ENEMYBULLET_STRONG_NUM = ENEMYSTATE_KNOCKBACK + STRONG_BULLET_NUM,
    ENEMYMOVE_MAX
};

//-------------------------------------------------------------
// **Enemy クラス**
//-------------------------------------------------------------
class Enemy : public BulletMaterial
{
private:
    //基本ステータス
    float m_Atk = 0.0f;
    float m_Def = 0.0f;
    int m_bulletCount = 0;

    //状態管理
    ENEMYSTATE m_State = ENEMYSTATE_NORMAL;
    ENEMYTYPE enemyType = ENEMYTYPE_STONE;
    bool m_Explode = false;
    bool m_ExplodeJump = true;
    int m_type = 0;

    //アニメーション・時間
    float m_EffectAnim = 0.0f;
    float m_EffectAnim02 = 0.0f;
    int m_StateSpanTime = 0;
    bool m_IsBlinking = false;
    unsigned int m_ToAbsorbedReturnTime = 0;
    unsigned int m_DamageTimeRetention = 0;
    unsigned int m_DamageTime = 0;


    //移動関連
    XMVECTOR m_Position = {};
    XMVECTOR m_Direction = {};
    XMFLOAT3 m_Center = {};
    XMFLOAT3 m_CenterVel = { 1.0f, 0.0f, 0.0f };
    XMFLOAT3 m_MoveVector = {};
    XMFLOAT2 m_ExplodeForce = { 0.0f, 0.0f };
    Enemy* m_centerEnemy = nullptr;
    int step = 0;
    bool isChase = true;
    float m_Radian = 0.0f;	//進む角度を変える変数
    float m_Distance = 0.0f;	//弾を飛ばした後のX軸の距離
    XMFLOAT2 m_StrongBulletSpeed = { 5.0f, 0.0f };

    //吸収関連
    float m_Radius = 0.0f;
    float m_Angle = 0.0f;
    float m_AbsorptionSpeed = 0.5f;
    float m_AbsorbbTime = 0.0f;
    int AbsorbedCount = 0;
    int m_RegistPoint = 1000;
    bool m_Drop = false;
    bool m_IsBarrierBreak = false;

    //移動処理
    XMFLOAT3 MoveStraight(Coordinate coordinate);
    XMFLOAT3 MoveChase(Coordinate coordinate);
    XMFLOAT3 MoveStrong(Coordinate coordinate);
    XMFLOAT3 MoveUpDown(Coordinate coordinate);
    XMFLOAT3 KnockbackMove();
    
    //吸収処理
    bool WeakRegistExplode();
    bool OtherExplode();
    void EnemyExplosion();
    void HoldBulletMove();
    void BulletRotation(XMFLOAT2 pos);
    

protected:

    Coordinate m_Coordinate;
    bool isShot = false;
    float m_Hp = 0.0f;
    float m_Speed = 10.0f;
    float m_OrgSpeed = m_Speed;
    float m_Time = 0.0f;
    float m_SpanTime = 0.0f;
    float m_HitShakeTime = 5.0f;
    float m_AnimationFrame = 0.0f;
    int m_frameNum = 0;
    bool m_CanAbsorb = true;
    bool m_isUse = false;
    bool isBullet = false;
    bool m_isHit = false;
    bool canMove = true;
    XMVECTOR m_EnemyVector = { 1.0f,0.0f,0.0f }; //エネミーのベクター
    ID3D11ShaderResourceView* m_Texture[ENEMYSTATE_MAX];
    ID3D11ShaderResourceView* m_HitTexture;
    ID3D11ShaderResourceView* m_HitTexture_02;
    void SetMoveVec(XMFLOAT3 MVec) { m_MoveVector = MVec; }
    void SetIsChase() { canMove = false; }
    void SetStep(int num) { step = num; }
    int GetStep() { return step; }
    void EnemyShot(XMFLOAT2 pos, int bulletPattern, int dir);

public:
    //コンストラクタ・デストラクタ
    Enemy() {}
    Enemy(Coordinate c, float hp, float atk, float def)
        : m_Coordinate(c), m_Hp(hp), m_Atk(atk), m_Def(def), m_isUse(true) {
        m_HitTexture_02 = InitTexture(L"asset\\texture\\EFFECT_HIT_04.png");
    }
    ~Enemy() { m_isUse = false; }

    //ゲッター・セッター
    virtual Coordinate GetCoordinate() const = 0;
    virtual void SetCodinate(Coordinate coordinat) = 0;
    virtual void SetState(ENEMYSTATE state) { m_State = state; }
    virtual ENEMYSTATE GetState() const { return m_State; }
    void SetUse(bool isUse) { m_isUse = isUse; }
    bool GetUse() const { return m_isUse; }
    void SetEnemyType(ENEMYTYPE type) { enemyType = type; }
    ENEMYTYPE GetEnemyType() const { return enemyType; }
    void SetIsBullet(bool b) { isBullet = b; }
    bool GetIsBullet() const { return isBullet; }
    float GetAngle() const { return m_Angle; }
    void SetAngle(float num) { m_Angle = num; }
    float GetSpeed() const { return m_Speed; }
    void SetSpeed(float num) { m_Speed = num; }
    void MoveRotation();
    void SetRotation(Coordinate c, float hp, float atk, float def,
        XMVECTOR center, float radius, float period, bool clockwise = true);
    virtual int LocalGetEnemyType() const { return m_type; }
    bool GetIsHit() { return m_isHit; }
    void SetIsHit(bool b) { m_isHit = b; }
    virtual int GetMovePattern() { return 0; }
    XMFLOAT3 GetCenter() { return m_Center; }
    void SetCenter(XMFLOAT3 c) { m_Center = c; }
    void SetMoveVector(XMVECTOR v) { m_EnemyVector = v; }
    float GetOrgSpeed() { return m_OrgSpeed; }
    void SetEffectTime(float t) { m_EffectAnim = t; }
    bool GetIsShot() { return isShot; }
    void SetDrop(bool drop) { m_Drop = drop; }
    bool GetDrop() const { return m_Drop; }
    void SetAbsorbedTime(unsigned int time) { m_ToAbsorbedReturnTime = time; }
    unsigned int GetAbsorbedTime () const { return m_ToAbsorbedReturnTime; }
    void SetDamageTimeRetention(unsigned int time) { m_DamageTimeRetention = time; }
    unsigned int GetDamageTimeRetention() const { return m_DamageTimeRetention; }
    unsigned int GetDamageTime() const { return m_DamageTime; }

    //移動
    XMFLOAT3 EnemyMove(int PatternNum, Coordinate coordinate);

    //吸収処理
    XMFLOAT3 EnemyAbsorbed(Enemy* enemy, float speed);
    void EnemyPostAbsorbed();

    //ダメージ・爆発処理
    void Damege();
    void SetRegistPoint(float num) { m_RegistPoint = num; }
    bool GetCanAbsorb() { return m_CanAbsorb; }
    void SetCanAbsorb(bool b) { m_CanAbsorb = b; }
    void DecreaseHp(float num) {
        m_Hp -= num;

        //HPは０以下にならない
        if (m_Hp < 0) { m_Hp = 0; }
    }

    //描画 
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void DrawUI() { return; }

    void DrawHitEffect();
    void DrawHitOutEffect(XMFLOAT3 pos);
    void EnemyExplode();

    bool GetEnemyExplode() const { return m_Explode; }

    void SetCenterVelX(float VelX) { m_CenterVel.x = VelX; }

    void BarrierBreakChecker();

    void BlinkingEnemy(); //点滅

    void ToReturnAbsorbed(); //抵抗中からもとに戻る

    void AddAbsorbedTime() { m_ToAbsorbedReturnTime++; }

    void AddDamageTime() { m_DamageTime++; }
};

#endif // !_ENEMY_H_
