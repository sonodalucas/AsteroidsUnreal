// Copyright Epic Games, Inc. All Rights Reserved.

#include "AsteroidsAtari/Public/Core/AsteroidsGameMode.h"

#include "PaperFlipbookComponent.h"
#include "Asteroid/AsteroidsAsteroid.h"
#include "AsteroidsAtari/Public/Core/AsteroidsPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraActor.h"
#include "Core/AsteroidsGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "ObjectPool/AsteroidsPoolableActor.h"
#include "ObjectPool/AsteroidsPoolManager.h"
#include "Ship/AsteroidsSaucer.h"

AAsteroidsGameMode::AAsteroidsGameMode()
{
	asteroidsPoolManager = CreateDefaultSubobject<UAsteroidsPoolManager>(TEXT("AsteroidsPool"));
	saucerPoolManager = CreateDefaultSubobject<UAsteroidsPoolManager>(TEXT("SaucerPool"));
}

void AAsteroidsGameMode::BeginPlay()
{
	Super::BeginPlay();

	// mainCamera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(this, ACameraActor::StaticClass()));

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* controller = It->Get();
		if (controller)
		{
			playerControllers.Add(Cast<AAsteroidsPlayerController>(controller));
		}
	}

	// Get player ship
	TArray<AActor*> actorArray;
	UGameplayStatics::GetAllActorsOfClass(this, AAsteroidsShipCharacter::StaticClass(), actorArray);

	for (auto actor : actorArray)
	{
		PlayerShips.Add(Cast<AAsteroidsShipCharacter>(actor));
	}

	for (auto ship : PlayerShips)
	{
		ship->GetSprite()->SetVisibility(false);
		Cast<AAsteroidsPlayerController>(ship->GetController())->LockPlayerInput();
	}

	// Spawn asteroids pool
	if (asteroidsPoolManager && AsteroidsClass)
	{
		asteroidsPoolManager->InitPool(AsteroidsClass, 40);
	}

	// Spawn saucer pool
	if (saucerPoolManager && SaucerClass)
	{
		saucerPoolManager->InitPool(SaucerClass, 1);
	}
	
}

void AAsteroidsGameMode::SetPlayerDefaults(APawn* PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	AAsteroidsShipCharacter* ship = Cast<AAsteroidsShipCharacter>(PlayerPawn);

	if (ship)
	{
		playerSpawnIndex += 1;
		ship->AuthSetColor(PlayerColors[playerSpawnIndex]);
		ship->SetActorLocation(PlayerSpawn + FVector(PlayerSpawnDistance * playerSpawnIndex, 0, 0));
		shipsSpawnPositions.Add(ship, PlayerSpawn + FVector(PlayerSpawnDistance * playerSpawnIndex, 0, 0));
	}
}

void AAsteroidsGameMode::StartGame()
{
	asteroidsGameState = Cast<AAsteroidsGameState>(GetWorld()->GetGameState());
	// Spawn asteroids
	for (int i = 0; i < 4; ++i)
	{
		SpawnAsteroidAtRandomPosition();
	}

	// Spawn saucer
	float NextSaucerSpawnDelay = FMath::RandRange(MinSaucerSpawnDelay, MaxSaucerSpawnDelay);
	GetWorldTimerManager().SetTimer(saucerSpawnTimerHandle, this, &AAsteroidsGameMode::SpawnSaucer,
		NextSaucerSpawnDelay, false);

	asteroidsGameState->ResetScore();
	asteroidsGameState->ResetLives();

	InitUI();

	for (auto ship : PlayerShips)
	{
		ship->GetSprite()->SetVisibility(true);
		Cast<AAsteroidsPlayerController>(ship->GetController())->UnlockPlayerInput();
	}

	for (auto ship : shipsSpawnPositions)
	{
		ship.Key->SetActorLocation(ship.Value);
	}
	
	asteroidsGameState->inGame = true;
}

void AAsteroidsGameMode::SpawnAsteroid(EAsteroidsSize size, FVector spawnPosition)
{
	AAsteroidsAsteroid* asteroid = Cast<AAsteroidsAsteroid>(asteroidsPoolManager->GetActorFromPool());
	asteroid->Initialize(size, spawnPosition);
	asteroid->Speed = size * asteroid->BaseSpeed;
	asteroidsOnScreen.Add(asteroid);

	float randomChance = FMath::FRand();
	bool shouldSpawnUnstableAsteroid = unstableAsteroidChance >= randomChance;
	if (shouldSpawnUnstableAsteroid)
		asteroid->SetAsteroidsUnstable(true);

	if (!asteroid->OnAsteroidDestroyed.IsBound())
		asteroid->OnAsteroidDestroyed.AddDynamic(this, &AAsteroidsGameMode::DestroyAsteroid);
}

