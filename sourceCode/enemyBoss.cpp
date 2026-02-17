//-------------------------------------------------------------
// enemyBoss.cpp
// 製作：楜澤旭渉　日付：2025/01/21
//-------------------------------------------------------------
#include "enemyBoss.h"
#include "sprite.h"
#include "texture.h"
#include "keyboard.h"
#include "stage3.h"
#include <cmath>
#include <ctime>
#include "sound.h"

void EnemyBoss::Draw()
{

	//影の表示
	if (((m_BossAttack == ATTACK_RISE  && m_RisePhase > 1) || m_BossAttack == ATTACK_BOSS_FALL) && GetState() == ENEMYSTATE_ATTACK) {

		//影の最大の大きさ
		XMFLOAT2 sadowSizeMax = { static_cast<float>(GetWidth(m_SadowTexture)) ,static_cast<float>(GetHeight(m_SadowTexture)) };//影の最大の大きさ

		//影の大きさ変更
		if (m_SadowSize.x < sadowSizeMax.x) {
			m_SadowSize.x += sadowSizeMax.x / 20;
		}
		if (m_SadowSize.y < sadowSizeMax.y) {
			m_SadowSize.y += sadowSizeMax.y / 20;
		}

		GetDeviceContext()->PSSetShaderResources(0, 1, &m_SadowTexture);
		DrawSpriteColor({ GetCoordinate().m_Pos.x, 600.f }, XM_PI, m_SadowSize, { 1.0, 1.0f, 1.0f, 0.8f });
	}

	GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[GetState()]);


	if (GetState() == ENEMYSTATE_ATTACK) {
		if (isPrepare) {
			GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[ENEMYSTATE_ATTACK_PREPARE]);
			DrawSpriteAnim(GetCoordinate(), 8, 1, static_cast<int>(m_AnimationFrame) % 5);
		}
		else if (m_BossAttack == ATTACK_STRAIGHT) {
			GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[ENEMYSTATE_ATTACK_02]);
			DrawSpriteAnim(GetCoordinate(), 8, 2, static_cast<int>(m_AnimationFrame) % 12);
		}
		else {
			DrawSpriteAnim(GetCoordinate(), 8, 2, static_cast<int>(m_AnimationFrame) % 16);
		}
	}

	else if (GetState() == ENEMYSTATE_ABSORBED) {
		DrawSpriteAnim(GetCoordinate(), 8, 1, static_cast<int>(m_AnimationFrame) % 8);
	}
	else if (GetState() == ENEMYSTATE_KNOCKOUT) {
		DrawSpriteAnim(GetCoordinate(), 8, 2, static_cast<int>(m_AnimationFrame * 0.5) % 10);
	}
	else if (GetState() == ENEMYSTATE_BULLET) {
		DrawSpriteAnim(GetCoordinate(), 1, 1, 1);
	}
	else if (GetState() == ENEMYSTATE_DAMEGED) {
		DrawSpriteAnim(GetCoordinate(), 1, 1, 1);
	}

	//瀕死状態のエフェクト
	if (isDyingEffect && m_Hp > 0.0f) {
		GetDeviceContext()->PSSetShaderResources(0, 1, &m_DyingTexture);
		DrawSpriteAnim(GetCoordinate(), 8, 1, static_cast<int>(m_AnimationFrame) % 8);
	}


}

void EnemyBoss::Update()
{

	m_AnimationFrame += 0.2f; //アニメーションさせる

	if (Entry()) {
		return;
	}

	//HPが500以下になったら行動パターンを変える
	if (m_Hp < 500 && !m_UsedSpecial) {
		m_BossAttack = ATTACK_ULTIMATE;
		m_UsedSpecial = true;
		m_isMovePhase = true;
	}
	

	Attack();//攻撃処理
	
	BossDamege();//ダメージ処理

	BossBreaked();//撃破処理

	RecordPlayerPosition();

	//ノックアウト状態でのアニメーションの調整
	if (GetState() == ENEMYSTATE_KNOCKOUT && m_AnimationFrame > 15) {
		m_AnimationFrame = 15;
		m_Coordinate.m_Pos = { SCREEN_WIDTH / 2,SCREEN_WIDTH / 2,0.0f };
		ResetViewport();
		BounceMovement();
	}
	
	//HPが低いほど瀕死エフェクトの表示確率が高くなる
	int probability = 100 - (m_Hp / 10); 
	static float dyingTimestart = 0;
	if (std::rand() % 100 < probability && !isDyingEffect) {
		isDyingEffect = true;
		dyingTimestart = m_AnimationFrame;
	}
	else if (m_AnimationFrame - dyingTimestart > 8) {
		isDyingEffect = false;
	}
	

	

	m_Time++;
}

