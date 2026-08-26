#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GrayBoxCharacter.generated.h"

class UCameraComponent;
class UInputMappingContext;
class UInputAction;

// The player, gray-boxed: WASD, mouselook, and a fire trace that denies whatever
// ThreatActor it hits. No weapon model, no animation, no bike - ADR 0004's seat choice
// doesn't exist yet either, since there's no bike to sit on. This stands in for
// "riding, on the back" only in the loosest sense: aiming and firing at threats is the
// verb being tested, not which seat the player is nominally in.
//
// Input is built entirely in C++ at BeginPlay rather than from a Content-asset input
// mapping, so this gray-box needs nothing beyond what's already in Source/ - no
// Blueprint, no .uasset, nothing to author outside a text editor.
UCLASS()
class OURBLOCK_API AGrayBoxCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGrayBoxCharacter();

	// How far the fire trace reaches. A placeholder, like DangerTallyComponent's
	// Threshold - what matters here is that denying a threat is a real action with a
	// real failure mode (missing, being out of range), not this specific number.
	UPROPERTY(EditAnywhere, Category = "Encounter")
	float FireRange = 5000.f;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Encounter")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> MappingContext;
	UPROPERTY()
	TObjectPtr<UInputAction> MoveForwardAction;
	UPROPERTY()
	TObjectPtr<UInputAction> MoveBackAction;
	UPROPERTY()
	TObjectPtr<UInputAction> MoveLeftAction;
	UPROPERTY()
	TObjectPtr<UInputAction> MoveRightAction;
	UPROPERTY()
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY()
	TObjectPtr<UInputAction> FireAction;

	void MoveForward(const FInputActionValue& Value);
	void MoveBack(const FInputActionValue& Value);
	void MoveLeft(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Fire(const FInputActionValue& Value);

	UInputAction* MakeBoolAction();
};
