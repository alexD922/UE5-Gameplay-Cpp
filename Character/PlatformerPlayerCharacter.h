// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TimerManager.h"
#include "Checkpoint.h"
#include "Blueprint/UserWidget.h"
#include "BaseLabCharacter.generated.h"

class ACheckpoint;
class UUserWidget;

UCLASS()
class PLATFORMERLAB_API ABaseLabCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	// Constructor
	ABaseLabCharacter();

	// Engine Overrides
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override; 
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* ================================
				MOVEMENT
	   ================================ */

	void MoveRight(const FInputActionValue& Value);

	// Jump
	void JumpStart();
	void EndJump();

	/* ================================
				ROLL
	   ================================ */

	void StartRoll();

	UFUNCTION(BlueprintCallable)
	void EndRoll();

	bool IsRolling() const;

	/* ================================
				COMBAT
	   ================================ */

	void Fire();

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAttacking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* ThrowMontage;

	/* ================================
				DAMAGE
	   ================================ */

	void GetDamaged(float DamageAmount);
	void ResetDamage();
	void Die();
	

	bool bCanBeDamaged = true;
	bool IsDead() const { return bIsDead; }

	/* ================================
			OVERLAP / COLLISION
	   ================================ */

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	/* ================================
			GAME FLOW
	   ================================ */

	UFUNCTION()
	void RestartLevel();

	UFUNCTION()
	void TriggerVictory(FName LevelToLoad);

	FName PendingLevelToLoad;

	UFUNCTION()
	void LoadNextLevel();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
	ACheckpoint* CurrentCheckpoint;

	UFUNCTION()
	ACheckpoint* GetCurrentCheckpoint() const;

	UFUNCTION()
	void SetCheckpoint(ACheckpoint* NewCheckpoint);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
	bool bHasCheckpoint = false;

	UFUNCTION()
	void RespawnAtCheckpoint();


	/* ================================
					UI
	================================ */

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> FadeInWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> FadeOutWidgetClass;



protected:

	/* ================================
				INPUT
	   ================================ */

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* AltFireAction;

	/* ================================
				CAMERA
	   ================================ */

	UPROPERTY(EditAnywhere, Category = "Camera")
	TSubclassOf<class UCameraShakeBase> RollCameraShake;

	/* ================================
				PROJECTILES
	   ================================ */

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ALabProjectile> ProjectileClass;

	/* ================================
				HEALTH
	   ================================ */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Death")
	bool bIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	bool bGotDamaged = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* Bandage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bBandageLost = false;

	FTimerHandle BandageCleanupTimer;

	void RemoveBandage();

	/* ================================
				MOVEMENT STATE
	   ================================ */

	float FacingDirection = 1.f;

	/* ================================
				ROLL SYSTEM
	   ================================ */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll")
	bool bIsRolling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll")
	int MaxRollCharges = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Roll")
	int CurrentRollCharges = 2;

	UPROPERTY(EditAnywhere, Category = "Roll")
	float RollRechargeTime = 5.0f;

	FVector RollDirection;

	void RechargeRoll();

	FTimerHandle RollRechargeTimer;
	FTimerHandle RollEndTimer;

	/* ================================
			JUMP / ANIMATION STATE
	   ================================ */

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Jump")
	bool bJustJumped = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Jump")
	bool bIsJumping = false;

	FVector JumpDirection;

	void Landed(const FHitResult& Hit);
	/* ================================
				KNOCKBACK
	 ================================ */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float KnockbackStrength = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float KnockbackUpForce = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bIsKnockedBack = false;

	void EndKnockback();

	FTimerHandle KnockbackTimer;

	/* ================================
				TIMERS
	   ================================ */

	FTimerHandle RestartTimer;
	FTimerHandle DamageCooldownTimer;
};
