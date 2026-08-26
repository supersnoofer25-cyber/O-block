#include "GrayBoxGameMode.h"
#include "GrayBoxCharacter.h"

AGrayBoxGameMode::AGrayBoxGameMode()
{
	DefaultPawnClass = AGrayBoxCharacter::StaticClass();
}
