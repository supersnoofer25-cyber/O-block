#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GrayBoxHUD.generated.h"

// A crosshair, nothing else - without one there's no way to know where the fire
// trace's origin (dead center of the camera) actually projects to on screen, which
// makes aiming at a small ThreatActor sphere close to guesswork.
UCLASS()
class OURBLOCK_API AGrayBoxHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
};