void AAsteroidsGameMode::SpawnAsteroidAtRandomPosition()
{
	float x = FMath::RandRange(-asteroidsGameState->ScreenWorldSize.X / 2, asteroidsGameState->ScreenWorldSize.X / 2);
	float z = FMath::RandRange(-asteroidsGameState->ScreenWorldSize.Y / 2, asteroidsGameState->ScreenWorldSize.Y / 2);
	FVector spawnLocation = FVector (x, 0, z);

	TArray<AActor*> ignoreActors;
	// Ignore self or remove this line to not ignore any
	ignoreActors.Init(this, 1);

	TArray<AActor*> overlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	// traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel3));
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel4));
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(static_cast<ECollisionChannel>(ECollisionChannel::ECC_GameTraceChannel5)));
	
	bool isOverlap = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), spawnLocation, 50.0f,
		traceObjectTypes, nullptr, ignoreActors, overlappingActors);
	// Debug
	if (showDebugAsteroidSpawnSphere)
		DrawDebugSphere(GetWorld(), spawnLocation, 50, 12, FColor::Red, true, 2.f);
	
	if (!isOverlap)
	{
		SpawnAsteroid(AsteroidLarge, spawnLocation);
		
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("Asteroid: Spawn location already ocuppied. Trying again."))
	SpawnAsteroid(AsteroidLarge, spawnLocation);
}

// void AAsteroidsGameMode::SetCameraFocus_Implementation()
// {
// 	for (auto playerController : playerControllers)
// 	{
// 		playerController->SetViewTargetWithBlend(mainCamera);
// 	}
// }

void AAsteroidsGameMode::SpawnSaucer()
{
	// UE_LOG(LogTemp, Warning, TEXT("Spawning saucer"))
	// Choose a random side of the screen where the saucer will spawn
	int32 Side = FMath::RandRange(0, 1); // 0 = left, 1 = right

	// Compute the spawn location based on the chosen side and the minimum and maximum distance
	FVector spawnLocation;
	FVector spawnDirection;
	if (Side == 0) // Spawn on the left side of the screen
	{
		spawnLocation = FVector((-asteroidsGameState->ScreenWorldSize.X / 2) - 200, 0.f, FMath::RandRange(-asteroidsGameState->ScreenWorldSize.Y / 2,
			asteroidsGameState->ScreenWorldSize.Y / 2));
		spawnDirection = FVector(1.f, 0.f, 0.f);
	}
	else // Spawn on the right side of the screen
	{
		spawnLocation = FVector((asteroidsGameState->ScreenWorldSize.X / 2) + 200, 0.f, FMath::RandRange(-asteroidsGameState->ScreenWorldSize.Y / 2,
			asteroidsGameState->ScreenWorldSize.Y / 2));
		spawnDirection = FVector(-1.f, 0.f, 0.f);
	}

	AAsteroidsSaucer* saucer = Cast<AAsteroidsSaucer>(saucerPoolManager->GetActorFromPool());

	// UE_LOG(LogTemp, Warning, TEXT("Spawning at %s"), *spawnLocation.ToString())

	if (IsValid(saucer))
	{
		ESaucerType saucerSize = static_cast<ESaucerType>(FMath::RandRange(1, 2));

		UE_LOG(LogTemp, Warning, TEXT("Spawning at %d"), saucerSize);

		saucer->Initialize(saucerSize, spawnLocation, spawnDirection);
	}
	
	// Schedule the next saucer spawn
	float nextSaucerSpawnDelay = FMath::RandRange(MinSaucerSpawnDelay, MaxSaucerSpawnDelay);
	GetWorldTimerManager().SetTimer(saucerSpawnTimerHandle, this, &AAsteroidsGameMode::SpawnSaucer,
		nextSaucerSpawnDelay, false);
}

void AAsteroidsGameMode::DestroyAsteroid(AAsteroidsAsteroid* asteroidDestroyed)
{
	asteroidsOnScreen.Remove(asteroidDestroyed);
	// UE_LOG(LogTemp, Warning, TEXT("Asteroids on screen %d"), asteroidsOnScreen.Num())

	if (asteroidsOnScreen.Num() == 0 && asteroidsGameState->inGame)
	{
		for (int i = 0; i < 4 + additionalAsteroids; ++i)
		{
			SpawnAsteroidAtRandomPosition();
		}
	}
}

void AAsteroidsGameMode::Multicast_GameOver_Implementation(AAsteroidsShipCharacter* ship)
{
	ship->SetActorHiddenInGame(true);
}

bool AAsteroidsGameMode::UpdateLives(int lives)
{
	if (lives <= 0)
	{
		asteroidsGameState->AuthGameEnded();
		for (auto playerController : playerControllers)
		{
			playerController->LockPlayerInput();
		}
		for (auto ship : PlayerShips)
		{
			Multicast_GameOver(ship);
		}
		
		// CallGameOverUI();
		GetWorldTimerManager().ClearTimer(saucerSpawnTimerHandle);
		asteroidsGameState->ProjectilePool->PoolAllActors();
		asteroidsPoolManager->PoolAllActors();
		saucerPoolManager->PoolAllActors();

		return true;
	}
	
	return false;
}

void AAsteroidsGameMode::UpdateScore(int score)
{
	scoreTracker = score;

	if (scoreTracker >= 10000)
	{
		scoreTracker -= 10000;
		asteroidsGameState->AddLife();
		if (additionalAsteroids + 4 < maxAsteroids)
			additionalAsteroids += 1;
	}
}

void AAsteroidsGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAsteroidsGameMode, mainCamera);
}
