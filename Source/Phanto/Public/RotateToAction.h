// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "RotateToAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRotateToActionOutputPin, FRotator, Rotator);

UCLASS()
class PHANTO_API URotateToAction : public UCancellableAsyncAction
{
	GENERATED_BODY()

	TWeakObjectPtr<UObject const> WorldContext;
	FRotator Start;
	FRotator Target;
	float Time;
	double StartTime;
	FTimerHandle TimerHandle;

public:
	UPROPERTY(BlueprintAssignable)
	FRotateToActionOutputPin Tick;

	UPROPERTY(BlueprintAssignable)
	FRotateToActionOutputPin Completed;
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContext"), Category = "AsyncNode")
	static URotateToAction* RotateTo(UObject const * WorldContext, FRotator Start, FRotator Target, float Time);

	virtual void Activate() override;
	virtual void Cancel() override;
};
