#include "DirtBike.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/StaticMesh.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "../OurBlock.h"

ADirtBike::ADirtBike()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// Long and narrow, roughly bike-proportioned - a placeholder shape standing in for
	// the bike the same way ThreatActor's sphere stands in for a gunman (Encounter/
	// README's whole gray-box vocabulary). A real mesh replaces this without touching
	// anything else here.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMeshFinder.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMeshFinder.Object);
	}
	Mesh->SetWorldScale3D(FVector(2.2f, 0.6f, 0.7f));
	Mesh->SetCollisionProfileName(TEXT("Pawn"));

	// Z=35 marks the bike mesh's own top surface (the default engine cube is 100 units
	// per side, and the 0.7 Z scale above halves to +/-35 locally) - this is where a
	// rider's feet belong, not an arbitrary height. AGrayBoxCharacter::AttachToBikeSeat
	// adds its own capsule half-height on top of whatever seat position it attaches
	// to, so this only needs to mark the surface, not account for capsule size itself.
	DriverSeat = CreateDefaultSubobject<USceneComponent>(TEXT("DriverSeat"));
	DriverSeat->SetupAttachment(Mesh);
	DriverSeat->SetRelativeLocation(FVector(40.f, 0.f, 35.f));

	PassengerSeat = CreateDefaultSubobject<USceneComponent>(TEXT("PassengerSeat"));
	PassengerSeat->SetupAttachment(Mesh);
	PassengerSeat->SetRelativeLocation(FVector(-40.f, 0.f, 35.f));

	// The driver's camera looks forward along the bike, fixed - ADR 0004 gives the
	// rider the approach and the escape, not free aim, which is what actually
	// differentiates this seat from the passenger's (AGrayBoxCharacter's own free-look
	// camera). Attached to the bike body, not the driver seat socket, so it doesn't
	// inherit any lean/wobble a real vehicle mesh might add later at the seat point.
	//
	// -450 clears the bike's own half-length (110, from the 220-unit scaled body) by a
	// real margin - the first version placed this at -150, only 40 units clear of the
	// body, which in Unreal's units is barely more than one capsule-width back and
	// filled most of the frame with the bike itself. Confirmed by screenshot, not
	// assumption - a small number here reads as "far enough" only if you forget how
	// large 220 units actually is next to it.
	DriverCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DriverCamera"));
	DriverCamera->SetupAttachment(Mesh);
	DriverCamera->SetRelativeLocation(FVector(-450.f, 0.f, 250.f));
	DriverCamera->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));

	// Input built in the constructor via CreateDefaultSubobject, not BeginPlay via
	// NewObject - see AGrayBoxCharacter for the two lessons this depends on:
	// SetupPlayerInputComponent runs before BeginPlay for a GameMode-spawned pawn, and
	// NewObject can't construct a UObject meant to persist from inside another
	// UObject's own constructor.
	MappingContext = CreateDefaultSubobject<UInputMappingContext>(TEXT("MappingContext"));
	AccelerateAction = MakeBoolAction(TEXT("AccelerateAction"));
	BrakeAction = MakeBoolAction(TEXT("BrakeAction"));
	TurnLeftAction = MakeBoolAction(TEXT("TurnLeftAction"));
	TurnRightAction = MakeBoolAction(TEXT("TurnRightAction"));

	MappingContext->MapKey(AccelerateAction, EKeys::W);
	MappingContext->MapKey(BrakeAction, EKeys::S);
	MappingContext->MapKey(TurnLeftAction, EKeys::A);
	MappingContext->MapKey(TurnRightAction, EKeys::D);
}

UInputAction* ADirtBike::MakeBoolAction(FName SubobjectName)
{
	UInputAction* Action = CreateDefaultSubobject<UInputAction>(SubobjectName);
	Action->ValueType = EInputActionValueType::Boolean;
	return Action;
}

USceneComponent* ADirtBike::GetSeatComponent(EBikeSeat Seat) const
{
	return Seat == EBikeSeat::Driver ? Cast<USceneComponent>(DriverSeat) : Cast<USceneComponent>(PassengerSeat);
}

void ADirtBike::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	UE_LOG(LogOurBlock, Log, TEXT("DirtBike PossessedBy: %s"), *GetNameSafe(NewController));
	ApplyMappingContextIfReady();
}

void ADirtBike::ApplyMappingContextIfReady()
{
	if (!MappingContext)
	{
		return;
	}
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}
	if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(MappingContext, 0);
			UE_LOG(LogOurBlock, Log, TEXT("DirtBike: mapping context added"));
		}
	}
	PC->SetInputMode(FInputModeGameOnly());
	PC->bShowMouseCursor = false;
}

void ADirtBike::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(AccelerateAction, ETriggerEvent::Started, this, &ADirtBike::OnAccelerateStarted);
		EIC->BindAction(AccelerateAction, ETriggerEvent::Completed, this, &ADirtBike::OnAccelerateStopped);
		EIC->BindAction(BrakeAction, ETriggerEvent::Started, this, &ADirtBike::OnBrakeStarted);
		EIC->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ADirtBike::OnBrakeStopped);
		EIC->BindAction(TurnLeftAction, ETriggerEvent::Started, this, &ADirtBike::OnTurnLeftStarted);
		EIC->BindAction(TurnLeftAction, ETriggerEvent::Completed, this, &ADirtBike::OnTurnLeftStopped);
		EIC->BindAction(TurnRightAction, ETriggerEvent::Started, this, &ADirtBike::OnTurnRightStarted);
		EIC->BindAction(TurnRightAction, ETriggerEvent::Completed, this, &ADirtBike::OnTurnRightStopped);
	}
}

void ADirtBike::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bAccelerateHeld)
	{
		CurrentSpeed = FMath::Min(CurrentSpeed + Acceleration * DeltaSeconds, MaxSpeed);
	}
	else if (bBrakeHeld)
	{
		CurrentSpeed = FMath::Max(CurrentSpeed - Acceleration * DeltaSeconds, -ReverseMaxSpeed);
	}
	else if (CurrentSpeed > 0.f)
	{
		CurrentSpeed = FMath::Max(CurrentSpeed - Drag * DeltaSeconds, 0.f);
	}
	else if (CurrentSpeed < 0.f)
	{
		CurrentSpeed = FMath::Min(CurrentSpeed + Drag * DeltaSeconds, 0.f);
	}

	// Turning only means anything while moving - a bike standing still doesn't pivot
	// in place, and letting it would make the movement read as a tank, not a bike.
	if (!FMath::IsNearlyZero(CurrentSpeed))
	{
		const float TurnDirection = CurrentSpeed > 0.f ? 1.f : -1.f;
		if (bTurnLeftHeld)
		{
			AddActorLocalRotation(FRotator(0.f, -TurnRateDegreesPerSecond * DeltaSeconds * TurnDirection, 0.f));
		}
		if (bTurnRightHeld)
		{
			AddActorLocalRotation(FRotator(0.f, TurnRateDegreesPerSecond * DeltaSeconds * TurnDirection, 0.f));
		}
	}

	FHitResult Hit;
	AddActorWorldOffset(GetActorForwardVector() * CurrentSpeed * DeltaSeconds, true, &Hit);
	if (Hit.bBlockingHit)
	{
		CurrentSpeed = 0.f;
	}
}
