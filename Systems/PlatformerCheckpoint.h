// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UPointLightComponent;

UCLASS()
class PLATFORMERLAB_API ACheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ACheckpoint();
	
	virtual void BeginPlay() override;

	UFUNCTION()
	FVector GetSpawnLocation() const;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SpawnPoint;
	
	UFUNCTION()
	void ActivateCheckpoint();

	UFUNCTION()
	void DeactivateCheckpoint();

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(VisibleAnywhere)
	UPointLightComponent* LampLight;

protected:
	

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* TriggerBox;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
