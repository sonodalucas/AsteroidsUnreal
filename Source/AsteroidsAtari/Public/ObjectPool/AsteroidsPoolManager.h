// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AsteroidsPoolManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASTEROIDSATARI_API UAsteroidsPoolManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAsteroidsPoolManager();

	UFUNCTION(BlueprintCallable)
	void InitPool(TSubclassOf<AAsteroidsPoolableActor> ActorClass, int32 InitialSize);

	UFUNCTION(BlueprintCallable)
	void PoolAllActors();

	UFUNCTION(BlueprintCallable)
	AAsteroidsPoolableActor* GetActorFromPool();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TArray<AAsteroidsPoolableActor*> pool;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
