#include "GrayBoxGameMode.h"
#include "GrayBoxCharacter.h"
#include "GrayBoxHUD.h"
#include "DirtBike.h"
#include "CompanionStandIn.h"
#include "ThreatActor.h"
#include "DangerTallyComponent.h"
#include "ScriptedDrive.h"
#include "Misc/CommandLine.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
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
		ACompanionStandIn* Passenger =
			GetWorld()->SpawnActor<ACompanionStandIn>(ACompanionStandIn::StaticClass(), Bike->GetActorTransform(), Params);
		if (Passenger)
		{
			Passenger->AttachToBikeSeat(Bike);
			UE_LOG(LogOurBlock, Log, TEXT("SpawnTheOtherSeatOccupant: companion stand-in attached to back seat"));
			WireUnaimedExposureThreats(Passenger);

			// Dev harness only - see AScriptedDrive. Absent the command-line flag,
			// normal play never spawns it.
			FString ScriptedDriveMode;
			if (FParse::Value(FCommandLine::Get(), TEXT("ScriptedDrive="), ScriptedDriveMode))
			{
				AScriptedDrive* Drive = GetWorld()->SpawnActor<AScriptedDrive>();
				if (Drive && !Drive->Start(ScriptedDriveMode, Bike, Passenger->Tally))
				{
					Drive->Destroy();
				}
			}
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

void AGrayBoxGameMode::WireUnaimedExposureThreats(ACompanionStandIn* Companion)
{
	if (!Companion || !GetWorld())
	{
		return;
	}

	int32 Count = 0;
	for (TActorIterator<AThreatActor> It(GetWorld()); It; ++It)
	{
		AThreatActor* Threat = *It;
		if (Threat->DenialMode == EThreatDenial::Exposure && !Threat->Target)
		{
			Threat->Target = Companion->Tally;
			++Count;
		}
	}
	UE_LOG(LogOurBlock, Log, TEXT("WireUnaimedExposureThreats: wired %d threat(s) to the companion on the back"), Count);
}
