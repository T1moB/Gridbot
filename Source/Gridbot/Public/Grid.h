// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridVisual.h"
#include "GridPathfinding.h"
#include "Grid.generated.h"

USTRUCT(BlueprintType)
struct FIndexesArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FIntPoint> indexes;
	FIndexesArray() {};
	FIndexesArray(TArray<FIntPoint> inIndexes) : indexes(inIndexes) {};
};


UCLASS(Blueprintable)
class GRIDBOT_API AGrid : public AActor
{

	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AGrid();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* GridVisual_ChildActor;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* GridPathfinding_ChildActor;

	
	UPROPERTY(BlueprintReadWrite)
	FVector gridBottomLeft;
	UPROPERTY(BlueprintReadOnly)
	AGridVisual* gridVisual;
	UPROPERTY(BlueprintReadOnly)
	AGridPathfinding* gridPathfinding;
	UPROPERTY(BlueprintReadOnly)
	TMap <TEnumAsByte<E_TileState>,FIndexesArray> tileStatesToIndexes;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite)
	TMap<FIntPoint, FS_TileData> gridTiles; 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Grid Settings")
	FVector spawnLocation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Settings")
	FVector tileSize = FVector(100, 100, 50);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Settings")
	FIntPoint gridSize = FIntPoint(10, 10);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Settings")
	bool showMouseOnGrid = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Settings")
	bool useEnvironment = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Settings")
	UStaticMesh* gridMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Settings")
	UMaterial* gridMat;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Settings")
	UStaticMesh* dGridMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Settings")
	UMaterial* dGridMat;

	UFUNCTION(BlueprintCallable)
	void SpawnGrid(FVector location, FVector newtileSize, FIntPoint newgridSize, bool environment);
	UFUNCTION(BlueprintCallable)
	FVector CalcCenterAndBottomLeft(FVector& center);
	FTransform TraceForGround(FVector position, E_TileType& hitType);
	UFUNCTION(BlueprintCallable)
	TEnumAsByte<E_TileType> TraceForGround(FVector position, FTransform& transform);
	UFUNCTION(BlueprintCallable)
	void DestroyGrid();

	UFUNCTION(BlueprintCallable)
	void AddGridTile(FS_TileData data);
	UFUNCTION(BlueprintCallable)
	void RemoveGridTile(FIntPoint index);
	UFUNCTION(BlueprintCallable)
	void AddStateToTile(FIntPoint index, E_TileState state);
	UFUNCTION(BlueprintCallable)
	void RemoveStateFromTile(FIntPoint index, E_TileState state);
	UFUNCTION(BlueprintCallable)
	void ClearStateFromTiles(E_TileState state);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool isIndexValid(FIntPoint index);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetTileLocationFromGrid(FIntPoint index);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetCursorLocationOnGrid(int playerIndex);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FIntPoint GetTileIndexFromWorldLocation(FVector location);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FIntPoint GetTileIndexUnderCursor(int playerIndex);
	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> GetAllTilesOfState(E_TileState state);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsTileWalkable(FIntPoint index);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsTypeWalkable(E_TileType type) { return type != NONE && type != OBSTACLE; }


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileDataUpdate, FIntPoint, index);
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnTileDataUpdate OnTileDataUpdated;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGridDestroyed);
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnGridDestroyed OnGridDestroyed;

};
