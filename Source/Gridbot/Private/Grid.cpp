// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"
#include "GridModifier.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGrid::AGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	GridVisual_ChildActor = CreateDefaultSubobject<UChildActorComponent>("GridVisual_ChildActor");
	GridVisual_ChildActor->SetupAttachment(RootSceneComponent);
	GridVisual_ChildActor->SetChildActorClass(TSubclassOf<AGridVisual>(AGridVisual::StaticClass()));

	gridVisual = Cast<AGridVisual>(GridVisual_ChildActor->GetChildActor());
	if (IsValid(gridVisual)) {
		SpawnGrid(GetActorLocation(), tileSize, gridSize, true);
	}

	GridPathfinding_ChildActor = CreateDefaultSubobject<UChildActorComponent>("GridPathfinding_ChildActor");
	GridPathfinding_ChildActor->SetupAttachment(RootSceneComponent);
	GridPathfinding_ChildActor->SetChildActorClass(TSubclassOf<AGridVisual>(AGridPathfinding::StaticClass()));

	gridPathfinding = Cast<AGridPathfinding>(GridPathfinding_ChildActor->GetChildActor());
	if (IsValid(gridPathfinding)) {
		gridPathfinding->grid = this;
	}

	//SpawnGrid(GetActorLocation(), tileSize, gridSize, true);

}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	//Super::BeginPlay();
	gridPathfinding = Cast<AGridPathfinding>(GridPathfinding_ChildActor->GetChildActor());
	if (IsValid(gridPathfinding)) {
		gridPathfinding->grid = this;
	}
	SpawnGrid(GetActorLocation(), tileSize, gridSize, useEnvironment);
	
}

// Called every frame
void AGrid::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);

}

void AGrid::SpawnGrid(FVector location, FVector newtileSize, FIntPoint newgridSize, bool environment)
{
	spawnLocation = location;
	tileSize = newtileSize;
	gridSize = newgridSize;
	useEnvironment = environment;
	gridVisual = Cast<AGridVisual>(GridVisual_ChildActor->GetChildActor());
	if (!gridVisual) return;
	DestroyGrid();
	gridVisual->InitializeGridVisual(this);
	gridBottomLeft = CalcCenterAndBottomLeft(spawnLocation);

	
	for (int i = 0; i < gridSize.X; i++) {
		for (int j = 0; j < gridSize.Y; j++) {
			FIntPoint index = FIntPoint(i,j);
			FTransform tileTransform;
			tileTransform.SetLocation(GetTileLocationFromGrid(index));
			if (useEnvironment) {
				E_TileType tileType;
				FTransform newTileTransform = TraceForGround(tileTransform.GetLocation(), tileType);
				AddGridTile(FS_TileData(index, tileType, newTileTransform));
			}
			else {
				AddGridTile(FS_TileData(index, E_TileType::NORMAL, tileTransform));
			}
		}

	}
}

FVector AGrid::CalcCenterAndBottomLeft(FVector& center)
{
	FVector bottomLeft;
	FVector offset = tileSize /2;
	/*if (gridSize.X % 2 == 0) {
		offset.X = tileSize.X / 2;
	}
	if (gridSize.Y % 2 == 0) {
		offset.Y = tileSize.Y / 2;
	}*/

	spawnLocation = spawnLocation.GridSnap(tileSize.X) + offset;
	bottomLeft = spawnLocation - (tileSize * FVector(gridSize / 2));
	return bottomLeft;

}

FTransform AGrid::TraceForGround(FVector position, E_TileType& hitType)
{
	float radius = tileSize.X / 3;

	FCollisionShape MySphere = FCollisionShape::MakeSphere(radius); // 5M Radius
	TArray<FHitResult> OutResults;
	GetWorld()->SweepMultiByChannel(OutResults, position + FVector(0,0,300), position - FVector(0, 0, 100), FQuat::Identity, ECC_GameTraceChannel1, MySphere);
	//DrawDebugCapsule(GetWorld(), position + FVector(0, 0, 100), 200, radius, FQuat::Identity, FColor::Red);
	bool isHeightFound = false;
	if (OutResults.Num() > 0) {

		float returnZ;
		FRotator rotation;
		hitType = E_TileType::NORMAL;
		for (FHitResult result : OutResults)
		{
			if (Cast<AGridModifier>(result.GetActor())) {
				AGridModifier* modifer = Cast<AGridModifier>(result.GetActor());
				hitType = modifer->type;
				if(modifer->useForTileHeight)
				{
					isHeightFound = true;
					returnZ = result.Location.Z - radius;
					rotation = FRotationMatrix::MakeFromZ(result.ImpactNormal).Rotator();
				}
			}
			else 
			{
				if (!isHeightFound) 
				{
					returnZ = result.Location.Z - radius;
					rotation = FRotationMatrix::MakeFromZ(result.ImpactNormal).Rotator();
				}
			}
		}
		return FTransform(rotation, FVector(position.X, position.Y, returnZ + 0.1f), FVector(1, 1, 1));
	}
	else 
	{
		hitType = E_TileType::EMPTY;
		return FTransform(position);
	}
}

