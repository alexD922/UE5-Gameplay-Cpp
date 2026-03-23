// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseLabCharacter.h"
#include "BaseEnemy.h"
#include "LabProjectile.h"
#include "KillVolume.h"
#include "BarrierGoal.h"

#include "Blueprint/UserWidget.h"

#include "Kismet/GameplayStatics.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

#include "Camera/CameraShakeBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

#include "TimerManager.h"



/*=================================================
					CONSTRUCTOR
=================================================*/

ABaseLabCharacter::ABaseLabCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetMesh()->SetRelativeRotation(FRotator(0, 90, 0));

	JumpMaxCount = 2;
	CurrentRollCharges = MaxRollCharges;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
}



/*=================================================
					BEGIN PLAY
=================================================*/

void ABaseLabCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FadeInWidgetClass)
	{
		UUserWidget* FadeWidget = CreateWidget<UUserWidget>(GetWorld(), FadeInWidgetClass);

		if (FadeWidget)
		{
			FadeWidget->AddToViewport(1000);
		}
	}

	Bandage = Cast<USkeletalMeshComponent>(GetDefaultSubobjectByName(TEXT("BandageMesh")));

	CurrentHealth = MaxHealth;

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}



/*=================================================
					  TICK
=================================================*/

void ABaseLabCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



/*=================================================
					INPUT SETUP
=================================================*/

void ABaseLabCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent =
		Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABaseLabCharacter::MoveRight);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABaseLabCharacter::JumpStart);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABaseLabCharacter::EndJump);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABaseLabCharacter::Fire);

		EnhancedInputComponent->BindAction(AltFireAction, ETriggerEvent::Started, this, &ABaseLabCharacter::StartRoll);
	}
}



/*=================================================
					  MOVEMENT
=================================================*/

void ABaseLabCharacter::MoveRight(const FInputActionValue& Value)
{
	if (bIsKnockedBack || bIsDead)
	{
		return;
	}

	float AxisValue = Value.Get<float>();

	if (bIsRolling)
	{
		AddMovementInput(RollDirection, 1.0f);
		return;
	}

	if (Controller && AxisValue != 0.f)
	{
		AddMovementInput(FVector(1.f, 0.f, 0.f), AxisValue);
	

		FacingDirection = AxisValue;

		if (FacingDirection > 0)
		{
			GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
		}
		else
		{
			GetMesh()->SetRelativeRotation(FRotator(0, 90, 0));
		}
	}
}



/*=================================================
						JUMP
=================================================*/

void ABaseLabCharacter::JumpStart()
{
	Super::Jump();
	
	if (CanJump())
	{
		bJustJumped = true;
		bIsJumping = true;
	}
}

void ABaseLabCharacter::EndJump()
{
	ACharacter::StopJumping();

}

void ABaseLabCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	bJustJumped = false;
	bIsJumping = false;
}


/*=================================================
						COMBAT
=================================================*/

void ABaseLabCharacter::Fire()
{
	if (bIsRolling) return;

	bIsAttacking = true;
	PlayAnimMontage(ThrowMontage);
}

void ABaseLabCharacter::SpawnProjectile()
{
	if (bIsRolling) return;

	if (!ProjectileClass)
	{
		return;
	}

	FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("Rock_Socket"));

	FRotator SpawnRotation = (FacingDirection > 0) ? FRotator(0.f, 0.f, 0.f) : FRotator(0.f, 180.f, 0.f);

	GetWorld()->SpawnActor<ALabProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
}

/*=================================================
						ROLL
=================================================*/

void ABaseLabCharacter::StartRoll()
{
	if (CurrentRollCharges <= 0)
		return;

	if (bIsRolling || bIsDead || bIsKnockedBack || bJustJumped)
		return;

	bIsRolling = true;
	CurrentRollCharges--;
	
	GetCharacterMovement()->MaxWalkSpeed = 900.f;
	GetCharacterMovement()->MaxAcceleration = 4000.f;

	RollDirection = FVector(FacingDirection, 0.f, 0.f);

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && RollCameraShake)
	{
		PC->ClientStartCameraShake(RollCameraShake);
	}

	GetWorld()->GetTimerManager().SetTimer(RollRechargeTimer, this, &ABaseLabCharacter::RechargeRoll, RollRechargeTime, false);
	GetWorld()->GetTimerManager().SetTimer(RollEndTimer, this, &ABaseLabCharacter::EndRoll, 0.45f, false);
	
}


bool ABaseLabCharacter::IsRolling() const
{
	return bIsRolling;
}

void ABaseLabCharacter::EndRoll()
{
	bIsRolling = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxAcceleration = 2048.f;
}

void ABaseLabCharacter::RechargeRoll()
{
	CurrentRollCharges = FMath::Clamp(CurrentRollCharges + 1, 0, MaxRollCharges);

	if (CurrentRollCharges < MaxRollCharges)
	{
		GetWorld()->GetTimerManager().SetTimer(RollRechargeTimer, this, &ABaseLabCharacter::RechargeRoll, RollRechargeTime, false);
	}
}



/*=================================================
					DAMAGE SYSTEM
=================================================*/

