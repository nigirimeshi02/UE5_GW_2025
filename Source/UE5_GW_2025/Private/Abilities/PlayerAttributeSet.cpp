// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/PlayerAttributeSet.h"

UPlayerAttributeSet::UPlayerAttributeSet():
	Health(300.f),
	MaxHealth(300.f),
	Stamina(100.f),
	MaxStamina(100.f),
	Energy(100.f),
	MaxEnergy(100.f),
	AttackPower(100.f),
	DefensePower(100.f),
	CriticalRate(5.f),
	CriticalDamage(150.f),
	AttackSpeed(1.0f),
	SkillCoolTime(5.0f),
	UltimateSkillCoolTime(30.0f)
{

}

void UPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// スタミナ値の更新が走った場合はスタミナ値をMaxとMinの間に収まるようにする
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, MaxHealth.GetCurrentValue()));
	}

	if (Data.EvaluatedData.Attribute == GetSkillCoolTimeAttribute())
	{
		if (GetSkillCoolTime() < 0)
		{
			SetSkillCoolTime(5.0f);
		}
	}

	if (Data.EvaluatedData.Attribute == GetUltimateSkillCoolTimeAttribute())
	{
		if (GetUltimateSkillCoolTime() < 0)
		{
			SetUltimateSkillCoolTime(30.0f);
		}
	}
}
