// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GWPlayerState.h"
#include "Abilities/PlayerAttributeSet.h"

AGWPlayerState::AGWPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AGWPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
