// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMeshInstance.h"
#include "GridVisual.h"

// Sets default values
AGridMeshInstance::AGridMeshInstance()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	instanceStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("MeshInstance"));
	RootComponent = instanceStaticMesh;

	if (IsValid(gMesh) && IsValid(gMaterial)) 
	{
		instanceStaticMesh->SetStaticMesh(gMesh);
		instanceStaticMesh->NumCustomDataFloats = 4;
		instanceStaticMesh->SetMaterial(0, gMaterial);
		instanceStaticMesh->SetVectorParameterValueOnMaterials("Color", FVector(FLinearColor::Black));
		instanceStaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	/*static ConstructorHelpers::FObjectFinder<UStaticMesh> mesh(TEXT("StaticMesh'/Game/RTS_Test/Materials/Grid/NewFolder/SM_Grid_SquareFlat.SM_Grid_SquareFlat'"));
	UStaticMesh* gridMesh = nullptr;
	if (mesh.Object != NULL)
	{
		gridMesh = (UStaticMesh*)mesh.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> material(TEXT("Material'/Game/RTS_Test/Materials/Grid/NewFolder/M_Grid_Flat.M_Grid_Flat'"));
	UMaterial* gridMaterial = nullptr;
	if (material.Object != NULL)
	{
		gridMaterial = (UMaterial*)material.Object;
	}

	if (IsValid(gridMesh) && IsValid(gridMaterial)) {
		instanceStaticMesh->SetStaticMesh(gridMesh);
		instanceStaticMesh->NumCustomDataFloats = 4;
		//instanceStaticMesh->SetMaterial(0, gridMaterial);
		//instanceStaticMesh->SetVectorParameterValueOnMaterials("Color", FVector(FLinearColor::Black));
		instanceStaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}*/

}


void AGridMeshInstance::AddInstance(FIntPoint index, FTransform instanceTransform, TArray<E_TileState> tileStates, int tileType)
{
	E_TileType type = BitCast<E_TileType>(tileType);
	RemoveInstance(index);
	instanceStaticMesh->AddInstance(instanceTransform, true);
	int i = staticMeshIndexes.Add(index);
	FLinearColor color;
	float filled = GetColorFromState(tileStates, type, color);
	instanceStaticMesh->SetCustomDataValue(i, 0, color.R);
	instanceStaticMesh->SetCustomDataValue(i, 1, color.G);
	instanceStaticMesh->SetCustomDataValue(i, 2, color.B);
	instanceStaticMesh->SetCustomDataValue(i, 3, filled);
}

void AGridMeshInstance::RemoveInstance(FIntPoint index)
{
	if (staticMeshIndexes.Contains(index)) {
		int i = staticMeshIndexes.Find(index);
		instanceStaticMesh->RemoveInstance(i);
		staticMeshIndexes.Remove(index);
	}
}

void AGridMeshInstance::ClearInstaces()
{
	instanceStaticMesh->ClearInstances();
	staticMeshIndexes.Empty();
}

void AGridMeshInstance::InitializeMeshInstances(UStaticMesh* mesh, UMaterial* material, FLinearColor color, ECollisionEnabled::Type collision, bool useTypeColor)
{
	instanceStaticMesh->SetStaticMesh(mesh);
	instanceStaticMesh->NumCustomDataFloats = 4;
	instanceStaticMesh->SetMaterial(0, material);
	//instanceStaticMesh->SetVectorParameterValueOnMaterials("Color", FVector(color));
	instanceStaticMesh->SetCollisionEnabled(collision);
	useTileTypeColor = useTypeColor;

}

void AGridMeshInstance::SetFilled(FIntPoint index, bool filled)
{
}

float AGridMeshInstance::GetColorFromState(TArray<E_TileState> states, int tileType, FLinearColor& color)
{
	E_TileType type = BitCast<E_TileType>(tileType);
	color = FLinearColor::Black;
	if (useTileTypeColor) 
	{
		switch (type)
		{
		case EMPTY:
			color = emptyColor;
			break;
		case NORMAL:
			color = normalColor;
			break;
		case OBSTACLE:
			color = obstacleColor;
			break;
		case MUD:
			color = mudColor;
			break;
		case AIR:
			color = airColor;
			break;
		case TRIPLE:
			color = tripleColor;
			break;
		default:
			color = FLinearColor::Black;
			break;
		}
		return 1.0f;
	}
	else 
	{
		if (states.Num() != 0) 
		{
			TArray<E_TileState> newArray = { E_TileState::SELECTED, E_TileState::HOVERED, E_TileState::NEIGHBOUR, E_TileState::PATH, E_TileState::ANALYZED, E_TileState::DISCOVERD };
			for (E_TileState state : newArray)
			{
				if (states.Contains(state)) 
				{
					switch (state) 
					{
					case E_TileState::HOVERED:
						color = hoveredColor;
						break;

					case E_TileState::SELECTED:
						color = selectedColor;
						break;

					case E_TileState::NEIGHBOUR:
						color = neighbourColor;
						break;

					case E_TileState::PATH:
						color = pathColor;
						break;

					case E_TileState::ANALYZED:
						color = analyzedColor;
						break;

					case E_TileState::DISCOVERD:
						color = discoverdColor;
						break;
					default:
						color = FLinearColor::Black;
						break;
					}
					return 1.0f;

				}
			}
		}
	}
	return 0.0f;
}
