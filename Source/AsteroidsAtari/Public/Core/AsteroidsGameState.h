// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ObjectPool/AsteroidsPoolManager.h"
#include "ObjectPool/AsteroidsPoolableActor.h"
#include "AsteroidsGameState.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerInfoUpdated, int, score);

UCLASS()
class ASTEROIDSATARI_API AAsteroidsGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	AAsteroidsGameState();
	
	UPROPERTY(VisibleAnywhere)
	UAsteroidsPoolManager* projectilePool;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int numberOfProjectilesOnPool = 10;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAsteroidsPoolableActor> ProjectileClass;

	UPROPERTY(BlueprintReadOnly)
	FVector2D ScreenWorldSize;

	UPROPERTY()
	bool gameStarted = false;

	// Get a projectile from the pool
	UFUNCTION(BlueprintCallable)
	AAsteroidsProjectile* GetProjectile();

	void AddScore(int value);

	void ResetScore();

	void ResetLives();

	void LooseLife();

	void AddLife();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetLives() {return Lives;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetPlayerScore() {return Score;}

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnPlayerInfoUpdated OnScoreUpdated;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnPlayerInfoUpdated OnLivesUpdated;

protected:

	virtual void BeginPlay() override;

	int Score = 0;

	int Lives = 3;
};
