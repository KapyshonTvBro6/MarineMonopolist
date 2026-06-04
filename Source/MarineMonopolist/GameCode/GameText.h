#pragma once

#include "CoreMinimal.h"

namespace MarineMonopolistText::UI
{
	inline FText Money(int32 Amount)
	{
		return FText::Format(NSLOCTEXT("MarineMonopolist.UI", "Money", "{0}$"), FText::AsNumber(Amount));
	}

	inline FText MoneyWithoutCurrency(int32 Amount)
	{
		return FText::AsNumber(Amount);
	}

	inline FText FishCount(int32 Count)
	{
		return FText::Format(NSLOCTEXT("MarineMonopolist.UI", "FishCount", "Рыба: {0}"), FText::AsNumber(Count));
	}

	inline FText GoalProgress(int32 GoalMoney, int32 CurrentMoney)
	{
		return FText::Format(
			NSLOCTEXT("MarineMonopolist.UI", "GoalProgress", "Цель: {0}$/{1}$"),
			FText::AsNumber(GoalMoney),
			FText::AsNumber(CurrentMoney)
		);
	}

	inline FText GoalReached()
	{
		return NSLOCTEXT("MarineMonopolist.UI", "GoalReached", "Вы достигли цели");
	}

	inline FText Day()
	{
		return NSLOCTEXT("MarineMonopolist.UI", "Day", "☀ День");
	}

	inline FText Night()
	{
		return NSLOCTEXT("MarineMonopolist.UI", "Night", "☁ Ночь");
	}

	inline FText UpgradeLevel(int32 Level, int32 MaxLevel)
	{
		return FText::Format(
			NSLOCTEXT("MarineMonopolist.UI", "UpgradeLevel", "Ур. {0}/{1}"),
			FText::AsNumber(Level),
			FText::AsNumber(MaxLevel)
		);
	}

	inline FText UpgradeMax()
	{
		return NSLOCTEXT("MarineMonopolist.UI", "UpgradeMax", "МАКС");
	}

	inline FText UpgradeCost(int32 Cost)
	{
		return Money(Cost);
	}

	inline FText UpgradeUnavailable()
	{
		return NSLOCTEXT("MarineMonopolist.UI", "UpgradeUnavailable", "---");
	}

	inline FText ShipMaxLevel()
	{
		return NSLOCTEXT("MarineMonopolist.UI", "ShipMaxLevel", "МАКС. УРОВЕНЬ");
	}

	inline FText ShipRequirements(int32 RodLevel, int32 NetLevel)
	{
		return FText::Format(
			NSLOCTEXT("MarineMonopolist.UI", "ShipRequirements", "Нужно: удочка ур. {0}, сеть ур. {1}"),
			FText::AsNumber(RodLevel),
			FText::AsNumber(NetLevel)
		);
	}

	inline FText UpgradeReady()
	{
		return NSLOCTEXT("MarineMonopolist.UI", "UpgradeReady", "Готово!");
	}
}
