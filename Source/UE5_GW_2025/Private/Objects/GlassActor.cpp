// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/GlassActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Field/FieldSystemComponent.h"
#include "Field/FieldSystemObjects.h"

AGlassActor::AGlassActor()
{
    PrimaryActorTick.bCanEverTick = false;

    GlassCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GC_Cube"));
    RootComponent = GlassCollection;

    GlassCollection->SetNotifyRigidBodyCollision(true);
    GlassCollection->OnComponentHit.AddDynamic(this, &AGlassActor::OnGlassHit);

    GlassCollection->SetSimulatePhysics(true);
    GlassCollection->SetNotifyBreaks(true); // 破砕イベント通知を有効化
}

void AGlassActor::BeginPlay()
{
    Super::BeginPlay();
}

void AGlassActor::OnGlassHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!OtherActor || !OtherComp) return;

    FVector Velocity = OtherComp->GetComponentVelocity();
    float Speed = Velocity.Size();

    if (Speed > BreakSpeedThreshold)
    {
        UE_LOG(LogTemp, Warning, TEXT("Glass Fractured! Speed: %f"), Speed);

        // RadialFalloff フィールドを生成
        URadialFalloff* RadialFalloff = NewObject<URadialFalloff>();
        RadialFalloff->Magnitude = 500.0f;          // 力の強さ
        RadialFalloff->MinRange = 0.0f;
        RadialFalloff->MaxRange = 200.0f;          // 影響半径
        RadialFalloff->Default = 0.0f;
        RadialFalloff->Radius = 200.0f;
        RadialFalloff->Position = Hit.ImpactPoint;

        // Chaos に破壊イベントを適用
        GlassCollection->ApplyPhysicsField(
            true,
            EGeometryCollectionPhysicsTypeEnum::Chaos_ExternalClusterStrain, // UE5.6での「破砕」カテゴリ
            nullptr,
            RadialFalloff
        );
    }
}

