// Fill out your copyright notice in the Description page of Project Settings.


#include "GridModifier.h"

// Sets default values
AGridModifier::AGridModifier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UMaterial> material(TEXT("Material'/Game/RTS_Test/Materials/Grid/NewFolder/M_Grid_Flat.M_Grid_Flat'"));
	UMaterial* gridMaterial = nullptr;
	if (material.Object != NULL)
	{
		gridMaterial = (UMaterial*)material.Object;
	}
	StaticMesh->SetMaterial(0, gridMaterial);
	StaticMesh->PostEditChange();
	/*FLinearColor color;
	switch (type)
	{
	case NONE:
		color = FLinearColor::Black;
		break;
	case NORMAL:
		color = FLinearColor::White;
		break;
	case OBSTACLE:
		color = FLinearColor::Red;
		break;
	default:
		break;
	}
	if (IsValid(StaticMesh)) 
	{
		StaticMesh->SetVectorParameterValueOnMaterials("Color", FVector(color));
		StaticMesh->SetScalarParameterValueOnMaterials("IsFilled", 1.0f);
		StaticMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
		StaticMesh->bHiddenInGame = true;
	}*/

}


