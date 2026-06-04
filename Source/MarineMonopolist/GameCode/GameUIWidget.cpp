#include "GameUIWidget.h"
#include "GameManager.h"
#include "Fisherman.h"
#include "FishingNet.h"
#include "UpgradeManagerComponent.h"
#include "DayNightManagerComponent.h"
#include "FishNotificationWidget.h"
#include "GameText.h"
#include "TimerManager.h"
#include "Components/SlateWrapperTypes.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UGameUIWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    AGameManager* GM = AGameManager::GetInstance();
    if (!GM) return;

    GM->OnMoneyChanged.AddDynamic(this, &UGameUIWidget::OnMoneyChanged);

    if (GM->GetDayNightManager())
    {
        GM->GetDayNightManager()->OnDayNightChanged.AddDynamic(this, &UGameUIWidget::OnDayNightChanged);
    }

    if (GM->GetUpgradeManager())
    {
        GM->GetUpgradeManager()->OnRodUpgraded.AddDynamic(this, &UGameUIWidget::OnRodUpgraded);
        GM->GetUpgradeManager()->OnNetUpgraded.AddDynamic(this, &UGameUIWidget::OnNetUpgraded);
        GM->GetUpgradeManager()->OnNightBaitUpgraded.AddDynamic(this, &UGameUIWidget::OnNightBaitUpgraded);
        GM->GetUpgradeManager()->OnShipUpgraded.AddDynamic(this, &UGameUIWidget::OnShipUpgraded);
    }

    if (RodUpgradeBtn) RodUpgradeBtn->OnClicked.AddDynamic(this, &UGameUIWidget::OnRodUpgradeClicked);
    if (NetUpgradeBtn) NetUpgradeBtn->OnClicked.AddDynamic(this, &UGameUIWidget::OnNetUpgradeClicked);
    if (NightBaitUpgradeBtn) NightBaitUpgradeBtn->OnClicked.AddDynamic(this, &UGameUIWidget::OnNightBaitUpgradeClicked);
    if (ShipUpgradeBtn) ShipUpgradeBtn->OnClicked.AddDynamic(this, &UGameUIWidget::OnShipUpgradeClicked);
    if (SpeedUpBtn) SpeedUpBtn->OnClicked.AddDynamic(this, &UGameUIWidget::OnSpeedUpClicked);
    if (NetCollectBtn) NetCollectBtn->OnClicked.AddDynamic(this, &UGameUIWidget::OnNetCollectClicked);

    if (EnterShopBtn) EnterShopBtn->OnClicked.AddDynamic(this, &UGameUIWidget::OnEnterShopClicked);
    if (ExitShopBtn) ExitShopBtn->OnClicked.AddDynamic(this, &UGameUIWidget::OnExitShopClicked);
    if (SettingBtn) SettingBtn->OnClicked.AddDynamic(this, &UGameUIWidget::OnSettingClicked);
    if (ExitGoalBtn) ExitGoalBtn->OnClicked.AddDynamic(this, &UGameUIWidget::OnExitGoalClicked);

    // Fallback for BindWidget — sometimes doesn't restore after editor restart
    if (WidgetTree)
    {
        if (!GameCanvas)
        {
            GameCanvas = WidgetTree->FindWidget(TEXT("GameCanvas"));
            UE_LOG(LogTemp, Warning, TEXT("GameUIWidget: BindWidget GameCanvas was null, FindWidget gave: %s"),
                GameCanvas ? *GameCanvas->GetName() : TEXT("null"));
        }
        if (!ShopCanvas)
        {
            ShopCanvas = WidgetTree->FindWidget(TEXT("ShopCanvas"));
            UE_LOG(LogTemp, Warning, TEXT("GameUIWidget: BindWidget ShopCanvas was null, FindWidget gave: %s"),
                ShopCanvas ? *ShopCanvas->GetName() : TEXT("null"));
        }
        if (!NavigationCanvas)
        {
            NavigationCanvas = WidgetTree->FindWidget(TEXT("NavigationCanvas"));
            UE_LOG(LogTemp, Warning, TEXT("GameUIWidget: BindWidget NavigationCanvas was null, FindWidget gave: %s"),
                NavigationCanvas ? *NavigationCanvas->GetName() : TEXT("null"));
        }
        if (!SettingBtn)
        {
            SettingBtn = Cast<UButton>(WidgetTree->FindWidget(TEXT("SettingBtn")));
            UE_LOG(LogTemp, Warning, TEXT("GameUIWidget: BindWidget SettingBtn was null, FindWidget gave: %s"),
                SettingBtn ? *SettingBtn->GetName() : TEXT("null"));
            if (SettingBtn)
            {
                SettingBtn->OnClicked.AddDynamic(this, &UGameUIWidget::OnSettingClicked);
            }
        }
        if (!ExitGoalBtn)
        {
            ExitGoalBtn = Cast<UButton>(WidgetTree->FindWidget(TEXT("ExitGoalBtn")));
            UE_LOG(LogTemp, Warning, TEXT("GameUIWidget: BindWidget ExitGoalBtn was null, FindWidget gave: %s"),
                ExitGoalBtn ? *ExitGoalBtn->GetName() : TEXT("null"));
            if (ExitGoalBtn)
            {
                ExitGoalBtn->OnClicked.AddDynamic(this, &UGameUIWidget::OnExitGoalClicked);
            }
        }
    }

    UpdateNavigationButtonVisibility();
    if (ExitGoalBtn) ExitGoalBtn->SetVisibility(ESlateVisibility::Collapsed);
    if (ShopCanvas) ShopCanvas->SetVisibility(ESlateVisibility::Collapsed);
    if (DarkOverlay)
    {
        DarkOverlay->SetColorAndOpacity(FLinearColor::Black);
        DarkOverlay->SetRenderOpacity(0.0f);
        DarkOverlay->SetVisibility(ESlateVisibility::Collapsed);
    }

    for (int32 i = 0; i < MaxSlots; i++)
    {
        if (FishNotificationWidgetClass && NotificationBox)
        {
            UFishNotificationWidget* NotificationSlot = CreateWidget<UFishNotificationWidget>(this, FishNotificationWidgetClass);
            if (NotificationSlot)
            {
                NotificationSlots.Add(NotificationSlot);
                NotificationBox->AddChildToVerticalBox(NotificationSlot);
                NotificationSlot->SetVisibility(ESlateVisibility::Collapsed);
            }
        }
    }

    UWorld* World = GetWorld();
    if (World && NotificationSlots.Num() > 0)
    {
        World->GetTimerManager().SetTimer(NotificationTimerHandle, this, &UGameUIWidget::OnAdvanceTimerTick, AdvanceInterval, true);
    }

    UpdateAll();
}

void UGameUIWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (FishingProgressBar)
    {
        AGameManager* GM = AGameManager::GetInstance();
        if (GM && GM->GetFisherman())
        {
            float Max = GM->GetFisherman()->GetFishingTime();
            if (Max > 0.0f)
            {
                FishingProgressBar->SetPercent(GM->GetFisherman()->GetCurrentFishingTime() / Max);
            }
        }
    }

    UpdateNetDisplay();
    UpdateNavigationButtonVisibility();
}

void UGameUIWidget::UpdateNetDisplay()
{
    AGameManager* GM = AGameManager::GetInstance();
    if (!GM) return;

    AFishingNet* Net = GM->GetCurrentNet();
    if (!Net)
    {
        if (NetProgressBar) NetProgressBar->SetPercent(0.0f);
        if (NetFishCountText) NetFishCountText->SetText(FText::GetEmpty());
        if (NetCollectBtn) NetCollectBtn->SetIsEnabled(false);
        return;
    }

    float Progress = Net->GetCatchProgress();
    if (NetProgressBar)
    {
        NetProgressBar->SetPercent(Progress);

        if (Progress >= 0.8f)
        {
            FLinearColor Color = Net->WasLastCatchSuccessful() ? FLinearColor::Green : FLinearColor::Red;
            NetProgressBar->SetFillColorAndOpacity(Color);
        }
        else
        {
            NetProgressBar->SetFillColorAndOpacity(FLinearColor::Blue);
        }
    }

    int32 FishCount = Net->GetFishCount();
    if (NetFishCountText)
    {
        NetFishCountText->SetText(MarineMonopolistText::UI::FishCount(FishCount));
    }

    if (NetCollectBtn)
    {
        NetCollectBtn->SetIsEnabled(FishCount > 0);
    }
}

void UGameUIWidget::UpdateAll()
{
    UpdateBalanceDisplay();
    UpdateDayNightDisplay();
    UpdateUpgradeButtons();
    UpdateNetDisplay();
}

void UGameUIWidget::UpdateBalanceDisplay()
{
    AGameManager* GM = AGameManager::GetInstance();
    if (!GM) return;

    const int32 CurrentMoney = GM->GetMoney();
    if (BalanceText)
    {
        BalanceText->SetText(MarineMonopolistText::UI::MoneyWithoutCurrency(CurrentMoney));
    }

    UpdateGoalDisplay(CurrentMoney);
}

