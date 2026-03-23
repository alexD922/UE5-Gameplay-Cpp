// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/PointLightComponent.h"
#include "TimerManager.h"
#include "BaseLabCharacter.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(Root);

	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnOverlapBegin);

	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(Root);

	LampLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("LampLight"));
	LampLight->SetupAttachment(Mesh); // attach to lamp
	LampLight->SetIntensity(0.0f); // OFF by default
	LampLight->SetAttenuationRadius(300.f);
	LampLight->SetLightColor(FLinearColor(1.0f, 0.7f, 0.3f)); // warm glow
}


// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
}

FVector ACheckpoint::GetSpawnLocation() const
{
	return SpawnPoint->GetComponentLocation();
}


void ACheckpoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABaseLabCharacter* Player = Cast<ABaseLabCharacter>(OtherActor);

	if (!Player) return;

	if (Player->GetCurrentCheckpoint() != this)
	{
		Player->SetCheckpoint(this);
		UE_LOG(LogTemp, Warning, TEXT("Overlap triggered by: %s"), *OtherComp->GetName());
	}
}


void ACheckpoint::ActivateCheckpoint()
{
	FTimerHandle TimerHandle;
	LampLight->SetIntensity(10000.0f);
	GetWorldTimerManager().SetTimer( TimerHandle, [this]() { LampLight->SetIntensity(3000.0f); }, 0.2f, false); // settle value
}

void ACheckpoint::DeactivateCheckpoint()
{
	LampLight->SetIntensity(0.0f);
}