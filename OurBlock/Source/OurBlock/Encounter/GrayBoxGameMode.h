#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GrayBoxGameMode.generated.h"

// Sets AGrayBoxCharacter as the default pawn so TestLevel's PlayerStart spawns
// something that can move, look, and fire - the engine's own DefaultPawn does none of
// those against ThreatActor. Referenced from DefaultEngine.ini's GlobalDefaultGameMode;
// nothing else in the project needs a GameMode yet.
UCLASS()
class OURBLOCK_API AGrayBoxGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGrayBoxGameMode();
};
