//-------------------------------------------------------------
// enemy.cpp
// 製作：笠原凜　日付：2024/11/06
//-------------------------------------------------------------
#include "enemyBase.h"
#include "enemy_01.h"
#include "sprite.h"
#include "keyboard.h"
#include "enemyProduct.h"
#include "score.h"
#include "enemyShotBullet.h"
#include "bullet.h"
#include <cmath>
#include "player.h"

static Bullet* g_pBullet = GetBullet();

//回転敵の移動
void Enemy::MoveRotation()
{
	////角度更新
	//float elapsedTime = GameFrameTime::GetElapsedTime();
	//m_EnemyState.m_Angle += m_EnemyState.m_Omega * elapsedTime;
	//
	////位置の計算
	//float x = m_EnemyState.m_Radius * std::cos(m_EnemyState.m_Angle);
	//float y = m_EnemyState.m_Radius * std::sin(m_EnemyState.m_Angle);

	////新しい座標を設定
	//m_Position = XMVectorAdd(m_EnemyState.m_Center, XMVectorSet(x, y, 0.0f, 0.0f));
}


void Enemy::SetRotation(Coordinate c, float hp, float atk, float def, XMVECTOR center, float radius, float period, bool clockwise)
{
	////角速度を計算
	//m_EnemyState.m_Omega = (2.0f * M_PI) / period;
	//if (!clockwise) {
	//	m_EnemyState.m_Omega = -m_EnemyState.m_Omega; // 反時計回りの場合は符号を変更
	//}
	//// 初期位置を計算
	//m_Position = XMVectorAdd(m_EnemyState.m_Center, XMVectorSet(radius, 0.0f, 0.0f, 0.0f));
}

XMFLOAT3 Enemy::EnemyMove(int PatternNum, Coordinate coordinate)
{
	

	if (GetIsBullet()) {
		return (m_Coordinate.m_Pos);
	}

	if ((GetState() != ENEMYSTATE_ATTACK && GetState() != ENEMYSTATE_ABSORBED) && GetEnemyType() == ENEMYTYPE_BOSS) {
		return (m_Coordinate.m_Pos);
	}

	//気絶状態の設定
	if (GetState() == ENEMYSTATE_KNOCKOUT && GetEnemyType() != ENEMYTYPE_BOSS && GetEnemyType() != ENEMYTYPE_BULLET) {
		PatternNum = ENEMYSTATE_KNOCKBACK;
	}
	switch (PatternNum)
	{
	case ENEMYMOVE_NONE:	//何もしない
		break;

	case ENEMYMOVE_STRAIGHT://直進移動
		
 		return MoveStraight(coordinate);
		break;

	case ENEMYMOVE_CHASE:	//追跡
		return MoveChase(coordinate);
		break;

	case ENEMYMOVE_UP_DOWN: //上下移動
		return MoveUpDown(coordinate);
		break;
	case ENEMYMOVE_ROTATION:	//回転敵

		//回転移動を更新
		MoveRotation();

		//現在の座標を反映
		XMStoreFloat3(&m_Coordinate.m_Pos, m_Position);
		return XMFLOAT3(m_Coordinate.m_Pos);
		break;

	case ENEMYMOVE_STRONG:
		return MoveStrong(coordinate);
		break;

	case ENEMYSTATE_KNOCKBACK:
        return KnockbackMove();
		break;
	}
}

