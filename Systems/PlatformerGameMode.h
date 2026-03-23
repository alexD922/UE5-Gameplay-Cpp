// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlatformerGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWinConditionMet);

UCLASS()
class PLATFORMERLAB_API APlatformerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY()
	int32 EnemiesKilled = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WinCondition")
	int32 EnemiesToKill = 6;

	void OnEnemyKilled();
	
	UPROPERTY(BlueprintAssignable)
	FOnWinConditionMet OnWinConditionMet;

protected:
	
	virtual void BeginPlay() override;

};
