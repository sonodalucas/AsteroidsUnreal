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

	void AuthSetColor(const FLinearColor& InColor);
	
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

	/** An arbitrary color that identifies this player; assigned by the game mode on spawn. Controls the color of the mesh. */
	UPROPERTY(ReplicatedUsing=OnRep_Color, Transient, BlueprintReadOnly, Category="Player")
	FLinearColor Color;

	UPROPERTY(BlueprintReadOnly)
	AAsteroidsGameMode* gameMode;

	UPROPERTY(BlueprintReadOnly)
	AAsteroidsGameState* gameState;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartSpriteFlicker();

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void StartFlicker();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ShootProjectile(FVector startPosition, FRotator startRotator, bool shotByPlayer);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OnShipHit();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RespawnShip();

	UFUNCTION(BlueprintCallable)
	void ActivateHyperSpace();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_RotateActor(FRotator rotation);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_OnPlayerHit();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_RespawnShip();
	
private:

	/** Updates the MeshMID's color parameter to match our current Color property. */
	UFUNCTION()
	void OnRep_Color();

};