bool EnemyBoss::Entry()
{
	if (IsEntry) { return false; }

	if(!isEntrySE){ 
		PlaySound(m_SE[0], false);
		isEntrySE = true;
	}

	if (m_Coordinate.m_Pos.x > 1000) {
		m_Coordinate.m_Pos.x -= m_Speed;
	
	}
	else {
		IsEntry = true;
		SetIsShake(false);
		//StopSound(m_SE[0]);
	}

	return true;
}

void EnemyBoss::Attack()
{
	if (isBreaked) {
		return;
	}

	
	switch (m_BossAttack)
	{
	case ATTACK_NONE:

		//HP量によって行動パターン変える
		if(!m_isMovePhase){
			FirstPhase();
		}
		else {
			SecondPhase();
		}
		
	
		break;
	case ATTACK_IDLE:
		AttackIdle();
		break;
	case ATTACK_SINGLE_POINST:
		AttackSinglePoint();
		break;
	case ATTACK_DISPERSION:
		AttackDispersion({ m_Coordinate.m_Pos.x, m_Coordinate.m_Pos.y });
		break;
	case ATTACK_STRAIGHT:
		AttackStraight();
		break;
	case ATTACK_RESET_POSITION:
		ResetPosition();
		break;
	case ATTACK_RISE:
		AttackRise();
		break;
	case ATTACK_ULTIMATE:
		AttackUltimate();
		break;
	case ATTACK_ULTIMATE_PREPARE:
		AttackPrepare();
		break;
	case ATTACK_BULLET_FALL:
		AttackBulletFall();
		break;
	case ATTACK_BOSS_FALL:
		AttackBossFall();
		break;
	case ATTACK_MAX:
		break;
	default:
		break;
	}
}

void EnemyBoss::FirstPhase()
{
	//次の攻撃へ
	if (m_BossAttack == ATTACK_NONE) {
		switch (m_AttackPhase) {
		case 0: //遠距離攻撃
			PlaySound(m_SE[1], false);
			if (rand() % 2 == 0) {
				m_BossAttack = ATTACK_SINGLE_POINST;
			}
			else {
				m_BossAttack = ATTACK_DISPERSION;
			}
			m_AttackStartTime = m_Time;
			m_AttackPhase++;
			break;

		case 1: //近距離攻撃
			if (rand() % 2 == 0) {
				PlaySound(m_SE[2], false);
				m_BossAttack = ATTACK_STRAIGHT;
			}
			else {
				m_BossAttack = ATTACK_RISE;
			}
			m_AttackPhase++;
			break;
		case 2: //近距離攻撃
			if (rand() % 2 == 0) {
				PlaySound(m_SE[2], false);
				m_BossAttack = ATTACK_STRAIGHT;
			}
			else {
				m_BossAttack = ATTACK_RISE;
			}
			m_AttackPhase = 0;
			break;
		case 3: // ループさせるためにフェーズをリセット
			m_AttackPhase = 0;  //フェーズを最初に戻す
			break;
		}
	}
}



