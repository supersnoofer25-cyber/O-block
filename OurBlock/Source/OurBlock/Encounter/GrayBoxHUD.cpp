#include "GrayBoxHUD.h"
#include "Engine/Canvas.h"

void AGrayBoxHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	const float CenterX = Canvas->SizeX * 0.5f;
	const float CenterY = Canvas->SizeY * 0.5f;
	const float Size = 8.f;

	DrawLine(CenterX - Size, CenterY, CenterX + Size, CenterY, FLinearColor::White, 2.f);
	DrawLine(CenterX, CenterY - Size, CenterX, CenterY + Size, FLinearColor::White, 2.f);
}
