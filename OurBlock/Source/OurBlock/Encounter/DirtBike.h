#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "DirtBike.generated.h"

class UStaticMeshComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

// Which of the bike's two seats something occupies. ADR 0004: a bike carries two, so
// the ride is capped by the fiction rather than an imposed rule - matching how
// campaign::BipOut.Companion is one field, not a collection, for the same reason.
UENUM(BlueprintType)
enum class EBikeSeat : uint8
{
	Driver,
	Passenger,
};

// A dirt bike, first production pass: kinematic movement (accelerate, turn) rather
// than Chaos Vehicle physics. ADR 0004 makes the bike the only means of bipping out and
// puts the whole seat choice on it; what's being tested at this stage is whether that
// choice and the resulting movement/aim feel right, not whether the suspension is
// realistic - full vehicle physics can replace this movement later without touching
// the seat structure or anything built on top of it.
//
// The player possesses this pawn directly when driving (Driver seat). The Passenger
// seat is a separate actor (AGrayBoxCharacter, attached via AttachToBikeSeat) - the
// bike itself never controls or contains the passenger's camera or fire logic.
UCLASS()
class OURBLOCK_API ADirtBike : public APawn
{
	GENERATED_BODY()

public:
	ADirtBike();

	UPROPERTY(EditAnywhere, Category = "Bike")
	float Acceleration = 800.f;

	UPROPERTY(EditAnywhere, Category = "Bike")
	float MaxSpeed = 1200.f;

	UPROPERTY(EditAnywhere, Category = "Bike")
	float ReverseMaxSpeed = 400.f;

	UPROPERTY(EditAnywhere, Category = "Bike")
	float Drag = 600.f;

	UPROPERTY(EditAnywhere, Category = "Bike")
	float TurnRateDegreesPerSecond = 90.f;

	// Where the driver's and passenger's camera/attachment sits.
	UFUNCTION(BlueprintPure, Category = "Bike")
	USceneComponent* GetSeatComponent(EBikeSeat Seat) const;

	// For AScriptedDrive only: sets the same held-key flags the input bindings set, so
	// a scripted run exercises exactly the movement code a player does - Tick can't
	// tell the difference. Not for gameplay; a real companion driving (bPlayerRides
	// == false) is ADR 0004's own unbuilt AI work, not this.
	void SetHeldInputs(bool bAccelerate, bool bBrake, bool bTurnLeft, bool bTurnRight)
	{
		bAccelerateHeld = bAccelerate;
		bBrakeHeld = bBrake;
		bTurnLeftHeld = bTurnLeft;
		bTurnRightHeld = bTurnRight;
	}

	float GetCurrentSpeed() const { return CurrentSpeed; }

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Bike")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Bike")
	TObjectPtr<USceneComponent> DriverSeat;

	UPROPERTY(VisibleAnywhere, Category = "Bike")
	TObjectPtr<USceneComponent> PassengerSeat;

	UPROPERTY(VisibleAnywhere, Category = "Bike")
	TObjectPtr<UCameraComponent> DriverCamera;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> MappingContext;
	UPROPERTY()
	TObjectPtr<UInputAction> AccelerateAction;
	UPROPERTY()
	TObjectPtr<UInputAction> BrakeAction;
	UPROPERTY()
	TObjectPtr<UInputAction> TurnLeftAction;
	UPROPERTY()
	TObjectPtr<UInputAction> TurnRightAction;

	UInputAction* MakeBoolAction(FName SubobjectName);
	void ApplyMappingContextIfReady();

	void OnAccelerateStarted(const FInputActionValue&) { bAccelerateHeld = true; }
	void OnAccelerateStopped(const FInputActionValue&) { bAccelerateHeld = false; }
	void OnBrakeStarted(const FInputActionValue&) { bBrakeHeld = true; }
	void OnBrakeStopped(const FInputActionValue&) { bBrakeHeld = false; }
	void OnTurnLeftStarted(const FInputActionValue&) { bTurnLeftHeld = true; }
	void OnTurnLeftStopped(const FInputActionValue&) { bTurnLeftHeld = false; }
	void OnTurnRightStarted(const FInputActionValue&) { bTurnRightHeld = true; }
	void OnTurnRightStopped(const FInputActionValue&) { bTurnRightHeld = false; }

	bool bAccelerateHeld = false;
	bool bBrakeHeld = false;
	bool bTurnLeftHeld = false;
	bool bTurnRightHeld = false;

	float CurrentSpeed = 0.f;
};
