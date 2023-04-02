// Fill out your copyright notice in the Description page of Project Settings.


#include "Ship/AsteroidsShipCharacter.h"
#include "Core/AsteroidsGameMode.h"
#include "Core/AsteroidsGameState.h"
#include "Core/AsteroidsPlayerController.h"
#include "Core/AsteroidsPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Ship/AsteroidsProjectile.h"

AAsteroidsShipCharacter::AAsteroidsShipCharacter()
{
	
}

void AAsteroidsShipCharacter::BeginPlay()
{
	Super::BeginPlay();

	gameMode = Cast<AAsteroidsGameMode>(GetWorld()->GetAuthGameMode());
	gameState = Cast<AAsteroidsGameState>(GetWorld()->GetGameState());

	SetScreenWrapper(this, gameState);
}

void AAsteroidsShipCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	TryToWrapActor();
}

void AAsteroidsShipCharacter::ShootProjectile(FVector startPosition, FRotator startRotator, bool shotByPlayer)
{
	AAsteroidsProjectile* projectile = gameState->GetProjectile();
	projectile->Initialize(startPosition, startRotator, this, shotByPlayer);
}

void AAsteroidsShipCharacter::OnShipHit()
{
	if (!isInvincible && !debugMode) 
	{
		GetCharacterMovement()->StopMovementImmediately();
		SetActorEnableCollision(ECollisionEnabled::NoCollision);
		Cast<AAsteroidsPlayerController>(GetController())->LockPlayerInput();

		FTimerHandle handle;
		GetWorldTimerManager().SetTimer(handle, this, &AAsteroidsShipCharacter::RespawnShip,
			timeToRegainControlAfterDeath,false);
	}
}

void AAsteroidsShipCharacter::RespawnShip()
{
	SetActorLocation(FVector(0, 0, 0));
	isInvincible = true;
	gameState->LooseLife();
	SetActorEnableCollision(ECollisionEnabled::QueryOnly);
	if (!gameMode->IsGameOver())
	{
		Cast<AAsteroidsPlayerController>(GetController())->UnlockPlayerInput();
		StartSpriteFlicker();
	}
}

void AAsteroidsShipCharacter::ActivateHyperSpace()
{
	float halfScreenX = gameState->ScreenWorldSize.X / 2;
	float halfScreenY = gameState->ScreenWorldSize.Y / 2;
	
	FVector SpawnLocation = FVector(FMath::RandRange(-halfScreenX, halfScreenX), 0,
		FMath::RandRange(-halfScreenY, halfScreenY));
	SetActorLocation(SpawnLocation);

	float randomNumber = FMath::FRand();

	if (randomNumber < chanceToExplodeOnHyperSpace)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ship exploded while on hyper space"))
		FTimerHandle handle;
		Cast<AAsteroidsPlayerController>(GetController())->LockPlayerInput();
		GetWorldTimerManager().SetTimer(handle, this, &AAsteroidsShipCharacter::OnShipHit,
			timeToRegainControlAfterDeath, false);
		OnShipHit();
	}
}

float AAsteroidsShipCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                          AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Ship took damage"))
	OnShipHit();
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
