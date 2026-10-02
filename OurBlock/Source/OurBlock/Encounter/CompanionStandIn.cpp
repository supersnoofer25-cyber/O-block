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

	// Collision off once riding, the same way AGrayBoxCharacter::AttachToBikeSeat turns
	// off its capsule. Unreal does not exempt an attached actor from its parent's
	// movement sweep: this cube sits overlapping the bike's body, so with collision on,
	// ADirtBike::Tick's swept move hit it every frame, zeroed CurrentSpeed, and the bike
	// never moved or turned at all. Nothing needs this cube to block anything - the
	// threats' sightline trace already ignores the companion itself, it only cares
	// what's in between.
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachToComponent(Seat, FAttachmentTransformRules::SnapToTargetIncludingScale);
	SetActorRelativeLocation(FVector(0.f, 0.f, CompanionStandInHalfHeight));
	SetActorRelativeRotation(FRotator::ZeroRotator);
}
