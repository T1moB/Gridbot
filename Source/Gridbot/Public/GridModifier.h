// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridVisual.h"
#include "GridModifier.generated.h"

UCLASS(Blueprintable)
class GRIDBOT_API AGridModifier : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	AGridModifier();

protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;

public:	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<E_TileType> type;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool useForTileHeight;

};