XMFLOAT3 Enemy::EnemyAbsorbed(Enemy* enemy, float speed)
{
	XMFLOAT3 Pos = enemy->GetCoordinate().m_Pos;

	Player* pPlayer = GetPlayer();
	m_Center = { pPlayer->GetAimingCenter().x ,pPlayer->GetAimingCenter().y, 0.0f };
	/*if (m_RegistPoint <= 0) {
		SetIsBullet(true);
	}*/


	// 通常状態 → 抵抗状態への遷移
  	if (GetState() == ENEMYSTATE_ATTACK || GetState() == ENEMYSTATE_BARRIER) {
		float distance = sqrt(pow(pPlayer->GetAimingCenter().x - Pos.x, 2) +
			pow(pPlayer->GetAimingCenter().y - m_Coordinate.m_Pos.y, 2));
		if (distance <= pPlayer->GetAimingRange().x) { // プレイヤーの吸収範囲をチェック
			SetState(ENEMYSTATE_ABSORBED);
			m_Angle = 0.0f;
		}
	}


	// 抵抗状態の動き
	else if (GetState() == ENEMYSTATE_ABSORBED) {
		// ランダムな振動を加えながら抵抗値を減少
		Pos.x += (rand() % 3 - 1) * 0.5f;  // -0.5～0.5のランダム振動
		Pos.y += (rand() % 3 - 1) * 0.5f;
		m_RegistPoint -= 50;  //抵抗値を減少
		enemy->SetSpeed(2);

		// 抵抗値が0以下になったら気絶状態に遷移
		if (m_RegistPoint <= 0) {

			m_Angle = atan2(pPlayer->GetAimingCenter().y - m_Coordinate.m_Pos.y,
				pPlayer->GetAimingCenter().x - m_Coordinate.m_Pos.x);
			m_Radius = sqrt(pow(pPlayer->GetAimingCenter().x - m_Coordinate.m_Pos.x, 2) +
				pow(pPlayer->GetAimingCenter().y - m_Coordinate.m_Pos.y, 2));

			SetState(ENEMYSTATE_KNOCKOUT);
			SetBulletExpGain(false);
			AbsorbedOnce();
			if (enemy->GetEnemyType() == ENEMYTYPE_BULLET) { DecMakingBulletExp(); }
			SetIsBullet(true);
		}
	}

	// 吸収中の動き
	else if (GetState() == ENEMYSTATE_KNOCKOUT || m_RegistPoint <= 0 && pPlayer->GetState() == PLAYERSTATE_ABSORB) {
		SetIsBullet(true);
		const float rotationSpeed = XM_2PI / 120.0f;  // 120フレームで1回転
		m_Angle += rotationSpeed;  // 一定速度で角度を増加


		// 半径を周期的に増減
		m_Radius -= m_AbsorptionSpeed;
		//m_Radius -= 0.5f;

		// 敵の位置を計算して更新
		Pos.x = pPlayer->GetAimingCenter().x + m_Radius * cos(m_Angle);
		Pos.y = pPlayer->GetAimingCenter().y + m_Radius * sin(m_Angle);

		
		// 半径が閾値を超えたら反転
		if (m_Radius < 10.0f) {
			m_AbsorptionSpeed = -m_AbsorptionSpeed;
			m_Radius = 10.0f;
		}
		else if (m_Radius > 80.0f) {
			m_AbsorptionSpeed = -m_AbsorptionSpeed;
			m_Radius = 80.0f;

		}
		m_RegistPoint -= 50;
	}



	return XMFLOAT3(Pos.x, Pos.y, 0.0f);
}

void Enemy::EnemyPostAbsorbed() {

	//移動不可なら処理しない
	if (!canMove) return;

	
	//画面外に出たら削除
	if (m_Coordinate.m_Pos.x > SCREEN_WIDTH + 100 || m_Coordinate.m_Pos.y > 2000) {
		SetUse(false);
		return;
	}

	Player* pPlayer = GetPlayer();
	PLAYERSTATE playerState = pPlayer->GetState();

	//吸収するサークルが爆発する処理
	if (WeakRegistExplode()) {
		pPlayer->SetState(PLAYERSTATE_EXPLODE);
		m_Explode = true;
	}

	if (OtherExplode()) {
		m_Explode = true;
	}

	if (m_Explode) {
		EnemyExplosion();
		return;
	}

	//弾の移動処理
	if (isShot) {
		m_Center.x += 10.0f * m_CenterVel.x;
 		BulletRotation({ m_Center.x, m_Center.y });
	}//弾の吸収・発射処理
	else if (GetIsBullet()) {
		
		HoldBulletMove();
	}

	

	//プレイヤーが死亡したら無効化
	if (playerState == PLAYERSTATE_DEATH) {

		SetUse(false);
	}

	//画面外に出たら削除
	if (m_Coordinate.m_Pos.x > SCREEN_WIDTH + 100.0f) {
		
		SetUse(false);
	}
}

