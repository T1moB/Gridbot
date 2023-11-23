// Fill out your copyright notice in the Description page of Project Settings.


#include "GridVisual.h"
#include "Grid.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGridVisual::AGridVisual()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	GridMeshInstance_ChildActor = CreateDefaultSubobject<UChildActorComponent>("GridMeshInstance_ChildActor");
	GridMeshInstance_ChildActor->SetupAttachment(RootSceneComponent);
	GridMeshInstance_ChildActor->SetChildActorClass(TSubclassOf<AGridMeshInstance>(AGridMeshInstance::StaticClass()));
	//GridMeshInstance_ChildActor->CreateChildActor();
	GridMeshInstance_Tactical_ChildActor = CreateDefaultSubobject<UChildActorComponent>("GridMeshInstance_Tactical_ChildActor");
	GridMeshInstance_Tactical_ChildActor->SetupAttachment(RootSceneComponent);
	GridMeshInstance_Tactical_ChildActor->SetChildActorClass(TSubclassOf<AGridMeshInstance>(AGridMeshInstance::StaticClass()));

}


void AGridVisual::InitializeGridVisual(AGrid* newgrid)
{
	grid = newgrid;
	gridMeshInstance = Cast<AGridMeshInstance>(GridMeshInstance_ChildActor->GetChildActor());
	gridMeshInstance->InitializeMeshInstances(newgrid->gridMesh, newgrid->gridMat, FLinearColor::Black, ECollisionEnabled::QueryOnly, false);
	if (IsValid(gridMeshInstance))
		gridMeshInstance->ClearInstaces();

	gridMeshInstanceTactical = Cast<AGridMeshInstance>(GridMeshInstance_Tactical_ChildActor->GetChildActor());
	gridMeshInstanceTactical->InitializeMeshInstances(newgrid->dGridMesh, newgrid->dGridMat, FLinearColor::Black, ECollisionEnabled::QueryOnly, true);
	if(IsValid(gridMeshInstanceTactical))
		gridMeshInstanceTactical->ClearInstaces();

	lowestZ = 9999;

	SetActorLocation(FVector(0, 0, .1));
}

void AGridVisual::DestroyGridVisual()
{
	if (gridMeshInstance) {
		gridMeshInstance->ClearInstaces();
	}
	if (gridMeshInstanceTactical) {
		gridMeshInstanceTactical->ClearInstaces();
	}
	lowestZ = 9999;
}

void AGridVisual::UpdateTileVisual(FS_TileData data)
{
	gridMeshInstance->RemoveInstance(data.index);
	if (isWalkable(data.type)) {
		gridMeshInstance->AddInstance(data.index, data.transform, (TArray<E_TileState>)data.states, data.type);
	}
	UpdateTileVisualTactical(data);
}

void AGridVisual::UpdateTileVisualTactical(FS_TileData data)
{
	if (data.transform.GetLocation().Z < lowestZ) lowestZ = data.transform.GetLocation().Z;

	if (isTactical) 
	{
		gridMeshInstanceTactical->RemoveInstance(data.index);
		if (data.type != NONE) {
			float scaleZ = (data.transform.GetScale3D().Z /* - lowestZ*/) + grid->tileSize.Z;
			UE_LOG(LogTemp, Warning, TEXT("data scale: %f, grid scale: %f, makes: %f"), data.transform.GetScale3D().Z, grid->tileSize.Z, scaleZ);
			scaleZ /= grid->tileSize.Z;
			FTransform newTrans = FTransform(data.transform.GetRotation(), data.transform.GetLocation(), FVector(data.transform.GetScale3D().X, data.transform.GetScale3D().Y, scaleZ));
			gridMeshInstanceTactical->AddInstance(data.index, newTrans, (TArray<E_TileState>)data.states, data.type);
		}
	}
	else 
	{
		regenerateTacticalGrid = true;
	}
}

int AGridVisual::GetMeshIndex(FS_TileData data)
{
	return (gridMeshInstance->staticMeshIndexes.Find(data.index));
}

void AGridVisual::SetTacticalGrid(bool tactical)
{
	isTactical = tactical;
	GridMeshInstance_Tactical_ChildActor->SetHiddenInGame(!tactical);
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "NotTactical", actors);
	for (AActor* a : actors)
	{
		a->SetActorHiddenInGame(tactical);
	}
	if (isTactical && regenerateTacticalGrid) 
	{
		regenerateTacticalGrid = false;
		for (TTuple<FIntPoint,FS_TileData> data : grid->gridTiles) 
		{
			UpdateTileVisual(data.Value);
		}
	}
}

