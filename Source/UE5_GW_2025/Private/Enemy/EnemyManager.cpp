// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyManager.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/EnemyBase.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AEnemyManager::AEnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	// Enemyタグを持つアクターをすべて取得
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), FoundEnemies);
	RemainingEnemies = FoundEnemies.Num();

	for (AActor* Actor : FoundEnemies)
	{
		if (AEnemyBase* Enemy = Cast<AEnemyBase>(Actor))
		{
			Enemy->OnEnemyDied.AddDynamic(this, &AEnemyManager::OnEnemyDied);
		}
	}
	
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyManager::OnEnemyDied()
{
	RemainingEnemies--;
	CheckAllEnemiesDefeated();
}

void AEnemyManager::CheckAllEnemiesDefeated()
{
	if (RemainingEnemies <= 0)
	{
		if (ClearWidgetClass)
		{
			UUserWidget* ClearWidget = CreateWidget<UUserWidget>(GetWorld(), ClearWidgetClass);
			if (ClearWidget)
			{
				ClearWidget->AddToViewport();
			}
		}

		// 3秒後にレベル遷移
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, [this]()
			{
				FName NextLevelName = FName("NextLevelName");
				UGameplayStatics::OpenLevel(GetWorld(), NextLevelName);
			}, 3.0f, false);
	}
}