//吸収爆発するか判定
bool Enemy::WeakRegistExplode()
{
	Player* pPlayer = GetPlayer();

	return LocalGetEnemyType() == ENEMYTYPE_WEAK_REGIST &&
		GetState() == ENEMYSTATE_KNOCKOUT && !pPlayer->GetUltStart() &&
		!m_Explode && !m_IsBarrierBreak;
}

bool Enemy::OtherExplode()
{
	return GetPlayer()->GetState() == PLAYERSTATE_COOLDOWN &&
		m_IsBarrierBreak &&
		GetIsBullet() && GetState() == ENEMYSTATE_KNOCKOUT;
}

//吸収するサークルの爆発処理
void Enemy::EnemyExplosion()
{
	EnemyExplode();

	if (m_Coordinate.m_Pos.y > SCREEN_HEIGHT * 2.0f) {
		SetUse(false);
		m_Explode = false;
	}
}

//弾の動き・吸収の処理
void Enemy::HoldBulletMove()
{
	Player* pPlayer = GetPlayer();
	XMFLOAT2 playerPosition = { pPlayer->m_Coordinate.m_Pos.x, pPlayer->m_Coordinate.m_Pos.y };
	XMFLOAT2 forwardOffset = { 200.0f, 0.0f };  // プレイヤーの少し前方
	XMFLOAT2 targetCenter = { playerPosition.x + forwardOffset.x, playerPosition.y };

	
	// 弾が攻撃状態なら発射
	if (GetBullet()->GetState() == BULLETSTATE_ATTACK && GetState() == ENEMYSTATE_BULLET && !isShot) {
		m_Center = { targetCenter.x, targetCenter.y, 0.0f };
		m_Coordinate.m_Vel = XMFLOAT3(10.0f, 0.0f, 0.0f);
		isShot = true;
		
		return;
	}

	// 吸収中の動き
	if (GetState() == ENEMYSTATE_KNOCKOUT) {
		
		if (GetPlayer()->GetState() == PLAYERSTATE_ATTRACT && m_Center.x > targetCenter.x) {
			m_Center.x -= 10.0f;
			
		}
		if (GetPlayer()->GetState() == PLAYERSTATE_HOLD) {
			SetState(ENEMYSTATE_BULLET);
			m_Center = { targetCenter.x, targetCenter.y, 0.0f };
		
		}
	}

	BlinkingEnemy();//点滅処理

	//弾として回転しながら吸収
	BulletRotation(targetCenter);
}

//弾の回転処理
void Enemy::BulletRotation(XMFLOAT2 targetpos)
{
	if (!GetIsBullet() || (GetState() != ENEMYSTATE_KNOCKOUT && GetState() != ENEMYSTATE_BULLET)) return;
	
	const float rotationSpeed = XM_2PI / 120.0f;
	m_Angle += rotationSpeed;

	//半径の増減
	float radiusAmplitude = GetBullet()->m_Coordinate.m_Size.x / 2;
	const float radiusFrequency = 0.05f;
	m_Radius -= m_AbsorptionSpeed;

	//位置の更新
	m_Coordinate.m_Pos.x = targetpos.x + m_Radius * cos(m_Angle);
	m_Coordinate.m_Pos.y = targetpos.y + m_Radius * sin(m_Angle);

	

	//吸収の最小・最大半径制御
	if (m_Radius < 10.0f) {
		m_AbsorptionSpeed *= -1;
		m_Radius = 10.0f;
	}
	else if (m_Radius > radiusAmplitude - radiusAmplitude / 5) {
		m_AbsorptionSpeed *= -1;
		m_Radius = radiusAmplitude - radiusAmplitude / 5;
	}
}

void Enemy::BlinkingEnemy()
{
	const XMFLOAT4 defaultColor = { 1.0f,1.0f,1.0f,1.0f };//通常
	const XMFLOAT4 dlinkingColor = { 1.0f,1.0f,1.0f,0.0f };//点滅
	const float BlinkTime = 120;
	//敵が消滅まで３秒未満なら処理する
	if (GetHoldingTime() >=  GetPlayer()->GetHoldingTimeLimit() || GetHoldingTime() < GetPlayer()->GetHoldingTimeLimit() - BlinkTime) {
		return; 
	}

	float blinkTime = 5 + (GetPlayer()->GetHoldingTimeLimit() - GetHoldingTime()) * 0.0668;
	//5秒ごとに点滅を裏返す
	if (static_cast<int>(GetEnemyProduct()->GetTime()) % static_cast<int>(blinkTime)  == 0) {
		m_IsBlinking = !m_IsBlinking;
	}

	Coordinate c = m_Coordinate;
	//点滅
	if (m_IsBlinking) {
		c.m_Color = dlinkingColor;
	}
	else {
		c.m_Color = defaultColor;
	}

	SetCodinate(c);
}

