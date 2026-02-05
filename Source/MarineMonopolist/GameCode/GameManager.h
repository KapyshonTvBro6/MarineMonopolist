#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FishingNet.h"
#include "GameManager.generated.h"

// Делегат для события изменения денег
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoneyChanged, int32, NewMoneyAmount);

// Делегат для события покупки сети
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNetPurchased, AFishingNet*, PurchasedNet);

// Делегат для события улучшения корабля
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShipUpgraded, int32, NewLevel);

// Предварительное объявление классов
class AFisherman;
class AShip;

UCLASS()
class MARINEMONOPOLIST_API AGameManager : public AActor 
{
	GENERATED_BODY()
    
public:
	AGameManager();
    
	// Статический метод для получения менеджера
	UFUNCTION(BlueprintCallable, Category = "Game Manager")
	static AGameManager* GetInstance();
    
	// Инициализация игры
	UFUNCTION(BlueprintCallable, Category = "Game Manager")
	void InitializeGame();
    
	// Добавить деньги
	UFUNCTION(BlueprintCallable, Category = "Game Manager")
	void AddMoney(int32 Amount);
    
	// Потратить деньги (возвращает true, если успешно)
	UFUNCTION(BlueprintCallable, Category = "Game Manager")
	bool SpendMoney(int32 Amount);
    
	// Получить текущее количество денег
	UFUNCTION(BlueprintCallable, Category = "Game Manager")
	int32 GetMoney() const;
    
	// Купить новую сеть (возвращает ссылку на созданную сеть, или nullptr если покупка не удалась)
	UFUNCTION(BlueprintCallable, Category = "Game Manager")
	AFishingNet* BuyNet();
    
	// Улучшить корабль
	UFUNCTION(BlueprintCallable, Category = "Game Manager")
	bool UpgradeShip();
    
	// Получить текущий корабль
	UFUNCTION(BlueprintCallable, Category = "Game Manager")
	AShip* GetCurrentShip() const { return CurrentShip; }
    
	// Получить все сети
	UFUNCTION(BlueprintCallable, Category = "Game Manager")
	TArray<AFishingNet*> GetFishingNets() const { return FishingNets; }
    
	// Получить рыбака
	UFUNCTION(BlueprintCallable, Category = "Game Manager")
	AFisherman* GetFisherman() const { return Fisherman; }
    
	// Событие изменения денег (можно подписаться в Blueprint)
	UPROPERTY(BlueprintAssignable, Category = "Game Manager")
	FOnMoneyChanged OnMoneyChanged;
    
	// Событие покупки сети (можно подписаться в Blueprint)
	UPROPERTY(BlueprintAssignable, Category = "Game Manager")
	FOnNetPurchased OnNetPurchased;
    
	// Событие улучшения корабля
	UPROPERTY(BlueprintAssignable, Category = "Game Manager")
	FOnShipUpgraded OnShipUpgraded;
    
protected:
	virtual void BeginPlay() override;
    
private:
	static AGameManager* Instance;
    
	// Стоимость покупки сети
	UPROPERTY(EditAnywhere, Category = "Game Manager")
	int32 NetCost = 100;
    
	// Стоимость улучшения корабля
	UPROPERTY(EditAnywhere, Category = "Game Manager")
	int32 ShipUpgradeCost = 500;
    
	UPROPERTY(VisibleAnywhere, Category = "Game Manager")
	int32 Money;
    
	// Ссылка на рыбака
	UPROPERTY(VisibleAnywhere, Category = "Game Manager")
	AFisherman* Fisherman;
    
	// Ссылка на корабль
	UPROPERTY(VisibleAnywhere, Category = "Game Manager")
	AShip* CurrentShip;
    
	// Массив сетей
	UPROPERTY(VisibleAnywhere, Category = "Game Manager")
	TArray<AFishingNet*> FishingNets;
    
	// Классы для спавна
	UPROPERTY(EditAnywhere, Category = "Game Manager")
	TSubclassOf<AFisherman> FishermanClass;
    
	UPROPERTY(EditAnywhere, Category = "Game Manager")
	TSubclassOf<AShip> ShipClass;
    
	UPROPERTY(EditAnywhere, Category = "Game Manager")
	TSubclassOf<AFishingNet> FishingNetClass;
    
	// Создать корабль
	void CreateShip();
    
	// Создать рыбака
	void CreateFisherman();
    
	// Разместить рыбака на корабле
	void PlaceFishermanOnShip();
    
	// Вызвать событие изменения денег
	void BroadcastMoneyChanged();
    
	// Вызвать событие покупки сети
	void BroadcastNetPurchased(AFishingNet* Net);
    
	// Вызвать событие улучшения корабля
	void BroadcastShipUpgraded(int32 NewLevel);
};