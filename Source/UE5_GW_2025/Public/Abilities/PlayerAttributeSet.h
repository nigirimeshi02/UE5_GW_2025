// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "PlayerAttributeSet.generated.h"

//アクセサ(Setter、Getter)を生成するためのマクロ
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * プレイヤーのAttributeSet
 */
UCLASS()
class UE5_GW_2025_API UPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:

	// コンストラクタ
	UPlayerAttributeSet();

	// GameplayEffect動作後の処理
	void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)override;

	//体力
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Health)

	//最大体力
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxHealth)

	//スタミナ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Stamina)

	//最大スタミナ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxStamina)

	//エネルギー
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData Energy;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Energy)

	//最大エネルギー
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData MaxEnergy;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxEnergy)

	//攻撃力
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, AttackPower)

	//防御力
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, DefensePower)

	//クリティカル率
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData CriticalRate;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, CriticalRate)

	//クリティカルダメージ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData CriticalDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, CriticalDamage)

	//攻撃速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, AttackSpeed)

	//スキルクールタイム
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData SkillCoolTime;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SkillCoolTime)

	//必殺技クールタイム
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData UltimateSkillCoolTime;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, UltimateSkillCoolTime)
};
