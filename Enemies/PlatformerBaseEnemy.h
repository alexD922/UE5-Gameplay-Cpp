// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

UCLASS()
class PLATFORMERLAB_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseEnemy();

protected:

	UPROPERTY(VisibleAnywhere, CATEGORY = "Stats")
	float Health = 30.f;

	UPROPERTY(EditAnywhere)
	float MoveSpeed = 150.f;

	FVector StartLocation;

	bool bMovingRight = true;

	UPROPERTY(EditAnywhere)
	float DetectionRange = 600.f;

	UPROPERTY(EditAnywhere)
	float ChaseSpeed = 400.f;

	APawn* PlayerPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bChasingPlayer = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDead = false;

	FTimerHandle DespawnTimer;


public:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void TakeEnemyDamage(float DamageAmount);

	virtual void Die();
};