void ABaseLabCharacter::GetDamaged(float DamageAmount)
{
	if (bIsDead || !bCanBeDamaged)
	{
		return;
	}

	if (!bBandageLost && Bandage)
	{
		Bandage->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		
		Bandage->SetAnimationMode(EAnimationMode::AnimationCustomMode);

		Bandage->SetCollisionProfileName(TEXT("Debris"));
		Bandage->SetSimulatePhysics(true);
		Bandage->WakeAllRigidBodies();
		
		FVector LaunchImpulse(FMath::RandRange(-150.f, 150.f), FMath::RandRange(-150.f, 150.f), 350.f);
		Bandage->AddImpulseToAllBodiesBelow(LaunchImpulse, NAME_None, true);
		
		bBandageLost = true;

		GetWorldTimerManager().SetTimer(BandageCleanupTimer, this, &ABaseLabCharacter::RemoveBandage, 3.0f, false);
	}

	bCanBeDamaged = false;

	CurrentHealth -= DamageAmount;

	CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);

	bGotDamaged = true;
	bIsKnockedBack = true;

	FVector KnockbackDirection = FVector(-FacingDirection, 0.f, 0.f);

	FVector LaunchVelocity =
		KnockbackDirection * KnockbackStrength +
		FVector(0.f, 0.f, KnockbackUpForce);

	

	GetWorld()->GetTimerManager().SetTimer(
		KnockbackTimer,
		this,
		&ABaseLabCharacter::EndKnockback,
		0.35f,
		false
	);

	LaunchCharacter(LaunchVelocity, true, true);

	if (CurrentHealth <= 0)
	{
		Die();
	}

	GetWorld()->GetTimerManager().SetTimer(DamageCooldownTimer, this, &ABaseLabCharacter::ResetDamage, 0.4f, false);
}


/*=================================================
						Reset Damage
=================================================*/

void ABaseLabCharacter::EndKnockback()
{
	bIsKnockedBack = false;
}

void ABaseLabCharacter::ResetDamage()
{
	bCanBeDamaged = true;
}

void ABaseLabCharacter::RemoveBandage()
{
	if (Bandage)
	{
		Bandage->SetVisibility(false);
		Bandage->SetSimulatePhysics(false);
		Bandage->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

/*=================================================
						DEATH
=================================================*/

void ABaseLabCharacter::Die()
{
	if (bIsDead) return;

	bIsDead = true;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	
	if (bHasCheckpoint)
	{
		GetWorld()->GetTimerManager().SetTimer(RestartTimer, this, &ABaseLabCharacter::RespawnAtCheckpoint, 1.0f, false);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(RestartTimer, this, &ABaseLabCharacter::RestartLevel, 2.0f, false);
	}
}



/*=================================================
				OVERLAP DETECTION
=================================================*/

void ABaseLabCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}
}



/*=================================================
					Game Flow
=================================================*/

void ABaseLabCharacter::RestartLevel()
{

	FName CurrentLevel = *UGameplayStatics::GetCurrentLevelName(GetWorld());

	UGameplayStatics::OpenLevel(GetWorld(), CurrentLevel);

}

void ABaseLabCharacter::TriggerVictory(FName LeveltToLoad)
{
	PendingLevelToLoad = LeveltToLoad;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		DisableInput(PC);
	}

	GetCharacterMovement()->DisableMovement();

	if (FadeOutWidgetClass)
	{
		UUserWidget* FadeWidget = CreateWidget<UUserWidget>(GetWorld(), FadeOutWidgetClass);

		if (FadeWidget)
		{
			FadeWidget->AddToViewport(1000);
		}
	}
	
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABaseLabCharacter::LoadNextLevel, 1.5f, false);
}

void ABaseLabCharacter::LoadNextLevel()
{
	UGameplayStatics::OpenLevel(this, PendingLevelToLoad);
}


/*=================================================
				Checkpoint Logic
=================================================*/

ACheckpoint* ABaseLabCharacter::GetCurrentCheckpoint() const
{
	return CurrentCheckpoint;
}

void ABaseLabCharacter::SetCheckpoint(ACheckpoint* NewCheckpoint)
{
	UE_LOG(LogTemp, Warning, TEXT("Setting checkpoint to: %s"), *NewCheckpoint->GetName());

	if (!NewCheckpoint) return;

	if (CurrentCheckpoint && CurrentCheckpoint != NewCheckpoint)
	{
		CurrentCheckpoint->DeactivateCheckpoint();
	}

	//Set New Checkpoint
	CurrentCheckpoint = NewCheckpoint;
	bHasCheckpoint = true;

	//Turn on New Checkpoint
	CurrentCheckpoint->ActivateCheckpoint();

	UE_LOG(LogTemp, Warning, TEXT("Checkpoint Set"));
}

void ABaseLabCharacter::RespawnAtCheckpoint()
{
	CurrentHealth = MaxHealth;
	// Move player
	if (CurrentCheckpoint)
	{
		FVector SpawnLocation = CurrentCheckpoint->GetSpawnLocation();
		SetActorLocation(SpawnLocation + FVector(0.f, 0.f, 100.f));
	}

	// Reset state
	bIsDead = false;
	bGotDamaged = false;
	bIsKnockedBack = false;
	bBandageLost = false;

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->bPauseAnims = false;

	// Re-enable movement
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	// Stop all movement (IMPORTANT)
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->Velocity = FVector::ZeroVector;

	// Reset velocity manually (extra safety)
	GetCharacterMovement()->Velocity = FVector::ZeroVector;

	AController* PlayerController = GetController();

	if (PlayerController)
	{
		SetActorRotation(FRotator(0.f, 0.f, 0.f));
	}

	if (FacingDirection > 0)
	{
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	}
	else
	{
		GetMesh()->SetRelativeRotation(FRotator(0, 90, 0));
	}

	if (Bandage)
	{
		Bandage->SetVisibility(true);
		// Stop physics
		Bandage->SetSimulatePhysics(false);
		Bandage->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Reattach to character mesh
		Bandage->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			TEXT("NAME_None") // OR NAME_None if no socket
		);

		// Restore animation
		Bandage->SetAnimationMode(EAnimationMode::AnimationBlueprint);

		// Reset state
		bBandageLost = false;
	}
}
