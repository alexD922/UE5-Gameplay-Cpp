// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformerGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BarrierGoal.h"
#include "LevelConfig.h"

void APlatformerGameMode::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> FoundConfigs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelConfig::StaticClass(), FoundConfigs);

	if (FoundConfigs.Num() > 0)
	{
		ALevelConfig* Config = Cast<ALevelConfig>(FoundConfigs[0]);

		if (Config)
		{
			EnemiesToKill = Config->EnemiesToKill;
		}
	}
}

void APlatformerGameMode::OnEnemyKilled()
{
	EnemiesKilled++;

	if (EnemiesKilled >= EnemiesToKill)
	{
		OnWinConditionMet.Broadcast();
	}
}
