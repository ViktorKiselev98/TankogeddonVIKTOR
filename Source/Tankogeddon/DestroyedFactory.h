// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestroyedFactory.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class TANKOGEDDON_API ADestroyedFactory : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BuildingMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UBoxComponent* HitCollider;
public:	
	// Sets default values for this actor's properties
	ADestroyedFactory();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
