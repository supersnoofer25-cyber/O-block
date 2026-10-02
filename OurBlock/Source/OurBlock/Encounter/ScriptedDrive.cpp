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
	// (1300,2400). (300,-100) is just a lane change, clear of the origin cluster's
	// cube at (0,0) and spheres at (600,0)/(0,600). (300,1900) is within ExposureRange
	// of all three threats; (300,4000) is well past the last one's range, and the
	// longest any of them keeps the angle at full speed along X=300 is about 2.5s.
	const FVector2D LaneChange(300.f, -100.f);
	if (Mode == TEXT("park"))
	{
	}
	else if (Mode == TEXT("camp"))
	{
		Route = { LaneChange, FVector2D(300.f, 1900.f) };
	}
	else if (Mode == TEXT("pass"))
	{
		Route = { LaneChange, FVector2D(300.f, 4000.f) };
	}
	else
	{
		UE_LOG(LogOurBlock, Error, TEXT("ScriptedDrive: unknown mode '%s' (expected park, camp or pass)"), *InMode);
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

	if (Route.Num() > 0 && !bArrived)
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

	// Every route here runs in +Y, so "passed a waypoint" is just "got level with it" -
	// sturdier than a radius check, which a bike with a wide turning circle can orbit.
	const bool bLast = RouteIndex == Route.Num() - 1;
	if (!bLast && Loc.Y >= Route[RouteIndex].Y)
	{
		++RouteIndex;
		return;
	}
	const FVector2D Target = Route[RouteIndex];
	const FVector2D ToTarget = Target - FVector2D(Loc.X, Loc.Y);
	const float Distance = ToTarget.Size();

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
	if (bLast && Distance <= StoppingDistance + 50.f)
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
	UE_LOG(LogOurBlock, Display, TEXT("ScriptedDrive[%s] RESULT: %d of %d exposure threats fired, companion tally %d (threshold %d)"),
		*Mode, Fired, ExposureThreats.Num(), TallyComp ? TallyComp->Tally : -1, TallyComp ? TallyComp->Threshold : -1);

	FPlatformMisc::RequestExit(false);
}
