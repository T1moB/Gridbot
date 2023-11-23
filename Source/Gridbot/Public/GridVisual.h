// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridMeshInstance.h"
#include "Components/ChildActorComponent.h"
#include "GridVisual.generated.h"

class AGrid;


UENUM(BlueprintType)
enum E_TileType : int {
	EMPTY = 0 UMETA(DisplayName = "NONE"),
	NORMAL = 1 UMETA(DisplayName = "Normal"),
	OBSTACLE = 2 UMETA(DisplayName = "Obstacle"),
	MUD = 3 UMETA(DisplayName = "Mud", ToolTip = "Double cost"),
	AIR = 4 UMETA(DisplayName = "Air", ToolTip = "Only for flying units"),
	TRIPLE = 5 UMETA(DisplayName = "Triple", ToolTip = "Triple cost")
};

USTRUCT(BlueprintType)
struct FS_TileData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint index = -999;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<E_TileType> type = EMPTY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform transform = FTransform::Identity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<E_TileState>> states;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* UnitOnTile = nullptr;

	FS_TileData(FIntPoint inIndex, E_TileType inTileType, FTransform inTransform) :
		index(inIndex),
		type(inTileType),
		transform(inTransform)
	{}
	FS_TileData() {}
};

UCLASS(Blueprintable)
class GRIDBOT_API AGridVisual : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridVisual();

protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleDefaultsOnly)
	UChildActorComponent* GridMeshInstance_ChildActor;
	UPROPERTY(VisibleDefaultsOnly)
	UChildActorComponent* GridMeshInstance_Tactical_ChildActor;

	UPROPERTY()
	AGrid* grid;
	UPROPERTY()
	AGridMeshInstance* gridMeshInstance;
	UPROPERTY()
	AGridMeshInstance* gridMeshInstanceTactical;
	UPROPERTY(BlueprintReadOnly)
	bool isTactical;
	bool regenerateTacticalGrid;

	float offsetFromGround;
	float lowestZ = 9999;

public:	

	void InitializeGridVisual(AGrid* grid);
	void DestroyGridVisual();
	void UpdateTileVisual(FS_TileData data);
	void UpdateTileVisualTactical(FS_TileData data);
	UFUNCTION(BlueprintCallable)
	int GetMeshIndex(FS_TileData data);
	UFUNCTION(BlueprintCallable)
	void SetTacticalGrid(bool tactical);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool isWalkable(E_TileType type) { return type != E_TileType::EMPTY && type != E_TileType::OBSTACLE; }

};
