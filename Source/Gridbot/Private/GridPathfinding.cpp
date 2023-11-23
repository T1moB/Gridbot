// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPathfinding.h"
#include "Grid.h"
#include "Math/UnrealMathUtility.h"
#include "Algo/Reverse.h"

// Sets default values
AGridPathfinding::AGridPathfinding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGridPathfinding::BeginPlay()
{
	//Super::BeginPlay();
	
}

TArray<FIntPoint> AGridPathfinding::FindPath(FIntPoint start, FIntPoint target, bool diagonals, TArray<TEnumAsByte<E_TileType>> validTypes, float delay)
{
	startIndex = start;
	targetIndex = target;
	useDiagonals = diagonals;
	validTileTypes = validTypes;
	delayBetweenIterations = delay;
	if (GeneratePathTimerHandle.IsValid()) { GetWorldTimerManager().ClearTimer(GeneratePathTimerHandle); }
	ClearGeneratedData();
	if (IsInputValid()) 
	{
		DiscoverTile(FS_PathfindingData(startIndex, 1.0, 0, GetMinimumCostBetweenTiles(startIndex, targetIndex, useDiagonals), FIntPoint()));
		if (delayBetweenIterations <= 0) 
		{
			while (discoverdIndexes.Num() > 0)
			{
				if (AnalyzeNextDiscoverTile())
				{
					TArray<FIntPoint> path = GeneratePath();
					OnPathFound.Broadcast(path);
					return path;
				}
			}
		}
		else 
		{
			FindPathWithDelay_Implementation();
			return TArray<FIntPoint>();
		}
	}
	OnPathFound.Broadcast(TArray<FIntPoint>());
	return TArray<FIntPoint>();
}

void AGridPathfinding::FindPathWithDelay_Implementation() 
{
	if (discoverdIndexes.Num() > 0)
	{
		if (AnalyzeNextDiscoverTile())
		{
			TArray<FIntPoint> path = GeneratePath();
			OnPathFound.Broadcast(path);
		}
		else 
		{
			GetWorldTimerManager().SetTimer(GeneratePathTimerHandle, this, &AGridPathfinding::FindPathWithDelay_Implementation, delayBetweenIterations, false);
		}
	}
	OnPathFound.Broadcast(TArray<FIntPoint>());
}

bool AGridPathfinding::IsInputValid()
{
	if (grid->isIndexValid(startIndex) && grid->isIndexValid(targetIndex)) 
	{
		bool startIsValid = validTileTypes.Contains(grid->gridTiles.Find(startIndex)->type);
		bool targetIsValid = validTileTypes.Contains(grid->gridTiles.Find(targetIndex)->type);
		return startIndex != targetIndex && startIsValid && targetIsValid;
	}
	return false;
}

void AGridPathfinding::DiscoverTile(FS_PathfindingData data)
{
	pathfindingData.Add(data.index, data);
	InsertTileInDiscoverd(data);
	OnPathfindingUpdated.Broadcast(data.index);
}

TArray<FS_PathfindingData> AGridPathfinding::GetValidTileNeighbours(FIntPoint index, bool includeDiagonals, TArray<TEnumAsByte<E_TileType>> validTypes)
{
	TArray<FS_PathfindingData> neighbours;
	if (grid->isIndexValid(index)) 
	{
		FS_TileData inputData = *grid->gridTiles.Find(index);
		for (FIntPoint neighbour : GetTileNeighbours(index, includeDiagonals))
		{
			if (grid->isIndexValid(neighbour))
			{
				FS_TileData data = *grid->gridTiles.Find(neighbour);
				if (validTypes.Contains(data.type))
				{
					if (FMath::Abs(inputData.transform.GetLocation().Z - data.transform.GetLocation().Z) <= grid->tileSize.Z)
					{
						neighbours.Add(FS_PathfindingData(data.index, GetTileCostOfType(data.type), -9999, -9999, index));
					}
				}
			}
		}
	}
	return neighbours;
}

TArray<FIntPoint> AGridPathfinding::GetTileNeighbours(FIntPoint index, bool includeDiagonals)
{
	TArray<FIntPoint> neighbours;
	neighbours.Add(index + FIntPoint(1, 0));
	neighbours.Add(index + FIntPoint(0, 1));
	neighbours.Add(index + FIntPoint(-1, 0));
	neighbours.Add(index + FIntPoint(0, -1));
	if (includeDiagonals) {
		neighbours.Add(index + FIntPoint(1, 1));
		neighbours.Add(index + FIntPoint(-1, 1));
		neighbours.Add(index + FIntPoint(1, -1));
		neighbours.Add(index + FIntPoint(-1, -1));
	}
	return neighbours;
}

float AGridPathfinding::GetMinimumCostBetweenTiles(FIntPoint index1, FIntPoint index2, bool diagonals)
{
	if (diagonals) {
		FIntPoint costs = index1 - index2;
		return FMath::Max(FMath::Abs(costs.X), FMath::Abs(costs.Y));
	}
	else {
		FIntPoint costs = index1 - index2;
		costs = FIntPoint(FMath::Abs(costs.X), FMath::Abs(costs.Y));
		return (costs.X + costs.Y);
	}
}

