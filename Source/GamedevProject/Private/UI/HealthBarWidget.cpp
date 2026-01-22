#include "UI/HealthBarWidget.h"

#include "Components/ProgressBar.h"

void UHealthBarWidget::SetHealthPercent(float InPercent)
{
	CachedPercent = FMath::Clamp(InPercent, 0.f, 1.f);
	if (HealthBar)
	{
		HealthBar->SetPercent(CachedPercent);
	}
}
