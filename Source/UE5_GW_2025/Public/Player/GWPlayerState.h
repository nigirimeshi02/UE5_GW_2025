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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// AttributeSet
	UPROPERTY()
	TObjectPtr<class UPlayerAttributeSet> AttributeSet;

public:
	// コンストラクタ
	AGWPlayerState();

public:
	// AbilitySystemComponentを取得する
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	// AttributeSetを取得する
	class UPlayerAttributeSet* GetAttributeSet() const { return AttributeSet; };

};
