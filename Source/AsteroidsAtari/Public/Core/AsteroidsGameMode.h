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

	UFUNCTION(BlueprintImplementableEvent)
	void InitUI();

	UFUNCTION(BlueprintCallable)
	void StartGame();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAsteroidsPoolableActor> AsteroidsClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAsteroidsSaucer> SaucerClass;

	UPROPERTY()
	TArray<AAsteroidsShipCharacter*> playerShips;

	UFUNCTION(BlueprintCallable)
	void SpawnAsteroid(EAsteroidsSize size, FVector spawnPosition);

	UFUNCTION()
	void SpawnAsteroidAtRandomPosition();

	UFUNCTION()
	bool IsGameOver() {return isGameOver;}
	
private:
	
	UPROPERTY()
	TObjectPtr<ACameraActor> mainCamera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAsteroidsPoolManager> asteroidsPoolManager;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAsteroidsPoolManager> saucerPoolManager;
	
	bool isGameOver = false;

protected:

	UPROPERTY(BlueprintReadOnly)
	AAsteroidsPlayerController* playerController;

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
};
