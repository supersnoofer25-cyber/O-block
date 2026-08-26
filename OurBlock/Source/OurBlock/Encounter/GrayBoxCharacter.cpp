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

AGrayBoxCharacter::AGrayBoxCharacter()
{
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight));

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

UInputAction* AGrayBoxCharacter::MakeBoolAction()
{
	UInputAction* Action = NewObject<UInputAction>(this);
	Action->ValueType = EInputActionValueType::Boolean;
	return Action;
}

void AGrayBoxCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Built at runtime rather than loaded from a Content asset - see the header for
	// why. Mouse2D and the digital keys below are engine-provided FKeys; nothing here
	// depends on any .uasset existing.
	MappingContext = NewObject<UInputMappingContext>(this);

	MoveForwardAction = MakeBoolAction();
	MoveBackAction = MakeBoolAction();
	MoveLeftAction = MakeBoolAction();
	MoveRightAction = MakeBoolAction();
	FireAction = MakeBoolAction();

	LookAction = NewObject<UInputAction>(this);
	LookAction->ValueType = EInputActionValueType::Axis2D;

	MappingContext->MapKey(MoveForwardAction, EKeys::W);
	MappingContext->MapKey(MoveBackAction, EKeys::S);
	MappingContext->MapKey(MoveLeftAction, EKeys::A);
	MappingContext->MapKey(MoveRightAction, EKeys::D);
	MappingContext->MapKey(LookAction, EKeys::Mouse2D);
	MappingContext->MapKey(FireAction, EKeys::LeftMouseButton);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
					LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->AddMappingContext(MappingContext, 0);
			}
		}
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}
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
		if (AThreatActor* Threat = Cast<AThreatActor>(Hit.GetActor()))
		{
			Threat->Deny();
		}
	}
}
