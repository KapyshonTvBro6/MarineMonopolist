#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ship.generated.h"

UCLASS()
class MARINEMONOPOLIST_API AShip : public AActor
{
	GENERATED_BODY()
    
public:
	AShip();
    
protected:
	virtual void BeginPlay() override;
    
public:
	virtual void Tick(float DeltaTime) override;
    
	// Получить точку для рыбака
	UFUNCTION(BlueprintCallable, Category = "Ship")
	USceneComponent* GetFishermanPoint() const { return FishermanPoint; }
    
	// Получить точку для второго рыбака (для мультиплеера)
	UFUNCTION(BlueprintCallable, Category = "Ship")
	USceneComponent* GetSecondFishermanPoint() const { return SecondFishermanPoint; }
    
	// Получить доступные точки для сетей
	UFUNCTION(BlueprintCallable, Category = "Ship")
	TArray<USceneComponent*> GetNetPoints() const { return NetPoints; }
    
	// Получить первую свободную точку для сети
	UFUNCTION(BlueprintCallable, Category = "Ship")
	USceneComponent* GetAvailableNetPoint();
    
	// Добавить сеть к кораблю
	UFUNCTION(BlueprintCallable, Category = "Ship")
	void AttachNet(AActor* NetActor);
    
	// Получить количество установленных сетей
	UFUNCTION(BlueprintCallable, Category = "Ship")
	int32 GetNetCount() const { return AttachedNets.Num(); }
    
	// Получить максимальное количество сетей
	UFUNCTION(BlueprintCallable, Category = "Ship")
	int32 GetMaxNets() const { return MaxNets; }
    
	// Улучшить корабль (увеличить количество слотов для сетей)
	UFUNCTION(BlueprintCallable, Category = "Ship")
	void Upgrade();
    
	// Создать новую точку для сети во время выполнения
	UFUNCTION(BlueprintCallable, Category = "Ship")
	void AddNewNetPoint();
    
	// Получить текущий уровень корабля
	UFUNCTION(BlueprintCallable, Category = "Ship")
	int32 GetShipLevel() const { return ShipLevel; }
    
	// Стоимость улучшения корабля
	UFUNCTION(BlueprintCallable, Category = "Ship")
	int32 GetUpgradeCost() const { return UpgradeCost; }
    
private:
	// Текущий уровень корабля
	UPROPERTY(EditAnywhere, Category = "Ship")
	int32 ShipLevel = 1;
    
	// Максимальное количество сетей на корабле (базовое + улучшения)
	UPROPERTY(VisibleAnywhere, Category = "Ship")
	int32 MaxNets = 4;
    
	// Базовая вместимость сетей (без улучшений)
	UPROPERTY(EditAnywhere, Category = "Ship")
	int32 BaseMaxNets = 4;
    
	// Стоимость улучшения корабля
	UPROPERTY(EditAnywhere, Category = "Ship")
	int32 UpgradeCost = 500;
    
	// Количество дополнительных слотов за уровень
	UPROPERTY(EditAnywhere, Category = "Ship")
	int32 NetsPerLevel = 2;
    
	// Точка для рыбака
	UPROPERTY(VisibleAnywhere, Category = "Ship")
	USceneComponent* FishermanPoint;
    
	// Точка для второго рыбака (мультиплеер)
	UPROPERTY(VisibleAnywhere, Category = "Ship")
	USceneComponent* SecondFishermanPoint;
    
	// Точки для сетей
	UPROPERTY(VisibleAnywhere, Category = "Ship")
	TArray<USceneComponent*> NetPoints;
    
	// Прикрепленные сети
	UPROPERTY(VisibleAnywhere, Category = "Ship")
	TArray<AActor*> AttachedNets;
    
	// Создать все компоненты точек в конструкторе
	void CreateAttachmentPoints();
    
	// Создать точку для сети (используется во время выполнения)
	USceneComponent* CreateNetPointComponent(int32 Index);
};