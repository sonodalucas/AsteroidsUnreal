// Fill out your copyright notice in the Description page of Project Settings.


#include "AsteroidsAtari/Public/Core/AsteroidsPlayerController.h"

void AAsteroidsPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AAsteroidsPlayerController::LockPlayerInput()
{
	inputLocked = true;
}

void AAsteroidsPlayerController::UnlockPlayerInput()
{
	inputLocked = false;
}