bool AGridPathfinding::AnalyzeNextDiscoverTile()
{
	currentAnalyzedTile = PullCheapestFromDiscoverd();
	OnPathfindingUpdated.Broadcast(currentAnalyzedTile.index);
	//grid->AddStateToTile(currentAnalyzedTile.index, ANALYZED);
	currentNeighbours = GetValidTileNeighbours(currentAnalyzedTile.index, useDiagonals, validTileTypes);
	while (currentNeighbours.Num() > 0) {
		if (DiscoverNextNeighbour()) {
			return true;
		}
	}
	return false;
}

TArray<FIntPoint> AGridPathfinding::GeneratePath()
{
	FIntPoint current = targetIndex;
	TArray<FIntPoint> invertedPath;
	while (current != startIndex) 
	{
		invertedPath.Add(current);
		current = pathfindingData.Find(current)->previousIndex;
	}
	Algo::Reverse(invertedPath);
	return invertedPath;
}

FS_PathfindingData AGridPathfinding::PullCheapestFromDiscoverd()
{
	FIntPoint tileIndex = discoverdIndexes[0];
	discoverTileCost.RemoveAt(0);
	discoverdIndexes.RemoveAt(0);
	analyzedIndexes.Add(tileIndex);
	return pathfindingData[tileIndex];
}

bool AGridPathfinding::DiscoverNextNeighbour()
{
	//currentNeighbour = currentNeighbours.Pop(true);
	currentNeighbour = currentNeighbours[0];
	currentNeighbours.RemoveAt(0);
	if (!analyzedIndexes.Contains(currentNeighbour.index)) 
	{
		float costFromStart = currentAnalyzedTile.costFromStart + currentNeighbour.costToEnterTile;//CalculateCostFromStart(currentAnalyzedTile, currentNeighbour);
		if (discoverdIndexes.Contains(currentNeighbour.index)) //Already checked neighbour
		{
			int indexInDiscoverd = discoverdIndexes.Find(currentNeighbour.index);
			currentNeighbour = *pathfindingData.Find(currentNeighbour.index);
			if (costFromStart < currentNeighbour.costFromStart) 
			{
				discoverdIndexes.RemoveAt(indexInDiscoverd);
				discoverTileCost.RemoveAt(indexInDiscoverd);
			}
			else
			{
				return false;
			}
		}
		float minimumCost = GetMinimumCostBetweenTiles(currentNeighbour.index, targetIndex, useDiagonals);
		DiscoverTile(FS_PathfindingData(currentNeighbour.index, currentNeighbour.costToEnterTile, costFromStart, minimumCost, currentAnalyzedTile.index));
		return (currentNeighbour.index == targetIndex);
	}
	return false;
}

void AGridPathfinding::InsertTileInDiscoverd(FS_PathfindingData data)
{
	float sortingCost = data.costFromStart + data.minimumCostToTarget;
	sortingCost = CalculateTileSortingCost(data); //if I want to switch to int's instead of floats
	//grid->AddStateToTile(data.index, DISCOVERD);
	if (discoverTileCost.Num() == 0 || sortingCost >= discoverTileCost[discoverTileCost.Num()-1])
	{
		discoverTileCost.Add(sortingCost);
		discoverdIndexes.Add(data.index);
	}
	else 
	{
		for (int i = 0; i < discoverTileCost.Num(); i++)
		{
			if (discoverTileCost[i] >= sortingCost) 
			{
				discoverTileCost.Insert(sortingCost, i);
				discoverdIndexes.Insert(data.index, i);
				break;
			}
		}
	}
}

void AGridPathfinding::ClearGeneratedData()
{
	TArray<FIntPoint> keys; 
	pathfindingData.GetKeys(keys);
	for (FIntPoint index : keys) {
		grid->RemoveStateFromTile(index, DISCOVERD);
		grid->RemoveStateFromTile(index, ANALYZED);
	}
	pathfindingData.Empty();
	discoverTileCost.Empty();
	analyzedIndexes.Empty();
	discoverdIndexes.Empty();
	OnPathfindingCleared.Broadcast();
}

bool AGridPathfinding::IsDiagonal(FIntPoint index1, FIntPoint index2)
{
	return !(GetTileNeighbours(index1, false).Contains(index2));
}
/// <summary>
/// If I want to use ints instead of floats for the costs
/// </summary>
/// <param name="data"></param>
/// <returns></returns>
int AGridPathfinding::CalculateTileSortingCost(FS_PathfindingData data)
{
	return ((data.costFromStart + data.minimumCostToTarget) * 2) + IsDiagonal(data.index, data.previousIndex);
}

float AGridPathfinding::CalculateCostFromStart(FS_PathfindingData data1, FS_PathfindingData data2)
{
	return data1.costFromStart + (data2.costToEnterTile + (IsDiagonal(data1.index, data2.index) ? 0.5f : 0.0f));
}

int AGridPathfinding::GetTileCostOfType(E_TileType type)
{
	switch (type)
	{
	case EMPTY:
		return 0;
	case NORMAL:
		return 1;
	case OBSTACLE:
		return 0;
	case MUD:
		return 2;
	case AIR:
		return 1;
	case TRIPLE:
		return 3;
	default:
		return 0;
	}
}

