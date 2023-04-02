// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AsteroidsPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ASTEROIDSATARI_API AAsteroidsPlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

public:
	
	UFUNCTION(BlueprintCallable)
	void LockPlayerInput();

	UFUNCTION(BlueprintCallable)
	void UnlockPlayerInput();

	UFUNCTION(BlueprintCallable)
	bool IsInputLocked() {return inputLocked;}

protected:
	
	bool inputLocked = false;
};
