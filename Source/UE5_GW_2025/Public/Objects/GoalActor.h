#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h" 
#include "GoalActor.generated.h"

UCLASS()
class UE5_GW_2025_API AGoalActor : public AActor
{
	GENERATED_BODY()

public:
	AGoalActor();

protected:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, Category = "Level")
	FName NextLevelName;

	UPROPERTY(EditAnywhere, Category = "Level")
	FName CurrentLevelName;

	// 次のレベルで探すべきPlayerStartのタグ
	UPROPERTY(EditAnywhere, Category = "Transition")
	FName TargetStartTag;

	// --- UI用 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelTransition")
	TSubclassOf<UUserWidget> ResultWidgetClass;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};