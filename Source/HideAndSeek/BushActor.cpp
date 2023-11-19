// Fill out your copyright notice in the Description page of Project Settings.


#include "BushActor.h"

#include "HideAndSeekCharacter.h"
#include "NPC_Character.h"

// Sets default values
ABushActor::ABushActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BushMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = BushMesh;

	// Setup Bush Box Collider
	BushBoxCollider = CreateDefaultSubobject<UBoxComponent>(FName("TriggerVolume"));
	BushBoxCollider->AttachToComponent(BushMesh, FAttachmentTransformRules::KeepRelativeTransform);
	BushBoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ABushActor::OnOverlapBegin);
	BushBoxCollider->OnComponentEndOverlap.AddDynamic(this, &ABushActor::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ABushActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABushActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ABushActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep,
                                const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ANPC_Character::StaticClass()))
	{
		NPCsOverlapping++;
	}

	if (OtherActor->IsA(AHideAndSeekCharacter::StaticClass()))
	{
		PlayersOverlapping++;

		if (PlayerReference == nullptr)
		{
			PlayerReference = Cast<AHideAndSeekCharacter>(OtherActor);
		}

		// Hide player from NPCs
		PlayerReference->UpdatePlayerHidden(true);
	}

	if ((PlayersOverlapping > 0) && (NPCsOverlapping > 0))
	{
		PlayerReference->EndGame(false);
	}
}


void ABushActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ANPC_Character::StaticClass()))
	{
		NPCsOverlapping--;
	}
	if (OtherActor->IsA(AHideAndSeekCharacter::StaticClass()))
	{
		PlayersOverlapping--;

		// Make Player visible to NPCs again
		PlayerReference->UpdatePlayerHidden(false);
	}
}
