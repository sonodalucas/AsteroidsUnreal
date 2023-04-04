// Fill out your copyright notice in the Description page of Project Settings.


#include "Asteroid/AsteroidsAsteroid.h"

#include "PaperSpriteComponent.h"
#include "Core/AsteroidsGameMode.h"
#include "Core/AsteroidsGameState.h"
#include "Core/AsteroidsPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Ship/AsteroidsProjectile.h"

// Sets default values
AAsteroidsAsteroid::AAsteroidsAsteroid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = SpriteComponent;

	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
	FlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	
	SpriteComponent->SetHiddenInGame(false);
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
	
	ModifyScale(scale * 3);
}

void AAsteroidsAsteroid::ReturnToPool()
{
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Super::ReturnToPool();
	OnAsteroidDestroyed.Broadcast(this);
	HideExplosion(true);
	// OnAsteroidDestroyed.Clear();
	SetAsteroidsUnstable(false);
}

float AAsteroidsAsteroid::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (IsValid(EventInstigator))
	{
		gameState->AddScore(Size * 100);
	}
	OnHitByProjectile(EventInstigator);
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AAsteroidsAsteroid::OnHitByProjectile(AController* eventInstigator)
{
	if (isUnstable)
	{
		SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		TArray<AActor*> ignoreActors;
		// Ignore self or remove this line to not ignore any
		ignoreActors.Init(this, 1);
		
		TArray<AActor*> overlappingActors;
		TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;

		traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel4));
		traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel5));
	
		bool isOverlap = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), explosionRadius,
			traceObjectTypes, nullptr, ignoreActors, overlappingActors);

		// DrawDebugSphere(GetWorld(), GetActorLocation(), explosionRadius, 12, FColor::Red, true, 2.f);

		UE_LOG(LogTemp, Warning, TEXT("%d"), overlappingActors.Num())
		for (auto actor : overlappingActors)
		{
			// UE_LOG(LogTemp, Warning, TEXT("%s"), *actor->GetName())
			UGameplayStatics::ApplyDamage(actor, 1, eventInstigator, this, nullptr);
		}

		TriggerExplosion();
	}
	
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
	
	if (!isUnstable)
	{
		// OnAsteroidDestroyed.Broadcast(this);
		ReturnToPool();
	}
}

void AAsteroidsAsteroid::ModifyScale(FVector scale)
{
	asteroidScale = scale;
	OnRep_AsteroidScale();
}

void AAsteroidsAsteroid::SetAsteroidsUnstable_Implementation(bool state)
{
	isUnstable = state;
	OnRep_IsUnstable();
}

void AAsteroidsAsteroid::HideExplosion(bool isHidden)
{
	explosionHidden = isHidden;
	OnRep_ExplosionHidden();
}

void AAsteroidsAsteroid::OnRep_AsteroidScale()
{
	SetActorScale3D(asteroidScale);
}

void AAsteroidsAsteroid::OnRep_IsUnstable()
{
	ToggleSpriteFlash(isUnstable);
}

void AAsteroidsAsteroid::OnRep_ExplosionHidden()
{
	FlipbookComponent->SetHiddenInGame(explosionHidden);
}

// Called every frame
void AAsteroidsAsteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!gameState->inGame) return;
	
	if (!IsPooled()) return;
	SetActorLocation(GetActorLocation() + initialVelocity * DeltaTime);
	TryToWrapActor();
}

void AAsteroidsAsteroid::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAsteroidsAsteroid, asteroidScale);
	DOREPLIFETIME(AAsteroidsAsteroid, explosionHidden);
}
