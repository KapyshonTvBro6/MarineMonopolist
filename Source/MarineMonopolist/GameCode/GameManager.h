#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FishData.h"
#include "GameManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoneyChanged, int32, NewMoneyAmount);

class AFisherman;
class AShip;
class AFishingNet;
class ANightBaitDevice;
class UGameUIWidget;
class UDayNightManagerComponent;
class UUpgradeManagerComponent;

UCLASS()
class MARINEMONOPOLIST_API AGameManager : public AActor
{
    GENERATED_BODY()

public:
    AGameManager();

    UFUNCTION(BlueprintCallable, Category = "Game Manager")
    static AGameManager* GetInstance();

    UFUNCTION(BlueprintCallable, Category = "Game Manager")
    void InitializeGame();

    UFUNCTION(BlueprintCallable, Category = "Game Manager")
    void AddMoney(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Game Manager")
    bool SpendMoney(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Game Manager")
    int32 GetMoney() const;

    UFUNCTION(BlueprintCallable, Category = "Game Manager")
    AShip* GetCurrentShip() const { return CurrentShip; }

    UFUNCTION(BlueprintCallable, Category = "Game Manager")
    AFisherman* GetFisherman() const { return Fisherman; }

    UFUNCTION(BlueprintCallable, Category = "Game Manager")
    AFishingNet* GetCurrentNet() const { return CurrentNet; }

    UFUNCTION(BlueprintCallable, Category = "Game Manager")
    ANightBaitDevice* GetNightBaitDevice() const { return NightBaitDevice; }

    UFUNCTION(BlueprintCallable, Category = "Game Manager")
    UGameUIWidget* GetGameUIWidget() const { return GameUIWidget; }

    UFUNCTION(BlueprintCallable, Category = "Game Manager")
    UDayNightManagerComponent* GetDayNightManager() const { return DayNightManager; }

    UFUNCTION(BlueprintCallable, Category = "Game Manager")
    UUpgradeManagerComponent* GetUpgradeManager() const { return UpgradeManager; }

    UFUNCTION(BlueprintCallable, Category = "Game Manager|Fish")
    UDataTable* GetActiveFishTable() const;

    UFUNCTION(BlueprintCallable, Category = "Game Manager|Upgrades")
    bool UpgradeRod();

    UFUNCTION(BlueprintCallable, Category = "Game Manager|Upgrades")
    bool UpgradeNet();

    UFUNCTION(BlueprintCallable, Category = "Game Manager|Upgrades")
    bool UpgradeNightBait();

    UFUNCTION(BlueprintCallable, Category = "Game Manager|Upgrades")
    bool UpgradeShip();

    UPROPERTY(BlueprintAssignable, Category = "Game Manager")
    FOnMoneyChanged OnMoneyChanged;

protected:
    virtual void BeginPlay() override;

private:
    static AGameManager* Instance;

    UPROPERTY(VisibleAnywhere, Category = "Game Manager")
    int32 Money;

    UPROPERTY(VisibleAnywhere, Category = "Game Manager")
    AFisherman* Fisherman;

    UPROPERTY(VisibleAnywhere, Category = "Game Manager")
    AShip* CurrentShip;

    UPROPERTY(VisibleAnywhere, Category = "Game Manager")
    AFishingNet* CurrentNet;

    UPROPERTY(VisibleAnywhere, Category = "Game Manager")
    ANightBaitDevice* NightBaitDevice;

    UPROPERTY(VisibleAnywhere, Category = "Game Manager|Components")
    UDayNightManagerComponent* DayNightManager;

    UPROPERTY(VisibleAnywhere, Category = "Game Manager|Components")
    UUpgradeManagerComponent* UpgradeManager;

    UPROPERTY(EditAnywhere, Category = "Game Manager")
    TSubclassOf<AFisherman> FishermanClass;

    UPROPERTY(EditAnywhere, Category = "Game Manager")
    TSubclassOf<AShip> ShipClass;

    UPROPERTY(EditAnywhere, Category = "Game Manager")
    TSubclassOf<AFishingNet> FishingNetClass;

    UPROPERTY(EditAnywhere, Category = "Game Manager")
    TSubclassOf<ANightBaitDevice> NightBaitDeviceClass;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UGameUIWidget> GameUIWidgetClass;

    UPROPERTY()
    TObjectPtr<UGameUIWidget> GameUIWidget;

    void CreateShip();
    void CreateFisherman();
    void PlaceFishermanOnShip();
    void SpawnNet();
    void SpawnNightBaitDevice();
    void CreateGameUIWidget();
    void BroadcastMoneyChanged();

    UFUNCTION()
    void OnFishermanCatchesFish(FFishData FishData);

    UFUNCTION()
    void OnNetCatchesFish(FFishData FishData);
};