void EnemyBoss::SecondPhase()
{
	if (m_BossAttack == ATTACK_NONE) {

		isPrepare = false;

		switch (m_AttackPhase) {
		case 0: //一点射撃
			PlaySound(m_SE[1], false);
			m_BossAttack = ATTACK_SINGLE_POINST;

			m_AttackStartTime = m_Time;
			m_AttackPhase++;
			break;
		case 1: //分散
			PlaySound(m_SE[1], false);
			m_BossAttack = ATTACK_DISPERSION;
			m_AttackStartTime = m_Time;
			m_AttackPhase++;
			break;
		case 2: //突進 or 上昇
			if (rand() % 2 == 0) {
				PlaySound(m_SE[2], false);
				m_BossAttack = ATTACK_STRAIGHT;
				isPrepare = true;
			}
			else {
				m_BossAttack = ATTACK_RISE;
			}
			m_AttackPhase++;
			break;
		case 3: //一点射撃
			PlaySound(m_SE[1], false);
			m_BossAttack = ATTACK_SINGLE_POINST;
			m_AttackStartTime = m_Time;
			m_AttackPhase++;
			break;

		case 4:  //突進
			PlaySound(m_SE[2], false);
			m_BossAttack = ATTACK_STRAIGHT;
			isPrepare = true;
			m_AttackPhase++;
			break;
		case 5:  //射撃
			PlaySound(m_SE[1], false);
			if (rand() % 2 == 0) {
				m_BossAttack = ATTACK_SINGLE_POINST;
			}
			else {
				m_BossAttack = ATTACK_DISPERSION;
			}
			m_AttackPhase++;
			break;
		case 6: //0に戻す
			m_AttackPhase = 0;
			break;
		}
	}
}

void EnemyBoss::AttackIdle()
{
	float IDLE_DURATION = 180;

	if (m_isMovePhase) { IDLE_DURATION = 60; }
	//上下移動
	float moveOffset = 3.5f * cos(m_Time * 0.02f)  ; // 上下にゆっくり振動
	m_Coordinate.m_Pos.y += moveOffset;

	//2秒経過したらATTACK_NONEに戻る
	if (m_Time - m_IdleStartTime >= IDLE_DURATION) {
		m_BossAttack = ATTACK_NONE;
	}
}

void EnemyBoss::EnemyBossShot(XMFLOAT2 pos, int type, int dir)
{
	EnemyShot({ pos.x, pos.y }, ENEMYTYPE_BOSS,dir + 3);
	EnemyShot({ pos.x, pos.y + 30 }, ENEMYTYPE_BOSS, dir);
	EnemyShot({ pos.x, pos.y - 30,}, ENEMYTYPE_BOSS, dir);
}



///突進攻撃
void EnemyBoss::AttackStraight()
{
	// まず攻撃準備アニメーションを再生
	if (isPrepare){
		
		//アニメーション時間をカウントし、一定時間経過したら攻撃開始
		if (m_Time - m_AttackStartTime >= 30) 
		{
			isPrepare = false;
			m_AttackStartTime = m_Time; //攻撃開始の時間を記録
			m_AnimationFrame = 0.0f;
			PlaySound(m_SE[3], false);
		}
		return; 
	}

	// ATTACK_STRAIGHT の処理
	if (m_BossAttack == ATTACK_STRAIGHT){
		m_Coordinate.m_Pos.x -= m_Speed * 3.0f; // 左に移動（突進）

		// `x` が -500 より小さくなったらワープ
		if (m_Coordinate.m_Pos.x < -500)
		{
			m_Coordinate.m_Pos.x = SCREEN_WIDTH + 300;
			m_Coordinate.m_Pos.y = 400;
			m_BossAttack = ATTACK_RESET_POSITION;
			isPrepare = false;
		}
	}
}

void EnemyBoss::ResetPosition()
{
	//x 軸のリセット
	if (m_Coordinate.m_Pos.x > BOSS_RESET_POS_X) {
		m_Coordinate.m_Pos.x -= m_Speed * 2.0f;
		if (abs(m_Coordinate.m_Pos.x - BOSS_RESET_POS_X) < 10.0f) {
			m_Coordinate.m_Pos.x = BOSS_RESET_POS_X;
		}
	}
	else if (m_Coordinate.m_Pos.x < BOSS_RESET_POS_X) {
		m_Coordinate.m_Pos.x += m_Speed * 2.0f;
		if (abs(m_Coordinate.m_Pos.x - BOSS_RESET_POS_X) < 10.0f) {
			m_Coordinate.m_Pos.x = BOSS_RESET_POS_X;
		}
	}

	//y 軸のリセット
	if (m_Coordinate.m_Pos.y > BOSS_RESET_POS_Y) {
		m_Coordinate.m_Pos.y -= m_Speed * 1.0f;
		if (abs(m_Coordinate.m_Pos.y - BOSS_RESET_POS_Y) < 10.0f) {
			m_Coordinate.m_Pos.y = BOSS_RESET_POS_Y;
		}
	}
	else if (m_Coordinate.m_Pos.y < BOSS_RESET_POS_Y) {
		m_Coordinate.m_Pos.y += m_Speed * 1.0f;
		if (abs(m_Coordinate.m_Pos.y - BOSS_RESET_POS_Y) < 10.0f) {
			m_Coordinate.m_Pos.y = BOSS_RESET_POS_Y;
		}
	}

	//両方がリセットされたらATTACK_IDLEに戻る
	if (m_Coordinate.m_Pos.x == BOSS_RESET_POS_X && m_Coordinate.m_Pos.y == BOSS_RESET_POS_Y) {
		m_BossAttack = ATTACK_IDLE;
		m_IdleStartTime = m_Time;
	}

	//影のリセット
	m_SadowSize = { 0.0f ,0.0f };
}


