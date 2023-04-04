// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/AsteroidsGameState.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Core/AsteroidsGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "ObjectPool/AsteroidsPoolableActor.h"
#include "ObjectPool/AsteroidsPoolManager.h"
#include "Ship/AsteroidsProjectile.h"

AAsteroidsGameState::AAsteroidsGameState()
{
	ProjectilePool = CreateDefaultSubobject<UAsteroidsPoolManager>(TEXT("ProjectilePool"));
}

AAsteroidsProjectile* AAsteroidsGameState::GetProjectile()
{
	return Cast<AAsteroidsProjectile>(ProjectilePool->GetActorFromPool());
}

void AAsteroidsGameState::AuthGameEnded()
{
	inGame = false;

	OnRep_InGame();
}

void AAsteroidsGameState::AddScore(int value)
{
	Score += value;
	UE_LOG(LogTemp, Warning, TEXT("Player score: %d"), Score);
	// OnScoreUpdated.Broadcast(Score);
	OnRep_Score();
}

void AAsteroidsGameState::ResetScore()
{
	Score = 0;
	UE_LOG(LogTemp, Warning, TEXT("Player score: %d"), Score);
	// OnScoreUpdated.Broadcast(Score);
	OnRep_Score();
}

void AAsteroidsGameState::ResetLives()
{
	Lives = StartingLives;
	// OnLivesUpdated.Broadcast(Lives);
	OnRep_Lives();
}

void AAsteroidsGameState::LooseLife()
{
	Lives -= 1;
	// OnLivesUpdated.Broadcast(Lives);
	OnRep_Lives();
}

void AAsteroidsGameState::AddLife()
{
	Lives += 1;
	// OnLivesUpdated.Broadcast(Lives);
	OnRep_Lives();
}


void AAsteroidsGameState::CheckEndGame_Implementation(int lives)
{
	if(Cast<AAsteroidsGameMode>(GetWorld()->GetAuthGameMode())->UpdateLives(lives))
	{
		CallGameOverUI();
	}
}

void AAsteroidsGameState::CheckExtraLife_Implementation(int score)
{
	Cast<AAsteroidsGameMode>(GetWorld()->GetAuthGameMode())->UpdateScore(score);
}

void AAsteroidsGameState::OnRep_InGame()
{
	if (!inGame)
	{
		CallGameOverUI();
	}
}

void AAsteroidsGameState::BeginPlay()
{
	Super::BeginPlay();

	// Spawn projectile pool
	if (ProjectilePool && ProjectileClass)
	{
		ProjectilePool->InitPool(ProjectileClass, NumberOfProjectilesOnPool);
	}

	auto mainCamera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(this, ACameraActor::StaticClass()));

	// Get the screen size in world units
	auto camera = mainCamera->GetCameraComponent();
	float width = camera->OrthoWidth;
	float height = width / camera->AspectRatio;

	ScreenWorldSize = FVector2D (width, height);

	UE_LOG(LogTemp, Warning, TEXT("%s"), *ScreenWorldSize.ToString());
}

void AAsteroidsGameState::OnRep_Score()
{
	UpdateScore(Score);
}

void AAsteroidsGameState::OnRep_Lives()
{
	UpdateLives(Lives);
}

void AAsteroidsGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAsteroidsGameState, Lives);
	DOREPLIFETIME(AAsteroidsGameState, Score);
	DOREPLIFETIME(AAsteroidsGameState, inGame)
}
