////-------------------------------------------------------------
//// enemy_01.cpp
//// 製作：笠原凜　日付：2024/11/06
////-------------------------------------------------------------
//#include "enemy_01.h"
//#include "sprite.h"
//#include "texture.h"
//#include "keyboard.h"
//#include "player.h"
//
//static ID3D11ShaderResourceView* g_Texture[2];
//
//
//static constexpr int SPOWN_POINT = SCREEN_WIDTH / 2.0f;
//
//static EnemyStraight g_Enemy[ENEMY_MAX];
//
//static int frameNum = 0;
//
//void EnemyStraight::Update()
//{
//	Player* pPlayer = GetPlayer();
//	m_Coordinate.m_OldPos = m_Coordinate.m_Pos;
//	if (GetUse())
//	{
//		m_Coordinate.m_Pos = EnemyMove(GetMovePattern(), m_Coordinate, m_EnemyState);
//	}
//
//	m_EnemyState.m_Time++;
//}
//
//void EnemyStraight::Draw()
//{
//	switch (GetState())
//	{
//	case ENEMYSTATE_NORMAL:
//	case ENEMYSTATE_ATTACK:
//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
//		m_Coordinate.m_Size = { 100.0f, 100.0f };
//		DrawSprite(m_Coordinate);
//		break;
//	case ENEMYSTATE_ABSORBED:
//	case ENEMYSTATE_KNOCKOUT:
//	case ENEMYSTATE_BULLET:
//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);
//		DrawSprite(m_Coordinate);
//		break;
//	default:
//		break;
//	}
//}
//
//
////=======================
////バリア付き敵処理
////======================
//void EnemyUnique::Update()
//{
//	m_Coordinate.m_OldPos = m_Coordinate.m_Pos;
//	m_Coordinate.m_Pos = EnemyMove(GetMovePattern(), m_Coordinate, m_EnemyState);
//
//	if (isBarrier) {
//
//	}
//	m_EnemyState.m_Time++;
//}
//
//
//void EnemyUnique::Draw()
//{
//	switch (GetState())
//	{
//	case ENEMYSTATE_NORMAL:
//		break;
//	case ENEMYSTATE_ATTACK:
//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_TextureEnemyUnique);
//		DrawSprite(m_Coordinate);
//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_TextureBarrier);
//		DrawSprite(m_BarrierState);
//		break;
//	case ENEMYSTATE_ABSORBED:
//		break;
//	case ENEMYSTATE_KNOCKOUT:
//		break;
//	case ENEMYSTATE_BULLET:
//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_TextureEnemyUnique);
//		DrawSprite(m_Coordinate);
//		break;
//	default:
//		break;
//	}
//}
//
//void EnemyStrong::Draw()
//{
//	switch (GetState())
//	{
//	case ENEMYSTATE_NORMAL:
//		break;
//	case ENEMYSTATE_ATTACK:
//		GetDeviceContext()->PSSetShaderResources(0, 1, & g_Texture[0]);
//		DrawSprite(m_Coordinate);
//		break;
//	case ENEMYSTATE_ABSORBED:
//		break;
//	case ENEMYSTATE_KNOCKOUT:
//		break;
//	case ENEMYSTATE_BULLET:
//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
//		DrawSprite(m_Coordinate);
//		break;
//	default:
//		break;
//	}
//}
//
//void EnemyStrong::Update()
//{
//	m_Coordinate.m_OldPos = m_Coordinate.m_Pos;
//	m_Coordinate.m_Pos = EnemyMove(GetMovePattern(), m_Coordinate, m_EnemyState);
//	m_EnemyState.m_Time++;
//}
//
//void EnemyBoss::Draw()
//{
//	switch (GetState())
//	{
//	case ENEMYSTATE_NORMAL:
//		break;
//	case ENEMYSTATE_ATTACK:
//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
//		DrawSprite(m_Coordinate);
//		break;
//	case ENEMYSTATE_ABSORBED:
//		break;
//	case ENEMYSTATE_KNOCKOUT:
//		break;
//	case ENEMYSTATE_BULLET:
//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
//		DrawSprite(m_Coordinate);
//		break;
//	default:
//		break;
//	}
//}
//
//void EnemyBoss::Update()
//{
//	m_Coordinate.m_OldPos = m_Coordinate.m_Pos;
//	m_Coordinate.m_Pos = EnemyMove(GetMovePattern(), m_Coordinate, m_EnemyState);
//	m_EnemyState.m_Time++;
//}
//
//void EnemyWeak::Draw()
//{
//
//
//	switch (GetState())
//	{
//	case ENEMYSTATE_NORMAL:
//		break;
//	case ENEMYSTATE_ATTACK:
//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
//		DrawSprite(m_Coordinate);
//		break;
//	case ENEMYSTATE_ABSORBED:
//		break;
//	case ENEMYSTATE_KNOCKOUT:
//		break;
//	case ENEMYSTATE_BULLET:
//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
//   		DrawSprite(m_Coordinate);
//		break;
//	default:
//		break;
//	}
//}
//
//void EnemyWeak::Update()
//{
//	m_Coordinate.m_OldPos = m_Coordinate.m_Pos;
//	m_Coordinate.m_Pos = EnemyMove(GetMovePattern(),m_Coordinate,m_EnemyState);
//	m_EnemyState.m_Time++;
//}
//
//void EnemyWeakRegist::Draw()
//{
//	switch (GetState())
//	{
//	case ENEMYSTATE_NORMAL:
//		break;
//	case ENEMYSTATE_ATTACK:
//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
//		DrawSprite(m_Coordinate);
//		break;
//	case ENEMYSTATE_ABSORBED:
//		break;
//	case ENEMYSTATE_KNOCKOUT:
//		break;
//	case ENEMYSTATE_BULLET:
//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
//		DrawSprite(m_Coordinate);
//		break;
//	default:
//		break;
//	}
//}
//
//void EnemyWeakRegist::Update()
//{
//	m_Coordinate.m_OldPos = m_Coordinate.m_Pos;
//	m_Coordinate.m_Pos = EnemyMove(GetMovePattern(), m_Coordinate, m_EnemyState);
//	m_EnemyState.m_Time++;
//}