void EnemyBoss::AttackSinglePoint()
{
	if (m_Time - m_SpanTime >= 5.0f){
		// 弾を発射
		EnemyBossShot({ m_Coordinate.m_Pos.x, m_Coordinate.m_Pos.y }, ENEMYTYPE_BOSS, 0);
		m_SpanTime = m_Time;
	}

	// 1秒（60フレーム）経過したら ATTACK_IDLE に戻す
	if (m_Time - m_AttackStartTime >= 60){
		m_BossAttack = ATTACK_IDLE;
		m_AttackStartTime = m_Time;
		m_IdleStartTime = m_Time;
	}
}


void EnemyBoss::AttackDispersion(XMFLOAT2 pos)
{
	// 攻撃の持続時間（1秒 = 60フレーム）
	const int DISPERSION_ATTACK_DURATION = 60;
	const int DISPERSION_SHOT_INTERVAL = 5;  // 5フレームごとに発射


	// 波のパラメータ
	const float INTERVAL = 0.1f;		  // 弾の発射間隔
	const float WAVE_FREQUENCY = 0.1f;    // 波の振動頻度
	const float WAVE_AMPLITUDE = 120.0f;   //波の振れ幅

	dispersionTime += GameFrameTime::GetElapsedTime(); // 時間経過を記録

	// 1秒間（60フレーム）攻撃を続ける
	if (m_Time - m_AttackStartTime >= DISPERSION_ATTACK_DURATION) {
		m_BossAttack = ATTACK_IDLE;
		m_IdleStartTime = m_Time;
		return;
	}

	if (dispersionTime < INTERVAL) { return; }
		dispersionTime = 0.0f; //タイマーリセット

	for (int i = -1; i <= 1; i += 2) {
		float phaseShift = (m_Time * WAVE_FREQUENCY) * i;

		float waveX = m_Speed;  //弾の横移動
		float waveY = WAVE_AMPLITUDE * cos(phaseShift); //上下に波打つ動き

		EnemyShot({ pos.x, pos.y + waveY }, ENEMYTYPE_BOSS, 0);
		EnemyShot({ pos.x, pos.y - waveY }, ENEMYTYPE_BOSS, 0);
	}
	
}


void EnemyBoss::AttackRise()
{
	const float RISE_SPEED = 10.0f;   // 上昇速度
	const float FALL_SPEED = 20.0f;   // 降下速度
	const int WAIT_TIME = 180;        // 3秒


	switch (m_RisePhase){
	case 0: // 上昇
		m_Coordinate.m_Pos.y -= RISE_SPEED;
		if (m_Coordinate.m_Pos.y <= -1000)
		{
			m_Coordinate.m_Pos.y = -1000;
			m_RisePhase = 1;  // 3秒待機フェーズへ
			m_RiseTime = m_Time;
		}
		break;

	case 1: // 3秒待機
		if (m_Time - m_RiseTime >= WAIT_TIME){
			m_RisePhase = 2;  // 落下フェーズへ
		}
		break;

	case 2: // 落下
		m_BossAttack = ATTACK_BOSS_FALL;
		m_RisePhase = 0;
		m_AttackStartTime = m_Time;
		break;
	}
}

