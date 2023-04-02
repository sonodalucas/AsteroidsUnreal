// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteComponent.h"
#include "Components/ArrowComponent.h"
#include "Core/AsteroidsScreenWrapper.h"
#include "ObjectPool/AsteroidsPoolableActor.h"
#include "AsteroidsSaucer.generated.h"

class AAsteroidsGameState;
UENUM()
enum ESaucerType
{
	SaucerNone = 0 UMETA(DisplayName = "None"), 
	SaucerSmall = 1 UMETA(DisplayName = "Small"),
	SaucerBig = 2 UMETA(DisplayName = "Big")
};

/**
 * 
 */
UCLASS()
class ASTEROIDSATARI_API AAsteroidsSaucer : public AAsteroidsPoolableActor, public AsteroidsScreenWrapper
{
	GENERATED_BODY()

public:
	AAsteroidsSaucer();

	UPROPERTY(EditAnywhere)
	UPaperSpriteComponent* SpriteComponent;

	UPROPERTY()
	FTimerHandle shootTimerHandle;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	TObjectPtr<AAsteroidsGameMode> gameMode;

	UPROPERTY()
	AAsteroidsGameState* gameState;

	// Speed of saucer movement
	UPROPERTY(EditAnywhere, Category = "Saucer")
	float speed = 200;

	// Direction of saucer movement
	UPROPERTY(VisibleAnywhere, Category = "Saucer")
	FVector direction;

	// Time between direction changes
	UPROPERTY()
	float changeDirectionTime;

	UPROPERTY(EditAnywhere, Category = "Saucer")
	float maxChangeDirectionTime = 2.f;

	UPROPERTY(EditAnywhere, Category = "Saucer")
	float minChangeDirectionTime = .5f;

	// Current time since last direction change
	UPROPERTY()
	float currentChangeDirectionTime;

	UPROPERTY()
	bool saucerOnScreen;

	UPROPERTY()
	TEnumAsByte<ESaucerType> saucerType;

	bool isShooting;
	
	UPROPERTY(EditAnywhere, Category = "Saucer")
	float maxShootTime = 2;

	UPROPERTY(EditAnywhere, Category = "Saucer")
	float minShootTime = 1;

	float currentShootTime = 0;

	float shootTime;

	// The amount of points the saucer gives when destroyed
	UPROPERTY(EditAnywhere, Category = "Saucer")
	int saucerScoreValue = 500;

	// Function to move the saucer
	void MoveSaucer(float DeltaTime);

	// Function to change the saucer's direction
	void ChangeDirection();

	virtual void Initialize() override;

	UFUNCTION(BlueprintCallable, Category = "Saucer")
	void Shoot();

	void StartShooting(float InInterval);

	void StopShooting();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:
	void Initialize(TEnumAsByte<ESaucerType> type, FVector startPosition, FVector saucerDirection);

	virtual void ReturnToPool() override;
};
