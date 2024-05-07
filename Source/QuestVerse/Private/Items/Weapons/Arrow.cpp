// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapons/Arrow.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AArrow::AArrow()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	ItemMesh->SetupAttachment(RootComponent);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	m_pBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	m_pBoxCollision->SetupAttachment(ItemMesh);
	m_pBoxCollision->OnComponentHit.AddDynamic(this, &AArrow::OnHit);
	m_pBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AArrow::OnOverlapBegin);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(RootComponent);
	ProjectileMovement->InitialSpeed = fArrowSpeed;
	ProjectileMovement->MaxSpeed = fArrowSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;

	SetLifeSpan(fArrowLife);
}

void AArrow::BeginPlay()
{
	Super::BeginPlay();
}

void AArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AArrow::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
}

void AArrow::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(ProjectileMovement->Velocity * 100.0f, Hit.ImpactPoint);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("ArrowHit"));
	}
}

void AArrow::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		// Attach
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->ProjectileGravityScale = 0.f;

		FAttachmentTransformRules AttachmentTransformRule = FAttachmentTransformRules::KeepWorldTransform;
		OtherActor->AttachToActor(GetParentActor(), AttachmentTransformRule);

		m_pBoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// SpawnEmitter
		UGameplayStatics* SpawnEmitter;
		SpawnEmitter->SpawnEmitterAtLocation(GetWorld(), m_pParticleAsset, m_pBoxCollision->GetComponentLocation());
	}
}