//ヒット処理
void EnemyBoss::BossDamege()
{
	if (!GetIsHit()) { return; }
	
	// シェイクのオフセットを計算
	float shakeOffsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20;
	float shakeOffsetY = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20;

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


//ボスのUI
void EnemyBoss::DrawUI()
{

	// ターゲットマーカーを表示
	if (m_Time - m_TargetStartTime < 60){
		for (const auto& pos : m_TargetPositions){
			GetDeviceContext()->PSSetShaderResources(0, 1, &m_TargetTexture);
			
			DrawSpriteAnim({ pos.x,pos.y }, 0.0f, { 100.0f, 100.0f }, 8, 1, static_cast<int>(m_AnimationFrame) % 8);

		}
	
	}

	XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };

	//プレイヤーと重なったときに透過
	if (GetPlayer()->m_Coordinate.m_Pos.x > SCREEN_XCENTER - 350.0f && GetPlayer()->m_Coordinate.m_Pos.x < SCREEN_XCENTER + 350.0f &&
		GetPlayer()->m_Coordinate.m_Pos.y > SCREEN_HEIGHT - 70.0f && GetPlayer()->m_Coordinate.m_Pos.y < SCREEN_HEIGHT){
		color = { 1.0f,1.0f,1.0f,0.5f };
	}


	//背景バー
	XMFLOAT2 bgPos = { SCREEN_XCENTER, SCREEN_HEIGHT - 40.0f };
	XMFLOAT2 bgSize = { 700.0f, 60.0f };
	GetDeviceContext()->PSSetShaderResources(0, 1, &m_FrameTexture);
	DrawSpriteColor(bgPos, 0.0f, bgSize, color);

	//HPバーの長さ位置を計算
	float hpPercentage = m_Hp / 1000.0f; //HP比率 
	const float barWidth = bgSize.x * hpPercentage;
	const float barMin = 370.0f;
	float barPositionX = barMin + (SCREEN_XCENTER - barMin) * hpPercentage;
	
	//HPバーを描画
	XMFLOAT2 barPos = { barPositionX, bgPos.y };
	XMFLOAT2 barSize = { barWidth, 50.0f };
	GetDeviceContext()->PSSetShaderResources(0, 1, &m_HpTexture);
	DrawSpriteColor(barPos, 0.0f, barSize, color);

	//名前の大きさ
	const XMFLOAT2 nameSize = { static_cast<float>(GetWidth(m_NameTexture)),   static_cast<float>(GetHeight(m_NameTexture))};
	const XMFLOAT2 secondNameSize = { static_cast<float>(GetWidth(m_SecondNameTexture)),   static_cast<float>(GetHeight(m_SecondNameTexture))};

	//ボスの名前表示
	GetDeviceContext()->PSSetShaderResources(0, 1, &m_NameTexture);
	DrawSpriteColor(XMFLOAT2(SCREEN_XCENTER , barPos.y - 60.0f ), 0.0f, nameSize, color);

	//ボスの二つ名表示
	GetDeviceContext()->PSSetShaderResources(0, 1, &m_SecondNameTexture);
	DrawSpriteColor(XMFLOAT2(SCREEN_XCENTER, barPos.y - 70.0f), 0.0f, secondNameSize, color);




	//クラッシュ画面
	if (m_IsShowCrackEffect) {

		TextureForVideo* videoTex = g_VideoTexture.getTexture();

		//動画再生
		if (videoTex && videoTex->shader_resource_view) {
			DrawVideoSpriteChromaKey({GetCoordinate().m_Pos.x - 200, GetCoordinate().m_Pos.y }, 0, {SCREEN_WIDTH * 2.0, SCREEN_HEIGHT * 2.0}, g_VideoTexture);
		}
	}

}

void EnemyBoss::BossBreaked()
{
	if (CheckBossBreaked() && !isBreaked) {
		SetCanAbsorb(true);
		SetState(ENEMYSTATE_KNOCKOUT);
		m_MovePattern = ATTACK_NONE;
		isBreaked = true;
		m_AnimationFrame = 0.0f;
		m_Coordinate.m_Pos = { 800.0f, 400.0f,0.0f };
		SetIsClear(true);
		ZoomViewport(m_Coordinate.m_Pos,0.5);
	}


	//発射されたとき
	if (isShot) {
		m_IsBossShot = true;
	}

	//撃破後の動き
	MoveBreaked();
}

