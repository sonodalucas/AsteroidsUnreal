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

	UPROPERTY(EditAnywhere)
	int StartingLives = 5;
	
	UPROPERTY(VisibleAnywhere)
	UAsteroidsPoolManager* ProjectilePool;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int NumberOfProjectilesOnPool = 10;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAsteroidsPoolableActor> ProjectileClass;

	UPROPERTY(BlueprintReadOnly)
	FVector2D ScreenWorldSize;

	UPROPERTY(ReplicatedUsing=OnRep_InGame, Replicated)
	bool inGame = false;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnPlayerInfoUpdated OnScoreUpdated;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnPlayerInfoUpdated OnLivesUpdated;

	// Get a projectile from the pool
	UFUNCTION(BlueprintCallable)
	AAsteroidsProjectile* GetProjectile();

	void AuthGameEnded();

	void AddScore(int value);

	void ResetScore();

	void ResetLives();

	void LooseLife();

	void AddLife();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetLives() {return Lives;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetPlayerScore() {return Score;}

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void CheckEndGame(int lives);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void CheckExtraLife(int score);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateLives(int lives);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateScore(int score);

	UFUNCTION(BlueprintImplementableEvent)
	void CallGameOverUI();
	
	UFUNCTION()
	void OnRep_InGame();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=OnRep_Score, Replicated)
	int Score = 0;

	UPROPERTY(ReplicatedUsing=OnRep_Lives, Replicated)
	int Lives;

	UFUNCTION()
	void OnRep_Score();

	UFUNCTION()
	void OnRep_Lives();
};
