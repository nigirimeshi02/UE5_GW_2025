#include "Objects/GoalActor.h"
#include "MyGameInstance.h"

AGoalActor::AGoalActor()
{
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AGoalActor::OnOverlapBegin);
}

void AGoalActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// プレイヤーが触れたかチェック
	if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
	{
		UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
		if (GI)
		{
			GI->ChangeLevelAsync(NextLevelName, CurrentLevelName, TargetStartTag);
		}
	}
}