TEnumAsByte<E_TileType> AGrid::TraceForGround(FVector position, FTransform& transform)
{
	float radius = tileSize.X / 3;
	E_TileType type = EMPTY;

	FCollisionShape MySphere = FCollisionShape::MakeSphere(radius); // 5M Radius
	TArray<FHitResult> OutResults;
	GetWorld()->SweepMultiByChannel(OutResults, position + FVector(0, 0, 300), position - FVector(0, 0, 100), FQuat::Identity, ECC_GameTraceChannel1, MySphere);
	//DrawDebugCapsule(GetWorld(), position + FVector(0, 0, 100), 200, radius, FQuat::Identity, FColor::Red);
	bool isHeightFound = false;
	if (OutResults.Num() > 0) {

		float returnZ;
		FRotator rotation;
		transform = FTransform::Identity;
		for (FHitResult result : OutResults)
		{
			if (Cast<AGridModifier>(result.GetActor())) {

				AGridModifier* modifer = Cast<AGridModifier>(result.GetActor());
				type = modifer->type;
				if (modifer->useForTileHeight)
				{
					isHeightFound = true;
					returnZ = result.Location.Z - radius;
					rotation = FRotationMatrix::MakeFromZ(result.ImpactNormal).Rotator();
				}
			}
			else 
			{
				if (!isHeightFound)
				{
					returnZ = result.Location.Z - radius;
					rotation = FRotationMatrix::MakeFromZ(result.ImpactNormal).Rotator();
				}
			}
		}
		transform = FTransform(rotation, FVector(position.X, position.Y, returnZ + 0.1f), FVector(1, 1, 1));
		return type;
	}
	else {
		transform = FTransform(position);
		return E_TileType::EMPTY;
	}
}

void AGrid::DestroyGrid()
{
	gridTiles.Empty();
	gridVisual->DestroyGridVisual();
	OnGridDestroyed.Broadcast();
}

void AGrid::AddGridTile(FS_TileData data)
{
	gridTiles.Add(data.index, data);
	gridVisual->UpdateTileVisual(data);
	OnTileDataUpdated.Broadcast(data.index);
}

void AGrid::RemoveGridTile(FIntPoint index)
{
	if (gridTiles.Remove(index)) {
		gridVisual->UpdateTileVisual(FS_TileData(index, EMPTY, FTransform::Identity));
		OnTileDataUpdated.Broadcast(index);

	}
}

void AGrid::AddStateToTile(FIntPoint index, E_TileState state)
{
	if (isIndexValid(index)) {
		FS_TileData data = *gridTiles.Find(index);
		data.states.AddUnique(state);
		if (data.states.Num() >= 0) {
			gridTiles.Add(data.index, data);
			TArray<FIntPoint> indexes = GetAllTilesOfState(state);
			indexes.Add(index);
			tileStatesToIndexes.Add(state, indexes);
			gridVisual->UpdateTileVisual(data);
			OnTileDataUpdated.Broadcast(index);
		}
	}
}

void AGrid::RemoveStateFromTile(FIntPoint index, E_TileState state)
{

	if (isIndexValid(index)) {
		FS_TileData data = *gridTiles.Find(index);
		if (data.states.Remove(state)) {
			gridTiles.Add(data.index, data);
			TArray<FIntPoint> indexes = GetAllTilesOfState(state);
			indexes.Remove(index);
			tileStatesToIndexes.Add(state, indexes);
			gridVisual->UpdateTileVisual(data);
			OnTileDataUpdated.Broadcast(index);
		}
	}
}

void AGrid::ClearStateFromTiles(E_TileState state)
{
	TArray<FIntPoint> tiles = GetAllTilesOfState(state);
	for (FIntPoint tile : tiles) {
		RemoveStateFromTile(tile, state);
	}
}

bool AGrid::isIndexValid(FIntPoint index)
{
	return gridTiles.Contains(index);
}

FVector AGrid::GetTileLocationFromGrid(FIntPoint index)
{
	return gridBottomLeft + (tileSize * FVector(index.X, index.Y, 0.1f));
}

FVector AGrid::GetCursorLocationOnGrid(int playerIndex)
{
	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), playerIndex);

	FHitResult OutResult;
	if (controller->GetHitResultUnderCursor(ECC_GameTraceChannel2, false, OutResult)) {
		return OutResult.Location;
	}
	else {
		FVector location, direction;
		controller->DeprojectMousePositionToWorld(location, direction);
		float t;
		FVector intersection;
		if (UKismetMathLibrary::LinePlaneIntersection(location, location + (direction * 99999), UKismetMathLibrary::MakePlaneFromPointAndNormal(spawnLocation, (FVector(0, 0, 1))), t, intersection)) {
			return intersection;
		}
		else 
		{
			return FVector(-9999, -9999, -9999);
		}
	}
}

FIntPoint AGrid::GetTileIndexFromWorldLocation(FVector location)
{
	FVector locationOnGrid = location - gridBottomLeft;
	FVector2D snappedToGrid = FVector2D(locationOnGrid.GridSnap(tileSize.X));
	return FIntPoint(snappedToGrid.X / tileSize.X, snappedToGrid.Y / tileSize.Y);
}

FIntPoint AGrid::GetTileIndexUnderCursor(int playerIndex)
{
	FVector cursorOnGrid = GetCursorLocationOnGrid(playerIndex);
	FIntPoint indexOnGrid = GetTileIndexFromWorldLocation(cursorOnGrid);
	return indexOnGrid;
}

TArray<FIntPoint> AGrid::GetAllTilesOfState(E_TileState state)
{
	TArray<FIntPoint> tiles;
	if (tileStatesToIndexes.Contains(state)) {
		tiles = tileStatesToIndexes.Find(state)->indexes;
	}
	return tiles;
}

bool AGrid::IsTileWalkable(FIntPoint index)
{
	if(isIndexValid(index))
		return gridVisual->isWalkable(gridTiles.Find(index)->type);
	return false;
}

