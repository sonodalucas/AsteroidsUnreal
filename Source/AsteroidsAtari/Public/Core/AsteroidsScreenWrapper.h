// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsteroidsGameState.h"

class AAsteroidsGameMode;
/**
 * 
 */
class ASTEROIDSATARI_API AsteroidsScreenWrapper
{
public:
	AsteroidsScreenWrapper();
	
	~AsteroidsScreenWrapper();

	void SetScreenWrapper (AActor* actorToWrap, AAsteroidsGameState* GameState);
	
	bool IsOutOfScreenHorizontally(FVector objectPosition, float screenX, FVector& positionAfterWrap);

	bool IsOutOfScreenVertically(FVector objectPosition, float screenY, FVector& positionAfterWrap);

	void TryToWrapActor();

private:
	UPROPERTY()
	AAsteroidsGameState* gameState;

	UPROPERTY()
	AActor* actorToWrap;

	UPROPERTY()
	FVector objectBox;
};
