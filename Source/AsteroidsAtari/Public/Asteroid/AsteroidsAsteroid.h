// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbookComponent.h"
#include "PaperSprite.h"
#include "Core/AsteroidsScreenWrapper.h"
#include "ObjectPool/AsteroidsPoolableActor.h"
#include "AsteroidsAsteroid.generated.h"

class UPaperSpriteComponent;
UENUM(BlueprintType)
enum EAsteroidsSize
{
	AsteroidNone = 0 UMETA(DisplayName = "None"),
	AsteroidSmall = 1 UMETA(DisplayName = "Small"),
	AsteroidMedium = 2 UMETA(DisplayName = "Medium"),
	AsteroidLarge = 3 UMETA(DisplayName = "Large")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAsteroidDestroyed, AAsteroidsAsteroid*, asteroidDestroyed);

UCLASS()
class ASTEROIDSATARI_API AAsteroidsAsteroid : public AAsteroidsPoolableActor, public AsteroidsScreenWrapper
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAsteroidsAsteroid();

	// Asteroid size
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EAsteroidsSize> Size;

	// Asteroid base speed
	UPROPERTY(EditAnywhere)
	float BaseSpeed = 50;

	// Asteroid current speed
	UPROPERTY()
	float Speed = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* SpriteComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperFlipbookComponent* FlipbookComponent;

	UPROPERTY(BlueprintCallable)
	FOnAsteroidDestroyed OnAsteroidDestroyed;

	UPROPERTY(EditAnywhere)
	TArray<UPaperSprite*> AsteroidSprites;

	UPROPERTY(EditAnywhere)
	float explosionRadius = 50;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	AAsteroidsGameMode* gameMode;

	UPROPERTY(BlueprintReadOnly)
	AAsteroidsGameState* gameState;

	UPROPERTY()
	FVector initialVelocity;

	UFUNCTION(BlueprintCallable)
	void OnHitByProjectile(AController* eventInstigator);

	UPROPERTY(ReplicatedUsing=OnRep_AsteroidScale, Transient, BlueprintReadOnly)
	FVector asteroidScale;

	void ModifyScale(FVector scale);

	UPROPERTY(ReplicatedUsing=OnRep_IsUnstable, Replicated)
	bool isUnstable;

	UPROPERTY(ReplicatedUsing=OnRep_ExplosionHidden, Replicated)
	bool explosionHidden;

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleSpriteFlash(bool state);

	UFUNCTION(BlueprintImplementableEvent)
	void TriggerExplosion();

	UFUNCTION(BlueprintCallable)
	void HideExplosion(bool isHidden);
	
private:
	UFUNCTION()
	void OnRep_AsteroidScale();

	UFUNCTION()
	void OnRep_IsUnstable();

	UFUNCTION()
	void OnRep_ExplosionHidden();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Initialize() override;
	
	virtual void Initialize(TEnumAsByte<EAsteroidsSize> asteroidSize, FVector startPosition);
	
	virtual void ReturnToPool() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(Client, Unreliable)
	void SetAsteroidsUnstable(bool state);
};
