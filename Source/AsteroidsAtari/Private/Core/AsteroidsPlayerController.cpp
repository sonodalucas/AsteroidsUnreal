// Fill out your copyright notice in the Description page of Project Settings.


#include "AsteroidsAtari/Public/Core/AsteroidsPlayerController.h"

#include "Net/UnrealNetwork.h"

void AAsteroidsPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AAsteroidsPlayerController::LockPlayerInput_Implementation()
{
	inputLocked = true;
}

void AAsteroidsPlayerController::UnlockPlayerInput_Implementation()
{
	inputLocked = false;
}

void AAsteroidsPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAsteroidsPlayerController, inputLocked);
}
