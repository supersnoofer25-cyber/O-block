#include "GrayBoxCharacter.h"
#include "ThreatActor.h"
#include "DirtBike.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "../OurBlock.h"

AGrayBoxCharacter::AGrayBoxCharacter()
{
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight));

	// bUseControllerRotationYaw makes the capsule itself turn to face where the player
	// looks horizontally - fine, a capsule has no visible orientation to look wrong.
	// Pitch is deliberately not handled the same way (the capsule should never tilt),
	// but that means nothing was applying the controller's pitch anywhere at all:
	// AddControllerPitchInput was updating the controller's rotation, and nothing ever
	// read it back out. bUsePawnControlRotation makes the camera itself follow the
	// controller's full rotation independent of the capsule's, which is the normal way
	// to decouple "look up/down" from a body that shouldn't visually pitch.
	bUseControllerRotationYaw = true;
	Camera->bUsePawnControlRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Built here, not in BeginPlay - it turns out SetupPlayerInputComponent (called
	// during possession) runs before BeginPlay does for a GameMode-spawned pawn, the
	// opposite of what the initial version assumed. BindAction against a still-null
	// UInputAction silently binds nothing, which is exactly what made every control
	// look wired up yet do nothing: the mapping context itself was being added
	// successfully, just to actions nothing had ever bound a callback to. The
	// constructor is the one place guaranteed to run before every one of these
	// lifecycle callbacks, so building the actions here removes the ordering question
	// entirely rather than trying to guess it correctly a second time.
	//
	// Must be CreateDefaultSubobject, not NewObject: the engine hard-asserts if a
	// UObject is constructed with NewObject's auto-generated (empty) name from inside
	// another UObject's constructor, since that produces inconsistent object names
	// across instances. CreateDefaultSubobject is the sanctioned way to create any
	// UObject subobject - not just components - from a constructor.
	MappingContext = CreateDefaultSubobject<UInputMappingContext>(TEXT("MappingContext"));

	MoveForwardAction = MakeBoolAction(TEXT("MoveForwardAction"));
	MoveBackAction = MakeBoolAction(TEXT("MoveBackAction"));
	MoveLeftAction = MakeBoolAction(TEXT("MoveLeftAction"));
	MoveRightAction = MakeBoolAction(TEXT("MoveRightAction"));
	FireAction = MakeBoolAction(TEXT("FireAction"));

	LookAction = CreateDefaultSubobject<UInputAction>(TEXT("LookAction"));
	LookAction->ValueType = EInputActionValueType::Axis2D;

	MappingContext->MapKey(MoveForwardAction, EKeys::W);
	MappingContext->MapKey(MoveBackAction, EKeys::S);
	MappingContext->MapKey(MoveLeftAction, EKeys::A);
	MappingContext->MapKey(MoveRightAction, EKeys::D);
	MappingContext->MapKey(LookAction, EKeys::Mouse2D);
	MappingContext->MapKey(FireAction, EKeys::LeftMouseButton);
}

UInputAction* AGrayBoxCharacter::MakeBoolAction(FName SubobjectName)
{
	UInputAction* Action = CreateDefaultSubobject<UInputAction>(SubobjectName);
	Action->ValueType = EInputActionValueType::Boolean;
	return Action;
}

void AGrayBoxCharacter::BeginPlay()
{
	Super::BeginPlay();
	ApplyMappingContextIfReady();
}

void AGrayBoxCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	UE_LOG(LogOurBlock, Log, TEXT("PossessedBy: %s"), *GetNameSafe(NewController));

	// The camera now follows the controller's rotation (bUsePawnControlRotation, added
	// to fix pitch), not the pawn's own actor rotation - but nothing guarantees the
	// controller's rotation starts synced to the pawn's, which is what PlayerStart's
	// own facing actually set. Without this, the first frame (or more) can render
	// facing wherever the controller's rotation happened to default to, not the
	// direction the level was set up to start you looking.
	if (NewController)
	{
		NewController->SetControlRotation(GetActorRotation());
	}

	ApplyMappingContextIfReady();
}

