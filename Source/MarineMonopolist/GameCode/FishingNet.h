#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "FishData.h"
#include "FishingNet.generated.h"

UCLASS()
class MARINEMONOPOLIST_API AFishingNet : public AActor
{
	GENERATED_BODY()
    
public:
	AFishingNet();
    
protected:
	virtual void BeginPlay() override;
    
public:
	virtual void Tick(float DeltaTime) override;
    
	// Начать автоматическую ловлю
	UFUNCTION(BlueprintCallable, Category = "Fishing Net")
	void StartFishing();
    
	// Остановить автоматическую ловлю
	UFUNCTION(BlueprintCallable, Category = "Fishing Net")
	void StopFishing();
    
	// Собрать всю рыбу из сети и продать
	UFUNCTION(BlueprintCallable, Category = "Fishing Net")
	void CollectAllFish();
    
	// Получить количество пойманной рыбы
	UFUNCTION(BlueprintCallable, Category = "Fishing Net")
	int32 GetFishCount() const;
    
	// Получить общую стоимость всей рыбы в сети
	UFUNCTION(BlueprintCallable, Category = "Fishing Net")
	int32 GetTotalValue() const;
    
	// Установить таблицу с данными о рыбе
	UFUNCTION(BlueprintCallable, Category = "Fishing Net")
	void SetFishDataTable(UDataTable* NewDataTable) { FishDataTable = NewDataTable; }
    
	// Получить уровень сети
	UFUNCTION(BlueprintCallable, Category = "Fishing Net")
	int32 GetNetLevel() const { return NetLevel; }
    
	// Установить уровень сети
	UFUNCTION(BlueprintCallable, Category = "Fishing Net")
	void SetNetLevel(int32 Level) { NetLevel = Level; }
    
	// Получить максимальную вместимость сети
	UFUNCTION(BlueprintCallable, Category = "Fishing Net")
	int32 GetMaxCapacity() const { return 2 + NetLevel; }
    
private:
	// Интервал между ловлей рыбы (в секундах)
	UPROPERTY(EditAnywhere, Category = "Fishing Net")
	float FishingInterval = 10.0f;
    
	// Уровень сети (влияет на вместимость)
	UPROPERTY(EditAnywhere, Category = "Fishing Net")
	int32 NetLevel = 1;
    
	// Таблица с данными о рыбе
	UPROPERTY(EditAnywhere, Category = "Fishing Net")
	UDataTable* FishDataTable;
    
	// Динамический массив для хранения ID пойманной рыбы
	UPROPERTY(VisibleAnywhere, Category = "Fishing Net")
	TArray<int32> CaughtFishIDs;
    
	// Таймер для автоматической ловли
	FTimerHandle FishingTimerHandle;
    
	// Флаг активности
	bool bIsFishing = false;
    
	// Обработчик завершения таймера ловли
	void OnFishingTimerComplete();
    
	// Поймать одну рыбу
	void CatchSingleFish();
    
	// Выбрать случайную рыбу на основе вероятностей
	FFishData* GetRandomFish();
};