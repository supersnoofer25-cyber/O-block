#include "ScriptedDrive.h"
#include "DirtBike.h"
#include "ThreatActor.h"
#include "DangerTallyComponent.h"
#include "EngineUtils.h"
#include "../OurBlock.h"

AScriptedDrive::AScriptedDrive()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool AScriptedDrive::Start(const FString& InMode, ADirtBike* InBike, UDangerTallyComponent* InTally)
{
	Mode = InMode.ToLower();
	Bike = InBike;
	Tally = InTally;

	// The threats populate_exposure_test.py places are at (600,1400), (-700,1900) and
	// (1300,2400). Why a stop at Y=600 on the X=300 lane is exposed to one threat and
	// Y=1900 hidden from all three is add_cover.py's layout - see there. Y=4000 is well
	// past the last threat's range, and the longest any of them can keep the angle at
	// full speed along the lane is about 2.5s.
	if (Mode == TEXT("park"))
	{
		bDrives = false;
		ExpectedFired = 0;
	}
	else if (Mode == TEXT("exposed"))
	{
		StopY = 600.f;
		ExpectedFired = 1;
	}
	else if (Mode == TEXT("hide"))
	{
		StopY = 1900.f;
		ExpectedFired = 0;
	}
	else if (Mode == TEXT("pass"))
	{
		StopY = 4000.f;
		ExpectedFired = 0;
	}
	else
	{
		UE_LOG(LogOurBlock, Error, TEXT("ScriptedDrive: unknown mode '%s' (expected park, exposed, hide or pass)"), *InMode);
		return false;
	}

	for (TActorIterator<AThreatActor> It(GetWorld()); It; ++It)
	{
		if (It->DenialMode == EThreatDenial::Exposure)
		{
			ExposureThreats.Add(*It);
			ExposureThreatNames.Add(It->GetName());
			UE_LOG(LogOurBlock, Log, TEXT("ScriptedDrive[%s]: tracking %s at %s, range %.0f, fires after %.1fs exposed, target %s"),
				*Mode, *It->GetName(), *It->GetActorLocation().ToCompactString(), It->ExposureRange, It->TimeToFire,
				It->Target ? *GetNameSafe(It->Target->GetOwner()) : TEXT("NONE"));
		}
	}

	if (InBike)
	{
		UE_LOG(LogOurBlock, Log, TEXT("ScriptedDrive[%s]: bike starts at %s facing yaw %.0f"),
			*Mode, *InBike->GetActorLocation().ToCompactString(), InBike->GetActorRotation().Yaw);
	}
	return true;
}

void AScriptedDrive::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bFinished)
	{
		return;
	}

	Elapsed += DeltaSeconds;

	if (bDrives && !bArrived)
	{
		Steer();
	}

	if (Elapsed >= NextStatusAt)
	{
		LogStatus();
		NextStatusAt += 1.f;
	}

	if (Elapsed >= Duration)
	{
		Finish();
	}
}

void AScriptedDrive::Steer()
{
	ADirtBike* B = Bike.Get();
	if (!B)
	{
		return;
	}

	const FVector Loc = B->GetActorLocation();
	const float Speed = B->GetCurrentSpeed();

	// ADirtBike::Tick zeroes speed on any blocking hit, so throttle held past the first
	// second with speed still at zero means something is in the way. Say so once,
	// rather than leaving a run that silently sat still to be read as a clean result.
	if (!bReportedBlocked && Elapsed > 1.f && FMath::IsNearlyZero(Speed))
	{
		bReportedBlocked = true;
		UE_LOG(LogOurBlock, Warning, TEXT("ScriptedDrive[%s]: bike BLOCKED at %s, t=%.1fs - route runs into something"),
			*Mode, *Loc.ToCompactString(), Elapsed);
	}

	// Brake once the remaining distance is about what braking needs (v^2 / 2a), then
	// let go the moment speed hits zero - holding brake past that point reverses.
	const float StoppingDistance = (Speed * Speed) / (2.f * B->Acceleration);
	if (StopY - Loc.Y <= StoppingDistance + 50.f)
	{
		if (Speed > 0.f)
		{
			B->SetHeldInputs(false, true, false, false);
		}
		else
		{
			B->SetHeldInputs(false, false, false, false);
			bArrived = true;
			UE_LOG(LogOurBlock, Log, TEXT("ScriptedDrive[%s]: stopped at %s, t=%.1fs"),
				*Mode, *Loc.ToCompactString(), Elapsed);
		}
		return;
	}

	// Follow the lane by always aiming at a point on it a fixed distance ahead, rather
	// than at discrete waypoints: the first version steered at a lane-change waypoint
	// and then the stop point, and with a 90 deg/s turn rate at full speed it swung
	// out to X~590 and clipped the end of a cover wall. Aiming ahead along the lane
	// eases onto it and stays there.
	const FVector2D Ahead(LaneX, FMath::Min(Loc.Y + LookAhead, StopY));
	const FVector2D ToTarget = Ahead - FVector2D(Loc.X, Loc.Y);

	// Positive yaw is a right turn (ADirtBike::Tick's TurnRight adds positive yaw).
	const float DesiredYaw = FMath::RadiansToDegrees(FMath::Atan2(ToTarget.Y, ToTarget.X));
	const float Delta = FMath::FindDeltaAngleDegrees(B->GetActorRotation().Yaw, DesiredYaw);
	B->SetHeldInputs(true, false, Delta < -3.f, Delta > 3.f);
}

void AScriptedDrive::LogStatus() const
{
	const ADirtBike* B = Bike.Get();
	FString Line = FString::Printf(TEXT("ScriptedDrive[%s] t=%4.1fs bike=%s speed=%4.0f |"),
		*Mode, Elapsed, B ? *B->GetActorLocation().ToCompactString() : TEXT("?"), B ? B->GetCurrentSpeed() : 0.f);

	for (int32 i = 0; i < ExposureThreats.Num(); ++i)
	{
		const AThreatActor* T = ExposureThreats[i].Get();
		if (!T)
		{
			Line += FString::Printf(TEXT(" %s: gone |"), *ExposureThreatNames[i]);
			continue;
		}
		const float Dist = B ? FVector::Dist(T->GetActorLocation(), B->GetActorLocation()) : -1.f;
		Line += FString::Printf(TEXT(" %s: dist %4.0f clock %.1f |"), *ExposureThreatNames[i], Dist, T->ExposureTime);
	}
	UE_LOG(LogOurBlock, Log, TEXT("%s"), *Line);
}

void AScriptedDrive::Finish()
{
	bFinished = true;
	LogStatus();

	int32 Fired = 0;
	for (const TWeakObjectPtr<AThreatActor>& T : ExposureThreats)
	{
		// A fired threat destroys itself, so gone means fired here - nothing denies an
		// Exposure threat.
		if (!T.IsValid() || T->HasFired())
		{
			++Fired;
		}
	}

	const UDangerTallyComponent* TallyComp = Tally.Get();
	// A run that got stuck can't pass, whatever it counted - it never drove the route
	// the expectation was written for.
	const bool bPass = Fired == ExpectedFired && !bReportedBlocked;
	UE_LOG(LogOurBlock, Display, TEXT("ScriptedDrive[%s] RESULT %s: %d of %d exposure threats fired (expected %d), companion tally %d (threshold %d)"),
		*Mode, bPass ? TEXT("PASS") : TEXT("FAIL"), Fired, ExposureThreats.Num(), ExpectedFired,
		TallyComp ? TallyComp->Tally : -1, TallyComp ? TallyComp->Threshold : -1);

	FPlatformMisc::RequestExit(false);
}
