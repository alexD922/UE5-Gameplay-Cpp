// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillVolume.generated.h"

class UBoxComponent;

UCLASS()
class PLATFORMERLAB_API AKillVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKillVolume();

protected:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* KillBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BounceStrength = 900.f;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
