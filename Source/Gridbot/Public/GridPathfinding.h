// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridVisual.h"
#include "GameFramework/Actor.h"
#include "GridPathfinding.generated.h"

USTRUCT(BlueprintType)
struct FS_PathfindingData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint index = -999;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float costToEnterTile = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float costFromStart = -999;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float minimumCostToTarget = -999;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint previousIndex = -999;

	FS_PathfindingData() {}
	FS_PathfindingData(FIntPoint inIndex, float inCostToEnter, float inCostFromStart, float inMinimumCostToTarget, FIntPoint inPreviousIndex) :
		index(inIndex),
		costToEnterTile(inCostToEnter),
		costFromStart(inCostFromStart),
		minimumCostToTarget(inMinimumCostToTarget),
		previousIndex(inPreviousIndex)
	{}
	//virtual FString ToString() override { return (TEXT("index: %s, to enter %d, from start %d, to target %d, previous %s"), *index.ToString(), costToEnterTile, costFromStart, minimumCostToTarget, *previousIndex.ToString()); }
};


class AGrid;

UCLASS()
class GRIDBOT_API AGridPathfinding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridPathfinding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> FindPath(FIntPoint start, FIntPoint target, bool diagonals, TArray<TEnumAsByte<E_TileType>> types, float delay);
	UFUNCTION(BlueprintNativeEvent)
	void FindPathWithDelay();
	void FindPathWithDelay_Implementation();
	UFUNCTION(BlueprintCallable)
	bool IsInputValid();
	UFUNCTION(BlueprintCallable)
	void DiscoverTile(FS_PathfindingData data);
	UFUNCTION(BlueprintCallable)
	TArray<FS_PathfindingData> GetValidTileNeighbours(FIntPoint index, bool includeDiagonals, TArray<TEnumAsByte<E_TileType>> validTypes);
	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> GetTileNeighbours(FIntPoint index, bool includeDiagonals);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMinimumCostBetweenTiles(FIntPoint index1, FIntPoint index2, bool diagonals);
	UFUNCTION(BlueprintCallable)
	bool AnalyzeNextDiscoverTile();
	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> GeneratePath();
	UFUNCTION(BlueprintCallable)
	FS_PathfindingData PullCheapestFromDiscoverd();
	UFUNCTION(BlueprintCallable)
	bool DiscoverNextNeighbour();
	UFUNCTION(BlueprintCallable)
	void InsertTileInDiscoverd(FS_PathfindingData data);
	UFUNCTION(BlueprintCallable)
	void ClearGeneratedData();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsDiagonal(FIntPoint index1, FIntPoint index2);
	UFUNCTION(BlueprintCallable)
	int CalculateTileSortingCost(FS_PathfindingData data);
	UFUNCTION(BlueprintCallable)
	float CalculateCostFromStart(FS_PathfindingData data1, FS_PathfindingData data2);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetTileCostOfType(E_TileType type);

	FIntPoint startIndex, targetIndex;
	UPROPERTY(BlueprintReadOnly)
	bool useDiagonals;
	UPROPERTY(BlueprintReadWrite)
	TArray<TEnumAsByte<E_TileType>> validTileTypes;
	UPROPERTY(BlueprintReadWrite)
	float delayBetweenIterations;
	UPROPERTY(BlueprintReadOnly)
	TArray<FIntPoint> discoverdIndexes;
	UPROPERTY(BlueprintReadOnly)
	TArray<FIntPoint> analyzedIndexes;
	UPROPERTY(BlueprintReadOnly)
	FS_PathfindingData currentAnalyzedTile;
	UPROPERTY(BlueprintReadOnly)
	TArray<FS_PathfindingData> currentNeighbours;
	UPROPERTY(BlueprintReadOnly)
	TArray<float> discoverTileCost;
	UPROPERTY(BlueprintReadOnly)
	FS_PathfindingData currentNeighbour;
	UPROPERTY(BlueprintReadOnly)
	TMap<FIntPoint, FS_PathfindingData> pathfindingData;

	FTimerHandle GeneratePathTimerHandle;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPathfindingUpdate, FIntPoint, index);
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnPathfindingUpdate OnPathfindingUpdated;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPathFound, const TArray<FIntPoint>&, path);
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnPathFound OnPathFound;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPathfindingDataCleared);
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnPathfindingDataCleared OnPathfindingCleared;



public:	
	AGrid* grid;

};
