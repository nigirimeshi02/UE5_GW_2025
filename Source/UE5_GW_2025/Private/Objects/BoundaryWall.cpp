#include "Objects/BoundaryWall.h"

ABoundaryWall::ABoundaryWall()
{
    PrimaryActorTick.bCanEverTick = false; // 描画はシェーダーに任せるのでTickは不要

    WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
    RootComponent = WallMesh;

    // デフォルトの壁メッシュを設定（エンジンのCubeなどを指定可能）
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube"));
    if (MeshAsset.Succeeded())
    {
        WallMesh->SetStaticMesh(MeshAsset.Object);
    }

    // Collision Settings
    WallMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void ABoundaryWall::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // 動的マテリアルを作成してパラメータを適用
    UMaterialInterface* BaseMaterial = WallMesh->GetMaterial(0);
    if (BaseMaterial)
    {
        DynamicMaterial = WallMesh->CreateDynamicMaterialInstance(0, BaseMaterial);
        if (DynamicMaterial)
        {
            DynamicMaterial->SetVectorParameterValue(TEXT("WallColor"), WallColor);
            DynamicMaterial->SetScalarParameterValue(TEXT("VisibleRadius"), VisibleRadius);
            DynamicMaterial->SetScalarParameterValue(TEXT("FadeRange"), FadeRange);
        }
    }
}

void ABoundaryWall::BeginPlay()
{
    Super::BeginPlay();
}