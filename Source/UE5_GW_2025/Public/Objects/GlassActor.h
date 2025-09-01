// GlassActor.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GlassActor.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class UE5_GW_2025_API AGlassActor : public AActor
{
    GENERATED_BODY()

public:
    AGlassActor();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, Category = "Glass")
    UGeometryCollectionComponent* GlassCollection;

    UFUNCTION()
    void OnGlassHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UPROPERTY(EditAnywhere, Category = "Glass")
    UMaterialInterface* BrokenGlassMaterial; // エディタで割れた後のガラス材質を指定

    UFUNCTION()
    void OnGlassBroken(const FChaosBreakEvent& BreakEvent);

    UPROPERTY(EditAnywhere, Category = "Glass")
    float BreakSpeedThreshold = 600.0f;

	bool bIsBroken = false;
};
