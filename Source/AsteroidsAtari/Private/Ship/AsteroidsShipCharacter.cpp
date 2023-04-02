// Fill out your copyright notice in the Description page of Project Settings.


#include "Ship/AsteroidsShipCharacter.h"

#include "PaperFlipbookComponent.h"
#include "Camera/CameraActor.h"
#include "Core/AsteroidsGameMode.h"
#include "Core/AsteroidsGameState.h"
#include "Core/AsteroidsPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
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

void AAsteroidsShipCharacter::AuthSetColor(const FLinearColor& InColor)
{
	checkf(HasAuthority(), TEXT("ARepsiPawn::AuthSetColor called on client"));
	
	Color = InColor;
	
	OnRep_Color();
}

void AAsteroidsShipCharacter::StartFlicker_Implementation()
{
	StartSpriteFlicker();
}

void AAsteroidsShipCharacter::ShootProjectile_Implementation(FVector startPosition, FRotator startRotator, bool shotByPlayer)
{
	AAsteroidsProjectile* projectile = gameState->GetProjectile();
	projectile->Initialize(startPosition, startRotator, this, shotByPlayer);
}

void AAsteroidsShipCharacter::OnShipHit_Implementation()
{
	if (!isInvincible && !debugMode) 
	{
		GetCharacterMovement()->StopMovementImmediately();
		SetActorEnableCollision(ECollisionEnabled::NoCollision);
		Cast<AAsteroidsPlayerController>(GetController())->LockPlayerInput();

		FTimerHandle handle;
		GetWorldTimerManager().SetTimer(handle, this, &AAsteroidsShipCharacter::Server_RespawnShip,
			timeToRegainControlAfterDeath,false);
	}
}

void AAsteroidsShipCharacter::RespawnShip_Implementation()
{
	SetActorLocation(FVector(0, 0, 0));
	isInvincible = true;
	gameState->LooseLife();
	SetActorEnableCollision(ECollisionEnabled::QueryOnly);
	if (gameState->inGame)
	{
		Cast<AAsteroidsPlayerController>(GetController())->UnlockPlayerInput();
		StartFlicker();
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
		GetWorldTimerManager().SetTimer(handle, this, &AAsteroidsShipCharacter::Server_OnPlayerHit,
			timeToRegainControlAfterDeath, false);
		// OnShipHit();
	}
}

float AAsteroidsShipCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                          AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Ship took damage"))
	Server_OnPlayerHit();
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AAsteroidsShipCharacter::Server_RotateActor_Implementation(FRotator rotation)
{
	checkf(HasAuthority(), TEXT("AAsteroidsShipCharacter::Server_RotateActor called on client"));
	AddActorWorldRotation(rotation);
}

void AAsteroidsShipCharacter::Server_OnPlayerHit_Implementation()
{
	checkf(HasAuthority(), TEXT("AAsteroidsShipCharacter::Server_OnPlayerHit called on client"));
	OnShipHit();
}

void AAsteroidsShipCharacter::Server_RespawnShip_Implementation()
{
	checkf(HasAuthority(), TEXT("AAsteroidsShipCharacter::Server_RespawnShip called on client"));

	RespawnShip();
}

void AAsteroidsShipCharacter::OnRep_Color()
{
	GetSprite()->SetSpriteColor(Color);
}

void AAsteroidsShipCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAsteroidsShipCharacter, Color);
}
