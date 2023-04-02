// Fill out your copyright notice in the Description page of Project Settings.


#include "Ship/AsteroidsProjectile.h"
#include "Core/AsteroidsGameMode.h"
#include "PaperFlipbook.h"
#include "PaperSpriteComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AAsteroidsProjectile::AAsteroidsProjectile()
{
	// Set up the sprite component
	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = SpriteComponent;

	// Set up the projectile movement component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(SpriteComponent);
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
}

void AAsteroidsProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SetScreenWrapper(this, Cast<AAsteroidsGameState>(GetWorld()->GetGameState()));
}

void AAsteroidsProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsPooled())
	{
		TryToWrapActor();
	}
}

void AAsteroidsProjectile::Initialize()
{
	Super::Initialize();

	SetActorHiddenInGame(false);
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileMovement->Activate();

	GetWorld()->GetTimerManager().SetTimer(handle, [this]()
	{
		ReturnToPool();
	}, LifeTime, false);
}

void AAsteroidsProjectile::Initialize(FVector position, FRotator rotator, APawn* instigator, bool shotByPlayer)
{
	Initialize();
	isPlayerProjectile = shotByPlayer;

	SpriteComponent->SetSprite(isPlayerProjectile ? PlayerSprite : SaucerSprite);
	
	SetInstigator(instigator);
	ProjectileMovement->SetVelocityInLocalSpace(rotator.Vector() * Speed);
	SetActorLocation(position);
	SetActorRotation(rotator);

	// Set collision channels
	ECollisionChannel objectCollisionChannel = static_cast<ECollisionChannel>(shotByPlayer ? ECC_PlayerProjectileObjectChannel : ECC_SaucerProjectileObjectChannel);
	SpriteComponent->SetCollisionObjectType(objectCollisionChannel);
	collisionChannelToIgnore = static_cast<ECollisionChannel>(shotByPlayer ? ECC_PlayerShipObjectChannel : ECC_SaucerObjectChannel);
	SpriteComponent->SetCollisionResponseToChannel(collisionChannelToIgnore, ECollisionResponse::ECR_Ignore);
}

void AAsteroidsProjectile::ReturnToPool()
{
	Super::ReturnToPool();
	SpriteComponent->SetCollisionResponseToChannel(collisionChannelToIgnore, ECollisionResponse::ECR_Block);
	GetWorldTimerManager().ClearTimer(handle);
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMovement->Deactivate();
}
