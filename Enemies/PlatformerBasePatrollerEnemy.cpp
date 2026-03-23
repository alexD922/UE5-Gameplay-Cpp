// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePatrollerEnemy.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"



ABasePatrollerEnemy::ABasePatrollerEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCharacterMovement()->bOrientRotationToMovement = false;
    bUseControllerRotationYaw = false;

    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->GroundFriction = 8.f;

}

void ABasePatrollerEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    bool bWasChasing = bChasingPlayer;

    // Check distance to player
    if (PlayerPawn)
    {
        float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
        bChasingPlayer = DistanceToPlayer < DetectionRange;
    }

    // If we JUST stopped chasing
    if (bWasChasing && !bChasingPlayer)
    {
        float DistanceFromStart = GetActorLocation().X - StartLocation.X;

        bMovingRight = DistanceFromStart <= 0;
    }

    //--------------------------------
    // CHASE
    //--------------------------------

    if (bChasingPlayer)
    {
        FVector ChaseDirection = PlayerPawn->GetActorLocation() - GetActorLocation();
        ChaseDirection.Z = 0.f;
        ChaseDirection.Normalize();

        GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
        AddMovementInput(ChaseDirection, 1.0f);

        SetActorRotation(ChaseDirection.X > 0 ? FRotator(0, 0, 0) : FRotator(0, 180, 0));
    }

    //--------------------------------
    // PATROL
    //--------------------------------

    else
    {
        FVector PatrolDirection = FVector(bMovingRight ? 1.f : -1.f, 0.f, 0.f);

        GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
        AddMovementInput(PatrolDirection, 1.0f);

        SetActorRotation(PatrolDirection.X > 0 ? FRotator(0, 0, 0) : FRotator(0, 180, 0));

        FVector Location = GetActorLocation();

        if (bMovingRight && Location.X >= StartLocation.X + PatrolDistance)
        {
            bMovingRight = false;
        }

        if (!bMovingRight && Location.X <= StartLocation.X - PatrolDistance)
        {
            bMovingRight = true;
        }
    }
}