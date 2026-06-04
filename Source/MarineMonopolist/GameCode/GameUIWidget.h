#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "FishData.h"
#include "GameUIWidget.generated.h"

class UFishNotificationWidget;

UCLASS()
class MARINEMONOPOLIST_API UGameUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void AddFishNotification(const FFishData& FishData);

protected:
    // Balance
    UPROPERTY(meta = (BindWidget))
    UTextBlock* BalanceText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BalanceTextGoal;
	
    // Day/Night
    UPROPERTY(meta = (BindWidget))
    UTextBlock* DayNightText;

    // Fishing rod progress
    UPROPERTY(meta = (BindWidget))
    UProgressBar* FishingProgressBar;

    // Speed up button
    UPROPERTY(meta = (BindWidget))
    UButton* SpeedUpBtn;

    // Net progress & collection
    UPROPERTY(meta = (BindWidget))
    UProgressBar* NetProgressBar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* NetFishCountText;

    UPROPERTY(meta = (BindWidget))
    UButton* NetCollectBtn;

    // Fish notifications container
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* NotificationBox;

    // Upgrade buttons
    UPROPERTY(meta = (BindWidget))
    UButton* RodUpgradeBtn;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RodLevelText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RodCostText;

    UPROPERTY(meta = (BindWidget))
    UButton* NetUpgradeBtn;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* NetLevelText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* NetCostText;

    UPROPERTY(meta = (BindWidget))
    UButton* NightBaitUpgradeBtn;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* NightBaitLevelText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* NightBaitCostText;

    UPROPERTY(meta = (BindWidget))
    UButton* ShipUpgradeBtn;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ShipLevelText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ShipCostText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ShipRequirementText;

	// Canvas containers
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MainOverlay;

	UPROPERTY(meta = (BindWidget))
	UWidget* GameCanvas;

	UPROPERTY(meta = (BindWidget))
	UWidget* ShopCanvas;

	UPROPERTY(meta = (BindWidget))
	UWidget* NavigationCanvas;

	// Navigation buttons
	UPROPERTY(meta = (BindWidget))
	UButton* EnterShopBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitShopBtn;

	// Dark overlay
	UPROPERTY(meta = (BindWidget))
	UImage* DarkOverlay;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UFishNotificationWidget> FishNotificationWidgetClass;

private:
	static constexpr int32 MaxSlots = 4;

	TArray<UFishNotificationWidget*> NotificationSlots;
	TArray<FFishData> NotificationQueue;
	float SlotDisplayTimes[MaxSlots] = {0.0f};
	float AdvanceInterval = 0.5f;
	FTimerHandle NotificationTimerHandle;

	bool bIsInShop = false;

	enum class EShopFadeState : uint8
	{
		Idle,
		FadeToBlack,
		FadeFromBlack
	};

	EShopFadeState FadeState = EShopFadeState::Idle;
	float FadeStartTime = 0.0f;
	float FadeDuration = 1.0f;
	FTimerHandle FadeTimerHandle;
	bool bFadeTargetIsShop = false;

	void FillSlotsFromQueue();
	void UpdateSlotWidget(int32 Index, const FFishData* FishData);
	void EnterShop();
	void ExitShop();
	void StartFade(bool bTargetIsShop);

	UFUNCTION()
	void OnEnterShopClicked();

	UFUNCTION()
	void OnExitShopClicked();

	UFUNCTION()
	void OnFadeTick();

	UFUNCTION()
	void OnMoneyChanged(int32 NewMoney);

    UFUNCTION()
    void OnDayNightChanged(bool bIsNight);

    UFUNCTION()
    void OnRodUpgraded(int32 NewLevel);

    UFUNCTION()
    void OnNetUpgraded(int32 NewLevel);

    UFUNCTION()
    void OnNightBaitUpgraded(int32 NewLevel);

    UFUNCTION()
    void OnShipUpgraded(int32 NewLevel);

    UFUNCTION()
    void OnRodUpgradeClicked();

    UFUNCTION()
    void OnNetUpgradeClicked();

    UFUNCTION()
    void OnNightBaitUpgradeClicked();

    UFUNCTION()
    void OnShipUpgradeClicked();

    UFUNCTION()
    void OnSpeedUpClicked();

    UFUNCTION()
    void OnNetCollectClicked();

    UFUNCTION()
    void OnAdvanceTimerTick();

    void UpdateAll();
    void UpdateBalanceDisplay();
    void UpdateDayNightDisplay();
    void UpdateUpgradeButtons();
    void UpdateNetDisplay();
    void SetupUpgradeRow(UButton* Btn, UTextBlock* LevelText, UTextBlock* CostText,
        int32 Level, int32 MaxLevel, int32 Cost, bool bCanUpgrade);
};
