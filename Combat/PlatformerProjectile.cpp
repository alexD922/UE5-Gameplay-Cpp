// Fill out your copyright notice in the Description page of Project Settings.


#include "LabProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BaseEnemy.h"


// Sets default values
ALabProjectile::ALabProjectile()
{
	InitialLifeSpan = 5.f;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = Collision;

	Collision->InitSphereRadius(15.f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);

	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ALabProjectile::OnOverlapBegin);


	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	ProjectileMovement->InitialSpeed = 1200.f;
	ProjectileMovement->MaxSpeed = 1200.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
}

// Called when the game starts or when spawned
void ALabProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALabProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (OtherActor && OtherActor != this)
	{
		ABaseEnemy* Enemy = Cast<ABaseEnemy>(OtherActor);

		if (Enemy)
		{
			Enemy->TakeEnemyDamage(Damage);
		}
	}
}

// Called every frame
void ALabProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

