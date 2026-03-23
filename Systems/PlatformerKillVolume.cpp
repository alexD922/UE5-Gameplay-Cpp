// Fill out your copyright notice in the Description page of Project Settings.


#include "KillVolume.h"
#include "Components/BoxComponent.h"
#include "BaseLabCharacter.h"
#include "BaseEnemy.h"
#include "TimerManager.h"
#include "Engine/World.h"


// Sets default values
AKillVolume::AKillVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	KillBox = CreateDefaultSubobject<UBoxComponent>(TEXT("KillBox"));
	RootComponent = KillBox;

	KillBox->SetCollisionProfileName(TEXT("Trigger"));
	KillBox->OnComponentBeginOverlap.AddDynamic(this, &AKillVolume::OnOverlapBegin);
	KillBox->SetGenerateOverlapEvents(true);
}

void AKillVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == nullptr) return;
	ABaseLabCharacter* Player = Cast<ABaseLabCharacter>(OtherActor);

	if (Player)
	{
		Player->Die();
	}

	ABaseEnemy* Enemy = Cast<ABaseEnemy>(OtherActor);

	if (Enemy)
	{
		Enemy->Die();
	}
}