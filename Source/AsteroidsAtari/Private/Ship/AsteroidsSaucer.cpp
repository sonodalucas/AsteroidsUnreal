// Fill out your copyright notice in the Description page of Project Settings.

#include "Ship/AsteroidsSaucer.h"

#include "Core/AsteroidsGameMode.h"
#include "Core/AsteroidsGameState.h"
#include "Core/AsteroidsPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Ship/AsteroidsProjectile.h"

class AAsteroidsPlayerState;

AAsteroidsSaucer::AAsteroidsSaucer()
{
	// Set up the sprite component
	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = SpriteComponent;
}

void AAsteroidsSaucer::BeginPlay()
{
	Super::BeginPlay();

	gameMode =  Cast<AAsteroidsGameMode>(GetWorld()->GetAuthGameMode());

	gameState = Cast<AAsteroidsGameState>(GetWorld()->GetGameState());
	
	SetScreenWrapper(this, gameState);
}

void AAsteroidsSaucer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (gameMode->IsGameOver()) return;
	
	if (!IsPooled()) return;

	MoveSaucer(DeltaSeconds);

	// Check if it's time to change direction
	currentChangeDirectionTime += DeltaSeconds;

	if (currentChangeDirectionTime >= changeDirectionTime)
	{
		ChangeDirection();
		changeDirectionTime = FMath::FRandRange(minChangeDirectionTime, maxChangeDirectionTime);
		currentChangeDirectionTime = 0;
	}

	currentShootTime += DeltaSeconds;

	if (currentShootTime > shootTime)
	{
		Shoot();
		currentShootTime = 0;
	}
	
	if (!saucerOnScreen) return;
	TryToWrapActor();
}

void AAsteroidsSaucer::MoveSaucer(float DeltaTime)
{
	SetActorLocation(GetActorLocation() + (direction * speed * DeltaTime));
}

void AAsteroidsSaucer::ChangeDirection()
{
	if (saucerOnScreen)
	{
		FVector randomDirection = FMath::VRand();
		randomDirection.Y = 0;
		randomDirection.Normalize();

		direction = randomDirection;
	}
	else
	{
		float halfScreenX = gameState->ScreenWorldSize.X / 2;
		saucerOnScreen = GetActorLocation().X > -halfScreenX && GetActorLocation().X < halfScreenX;
	}
}

void AAsteroidsSaucer::Initialize()
{
	Super::Initialize();

	currentShootTime = 0;
	currentChangeDirectionTime = 0;
	SetActorHiddenInGame(false);
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	changeDirectionTime = FMath::FRandRange(minChangeDirectionTime, maxChangeDirectionTime);
	StartShooting(FMath::FRandRange(minShootTime, maxShootTime));
}

void AAsteroidsSaucer::Shoot()
{
	// Spawn a projectile actor
	AAsteroidsProjectile* Projectile = gameState->GetProjectile();

	// Set the projectile rotation
	FVector shootDirection;
	if (saucerType == SaucerSmall)
	{
		float smallerDistance = FLT_MAX;
		AAsteroidsShipCharacter* closestShip = nullptr;
		
		for (auto ship : gameMode->playerShips)
		{
			if (FVector::Distance(GetActorLocation(), ship->GetActorLocation()) < smallerDistance)
			{
				closestShip = ship;
			}
		}
		if (closestShip == nullptr)
			UE_LOG(LogTemp, Error, TEXT("Closest ship null."))
		shootDirection = (closestShip->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	}
	else
	{
		shootDirection = FMath::VRand();
		shootDirection.Y = 0;
		shootDirection.Normalize();
	}
	
	FRotator rotation = FRotationMatrix::MakeFromX(shootDirection).Rotator();
	
	Projectile->Initialize(GetActorLocation(), rotation, nullptr, false);

	shootTime = FMath::FRandRange(minShootTime, maxShootTime);
	// GetWorldTimerManager().SetTimer(shootTimerHandle, this, &AAsteroidsSaucer::Shoot, shootTime, false);
}

void AAsteroidsSaucer::StartShooting(float InInterval)
{
	if (isShooting) return;

	isShooting = true;

	shootTime = InInterval;

	// GetWorldTimerManager().SetTimer(shootTimerHandle, this, &AAsteroidsSaucer::Shoot, InInterval, false);
}

void AAsteroidsSaucer::StopShooting()
{
	if (!isShooting) return;

	isShooting = false;
	GetWorldTimerManager().ClearTimer(shootTimerHandle);
}

float AAsteroidsSaucer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Saucer took damage"))
	if (IsValid(EventInstigator))
	{
		gameState->AddScore(saucerScoreValue);
	}
	ReturnToPool();
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AAsteroidsSaucer::Initialize(TEnumAsByte<ESaucerType> type, FVector startPosition, FVector saucerDirection)
{
	SetActorLocation(startPosition);
	direction = saucerDirection;
	Initialize();

	saucerType = type;
	FVector scale = saucerType == SaucerBig ? FVector::One() : FVector(.5f, .5f, .5f);
	SetActorScale3D(scale * 3);
}

void AAsteroidsSaucer::ReturnToPool()
{
	saucerOnScreen = false;
	Super::ReturnToPool();
	StopShooting();
}
