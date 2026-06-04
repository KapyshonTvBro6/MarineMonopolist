#include "FishNotificationWidget.h"
#include "TimerManager.h"

void UFishNotificationWidget::SetFishData(const FFishData& InFishData)
{
	if (FishNameText)
	{
		FishNameText->SetText(FText::FromString(InFishData.FishName));
	}
	if (FishPriceText)
	{
		FishPriceText->SetText(FText::AsNumber(InFishData.Price));
	}
	if (FishImage && InFishData.FishModel)
	{
		FishImage->SetBrushFromTexture(InFishData.FishModel);
	}
}

void UFishNotificationWidget::StartDisplay(float Duration)
{
	UWorld* World = GetWorld();
	if (!World) return;
	World->GetTimerManager().SetTimer(
		DisplayTimerHandle,
		this,
		&UFishNotificationWidget::OnDisplayTimerFinished,
		Duration,
		false
	);
}

void UFishNotificationWidget::OnDisplayTimerFinished()
{
	OnFinished.Broadcast(this);
	RemoveFromParent();
}
