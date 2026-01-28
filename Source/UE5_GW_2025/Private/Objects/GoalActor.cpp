#include "Objects/GoalActor.h"
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Player/GWPlayerController.h"

AGoalActor::AGoalActor()
{
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AGoalActor::OnOverlapBegin);
}

void AGoalActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
	{
		// --- スローモーション開始 ---
		// 1.0 が等倍、0.1 なら 1/10 の速度になります
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);

		// 1. リザルトUIを表示
		if (ResultWidgetClass)
		{
			UUserWidget* ResultUI = CreateWidget<UUserWidget>(GetWorld(), ResultWidgetClass);
			if (ResultUI)
			{
				ResultUI->AddToViewport();

				// 2. マウスカーソルを表示し、操作をUIに切り替える
				AGWPlayerController* PC = Cast<AGWPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
				if (PC)
				{
					PC->bShowMouseCursor = true;
					PC->SetInputMode(FInputModeUIOnly());
				}

				// ※このアクターが持っている遷移情報を、Widget側に渡す仕組みが別途必要です（後述）
				// または GameInstance に「次にいく場所」を一旦保存しておく
				UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
				if (GI)
				{
					// GameInstanceに情報を一時保持（ChangeLevelAsyncを呼ぶ直前に使うため）
					GI->PendingNextLevelName = NextLevelName;
					GI->PendingTargetTag = TargetStartTag;
					GI->PendingCurrentLevelName = CurrentLevelName;
				}
			}
		}

		// ゴールアクターが何度も反応しないようにコリジョンを無効化
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}