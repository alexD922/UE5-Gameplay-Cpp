// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "BaseLabCharacter.h"
#include "BasePatrollerEnemy.h"
#include "PlatformerGameMode.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "TimerManager.h"

// Constructor
ABaseEnemy::ABaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    GetCapsuleComponent()->SetGenerateOverlapEvents(true);
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnOverlapBegin);

    SetActorRotation(FRotator(0, 180, 0));
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
    Super::BeginPlay();

    StartLocation = GetActorLocation();
    PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

    // Set initial facing direction
    if (bMovingRight)
    {
        SetActorRotation(FRotator(0, 0, 0));
    }
    else
    {
        SetActorRotation(FRotator(0, 180, 0)); 
    }
}


// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

//Damage Handling
void ABaseEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABaseLabCharacter* Player = Cast<ABaseLabCharacter>(OtherActor);
    
    if (!Player) return;
    if (Player->IsDead()) return;

    if (Player)
    {
        //If Player is Rolling
        if (Player->IsRolling() && Player->GetCharacterMovement()->IsMovingOnGround())
        {
            TakeEnemyDamage(30.f);
        }
        else
        {
            Player->GetDamaged(50.f);
        }
    }
}
void ABaseEnemy::TakeEnemyDamage(float DamageAmount)
{
	Health -= DamageAmount;

	Health = FMath::Clamp(Health, 0.f, 30.f);

	if (Health <= 0)
	{
        bIsDead = true;

        GetCharacterMovement()->DisableMovement();

        SetActorEnableCollision(false);

        GetWorld()->GetTimerManager().SetTimer(DespawnTimer, this, &ABaseEnemy::Die, 2.0f, false);
	}
}

void ABaseEnemy::Die()
{
    APlatformerGameMode* GM = Cast<APlatformerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    
    if (GM)
    {
        GM->OnEnemyKilled();
    }

    Destroy();
}