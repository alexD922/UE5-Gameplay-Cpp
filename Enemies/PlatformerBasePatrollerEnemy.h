// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "BasePatrollerEnemy.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERLAB_API ABasePatrollerEnemy : public ABaseEnemy
{
	GENERATED_BODY()

public:
	ABasePatrollerEnemy();

protected:

	//Distance Enemy Patrols
	UPROPERTY(EditAnywhere)
	float PatrolDistance = 300.f;

public:
	virtual void Tick(float DeltaTime) override;
};