void Enemy::ToReturnAbsorbed()
{
	if (GetState() == !ENEMYSTATE_ABSORBED) { return; }

	if (m_IsBarrierBreak) {
		SetState(ENEMYSTATE_ATTACK);
	}
	else {
		SetState(ENEMYSTATE_BARRIER);
	}
}

//弾の発射
void Enemy::EnemyShot(XMFLOAT2 pos, int bulletpattern, int dir)
{
	GetEnemyProduct()->Create(dir,ENEMYTYPE_BULLET,pos.x,pos.y, bulletpattern);
}

void Enemy::EnemyExplode()
{
	m_Coordinate.m_Pos.x += m_ExplodeForce.x;
	m_Coordinate.m_Pos.y += m_ExplodeForce.y;

	if (m_ExplodeJump) {
		m_ExplodeForce.x = -10 + rand() % 20;
		m_ExplodeForce.y = -(5 + rand() % 15);
		m_ExplodeJump = false;
	}

	m_ExplodeForce.y += 0.5f;
	isBullet = false;
}

void Enemy::BarrierBreakChecker()
{
	if (GetState() == ENEMYSTATE_BARRIER) {
		m_IsBarrierBreak = false;
	}
	else if (GetState() == ENEMYSTATE_ATTACK) {
		m_IsBarrierBreak = true;
	}
}

XMFLOAT3 Enemy::MoveStraight(Coordinate coordinate)
{
	coordinate.m_Pos.y += -this->GetSpeed() * XMVectorGetY(m_EnemyVector);
	coordinate.m_Pos.x += -this->GetSpeed() * XMVectorGetX(m_EnemyVector);

	return XMFLOAT3(coordinate.m_Pos.x, coordinate.m_Pos.y, 0.0f);
}

XMFLOAT3 Enemy::MoveChase(Coordinate coordinate)
{
	Player* pPlayer = GetPlayer();

	//プレイヤーの位置をベクターに変換
	XMVECTOR m_PlayerVector = XMLoadFloat3(&pPlayer->m_Coordinate.m_Pos);
	m_EnemyVector = XMLoadFloat3(&coordinate.m_Pos);

	//プレイヤーとの距離を計算
	m_Direction = m_PlayerVector - m_EnemyVector;
	float distance = XMVectorGetX(XMVector3Length(m_Direction));


	//距離が150以上の場合は追尾、100以下の場合は真っ直ぐに移動
	if (distance > 300.0f && isChase) {
		//方向を正規化
		m_Direction = XMVector3Normalize(m_Direction);
		//正規化された方向ベクトルを格納
		XMStoreFloat3(&m_MoveVector, m_Direction);
		//プレイヤー方向に移動
		coordinate.m_Pos.x += m_MoveVector.x * this->GetSpeed();
		coordinate.m_Pos.y += m_MoveVector.y * this->GetSpeed();
		SetMoveVec(m_MoveVector);
	}
	else {
		//プレイヤーとの距離が100以下の場合、移動ベクトルをそのまま使って真っ直ぐに移動
		isChase = false;

		coordinate.m_Pos.x += m_MoveVector.x * this->GetSpeed();
		coordinate.m_Pos.y += m_MoveVector.y * this->GetSpeed();
	}
	return XMFLOAT3(coordinate.m_Pos.x, coordinate.m_Pos.y, 0.0f);
}

