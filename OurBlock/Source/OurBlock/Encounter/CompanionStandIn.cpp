#include "CompanionStandIn.h"
#include "DangerTallyComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

ACompanionStandIn::ACompanionStandIn()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMeshFinder.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMeshFinder.Object);
	}
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	Tally = CreateDefaultSubobject<UDangerTallyComponent>(TEXT("Tally"));
}
