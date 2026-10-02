#include "CompanionStandIn.h"
#include "DangerTallyComponent.h"
#include "DirtBike.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

// The default engine cube (Mesh's static mesh below) is 100 units per side, unscaled
// by this actor - half-height 50. Matches the reasoning in DirtBike.cpp's own seat
// comment for the same shape.
static constexpr float CompanionStandInHalfHeight = 50.0f;

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

void ACompanionStandIn::AttachToBikeSeat(ADirtBike* Bike)
{
	if (!Bike)
	{
		return;
	}

	USceneComponent* Seat = Bike->GetSeatComponent(EBikeSeat::Passenger);
	if (!Seat)
	{
		return;
	}

	AttachToComponent(Seat, FAttachmentTransformRules::SnapToTargetIncludingScale);
	SetActorRelativeLocation(FVector(0.f, 0.f, CompanionStandInHalfHeight));
	SetActorRelativeRotation(FRotator::ZeroRotator);
}
