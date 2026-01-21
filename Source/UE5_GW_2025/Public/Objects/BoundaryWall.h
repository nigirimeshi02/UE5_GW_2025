#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoundaryWall.generated.h"

UCLASS()
class UE5_GW_2025_API ABoundaryWall : public AActor
{
    GENERATED_BODY()

public:
    ABoundaryWall();

protected:
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* WallMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Settings")
    FLinearColor WallColor = FLinearColor::Red;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Settings")
    float VisibleRadius = 200.0f; // å©Ç¶énÇﬂÇÈãóó£

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Settings")
    float FadeRange = 300.0f;    // äÆëSÇ…ïsìßñæÇ…Ç»ÇÈÇ‹Ç≈ÇÃïù

private:
    UPROPERTY()
    UMaterialInstanceDynamic* DynamicMaterial;
};