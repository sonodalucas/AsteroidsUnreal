// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/AsteroidsPoolManager.h"

#include "ObjectPool/AsteroidsPoolableActor.h"

// Sets default values for this component's properties
UAsteroidsPoolManager::UAsteroidsPoolManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UAsteroidsPoolManager::InitPool(TSubclassOf<AAsteroidsPoolableActor> ActorClass, int32 InitialSize)
{
	for (int32 i = 0; i < InitialSize; ++i)
	{
		AAsteroidsPoolableActor* Actor = GetWorld()->SpawnActor<AAsteroidsPoolableActor>(ActorClass,
			FVector(0 ,1000, 0), FRotator(0, 0, 0));
		Actor->ReturnToPool();
		Actor->PoolManager = this;
		pool.Add(Actor);
	}
}

void UAsteroidsPoolManager::PoolAllActors()
{
	for (AAsteroidsPoolableActor* actor : pool)
	{
		actor->ReturnToPool();
	}
}

AAsteroidsPoolableActor* UAsteroidsPoolManager::GetActorFromPool()
{
	for (AAsteroidsPoolableActor* Actor : pool)
	{
		if (Actor && !Actor->IsPooled())
		{
			return Actor;
		}
	}

	return nullptr;	
}


// Called when the game starts
void UAsteroidsPoolManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAsteroidsPoolManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

