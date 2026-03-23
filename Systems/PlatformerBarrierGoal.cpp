// Fill out your copyright notice in the Description page of Project Settings.


#include "BarrierGoal.h"
#include "BaseLabCharacter.h"
#include "PlatformerGameMode.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

#include "Kismet/GameplayStatics.h"


// Sets default values
ABarrierGoal::ABarrierGoal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	BarrierMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrierMesh"));
	BarrierMesh->SetupAttachment(SceneRoot);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(SceneRoot);

	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABarrierGoal::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ABarrierGoal::BeginPlay()
{
	Super::BeginPlay();

	APlatformerGameMode* GM = Cast<APlatformerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GM)
	{
		GM->OnWinConditionMet.AddDynamic(this, &ABarrierGoal::UnlockBarrier);
	}

	if (BarrierMesh)
	{
		DynamicMaterial = BarrierMesh->CreateDynamicMaterialInstance(0);
	}

	LockBarrier();
}

void ABarrierGoal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsUnlocked) return;

	ABaseLabCharacter* Player = Cast<ABaseLabCharacter>(OtherActor);

	if (!Player) return;

	if (Player && bIsUnlocked)
	{
		if (!NextLevelName.IsNone())
		{
			Player->TriggerVictory(NextLevelName);
		}
	}
}

void ABarrierGoal::LockBarrier()
{
	bIsUnlocked = false;

	BarrierMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	if (DynamicMaterial)
	{
		//Locked = Red
		DynamicMaterial->SetVectorParameterValue(TEXT("Barrier Color"), FLinearColor(1.0f, 0.1f, 0.0f));
		DynamicMaterial->SetVectorParameterValue(TEXT("Edge Color"), FLinearColor(1.0f, 0.3f, 0.0f));
		DynamicMaterial->SetVectorParameterValue(TEXT("Smoke Color"), FLinearColor(1.0f, 0.1f, 0.1f));
	}
}

void ABarrierGoal::UnlockBarrier()
{
	bIsUnlocked = true;

	if (DynamicMaterial)
	{
		//Main Barrier Color
		DynamicMaterial->SetVectorParameterValue(TEXT("Barrier Color"), FLinearColor(0.0f, 1.0f, 0.3f));

		//Edge Glow
		DynamicMaterial->SetVectorParameterValue(TEXT("Edge Color"), FLinearColor(0.0f, 0.8f, 1.0f));

		//Smoke Effect
		DynamicMaterial->SetVectorParameterValue(TEXT("Smoke Color"), FLinearColor(0.2f, 0.8f, 0.4f));
	}

	BarrierMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