void AGrayBoxCharacter::ApplyMappingContextIfReady()
{
	if (!MappingContext)
	{
		UE_LOG(LogOurBlock, Warning, TEXT("ApplyMappingContextIfReady: no MappingContext yet"));
		return; // BeginPlay hasn't built it yet
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		UE_LOG(LogOurBlock, Warning, TEXT("ApplyMappingContextIfReady: no PlayerController yet"));
		return; // not possessed yet
	}

	if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(MappingContext, 0);
			UE_LOG(LogOurBlock, Log, TEXT("ApplyMappingContextIfReady: mapping context added successfully"));
		}
		else
		{
			UE_LOG(LogOurBlock, Warning, TEXT("ApplyMappingContextIfReady: no EnhancedInputLocalPlayerSubsystem on LocalPlayer"));
		}
	}
	else
	{
		UE_LOG(LogOurBlock, Warning, TEXT("ApplyMappingContextIfReady: PlayerController has no LocalPlayer"));
	}
	PC->SetInputMode(FInputModeGameOnly());
	PC->bShowMouseCursor = false;
}

void AGrayBoxCharacter::AttachToBikeSeat(ADirtBike* Bike)
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

	// Movement mode has to go to None, not just have input ignored - a Walking capsule
	// still tries to resolve overlaps/floor collision against the moving bike under
	// it, which fights the attachment instead of riding along with it.
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachToComponent(Seat, FAttachmentTransformRules::SnapToTargetIncludingScale);

	// The seat marks where the passenger's feet belong, not where the capsule's own
	// origin should sit - SnapToTarget places the capsule's center (not its base)
	// exactly at the seat point, which buries roughly the bottom half of the capsule,
	// and the camera with it, inside the bike mesh unless corrected here.
	const float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SetActorRelativeLocation(FVector(0.f, 0.f, HalfHeight));
	SetActorRelativeRotation(FRotator::ZeroRotator);
}

void AGrayBoxCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AGrayBoxCharacter::MoveForward);
		EIC->BindAction(MoveBackAction, ETriggerEvent::Triggered, this, &AGrayBoxCharacter::MoveBack);
		EIC->BindAction(MoveLeftAction, ETriggerEvent::Triggered, this, &AGrayBoxCharacter::MoveLeft);
		EIC->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AGrayBoxCharacter::MoveRight);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGrayBoxCharacter::Look);
		EIC->BindAction(FireAction, ETriggerEvent::Started, this, &AGrayBoxCharacter::Fire);
	}
}

void AGrayBoxCharacter::MoveForward(const FInputActionValue&) { AddMovementInput(GetActorForwardVector(), 1.f); }
void AGrayBoxCharacter::MoveBack(const FInputActionValue&) { AddMovementInput(GetActorForwardVector(), -1.f); }
void AGrayBoxCharacter::MoveLeft(const FInputActionValue&) { AddMovementInput(GetActorRightVector(), -1.f); }
void AGrayBoxCharacter::MoveRight(const FInputActionValue&) { AddMovementInput(GetActorRightVector(), 1.f); }

void AGrayBoxCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookVec = Value.Get<FVector2D>();
	AddControllerYawInput(LookVec.X);
	AddControllerPitchInput(LookVec.Y);
}

void AGrayBoxCharacter::Fire(const FInputActionValue&)
{
	UE_LOG(LogOurBlock, Log, TEXT("Fire() invoked"));

	if (!Camera || !GetWorld())
	{
		return;
	}

	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetForwardVector() * FireRange;

	// Visible for a few seconds so a shot that missed shows exactly where it actually
	// went, rather than leaving "hit nothing" to mean either bad aim or a real bug.
	// Thickness 0 draws a true thin line rather than a camera-facing extruded quad -
	// the trace starts at the camera itself, so a thick line viewed nearly end-on
	// rendered as a big flat rectangle covering most of the view.
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 3.0f, 0, 0.0f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		UE_LOG(LogOurBlock, Log, TEXT("Fire() hit %s at %s"), *GetNameSafe(Hit.GetActor()), *Hit.Location.ToString());
		DrawDebugSphere(GetWorld(), Hit.Location, 15.f, 8, FColor::Red, false, 3.0f);
		if (AThreatActor* Threat = Cast<AThreatActor>(Hit.GetActor()))
		{
			Threat->Deny();
		}
	}
	else
	{
		UE_LOG(LogOurBlock, Log, TEXT("Fire() hit nothing, traced from %s to %s"), *Start.ToString(), *End.ToString());
	}
}
