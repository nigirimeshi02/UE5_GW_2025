// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBullet.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"

// Sets default values
AEnemyBullet::AEnemyBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // �R���W�����i���́j
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(10.0f);
    CollisionComp->SetCollisionProfileName("BlockAllDynamic");
    RootComponent = CollisionComp;

    // �q�b�g�C�x���g�o�^
    CollisionComp->OnComponentHit.AddDynamic(this, &AEnemyBullet::OnHit);

    // �ړ��R���|�[�l���g
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = Speed;
    ProjectileMovement->MaxSpeed = Speed;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;

    // ��莞�Ԍ�Ɏ����ŏ���
    InitialLifeSpan = LifeTime;
}

// Called when the game starts or when spawned
void AEnemyBullet::BeginPlay()
{
	Super::BeginPlay();
	
	/*Velocity = GetActorForwardVector() * Speed;
	SetLifeSpan(LifeTime);*/
}

void AEnemyBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this)
    {
        if (OtherActor->ActorHasTag(TEXT("Player"))) {
            // �_���[�W��^����
            UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, UDamageType::StaticClass());

            // �e������
            Destroy();
        }
        else{
            // ���̃I�u�W�F�N�g�ɓ��������ꍇ�͒e������
			Destroy();
        }
    }
}

// Called every frame
void AEnemyBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SetActorLocation(GetActorLocation() + Velocity * DeltaTime, true);
}

