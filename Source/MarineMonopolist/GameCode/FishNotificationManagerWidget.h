#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "FishData.h"
#include "FishNotificationWidget.h"
#include "FishNotificationManagerWidget.generated.h"

UCLASS()
class MARINEMONOPOLIST_API UFishNotificationManagerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Fish Notification")
	void AddNotification(const FFishData& FishData);

	UFUNCTION(BlueprintCallable, Category = "Fish Notification")
	void AddNotifications(const TArray<FFishData>& FishDataArray);

	UFUNCTION(BlueprintCallable, Category = "Fish Notification")
	void SetNotificationWidgetClass(TSubclassOf<UFishNotificationWidget> NewClass) { FishNotificationWidgetClass = NewClass; }

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> NotificationBox;

	UPROPERTY(EditAnywhere, Category = "Fish Notification")
	TSubclassOf<UFishNotificationWidget> FishNotificationWidgetClass;

	UPROPERTY()
	TArray<TObjectPtr<UFishNotificationWidget>> FishSlots;

	TArray<FFishData> NotificationQueue;

	static constexpr int32 MaxSlots = 4;

	bool SlotActive[MaxSlots] = { false, false, false, false };
	FFishData SlotData[MaxSlots];
	float Slot1DisplayTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Fish Notification")
	float AdvanceInterval = 0.5f;

	FTimerHandle AdvanceTimerHandle;

	void PreCreateSlots();
	void FillSlotsFromQueue();
	void TryAdvance();

	UFUNCTION()
	void OnAdvanceTimerTick();

	void StartTimer();
	void StopTimer();
	void UpdateSlotWidget(int32 Index);
};