void UGameUIWidget::UpdateGoalDisplay(int32 CurrentMoney)
{
    if (CurrentMoney >= GoalMoney)
    {
        bGoalReached = true;
    }

    if (BalanceTextGoal)
    {
        if (bGoalReached)
        {
            BalanceTextGoal->SetText(MarineMonopolistText::UI::GoalReached());
        }
        else
        {
            BalanceTextGoal->SetText(MarineMonopolistText::UI::GoalProgress(GoalMoney, CurrentMoney));
        }
    }

    if (ExitGoalBtn)
    {
        ExitGoalBtn->SetVisibility(bGoalReached ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UGameUIWidget::UpdateDayNightDisplay()
{
    if (!DayNightText) return;
    AGameManager* GM = AGameManager::GetInstance();
    if (GM && GM->GetDayNightManager())
    {
        bool bIsNight = GM->GetDayNightManager()->IsNight();
        DayNightText->SetText(bIsNight ? MarineMonopolistText::UI::Night() : MarineMonopolistText::UI::Day());
    }
}

void UGameUIWidget::SetupUpgradeRow(UButton* Btn, UTextBlock* LevelText, UTextBlock* CostText,
    int32 Level, int32 MaxLevel, int32 Cost, bool bCanUpgrade)
{
    if (LevelText)
    {
        LevelText->SetText(MarineMonopolistText::UI::UpgradeLevel(Level, MaxLevel));
    }
    if (CostText)
    {
        if (Level >= MaxLevel)
        {
            CostText->SetText(MarineMonopolistText::UI::UpgradeMax());
        }
        else if (Cost > 0)
        {
            CostText->SetText(MarineMonopolistText::UI::UpgradeCost(Cost));
        }
        else
        {
            CostText->SetText(MarineMonopolistText::UI::UpgradeUnavailable());
        }
    }
    if (Btn)
    {
        Btn->SetIsEnabled(bCanUpgrade && Level < MaxLevel);
    }
}

void UGameUIWidget::UpdateUpgradeButtons()
{
    AGameManager* GM = AGameManager::GetInstance();
    if (!GM) return;

    UUpgradeManagerComponent* Upgrades = GM->GetUpgradeManager();
    if (!Upgrades) return;

    SetupUpgradeRow(RodUpgradeBtn, RodLevelText, RodCostText,
        Upgrades->GetRodLevel(), Upgrades->GetRodMaxLevel(), Upgrades->GetRodUpgradeCost(), Upgrades->CanUpgradeRod());

    SetupUpgradeRow(NetUpgradeBtn, NetLevelText, NetCostText,
        Upgrades->GetNetLevel(), Upgrades->GetNetMaxLevel(), Upgrades->GetNetUpgradeCost(), Upgrades->CanUpgradeNet());

    bool bCanUpgradeNightBait = Upgrades->CanUpgradeNightBait();
    int32 NightBaitMax = Upgrades->GetNightBaitMaxLevel();
    SetupUpgradeRow(NightBaitUpgradeBtn, NightBaitLevelText, NightBaitCostText,
        Upgrades->GetNightBaitLevel(), NightBaitMax, Upgrades->GetNightBaitUpgradeCost(), bCanUpgradeNightBait);

    if (ShipRequirementText)
    {
        if (Upgrades->GetShipLevel() >= 3)
        {
            ShipRequirementText->SetText(MarineMonopolistText::UI::ShipMaxLevel());
        }
        else if (!Upgrades->CanUpgradeShip())
        {
            ShipRequirementText->SetText(MarineMonopolistText::UI::ShipRequirements(
                Upgrades->GetRodMaxLevel(),
                Upgrades->GetNetMaxLevel()
            ));
        }
        else
        {
            ShipRequirementText->SetText(MarineMonopolistText::UI::UpgradeReady());
        }
    }

    SetupUpgradeRow(ShipUpgradeBtn, ShipLevelText, ShipCostText,
        Upgrades->GetShipLevel(), 3, Upgrades->GetShipUpgradeCost(), Upgrades->CanUpgradeShip());
}

void UGameUIWidget::AddFishNotification(const FFishData& FishData)
{
    NotificationQueue.Add(FishData);
    FillSlotsFromQueue();
}

void UGameUIWidget::FillSlotsFromQueue()
{
    if (NotificationQueue.Num() == 0) return;
    if (NotificationSlots.Num() < MaxSlots) return;

    for (int32 i = 0; i < MaxSlots; i++)
    {
        if (NotificationSlots[i]->GetVisibility() == ESlateVisibility::Collapsed && NotificationQueue.Num() > 0)
        {
            FFishData Data = NotificationQueue[0];
            NotificationQueue.RemoveAt(0);
            UpdateSlotWidget(i, &Data);
            SlotDisplayTimes[i] = 0.0f;
        }
    }
}

void UGameUIWidget::UpdateSlotWidget(int32 Index, const FFishData* FishData)
{
    if (!NotificationSlots.IsValidIndex(Index) || !FishData) return;

    NotificationSlots[Index]->SetFishData(*FishData);
    NotificationSlots[Index]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UGameUIWidget::OnAdvanceTimerTick()
{
    if (NotificationSlots.Num() < MaxSlots) return;

    // 1. Check if the oldest slot (index 0) needs to expire
    if (NotificationSlots[0]->GetVisibility() != ESlateVisibility::Collapsed)
    {
        SlotDisplayTimes[0] += AdvanceInterval;

        float AdvanceTime = (NotificationQueue.Num() > 0) ? 0.5f : 3.0f;

        if (SlotDisplayTimes[0] >= AdvanceTime)
        {
            // Hide the oldest slot
            NotificationSlots[0]->SetVisibility(ESlateVisibility::Collapsed);

            // Rotate widget to bottom of the VerticalBox
            if (NotificationBox)
            {
                NotificationBox->RemoveChild(NotificationSlots[0]);
                NotificationBox->AddChild(NotificationSlots[0]);
            }

            // Rotate array: move slot 0 to end
            UFishNotificationWidget* ExpiredSlot = NotificationSlots[0];
            NotificationSlots.RemoveAt(0);
            NotificationSlots.Add(ExpiredSlot);

            // Rotate display times accordingly
            float ExpiredTime = SlotDisplayTimes[0];
            for (int32 i = 0; i < MaxSlots - 1; i++)
            {
                SlotDisplayTimes[i] = SlotDisplayTimes[i + 1];
            }
            SlotDisplayTimes[MaxSlots - 1] = ExpiredTime;

            // Reset the timer for the new slot 0 content
            SlotDisplayTimes[0] = 0.0f;
        }
    }

    // 2. Fill empty slots from queue
    FillSlotsFromQueue();
}

void UGameUIWidget::OnMoneyChanged(int32 NewMoney)
{
    if (BalanceText)
    {
        BalanceText->SetText(MarineMonopolistText::UI::Money(NewMoney));
    }
    UpdateGoalDisplay(NewMoney);
    UpdateUpgradeButtons();
}

void UGameUIWidget::OnDayNightChanged(bool bIsNight)
{
    UpdateDayNightDisplay();
}

void UGameUIWidget::OnRodUpgraded(int32 NewLevel)
{
    UpdateUpgradeButtons();
}

void UGameUIWidget::OnNetUpgraded(int32 NewLevel)
{
    UpdateUpgradeButtons();
}

void UGameUIWidget::OnNightBaitUpgraded(int32 NewLevel)
{
    UpdateUpgradeButtons();
}

void UGameUIWidget::OnShipUpgraded(int32 NewLevel)
{
    UpdateUpgradeButtons();
}

void UGameUIWidget::OnRodUpgradeClicked()
{
    AGameManager* GM = AGameManager::GetInstance();
    if (GM) GM->UpgradeRod();
    UpdateUpgradeButtons();
}

void UGameUIWidget::OnNetUpgradeClicked()
{
    AGameManager* GM = AGameManager::GetInstance();
    if (GM) GM->UpgradeNet();
    UpdateUpgradeButtons();
}

void UGameUIWidget::OnNightBaitUpgradeClicked()
{
    AGameManager* GM = AGameManager::GetInstance();
    if (GM) GM->UpgradeNightBait();
    UpdateUpgradeButtons();
}

void UGameUIWidget::OnShipUpgradeClicked()
{
    AGameManager* GM = AGameManager::GetInstance();
    if (GM) GM->UpgradeShip();
    UpdateUpgradeButtons();
}

void UGameUIWidget::OnSpeedUpClicked()
{
    AGameManager* GM = AGameManager::GetInstance();
    if (GM && GM->GetFisherman())
    {
        GM->GetFisherman()->SpeedUpFishing();
    }
}

void UGameUIWidget::OnNetCollectClicked()
{
    AGameManager* GM = AGameManager::GetInstance();
    if (!GM) return;

    AFishingNet* Net = GM->GetCurrentNet();
    if (Net && Net->GetFishCount() > 0)
    {
        Net->CollectAllFish();
        UpdateNetDisplay();
        UpdateBalanceDisplay();
    }
}

void UGameUIWidget::OnEnterShopClicked()
{
    EnterShop();
}

void UGameUIWidget::OnExitShopClicked()
{
    ExitShop();
}

void UGameUIWidget::OnSettingClicked()
{
    if (EnterShopBtn) EnterShopBtn->SetVisibility(ESlateVisibility::Collapsed);
    if (ExitShopBtn) ExitShopBtn->SetVisibility(ESlateVisibility::Collapsed);
    if (SettingBtn) SettingBtn->SetVisibility(ESlateVisibility::Collapsed);
}

void UGameUIWidget::OnExitGoalClicked()
{
    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PlayerController = World->GetFirstPlayerController();
    UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit, false);
}

bool UGameUIWidget::IsGamePaused() const
{
    UWorld* World = GetWorld();
    return World && UGameplayStatics::IsGamePaused(World);
}

bool UGameUIWidget::CanStartShopFade() const
{
    return !IsGamePaused() && FadeState == EShopFadeState::Idle;
}

void UGameUIWidget::UpdateNavigationButtonVisibility()
{
    const bool bHideButtons = IsGamePaused() || FadeState != EShopFadeState::Idle;

    if (EnterShopBtn)
    {
        EnterShopBtn->SetVisibility(!bHideButtons && !bIsInShop ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

    if (ExitShopBtn)
    {
        ExitShopBtn->SetVisibility(!bHideButtons && bIsInShop ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

    if (SettingBtn)
    {
        SettingBtn->SetVisibility(!bHideButtons ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UGameUIWidget::EnterShop()
{
    if (bIsInShop || !CanStartShopFade()) return;
    StartFade(true);
}

void UGameUIWidget::ExitShop()
{
    if (!bIsInShop || !CanStartShopFade()) return;
    StartFade(false);
}

void UGameUIWidget::StartFade(bool bTargetIsShop)
{
    if (!CanStartShopFade()) return;

    bFadeTargetIsShop = bTargetIsShop;
    FadeState = EShopFadeState::FadeToBlack;
    FadeStartTime = GetWorld()->GetTimeSeconds();
    UpdateNavigationButtonVisibility();

    if (DarkOverlay)
    {
        DarkOverlay->SetRenderOpacity(0.0f);
        DarkOverlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    }

    GetWorld()->GetTimerManager().SetTimer(FadeTimerHandle, this, &UGameUIWidget::OnFadeTick, 0.016f, true);
}

void UGameUIWidget::OnFadeTick()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    float Elapsed = CurrentTime - FadeStartTime;
    float Alpha = FMath::Clamp(Elapsed / FadeDuration, 0.0f, 1.0f);

    switch (FadeState)
    {
    case EShopFadeState::FadeToBlack:
        if (DarkOverlay) DarkOverlay->SetRenderOpacity(Alpha);
        if (Alpha >= 1.0f)
        {
            AGameManager* GM = AGameManager::GetInstance();
            if (bFadeTargetIsShop)
            {
                bIsInShop = true;
                if (GameCanvas) GameCanvas->SetVisibility(ESlateVisibility::Collapsed);
                if (ShopCanvas) ShopCanvas->SetVisibility(ESlateVisibility::Visible);
                if (GM) GM->SetShopMode(true);
            }
            else
            {
                bIsInShop = false;
                if (GameCanvas) GameCanvas->SetVisibility(ESlateVisibility::Visible);
                if (ShopCanvas) ShopCanvas->SetVisibility(ESlateVisibility::Collapsed);
                if (GM) GM->SetShopMode(false);
            }

            FadeState = EShopFadeState::FadeFromBlack;
            FadeStartTime = CurrentTime;
            UpdateNavigationButtonVisibility();
        }
        break;

    case EShopFadeState::FadeFromBlack:
        if (DarkOverlay) DarkOverlay->SetRenderOpacity(1.0f - Alpha);
        if (Alpha >= 1.0f)
        {
            FadeState = EShopFadeState::Idle;
            if (DarkOverlay)
            {
                DarkOverlay->SetVisibility(ESlateVisibility::Collapsed);
            }
            GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);
            UpdateNavigationButtonVisibility();
        }
        break;
    }
}
