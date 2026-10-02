#include "ThreatActor.h"
#include "DangerTallyComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "../OurBlock.h"

AThreatActor::AThreatActor()
{
	// Always ticks: cheap for a gray-box's handful of threats, and it means Exposure
	// mode is just a config toggle (DenialMode, an EditAnywhere property) rather than
	// something that has to flip PrimaryActorTick.bCanEverTick at BeginPlay to match.
	PrimaryActorTick.bCanEverTick = true;

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

	// Exposure mode has no fixed fuse from spawn - it fires from sustained sightline,
	// tracked in Tick() - so starting FireTimer here would fire it regardless of
	// whether the player ever gave it an angle.
	if (DenialMode == EThreatDenial::Aimed)
	{
		GetWorldTimerManager().SetTimer(FireTimer, this, &AThreatActor::Fire, TimeToFire, false);
	}
}

void AThreatActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (DenialMode != EThreatDenial::Exposure)
	{
		return;
	}
	TickExposure(DeltaSeconds, HasSightlineToTarget());
}

bool AThreatActor::HasSightlineToTarget() const
{
	if (!Target || !Target->GetOwner() || !GetWorld())
	{
		return false;
	}

	const AActor* TargetOwner = Target->GetOwner();
	const FVector Start = GetActorLocation();
	const FVector End = TargetOwner->GetActorLocation();

	if (FVector::Dist(Start, End) > ExposureRange)
	{
		return false;
	}

	// Ignore this threat and the companion itself - what this checks is whether
	// anything else (a wall, terrain, the bike's own bulk) sits between them, not
	// whether the trace's own endpoints block it.
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(TargetOwner);

	FHitResult Hit;
	const bool bBlocked = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
	return !bBlocked;
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

void AThreatActor::TickExposure(float DeltaSeconds, bool bHasSightline)
{
	if (bDenied || bFired)
	{
		return;
	}

	if (bHasSightline)
	{
		ExposureTime += DeltaSeconds;
		if (ExposureTime >= TimeToFire)
		{
			Fire();
		}
	}
	else
	{
		// A clean break resets to zero rather than pausing - see EThreatDenial::Exposure.
		ExposureTime = 0.0f;
	}
}
