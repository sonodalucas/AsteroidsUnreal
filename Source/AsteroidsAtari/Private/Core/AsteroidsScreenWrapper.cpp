// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/AsteroidsScreenWrapper.h"
#include "Kismet/GameplayStatics.h"

AsteroidsScreenWrapper::AsteroidsScreenWrapper()
{
}

AsteroidsScreenWrapper::~AsteroidsScreenWrapper()
{
}

void AsteroidsScreenWrapper::SetScreenWrapper(AActor* ActorToWrap, AAsteroidsGameState* GameState)
{
	actorToWrap = ActorToWrap;
	gameState = GameState;
	FVector objectPosition;
	actorToWrap->GetActorBounds(false, objectPosition, objectBox);
}

bool AsteroidsScreenWrapper::IsOutOfScreenHorizontally(FVector objectPosition, float screenX, FVector& positionAfterWrap)
{
	float maxScreenWidth = screenX / 2;
	float objectHalfX = objectBox.X / 2;

	if (objectPosition.X - objectHalfX > maxScreenWidth)
	{
		positionAfterWrap = objectPosition - FVector(screenX + objectHalfX, 0, 0);
		return true;
	}

	if (objectPosition.X + objectHalfX < maxScreenWidth * -1)
	{
		positionAfterWrap = objectPosition + FVector(screenX + objectHalfX, 0, 0);
		return true;
	}

	return false;
}

bool AsteroidsScreenWrapper::IsOutOfScreenVertically(FVector objectPosition, float screenY, FVector& positionAfterWrap)
{
	float maxScreenHeight = screenY / 2;
	float objectHalfY = objectBox.Z / 2;

	if (objectPosition.Z - objectHalfY > maxScreenHeight)
	{
		positionAfterWrap = objectPosition - FVector(0, 0,screenY + objectHalfY);
		return true;
	}
	
	if (objectPosition.Z + objectHalfY < maxScreenHeight * -1)
	{
		positionAfterWrap = objectPosition + FVector(0, 0,screenY + objectHalfY);
		return true;
	}

	return false;
}

void AsteroidsScreenWrapper::TryToWrapActor()
{
	if (!IsValid(gameState)) return;
	FVector wrapPosition;
	FVector objectPosition = actorToWrap->GetActorLocation();

	if (IsOutOfScreenHorizontally(objectPosition, gameState->ScreenWorldSize.X, wrapPosition))
	{
		actorToWrap->SetActorLocation(wrapPosition);
	}

	if (IsOutOfScreenVertically(objectPosition, gameState->ScreenWorldSize.Y, wrapPosition))
	{
		actorToWrap->SetActorLocation(wrapPosition);
	}
}
