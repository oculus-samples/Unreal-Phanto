// Copyright (c) Meta Platforms, Inc. and affiliates.


#include "RotateToAction.h"

URotateToAction* URotateToAction::RotateTo(UObject const * WorldContext, FRotator Start, FRotator Target, float Time)
{
	auto const Node = NewObject<URotateToAction>();
	if (Node)
	{
		Node->WorldContext = WorldContext;
		Node->Start = Start;
		Node->Target = Target;
		Node->Time = Time;
		Node->StartTime = WorldContext->GetWorld()->TimeSeconds;
		Node->RegisterWithGameInstance(WorldContext);
	}
	return Node;
}

void URotateToAction::Activate()
{
	if (WorldContext.IsValid())
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindWeakLambda(this, [this]
			{
				if (!WorldContext.IsValid())
				{
					Cancel();
					return;
				}
				
				auto const Alpha = WorldContext->GetWorld()->TimeSince(StartTime) / Time;

				if (Alpha < 1)
				{
					FQuat AQuat(Start);
					FQuat BQuat(Target);
					FQuat Result = FQuat::Slerp(AQuat, BQuat, Alpha);
					Tick.Broadcast(Result.Rotator());
				}
				else
				{
					Completed.Broadcast(Target);
					GetTimerManager()->ClearTimer(TimerHandle);
				}
			});
		GetTimerManager()->SetTimer(TimerHandle, TimerDelegate, 0.01f, true);
	}
}

void URotateToAction::Cancel()
{
	if (auto TimerManager = GetTimerManager())
	{
		TimerManager->ClearTimer(TimerHandle);
	}
}
