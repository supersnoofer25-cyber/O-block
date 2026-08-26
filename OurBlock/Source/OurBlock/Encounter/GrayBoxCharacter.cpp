#include "GrayBoxCharacter.h"
#include "ThreatActor.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "../OurBlock.h"

AGrayBoxCharacter::AGrayBoxCharacter()
{
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight));

	bUseControllerRotationYaw = true;
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

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		UE_LOG(LogOurBlock, Log, TEXT("Fire() hit %s"), *GetNameSafe(Hit.GetActor()));
		if (AThreatActor* Threat = Cast<AThreatActor>(Hit.GetActor()))
		{
			Threat->Deny();
		}
	}
	else
	{
		UE_LOG(LogOurBlock, Log, TEXT("Fire() hit nothing"));
	}
}
