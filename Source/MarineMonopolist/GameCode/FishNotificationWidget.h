#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "FishData.h"
#include "FishNotificationWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFishNotificationFinished, UFishNotificationWidget*, Notification);

UCLASS()
class MARINEMONOPOLIST_API UFishNotificationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetFishData(const FFishData& InFishData);

	void StartDisplay(float Duration);

	UPROPERTY(BlueprintAssignable)
	FOnFishNotificationFinished OnFinished;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FishImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FishNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FishPriceText;

	UFUNCTION()
	void OnDisplayTimerFinished();

	FTimerHandle DisplayTimerHandle;
};
