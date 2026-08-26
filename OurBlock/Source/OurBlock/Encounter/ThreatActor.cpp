#include "ThreatActor.h"
#include "DangerTallyComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "../OurBlock.h"

AThreatActor::AThreatActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshFinder(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMeshFinder.Succeeded())
	{
		Mesh->SetStaticMesh(SphereMeshFinder.Object);
	}
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

void AThreatActor::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(FireTimer, this, &AThreatActor::Fire, TimeToFire, false);
}

void AThreatActor::Deny()
{
	if (bFired)
	{
		return;
	}
	bDenied = true;

	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(FireTimer);
	}

	UE_LOG(LogOurBlock, Log, TEXT("%s denied"), *GetName());

	if (GetWorld())
	{
		Destroy();
	}
}

void AThreatActor::Fire()
{
	if (bDenied || bFired)
	{
		return;
	}
	bFired = true;

	if (Target)
	{
		Target->AddDanger(DangerOnFire);
	}

	UE_LOG(LogOurBlock, Log, TEXT("%s fired, danger +%d"), *GetName(), DangerOnFire);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("%s fired"), *GetName()));
	}

	if (GetWorld())
	{
		Destroy();
	}
}