// 必殺技の開始
void EnemyBoss::AttackUltimate()
{
	PlaySound(m_SE[1], false);
	m_BossAttack = ATTACK_ULTIMATE_PREPARE;
	m_AttackStartTime = m_Time; // 開始時間を記録
}

// 画面外移動準備 & SinglePoint弾発射
void EnemyBoss::AttackPrepare()
{
	// 1秒（60フレーム）経過したら ATTACK_IDLE に戻す
	if (m_Time - m_AttackStartTime >= 60) {
		// ボスが左上の画面外へ移動
		m_Coordinate.m_Pos.y -= 10.0f;

		if (m_Coordinate.m_Pos.x < -200 || m_Coordinate.m_Pos.y < -200) {
			m_BossAttack = ATTACK_BULLET_FALL;
			m_AttackStartTime = m_Time;
		}
	}
	else if(m_Time - m_SpanTime >= 7.0f) {
		// 弾を発射
		EnemyBossShot({ m_Coordinate.m_Pos.x, m_Coordinate.m_Pos.y }, ENEMYTYPE_BOSS, 1);
		m_SpanTime = m_Time;
	}
}

// 弾落下フェーズ
void EnemyBoss::AttackBulletFall()
{
	if (playerHistory.empty()) return;

	// ターゲットマーカーと弾の処理の間隔
	const int TARGET_DISPLAY_TIME = 60; // ターゲットマーカー表示時間
	const int BULLET_FALL_INTERVAL = 90; // 弾の発射間隔
	const int MAX_BULLET_FALLS = 3;  // 落下回数

	// 新しいターゲットを作る
	if (m_BulletFallCount < MAX_BULLET_FALLS && (m_Time - m_TargetStartTime) >= BULLET_FALL_INTERVAL)
	{
		XMFLOAT3 targetPos = GetPlayer()->m_Coordinate.m_Pos;
		m_TargetPositions.clear(); // 直前のターゲットを消す
		m_TargetPositions.push_back(targetPos);
		m_TargetStartTime = m_Time; // マーカーの開始時間を記録
		
		
	}

	
	// ターゲット表示中でも弾を落とす
	if (static_cast<int>(m_Time - m_TargetStartTime) % 90 == 30) // 10フレームごとに発射
	{
		XMFLOAT3 targetPos = m_TargetPositions.front();
		for (int i = 0; i < 12; i++)
		{
			EnemyShot({ targetPos.x, -30.0f * i }, ENEMYTYPE_BOSS, 5);
			EnemyShot({ targetPos.x + 30, -30.0f * i }, ENEMYTYPE_BOSS, 2);
			EnemyShot({ targetPos.x - 30, -30.0f * i }, ENEMYTYPE_BOSS, 2);
		}
		m_BulletFallCount++;
		PlaySound(m_SE[1], false);
	}
	

	

	// 3回繰り返したら次のフェーズへ
	if (m_BulletFallCount >= MAX_BULLET_FALLS)
	{
		m_BossAttack = ATTACK_BOSS_FALL;
		m_AttackStartTime = m_Time;
		m_BulletFallCount = 0; // 次回のためにリセット
	}
}



// ボス落下フェーズ
void EnemyBoss::AttackBossFall()
{
	const float FALL_SPEED = 20.0f;
	const float MOVE_SPEED_X = 20.0f; // X方向の移動速度
	const int WAIT_TIME = 60; 

	

	// 0.5秒間、プレイヤーの位置へ X 軸移動
	if (m_Time - m_AttackStartTime < WAIT_TIME && m_Coordinate.m_Pos.y < 0) {
		XMFLOAT3 targetPos = GetPlayer()->m_Coordinate.m_Pos;

		// X 軸のみプレイヤーの位置に近づける
		if (m_Coordinate.m_Pos.x < targetPos.x ) {
			m_Coordinate.m_Pos.x += MOVE_SPEED_X;
			if (m_Coordinate.m_Pos.x > targetPos.x) {
				m_Coordinate.m_Pos.x = targetPos.x;
			}
		}
		else if (m_Coordinate.m_Pos.x > targetPos.x) {
			m_Coordinate.m_Pos.x -= MOVE_SPEED_X;
			if (m_Coordinate.m_Pos.x < targetPos.x) {
				m_Coordinate.m_Pos.x = targetPos.x;
			}
		}

		// 0.5秒後に落下位置を確定
		if (m_Time - m_AttackStartTime == WAIT_TIME - 1) {
			m_TargetPositions.clear();
			m_TargetPositions.push_back({ m_Coordinate.m_Pos.x, -500.0f ,0.0f});
		}
		return;
	}

	// 落下処理開始
	if (!m_TargetPositions.empty()) {
		m_Coordinate.m_Pos.y += FALL_SPEED;
	}

	// 地面に到達したらリセット
	if (m_Coordinate.m_Pos.y >= 500) {
		PlaySound(m_SE[4], false);
		m_Coordinate.m_Pos.y = 500; // 元の位置に戻る
		m_BossAttack = ATTACK_RESET_POSITION; // 通常状態へ戻る
	}
}


