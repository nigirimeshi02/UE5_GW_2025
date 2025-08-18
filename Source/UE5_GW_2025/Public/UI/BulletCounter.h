// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BulletCounter.generated.h"

/**
 * 
 */
UCLASS(abstract)
class UE5_GW_2025_API UBulletCounter : public UUserWidget
{
	GENERATED_BODY()
	
public:

	/** Allows Blueprint to update sub-widgets with the new bullet count */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shooter", meta = (DisplayName = "UpdateBulletCounter"))
	void BP_UpdateBulletCount(int32 MagazineSize, int32 BulletCount);

};
