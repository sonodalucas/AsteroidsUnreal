// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/AsteroidsScreenWrapper.h"
#include "PaperCharacter.h"
#include "AsteroidsShipCharacter.generated.h"

class AAsteroidsGameState;
class AAsteroidsGameMode;
class AAsteroidsPoolableActor;
class UAsteroidsPoolManager;
/**
 * 
 */
UCLASS()
class ASTEROIDSATARI_API AAsteroidsShipCharacter : public APaperCharacter, public AsteroidsScreenWrapper
{
	GENERATED_BODY()

public:
	AAsteroidsShipCharacter();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float turnVelocity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float invincibilityTime = 1;

	// Percentage chance of the hyper space killing the player (0 - 1)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0, ClampMax = 1))
	float chanceToExplodeOnHyperSpace = .1f;

	// Time the input keeps deactivated after death
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float timeToRegainControlAfterDeath = .2f;

	UPROPERTY(BlueprintReadWrite)
	bool isInvincible = false;

	// Debug only, stop the ship from taking damage
	UPROPERTY(EditAnywhere)
	bool debugMode = false;

	UPROPERTY(BlueprintReadOnly)
	AAsteroidsGameMode* gameMode;

	UPROPERTY(BlueprintReadOnly)
	AAsteroidsGameState* gameState;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintImplementableEvent)
	void StartSpriteFlicker();

	UFUNCTION(BlueprintCallable)
	void ShootProjectile(FVector startPosition, FRotator startRotator, bool shotByPlayer);

	void OnShipHit();

	void RespawnShip();

	UFUNCTION(BlueprintCallable)
	void ActivateHyperSpace();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
