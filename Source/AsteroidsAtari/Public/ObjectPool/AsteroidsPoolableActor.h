// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AsteroidsPoolableActor.generated.h"

class UAsteroidsPoolManager;
UCLASS()
class ASTEROIDSATARI_API AAsteroidsPoolableActor : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AAsteroidsPoolableActor();

	// The pool manager that owns this actor.
	UAsteroidsPoolManager* PoolManager;

	// Initializes the actor for reuse.
	UFUNCTION(BlueprintCallable)
	virtual void Initialize() ;

	// Called when the actor is returned to the object pool.
	UFUNCTION(BlueprintCallable)
	virtual void ReturnToPool();

	UFUNCTION(BlueprintCallable)
	bool IsPooled() const {return isPooled;}

private:
	bool isPooled;
};
