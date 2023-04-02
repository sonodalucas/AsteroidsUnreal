// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Ship/AsteroidsSaucer.h"
#include "Ship/AsteroidsShipCharacter.h"
#include "AsteroidsGameMode.generated.h"

class AAsteroidsGameState;
class AAsteroidsAsteroid;
enum EAsteroidsSize : int;
class AAsteroidsPoolableActor;
class UAsteroidsPoolManager;
class AAsteroidsPlayerController;
/**
 * 
 */
UCLASS()
class ASTEROIDSATARI_API AAsteroidsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAsteroidsGameMode();
	
	virtual void BeginPlay() override;
	
	virtual void SetPlayerDefaults(APawn* PlayerPawn) override;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InitUI();

	UFUNCTION(BlueprintCallable)
	void StartGame();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Players")
	TArray<FLinearColor> PlayerColors;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAsteroidsPoolableActor> AsteroidsClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAsteroidsSaucer> SaucerClass;

	UPROPERTY()
	TArray<AAsteroidsShipCharacter*> PlayerShips;

	// Where the first player will be spawned
	UPROPERTY(EditAnywhere)
	FVector PlayerSpawn;

	// Distance between players, the distance increases horizontally and to the right
	UPROPERTY(EditAnywhere)
	float PlayerSpawnDistance = 100;

	UFUNCTION(BlueprintCallable)
	void SpawnAsteroid(EAsteroidsSize size, FVector spawnPosition);

	UFUNCTION()
	void SpawnAsteroidAtRandomPosition();

	// UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	// void SetCameraFocus();
	
private:

	UPROPERTY()
	int playerSpawnIndex = -1;
	
	UPROPERTY(Replicated)
	TObjectPtr<ACameraActor> mainCamera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAsteroidsPoolManager> asteroidsPoolManager;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAsteroidsPoolManager> saucerPoolManager;

	UPROPERTY()
	TMap<AAsteroidsShipCharacter*, FVector> shipsSpawnPositions;
	
protected:

	TArray<AAsteroidsPlayerController*> playerControllers;

	UPROPERTY(BlueprintReadOnly)
	AAsteroidsGameState* asteroidsGameState;
	
	UPROPERTY(EditAnywhere)
	bool showDebugAsteroidSpawnSphere = true;

	UPROPERTY()
	TArray<AAsteroidsAsteroid*> asteroidsOnScreen;

	// The minimum delay (in seconds) between saucer spawns
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Saucer")
    float MinSaucerSpawnDelay = 10.f;

	// The maximum delay (in seconds) between saucer spawns
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Saucer")
    float MaxSaucerSpawnDelay = 20.f;

	UPROPERTY()
	FTimerHandle saucerSpawnTimerHandle;

	// It tracks when the score hits 10000
	UPROPERTY()
	int scoreTracker;

	// Spawn a saucer at a random position off screen
	UFUNCTION()
	void SpawnSaucer();

	UFUNCTION(BlueprintImplementableEvent)
	void CallGameOverUI();
	
	// Removes asteroid from array of asteroids on screen
	UFUNCTION()
	void DestroyAsteroid(AAsteroidsAsteroid* asteroidDestroyed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateScore(int score);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateLives(int lives);

private:

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GameOver(AAsteroidsShipCharacter* ship);
};
