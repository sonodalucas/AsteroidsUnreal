// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/AsteroidsPoolableActor.h"

#include "ObjectPool/AsteroidsPoolManager.h"

// Sets default values
AAsteroidsPoolableActor::AAsteroidsPoolableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	isPooled = false;
}

void AAsteroidsPoolableActor::Initialize()
{
	isPooled = true;
}

void AAsteroidsPoolableActor::ReturnToPool()
{
	if (PoolManager)
	{
		isPooled = false;
		SetActorHiddenInGame(true);
		SetActorLocation(FVector (1000, 1000, 1000));
		SetActorRotation(FRotator::ZeroRotator);
	}
}