XMFLOAT3 Enemy::MoveUpDown(Coordinate coordinate)
{

	if (GetStep() == 0) {
		//m_Coordinate.m_Pos.x += -this->GetSpeed() * 0.5f;
		m_Coordinate.m_Pos.y += -this->GetSpeed();

		if (m_Coordinate.m_Pos.y <= 0.0f) {
			SetStep(1);
		}
	}
	else if (GetStep() == 1) {
		//m_Coordinate.m_Pos.x += -this->GetSpeed() * 0.5f;
		m_Coordinate.m_Pos.y += this->GetSpeed();

		if (m_Coordinate.m_Pos.y >= SCREEN_HEIGHT) {
			SetStep(0);
		}
	}

	return XMFLOAT3(m_Coordinate.m_Pos.x, m_Coordinate.m_Pos.y, 0.0f);
}

XMFLOAT3 Enemy::MoveStrong(Coordinate coordinate )
{
	if (GetStep() == 0) {
		m_Coordinate.m_Pos.x += -this->GetSpeed() * 0.5f;

		if (m_Coordinate.m_Pos.x <= 1000.0f) {
			SetStep(1);
		}
	}
	else if (GetStep() == 1) {
		m_Coordinate.m_Pos.y += -this->GetSpeed();

		if (m_Coordinate.m_Pos.y <= m_Coordinate.m_Size.y * 0.5f) {
			SetStep(2);
		}
	}
	else if (GetStep() == 2) {
		m_Coordinate.m_Pos.y += this->GetSpeed();

		if (m_Coordinate.m_Pos.y >= SCREEN_HEIGHT + -(m_Coordinate.m_Size.y * 0.5f)) {
			SetStep(1);
		}
	}

	return XMFLOAT3(m_Coordinate.m_Pos.x, m_Coordinate.m_Pos.y, 0.0f);
}


//ヒット処理
void Enemy::Damege()
{
	if (!GetIsHit()) { return; }

	// シェイクのオフセットを計算
	float shakeOffsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 5;
	float shakeOffsetY = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 5;

	m_Coordinate.m_Pos.x += shakeOffsetX;
	m_Coordinate.m_Pos.y += shakeOffsetY;

	// シェイク時間を減少
	m_HitShakeTime -= GameFrameTime::GetElapsedTime() * 20;// deltaTimeはフレーム間の経過時間

	//点滅
	if ((static_cast<int>(m_HitShakeTime * 10)) % 5 == 0) {
		m_Coordinate.m_Color = { 5.0f, 5.0f, 5.0f, 1.0f };
		//m_Coordinate.m_Color = { 1.0f, 1.0f, 1.0f, 0.0f };
	}
	else {
		m_Coordinate.m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	}

	if (m_HitShakeTime < 0) {
		m_HitShakeTime = 5.0f;
		SetIsHit(false);
		m_Coordinate.m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	}
}

//ヒットエフェクト
void Enemy::DrawHitEffect()
{
	if (!GetIsHit()) { return; }

	if (m_EffectAnim > 8) { return; }

	GetDeviceContext()->PSSetShaderResources(0, 1, &m_HitTexture);

	Coordinate m_effectC = m_Coordinate;
	m_effectC.m_Size = { 120.0f, 120.0f};
 	DrawSpriteAnim(m_effectC, 8, 1, (int)m_EffectAnim % 8);

	m_EffectAnim += 0.5f;
}


void Enemy::DrawHitOutEffect( XMFLOAT3 pos)
{
	if (!GetIsHit()) { return; }

	if (m_EffectAnim02 > 8) { return; }

	GetDeviceContext()->PSSetShaderResources(0, 1, &m_HitTexture);

	Coordinate m_effectC;
	m_effectC.m_Pos = pos;
	m_effectC.m_Size = { 120.0f, 120.0f };
	DrawSpriteAnim(m_effectC, 8, 1, (int)m_EffectAnim02 % 8);

	m_EffectAnim02 += 0.5f;
}


XMFLOAT3 Enemy::KnockbackMove()
{
	//ノックバックスピード
	float knockBackSpeed = m_Speed / 3;
	
	XMStoreFloat3(&m_MoveVector,m_EnemyVector);

	//移動
	m_Coordinate.m_Pos.x += m_MoveVector.x * knockBackSpeed;
	m_Coordinate.m_Pos.y += m_MoveVector.y * knockBackSpeed;
	return XMFLOAT3(m_Coordinate.m_Pos);
}

