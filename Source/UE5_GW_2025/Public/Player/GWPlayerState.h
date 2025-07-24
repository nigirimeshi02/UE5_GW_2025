// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GWPlayerState.generated.h"

/**
 * GWプレイヤーステート
 */
UCLASS()
class UE5_GW_2025_API AGWPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:

	// AbilitySystemComponent
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// AttributeSet
	UPROPERTY()
	TObjectPtr<class UAttributeSet> AttributeSet;

public:
	// コンストラクタ
	AGWPlayerState();

	// AbilitySystemComponentを取得する
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// AttributeSetを取得する
	class UAttributeSet* GetAttributeSet() const { return AttributeSet; };

};
