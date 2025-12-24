// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "AI/Navigation/NavLinkDefinition.h"
#include "CoreMinimal.h"
#include "Containers/UnrealString.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Misc/Optional.h"
#include "Navigation/PathFollowingComponent.h"
#include "OculusXRSceneActor.h"
#include "PhantoBlueprintFunctionLibrary.generated.h"

UCLASS()
class PHANTO_API UPopulateSceneAsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	AOculusXRSceneActor* SceneActor;
	float CheckLoopTimeSeconds;
	FTimerHandle TimerHandle;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScenePopulated);
	
	UPROPERTY(BlueprintAssignable)
	FOnScenePopulated OnScenePopulated;
	
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category = "OculusXR|Scene Actor", meta = (BlueprintInternalUseOnly = "true"))
	static UPopulateSceneAsyncAction* PopulateSceneAsync(AOculusXRSceneActor* SceneActor, float CheckLoopTimeSeconds);
};

/**
 * 
 */
UCLASS()
class PHANTO_API UPhantoBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	static FString Repeat(FString String, int Count);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Angle Between (in Radians)", CompactNodeTitle = "Radians Between"), Category = "Math|Vector")
	static double AngleBetweenRadians(FVector const& A, FVector const& B);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Angle Between (in Degrees)", CompactNodeTitle = "Degrees Between"), Category = "Math|Vector")
	static double AngleBetween(FVector const& A, FVector const& B);

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType="TargetClass", ExpandBoolAsExecs="IsValid", CompactNodeTitle = "Cast"), Category = "Utilities|Casting")
	static UObject* DynamicCast(UObject* Object, TSubclassOf<UObject> TargetClass, bool & IsValid);

	UFUNCTION(BlueprintCallable, Category = "AI|Navigation")
	static void SetLinkData(UNavLinkCustomComponent* NavLink, const FVector& RelativeStart, const FVector& RelativeEnd, ENavLinkDirection::Type Direction);

	UFUNCTION(BlueprintPure, Category = "AI|Navigation")
	static void GetLinkData(const UNavLinkCustomComponent* NavLink, FVector& LeftPt, FVector& RightPt, TEnumAsByte<ENavLinkDirection::Type>& Direction);

	UFUNCTION(BlueprintCallable, Category = "AI|Navigation")
	static void SetCanEverAffectNavigation(UActorComponent* Component, bool bCanEverAffectNavigationData);

	UFUNCTION(BlueprintCallable, Category = "AI|Navigation", meta = (WorldContext = "WorldContextObject"))
	static void RebuildAll(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "AI|Navigation", meta = (WorldContext = "WorldContextObject"))
	static void SetRebuildingSuspended(UObject* WorldContextObject, const bool bNewSuspend);

	UFUNCTION(BlueprintCallable, Category = "AI|Navigation", meta = (WorldContext = "WorldContextObject"))
	static void ReleaseInitialBuildingLock(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "AI|Navigation")
	static void ConfigureNavigationSystemAsStatic(bool bIsStatic);

	UFUNCTION(BlueprintCallable, Category = "AI|Navigation", meta = (WorldContext = "WorldContextObject"))
	static void SetNavigationRuntimeGenerationMode(UObject* WorldContextObject, ERuntimeGenerationType Mode);

	UFUNCTION(BlueprintPure, Category = "AI|Navigation", meta = (WorldContext = "WorldContextObject"))
	static ERuntimeGenerationType GetNavigationRuntimeGenerationMode(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "AI")
	static void SetPreciseReachThreshold(UPathFollowingComponent* Component, float AgentRadiusMultiplier, float AgentHalfHeightMultiplier);
};