// プレイヤーの座標を記録
void EnemyBoss::RecordPlayerPosition()
{
	XMFLOAT2 playerPos = { GetPlayer()->m_Coordinate.m_Pos.x ,GetPlayer()->m_Coordinate.m_Pos.y };
	playerHistory.push(playerPos);
	if (playerHistory.size() > 10) { //1/6秒前のデータを保持（30フレーム分）
		playerHistory.pop();
	}
}

void EnemyBoss::MoveBreaked()
{
	if (m_IsBossShot && !m_IsShowCrackEffect) {
		SetCenter({ GetCenter().x + 2.0f,GetCenter().y,  GetCenter().z});
		
		// 弾を回転
		m_Coordinate.m_Rotation += 0.1f;

		// サイズを徐々に大きく
		m_Coordinate.m_Size.x += 5.0f;
		m_Coordinate.m_Size.y += 5.0f;

		// 一定サイズ以上でひび割れエフェクトを表示
		if (m_Coordinate.m_Size.x > 700.0f) {
			m_IsShowCrackEffect = true;
			m_CrashPos = GetCenter();
			canMove = false;
			SetToResult(true);
			PlaySound(m_SE[5], false);
		}
	}

	if (m_IsShowCrackEffect){
		SetCenter(m_CrashPos);
		g_VideoTexture.update(1.0f);
	}

}


float EnemyBoss::EaseOutBounce(float t)
{
	if (t < 1 / 2.75f)
	{
		return (7.5625f * t * t);
	}
	else if (t < 2 / 2.75f)
	{
		t -= 1.5f / 2.75f;
		return (7.5625f * t * t + 0.75f);
	}
	else if (t < 2.5f / 2.75f)
	{
		t -= 2.25f / 2.75f;
		return (7.5625f * t * t + 0.9375f);
	}
	else
	{
		t -= 2.625f / 2.75f;
		return (7.5625f * t * t + 0.984375f);
	}
}


void EnemyBoss::BounceMovement()
{
	//開始時間を設定
	if (m_BounceStartTime == 0)
	{
		m_BounceStartTime = m_Time;
		m_MaxPosY = 200.0f;  //落下開始位置
		m_TargetPosY = 400.0f; //着地地点
		m_Coordinate.m_Pos.y = m_MaxPosY; 

		//X 軸の移動設定
		m_StartPosX = m_Coordinate.m_Pos.x;
		m_TargetPosX = m_StartPosX + 200.0f; 
	}

	//経過時間
	float elapsedTime = (m_Time - m_BounceStartTime) / 60.0f; 
	//移動の持続時間
	float moveDuration = 3.0f; 

	//落下 バウンド
	float bounceValue = EaseOutBounce(elapsedTime / moveDuration);

	
	m_Coordinate.m_Pos.y = m_MaxPosY + (m_TargetPosY - m_MaxPosY) * bounceValue;
	m_Coordinate.m_Pos.x = m_StartPosX + (m_TargetPosX - m_StartPosX) * (elapsedTime / moveDuration);

	//終了判定
	if (elapsedTime > moveDuration)
	{
		m_Coordinate.m_Pos.y = m_TargetPosY; 
		m_Coordinate.m_Pos.x = m_TargetPosX; 
		m_BossAttack = ATTACK_IDLE; // 通常状態へ戻る
	}
}
