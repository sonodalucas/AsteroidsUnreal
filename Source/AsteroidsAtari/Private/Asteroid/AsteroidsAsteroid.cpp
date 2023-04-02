// Fill out your copyright notice in the Description page of Project Settings.


#include "Asteroid/AsteroidsAsteroid.h"

#include "PaperSpriteComponent.h"
#include "Core/AsteroidsGameMode.h"
#include "Core/AsteroidsGameState.h"
#include "Core/AsteroidsPlayerState.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAsteroidsAsteroid::AAsteroidsAsteroid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = SpriteComponent;
}

// Called when the game starts or when spawned
void AAsteroidsAsteroid::BeginPlay()
{
	Super::BeginPlay();

	gameMode = Cast<AAsteroidsGameMode>(GetWorld()->GetAuthGameMode());
	gameState = Cast<AAsteroidsGameState>(GetWorld()->GetGameState());

	SetScreenWrapper(this, gameState);
}

void AAsteroidsAsteroid::Initialize()
{
	Super::Initialize();
	
	SetActorHiddenInGame(false);
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// Set initial veloccity
	initialVelocity = FVector(FMath::RandRange(-1.0f, 1.0f), 0, FMath::RandRange(-1.0f, 1.0f));
	initialVelocity.Normalize();
	initialVelocity *= Speed;
	// SpriteComponent->SetPhysicsLinearVelocity(initialVelocity);
	// UE_LOG(LogTemp, Warning, TEXT("%s"), *SpriteComponent->GetPhysicsLinearVelocity().ToString())
	
}

void AAsteroidsAsteroid::Initialize(TEnumAsByte<EAsteroidsSize> asteroidSize, FVector startPosition)
{
	// Set random sprite
	if (AsteroidSprites.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Random asteroid sprited"))
		int randomNumber = FMath::RandRange(0, AsteroidSprites.Num() - 1);
		SpriteComponent->SetSprite(AsteroidSprites[randomNumber]);
	}
	
	SetActorLocation(startPosition);
	Initialize();
	Size = asteroidSize;

	FVector scale = FVector::One();

	switch (Size)
	{
		case AsteroidMedium:
			scale = FVector(.7f,.7f, .7f);
		break;
		case AsteroidSmall:
			scale = FVector(.35f,.35f, .35f);
			break;
		default: ;
	}
	
	SetActorScale3D(scale * 3);
}

void AAsteroidsAsteroid::ReturnToPool()
{
	Super::ReturnToPool();

	OnAsteroidDestroyed.Clear();
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

float AAsteroidsAsteroid::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (IsValid(EventInstigator))
	{
		gameState->AddScore(Size * 100);
	}
	OnHitByProjectile();
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AAsteroidsAsteroid::OnHitByProjectile()
{
	switch (Size)
	{
	case AsteroidLarge:
		gameMode->SpawnAsteroid(AsteroidMedium, GetActorLocation());
		gameMode->SpawnAsteroid(AsteroidMedium, GetActorLocation());
		break;
	case AsteroidMedium:
		gameMode->SpawnAsteroid(AsteroidSmall, GetActorLocation());
		gameMode->SpawnAsteroid(AsteroidSmall, GetActorLocation());
		break;
	case AsteroidSmall:
		break;
	default: ;
	}
	
	OnAsteroidDestroyed.Broadcast(this);
	ReturnToPool();
}

// Called every frame
void AAsteroidsAsteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (gameMode->IsGameOver()) return;
	
	if (!IsPooled()) return;
	SetActorLocation(GetActorLocation() + initialVelocity * DeltaTime);
	TryToWrapActor();
}

