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
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void LockPlayerInput();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void UnlockPlayerInput();

	UFUNCTION(BlueprintCallable)
	bool IsInputLocked() {return inputLocked;}

protected:
	UPROPERTY(Replicated)
	bool inputLocked = false;
};
