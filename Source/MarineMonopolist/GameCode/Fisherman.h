#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Engine/DataTable.h"
#include "FishData.h"
#include "Fisherman.generated.h"

// Делегат для события поимки рыбы
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFishCaught, int32, FishID, const FString&, FishName);

UCLASS()
class MARINEMONOPOLIST_API AFisherman : public APawn
{
	GENERATED_BODY()
    
public:
	AFisherman();
    
protected:
	virtual void BeginPlay() override;
    
public:
	virtual void Tick(float DeltaTime) override;
    
	// Начать ловлю рыбы
	UFUNCTION(BlueprintCallable, Category = "Fishing")
	void StartFishing();
    
	// Ускорить ловлю (вызывается из UI)
	UFUNCTION(BlueprintCallable, Category = "Fishing")
	void SpeedUpFishing();
    
	// Получить рыбу
	void CatchFish();
    
	// Событие поимки рыбы (для обновления UI)
	UPROPERTY(BlueprintAssignable, Category = "Fishing")
	FOnFishCaught OnFishCaught;
    
	// Получить текущее время ловли
	UFUNCTION(BlueprintCallable, Category = "Fishing")
	float GetCurrentFishingTime() const { return CurrentFishingTime; }
    
	// Получить общее время ловли
	UFUNCTION(BlueprintCallable, Category = "Fishing")
	float GetFishingTime() const { return FishingTime; }
    
private:
	// Время ловли рыбы (в секундах)
	UPROPERTY(EditAnywhere, Category = "Fishing")
	float FishingTime = 5.0f;
    
	// Удача рыбака
	UPROPERTY(EditAnywhere, Category = "Fishing")
	int32 Luck = 1;
    
	// Базовое ускорение от клика (значение для прокачки)
	UPROPERTY(EditAnywhere, Category = "Fishing")
	float SpeedUpValue = 1.0f;
    
	// Таблица с данными о рыбе
	UPROPERTY(EditAnywhere, Category = "Fishing")
	UDataTable* FishDataTable;
    
	// Текущее время ловли
	float CurrentFishingTime = 0.0f;
    
	// Флаг, идет ли сейчас ловля
	bool bIsFishing = false;
    
	// Выбрать случайную рыбу на основе вероятностей
	FFishData* GetRandomFish();
};