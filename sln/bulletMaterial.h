//-------------------------------------------------------------
// bulletMaterial.h
// 製作：笠原凜　日付：2024/11/01
//-------------------------------------------------------------


//--------------------------概要-------------------------------
// 弾の素材になれるやつらのクラスを管理しています。
// 完成した弾へ、弾としての攻撃力やらの数値を与えるために存在します。
// 
// 敵は基本これを継承します。
// ただしボス・準ボスエネミーは吸えないため、パラメーターは無効です。
//-------------------------------------------------------------


#ifndef _BULLETMATERIAL_H
#define _BULLETMATERIAL_H

#include "bulletType.h"

class BulletMaterial {
private:
	float m_BulletSize = 0.0f;				//弾素材としての大きさ
	float m_BulletHp = 10.0f;				//弾素材としての体力・耐久値
	float m_BulletWeight = 0.0f;			//弾素材としての重さ
	float m_BulletAtk = 0.0f;				//弾素材になったときの攻撃力
	BulletType m_BulletTypeParam;			//弾素材としての属性値
	bool m_BulletExpGain = true;			//弾素材になったときに弾が経験値を獲得するか

	bool m_CanAbsorbe = true;
public:
	BulletMaterial() {};
	BulletMaterial(float size, float hp, float weight, float atk, BulletType typeParam)
	{
		m_BulletSize = size;
		m_BulletHp = hp;
		m_BulletWeight = weight;
		m_BulletAtk = atk;
		m_BulletTypeParam = typeParam;
	}

	~BulletMaterial() {};

	void SetBulletSize(float size) { m_BulletSize = size; }
	float GetBulletSize(void) { return m_BulletSize; }

	void SetBulletHp(float hp) { m_BulletHp = hp; }
	float GetBulletHp(void) { return m_BulletHp; }

	void SetBulletWeight(float weight) { m_BulletWeight = weight; }
	float GetBulletWeight(void) { return m_BulletWeight; }

	void SetBulletAtk(float atk) { m_BulletAtk = atk; }
	float GetBulletAtk(void) { return m_BulletAtk; }

	void SetBulletTypeParam(BulletType typeParam) { m_BulletTypeParam = typeParam; }
	BulletType GetBulletTypeParam(void) { return m_BulletTypeParam; }

	void SetBulletExpGain(bool expgain) { m_BulletExpGain = expgain; }
	bool GetBulletExpGain(void) { return m_BulletExpGain; }
};


#endif // !_BULLETMATERIAL_H_