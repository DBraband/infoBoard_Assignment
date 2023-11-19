// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "BushActor.generated.h"

UCLASS()
class HIDEANDSEEK_API ABushActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABushActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Behavior, meta=(AllowPrivateAccess = "true"))
	UStaticMeshComponent* BushMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Behavior, meta=(AllowPrivateAccess = "true"))
	UBoxComponent* BushBoxCollider;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);

private:
	// Keep track of overlapping actors
	int PlayersOverlapping = 0;
	int NPCsOverlapping = 0;

	class AHideAndSeekCharacter* PlayerReference;
};
