// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Math/IntPoint.h"
#include "GridMeshInstance.generated.h"


UENUM(BlueprintType)
enum E_TileState {
	NONE UMETA(DisplayName = "NONE"),
	HOVERED UMETA(DisplayName = "Hovered"),
	SELECTED UMETA(DisplayName = "Selected"),
	NEIGHBOUR UMETA(DisplayName = "Neighbour"),
	PATH UMETA(DisplayName = "Path"),
	DISCOVERD UMETA(DisplayName = "Discoverd"),
	ANALYZED UMETA(DisplayName = "Analyzed")
};

UCLASS(Blueprintable)
class GRIDBOT_API AGridMeshInstance : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridMeshInstance();


protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	UInstancedStaticMeshComponent* instanceStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State Colors")
	FLinearColor selectedColor = FLinearColor(0, 1, 0.4, 1);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Colors")
	FLinearColor hoveredColor = FLinearColor(1.0, 0.48, 0.21, 1);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Colors")
	FLinearColor neighbourColor = FLinearColor(.9, 0.5, 0.0, 1);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Colors")
	FLinearColor pathColor = FLinearColor(.06, 0.10, 1, 1);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Colors")
	FLinearColor analyzedColor = FLinearColor(0.42, 0.16, 0.56, 1);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Colors")
	FLinearColor discoverdColor = FLinearColor(1.0, 0.4, 1, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type Colors")
	FLinearColor emptyColor = FLinearColor::Black;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type Colors")
	FLinearColor normalColor = FLinearColor::Gray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type Colors")
	FLinearColor obstacleColor = FLinearColor::Red;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type Colors")
	FLinearColor mudColor = FLinearColor(0.33, 0.2, 0.15, 1);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type Colors")
	FLinearColor airColor = FLinearColor(0.3, 1, 1, 1);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type Colors")
	FLinearColor tripleColor = FLinearColor(0.2, 0.07, 0.02, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	UStaticMesh* gMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	UMaterial* gMaterial;
	

public:	
	
	UFUNCTION(BlueprintCallable)
	void InitializeMeshInstances(UStaticMesh* mesh, UMaterial* material, FLinearColor color, ECollisionEnabled::Type collision, bool useTypeColor);
	void AddInstance(FIntPoint index, FTransform instanceTransform, TArray<E_TileState> tileState, int type);
	void RemoveInstance(FIntPoint index);
	void ClearInstaces();
	void SetFilled(FIntPoint index, bool filled);
	float GetColorFromState(TArray<E_TileState> states, int tileType, FLinearColor& color);

	TArray<FIntPoint> staticMeshIndexes;
	UPROPERTY(EditAnyWhere)
	bool useTileTypeColor = false;

};