#include "GrayBoxGameMode.h"
#include "GrayBoxCharacter.h"
#include "GrayBoxHUD.h"
#include "DirtBike.h"
#include "GameFramework/PlayerController.h"
#include "../OurBlock.h"

AGrayBoxGameMode::AGrayBoxGameMode()
{
	// DefaultPawnClass is still set here for anything that reads the property
	// directly, but GetDefaultPawnClassForController_Implementation below is what
	// RestartPlayer actually calls, and it's the one that branches on bPlayerRides.
	DefaultPawnClass = AGrayBoxCharacter::StaticClass();
	HUDClass = AGrayBoxHUD::StaticClass();
}

UClass* AGrayBoxGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (bPlayerRides)
	{
		return ADirtBike::StaticClass();
	}
	return AGrayBoxCharacter::StaticClass();
}

void AGrayBoxGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	SpawnTheOtherSeatOccupant(NewPlayer);
}

void AGrayBoxGameMode::SpawnTheOtherSeatOccupant(APlayerController* PlayerController)
{
	if (!PlayerController || !GetWorld())
	{
		return;
	}

	if (bPlayerRides)
	{
		ADirtBike* Bike = Cast<ADirtBike>(PlayerController->GetPawn());
		if (!Bike)
		{
			UE_LOG(LogOurBlock, Warning, TEXT("SpawnTheOtherSeatOccupant: bPlayerRides but player pawn is not a DirtBike"));
			return;
		}

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AGrayBoxCharacter* Passenger =
			GetWorld()->SpawnActor<AGrayBoxCharacter>(AGrayBoxCharacter::StaticClass(), Bike->GetActorTransform(), Params);
		if (Passenger)
		{
			Passenger->AttachToBikeSeat(Bike);
			UE_LOG(LogOurBlock, Log, TEXT("SpawnTheOtherSeatOccupant: placeholder passenger attached"));
		}
	}
	else
	{
		AGrayBoxCharacter* PlayerCharacter = Cast<AGrayBoxCharacter>(PlayerController->GetPawn());
		if (!PlayerCharacter)
		{
			UE_LOG(LogOurBlock, Warning, TEXT("SpawnTheOtherSeatOccupant: !bPlayerRides but player pawn is not a GrayBoxCharacter"));
			return;
		}

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ADirtBike* Bike =
			GetWorld()->SpawnActor<ADirtBike>(ADirtBike::StaticClass(), PlayerCharacter->GetActorTransform(), Params);
		if (Bike)
		{
			PlayerCharacter->AttachToBikeSeat(Bike);
			UE_LOG(LogOurBlock, Log, TEXT("SpawnTheOtherSeatOccupant: player attached to placeholder-driven bike"));
		}
	}
}
