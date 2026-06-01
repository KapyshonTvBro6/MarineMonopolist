#include "FishNotificationManagerWidget.h"
#include "TimerManager.h"

void UFishNotificationManagerWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	PreCreateSlots();
}

void UFishNotificationManagerWidget::NativeDestruct()
{
	StopTimer();
	Super::NativeDestruct();
}

void UFishNotificationManagerWidget::PreCreateSlots()
{
	if (!FishNotificationWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("FishNotificationManager: FishNotificationWidgetClass not set!"));
		return;
	}
	if (!NotificationBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("FishNotificationManager: NotificationBox (VerticalBox) is not bound!"));
		return;
	}

	for (int32 i = 0; i < MaxSlots; i++)
	{
		UFishNotificationWidget* Widget = CreateWidget<UFishNotificationWidget>(this, FishNotificationWidgetClass);
		if (Widget)
		{
			FishSlots.Add(Widget);
			NotificationBox->AddChild(Widget);
			Widget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("FishNotificationManager: pre-created %d widget slots"), FishSlots.Num());
}

void UFishNotificationManagerWidget::AddNotification(const FFishData& FishData)
{
	NotificationQueue.Add(FishData);
	FillSlotsFromQueue();
	StartTimer();
}

void UFishNotificationManagerWidget::AddNotifications(const TArray<FFishData>& FishDataArray)
{
	NotificationQueue.Append(FishDataArray);
	FillSlotsFromQueue();
	StartTimer();
}

void UFishNotificationManagerWidget::FillSlotsFromQueue()
{
	bool bFilledAny = false;

	for (int32 i = 0; i < MaxSlots; i++)
	{
		if (!SlotActive[i] && !NotificationQueue.IsEmpty())
		{
			SlotActive[i] = true;
			SlotData[i] = NotificationQueue[0];
			NotificationQueue.RemoveAt(0);
			UpdateSlotWidget(i);
			bFilledAny = true;
		}
	}

	if (bFilledAny)
	{
		UE_LOG(LogTemp, Verbose, TEXT("FishNotificationManager: filled slots from queue, queue size: %d"), NotificationQueue.Num());
	}
}

void UFishNotificationManagerWidget::StartTimer()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (AdvanceTimerHandle.IsValid()) return;

	World->GetTimerManager().SetTimer(
		AdvanceTimerHandle,
		this,
		&UFishNotificationManagerWidget::OnAdvanceTimerTick,
		AdvanceInterval,
		true
	);
}

void UFishNotificationManagerWidget::StopTimer()
{
	if (AdvanceTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(AdvanceTimerHandle);
	}
}

void UFishNotificationManagerWidget::OnAdvanceTimerTick()
{
	if (SlotActive[0])
	{
		Slot1DisplayTime += AdvanceInterval;
	}

	TryAdvance();

	bool bAnyActive = false;
	for (int32 i = 0; i < MaxSlots; i++)
	{
		if (SlotActive[i])
		{
			bAnyActive = true;
			break;
		}
	}

	if (!bAnyActive && NotificationQueue.IsEmpty())
	{
		StopTimer();
	}
}

void UFishNotificationManagerWidget::TryAdvance()
{
	if (!SlotActive[0]) return;

	if (NotificationQueue.IsEmpty())
	{
		if (Slot1DisplayTime < 3.0f) return;
	}
	else
	{
		if (Slot1DisplayTime < 1.0f) return;
	}

	UE_LOG(LogTemp, Verbose, TEXT("FishNotificationManager: advancing slot 1 fish '%s'"), *SlotData[0].FishName);

	for (int32 i = 0; i < MaxSlots - 1; i++)
	{
		SlotActive[i] = SlotActive[i + 1];
		if (SlotActive[i])
		{
			SlotData[i] = SlotData[i + 1];
		}
	}

	if (!NotificationQueue.IsEmpty())
	{
		SlotActive[MaxSlots - 1] = true;
		SlotData[MaxSlots - 1] = NotificationQueue[0];
		NotificationQueue.RemoveAt(0);
	}
	else
	{
		SlotActive[MaxSlots - 1] = false;
	}

	Slot1DisplayTime = SlotActive[0] ? 0.0f : 0.0f;

	for (int32 i = 0; i < MaxSlots; i++)
	{
		UpdateSlotWidget(i);
	}
}

void UFishNotificationManagerWidget::UpdateSlotWidget(int32 Index)
{
	if (!FishSlots.IsValidIndex(Index) || !FishSlots[Index]) return;

	if (SlotActive[Index])
	{
		FishSlots[Index]->SetFishData(SlotData[Index]);
		FishSlots[Index]->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		FishSlots[Index]->SetVisibility(ESlateVisibility::Collapsed);
	}
}
