#include "GrayBoxGameMode.h"
#include "GrayBoxCharacter.h"
#include "GrayBoxHUD.h"

AGrayBoxGameMode::AGrayBoxGameMode()
{
	DefaultPawnClass = AGrayBoxCharacter::StaticClass();
	HUDClass = AGrayBoxHUD::StaticClass();
}
