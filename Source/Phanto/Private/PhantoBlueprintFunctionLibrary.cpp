// Copyright (c) Meta Platforms, Inc. and affiliates.


#include "PhantoBlueprintFunctionLibrary.h"

#include "NavigationData.h"
#include "NavLinkCustomComponent.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

void UPopulateSceneAsyncAction::Activate()
{
	Super::Activate();

	if (auto World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindWeakLambda(this, [this, World]
		{
			if (SceneActor->IsScenePopulated())
			{
				OnScenePopulated.Broadcast();
						
				auto&& TimerManager = World->GetTimerManager();
				TimerManager.ClearTimer(TimerHandle);
			}
			else
			{
				SceneActor->PopulateScene();
			}
		});
		
		auto&& TimerManager = World->GetTimerManager();
		TimerManager.SetTimer(TimerHandle, TimerDelegate, CheckLoopTimeSeconds, true);

		SceneActor->PopulateScene();
	}
}

UPopulateSceneAsyncAction* UPopulateSceneAsyncAction::PopulateSceneAsync(AOculusXRSceneActor* SceneActor,
	float CheckLoopTimeSeconds)
{
	auto Action = NewObject<UPopulateSceneAsyncAction>(SceneActor->GetWorld());
	Action->SceneActor = SceneActor;
	Action->CheckLoopTimeSeconds = CheckLoopTimeSeconds;
	return Action;
}

FString UPhantoBlueprintFunctionLibrary::Repeat(FString String, int Count)
{
	FString RepeatedString(FString(), Count * String.Len());
	while (Count--)
		RepeatedString.Append(String);
	return RepeatedString;
}

double UPhantoBlueprintFunctionLibrary::AngleBetweenRadians(FVector const & A, FVector const & B)
{
	return FMath::Acos(A.Dot(B));
}

double UPhantoBlueprintFunctionLibrary::AngleBetween(FVector const & A, FVector const & B)
{
	return FMath::RadiansToDegrees(AngleBetweenRadians(A, B));
}

UObject* UPhantoBlueprintFunctionLibrary::DynamicCast(UObject* Object, TSubclassOf<UObject> TargetClass, bool & IsValid)
{
	IsValid = Object && Object->GetClass()->IsChildOf(TargetClass);
	return Object;
}

void UPhantoBlueprintFunctionLibrary::SetLinkData(
	UNavLinkCustomComponent* NavLink,
	const FVector& RelativeStart,
	const FVector& RelativeEnd,
	ENavLinkDirection::Type Direction)
{
	NavLink->Activate();
	NavLink->SetLinkData(RelativeStart, RelativeEnd, Direction);
	NavLink->ForceNavigationRelevancy(true);

	if (auto NavSys = UNavigationSystemV1::GetNavigationSystem(NavLink))
	{
		if (NavSys->GetCustomLink(NavLink->GetId()) != NavLink)
		{
			NavSys->RegisterCustomLink(*NavLink);
		}
		else
		{
			NavSys->UpdateCustomLink(NavLink);
		}
	}
}

void UPhantoBlueprintFunctionLibrary::GetLinkData(const UNavLinkCustomComponent* NavLink, FVector& LeftPt, FVector& RightPt, TEnumAsByte<ENavLinkDirection::Type>& Direction)
{
	ENavLinkDirection::Type DirectionEnum;
	NavLink->GetLinkData(LeftPt, RightPt, DirectionEnum);
	Direction = DirectionEnum;
}

void UPhantoBlueprintFunctionLibrary::SetCanEverAffectNavigation(UActorComponent* Component, bool bCanEverAffectNavigationData)
{
	Component->SetCanEverAffectNavigation(bCanEverAffectNavigationData);
}

void UPhantoBlueprintFunctionLibrary::RebuildAll(UObject* WorldContextObject)
{
	auto NavSystem = UNavigationSystemV1::GetNavigationSystem(WorldContextObject);
	if (!NavSystem)
		return;

	auto NavMesh = NavSystem->GetDefaultNavDataInstance();
	if (!NavMesh)
		return;

	NavMesh->RebuildAll();
}

void UPhantoBlueprintFunctionLibrary::SetRebuildingSuspended(UObject* WorldContextObject, const bool bNewSuspend)
{
	auto NavSystem = UNavigationSystemV1::GetNavigationSystem(WorldContextObject);
	if (!NavSystem)
		return;

	auto NavMesh = NavSystem->GetDefaultNavDataInstance();
	if (!NavMesh)
		return;

	NavMesh->SetRebuildingSuspended(bNewSuspend);
}

void UPhantoBlueprintFunctionLibrary::ReleaseInitialBuildingLock(UObject* WorldContextObject)
{
	auto NavSystem = UNavigationSystemV1::GetNavigationSystem(WorldContextObject);
	NavSystem->ReleaseInitialBuildingLock();
}

void UPhantoBlueprintFunctionLibrary::ConfigureNavigationSystemAsStatic(bool bIsStatic)
{
	UNavigationSystemV1::ConfigureAsStatic(bIsStatic);
}

void UPhantoBlueprintFunctionLibrary::SetNavigationRuntimeGenerationMode(UObject* WorldContextObject, ERuntimeGenerationType Mode)
{
	struct AUnprotectedNavigationData : ANavigationData
	{
		void SetRuntimeGenerationMode(ERuntimeGenerationType Mode)
		{
			RuntimeGeneration = Mode;
		}
	};

	auto NavSystem = UNavigationSystemV1::GetNavigationSystem(WorldContextObject);
	if (!NavSystem)
		return;

	auto NavMesh = NavSystem->GetDefaultNavDataInstance();
	if (!NavMesh)
		return;

	((AUnprotectedNavigationData*)NavMesh)->SetRuntimeGenerationMode(Mode);

	NavSystem->SetNavigationOctreeLock(Mode != ERuntimeGenerationType::Dynamic);
}

ERuntimeGenerationType UPhantoBlueprintFunctionLibrary::GetNavigationRuntimeGenerationMode(UObject* WorldContextObject)
{
	auto NavSystem = UNavigationSystemV1::GetNavigationSystem(WorldContextObject);
	if (!NavSystem)
		return (ERuntimeGenerationType)-1;

	auto NavMesh = NavSystem->GetDefaultNavDataInstance();
	if (!NavMesh)
		return (ERuntimeGenerationType)-1;

	return NavMesh->GetRuntimeGenerationMode();
}

void UPhantoBlueprintFunctionLibrary::SetPreciseReachThreshold(UPathFollowingComponent* Component, float AgentRadiusMultiplier, float AgentHalfHeightMultiplier)
{
	if (Component)
	{
		Component->SetPreciseReachThreshold(AgentRadiusMultiplier, AgentHalfHeightMultiplier);
	}
}
