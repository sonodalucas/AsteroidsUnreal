// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSprite.h"
#include "ObjectPool/AsteroidsPoolableActor.h"
#include "Core/AsteroidsScreenWrapper.h"
#include "AsteroidsProjectile.generated.h"

class UPaperSpriteComponent;
class UProjectileMovementComponent;
/**
 * 
 */

UENUM()
enum EAsteroidsCollisionChannel
{
	ECC_PlayerProjectileObjectChannel = ECC_GameTraceChannel1,
	ECC_SaucerProjectileObjectChannel = ECC_GameTraceChannel2,
	ECC_PlayerShipObjectChannel = ECC_GameTraceChannel3,
	ECC_SaucerObjectChannel = ECC_EngineTraceChannel4
};

UCLASS()
class ASTEROIDSATARI_API AAsteroidsProjectile : public AAsteroidsPoolableActor, public AsteroidsScreenWrapper
{
	GENERATED_BODY()

public:
	AAsteroidsProjectile();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	FTimerHandle handle;

	UPROPERTY(BlueprintReadOnly)
	bool isPlayerProjectile;

	UPROPERTY()
	TEnumAsByte<ECollisionChannel> collisionChannelToIgnore;

public:

	// Speed of the bullet
	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed = 300.0f;

	// Lifetime of the bullet
	UPROPERTY(EditAnywhere)
	float LifeTime = 5.f;

	

	// Srite for the player projectile
	UPROPERTY(EditAnywhere)
	UPaperSprite* PlayerSprite;

	// Sprite for the saucer projectile
	UPROPERTY(EditAnywhere)
	UPaperSprite* SaucerSprite;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPaperSpriteComponent* SpriteComponent;

	UPROPERTY(EditAnywhere, Category="Components")
	UProjectileMovementComponent* ProjectileMovement;

	virtual void Initialize() override;

	void Initialize(FVector position, FRotator rotator, APawn* instigator, bool shotByPlayer);

	virtual void ReturnToPool() override;
};
