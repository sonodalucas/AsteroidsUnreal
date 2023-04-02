// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/AsteroidsGameState.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ObjectPool/AsteroidsPoolableActor.h"
#include "ObjectPool/AsteroidsPoolManager.h"
#include "Ship/AsteroidsProjectile.h"

AAsteroidsGameState::AAsteroidsGameState()
{
	projectilePool = CreateDefaultSubobject<UAsteroidsPoolManager>(TEXT("ProjectilePool"));
}

AAsteroidsProjectile* AAsteroidsGameState::GetProjectile()
{
	return Cast<AAsteroidsProjectile>(projectilePool->GetActorFromPool());
}

void AAsteroidsGameState::AddScore(int value)
{
	Score += value;
	UE_LOG(LogTemp, Warning, TEXT("Player score: %d"), Score);
	OnScoreUpdated.Broadcast(Score);
}

void AAsteroidsGameState::ResetScore()
{
	Score = 0;
	UE_LOG(LogTemp, Warning, TEXT("Player score: %d"), Score);
	OnScoreUpdated.Broadcast(Score);
}

void AAsteroidsGameState::ResetLives()
{
	Lives = 3;
	OnLivesUpdated.Broadcast(Lives);
}

void AAsteroidsGameState::LooseLife()
{
	Lives -= 1;
	OnLivesUpdated.Broadcast(Lives);
}

void AAsteroidsGameState::AddLife()
{
	Lives += 1;
	OnLivesUpdated.Broadcast(Lives);
}

void AAsteroidsGameState::BeginPlay()
{
	Super::BeginPlay();

	// Spawn projectile pool
	if (projectilePool && ProjectileClass)
	{
		projectilePool->InitPool(ProjectileClass, numberOfProjectilesOnPool);
	}

	auto mainCamera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(this, ACameraActor::StaticClass()));

	// Get the screen size in world units
	auto camera = mainCamera->GetCameraComponent();
	float width = camera->OrthoWidth;
	float height = width / camera->AspectRatio;

	ScreenWorldSize = FVector2D (width, height);

	UE_LOG(LogTemp, Warning, TEXT("%s"), *ScreenWorldSize.ToString());
}
