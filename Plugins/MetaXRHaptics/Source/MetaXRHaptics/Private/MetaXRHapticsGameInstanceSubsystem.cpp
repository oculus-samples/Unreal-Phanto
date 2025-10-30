/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * Licensed under the Oculus SDK License Agreement (the "License");
 * you may not use the Oculus SDK except in compliance with the License,
 * which is provided at the time of installation or download, or which
 * otherwise accompanies this software in either electronic or hard copy form.
 *
 * You may obtain a copy of the License at
 *
 * https://developer.oculus.com/licenses/oculussdk/
 *
 * Unless required by applicable law or agreed to in writing, the Oculus SDK
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "MetaXRHapticsGameInstanceSubsystem.h"
#include "MetaXRHaptics.h"
#include "MetaXRHapticsOpenXRExtension.h"
#include "Misc/CoreDelegates.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/AutomationTest.h"

void UMetaXRHapticsGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	FMetaXRHapticsModule* const HapticsModule = FMetaXRHapticsModule::GetIfLibraryLoaded();
	if (HapticsModule == nullptr)
	{
		return;
	}

	UE_LOG(LogHapticsSDK, Log, TEXT("Initializing Native SDK"));

#if WITH_DEV_AUTOMATION_TESTS && !UE_BUILD_SHIPPING
	if (FAutomationTestFramework::GetInstance().GetCurrentTest())
	{
		UE_LOG(LogHapticsSDK, Log, TEXT("Using null backend"));
		HapticsModule->HapticsSDKInitializeWithNullBackend();
		return;
	}
#endif

	const char* const GameEngine = "UnrealEngine";
	const FString UnrealVersion = UKismetSystemLibrary::GetEngineVersion();
	FString SdkVersion;

	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("MetaXRHaptics"));
	if (Plugin.IsValid())
	{
		SdkVersion = Plugin->GetDescriptor().VersionName;
	}
	else
	{
		UE_LOG(LogHapticsSDK, Error, TEXT("Fail to get SDK version name"));
	}
	const auto OpenXRExtension = HapticsModule->GetOpenXRExtension();
	if (OpenXRExtension && OpenXRExtension->IsOpenXRPluginUsed())
	{
		ensure(FModuleManager::Get().IsModuleLoaded("OpenXRHMD"));
		UE_LOG(LogHapticsSDK, Log, TEXT("Using OpenXR backend"));

		HapticsModule->HapticsSDKInitializeWithOpenXr(OpenXRExtension->GetOpenXRInstance(),
			GameEngine, TCHAR_TO_ANSI(*UnrealVersion), TCHAR_TO_ANSI(*SdkVersion));
		OpenXRExtension->SetupActionSet();
	}
	else
	{
		ensure(FModuleManager::Get().IsModuleLoaded("OculusXRHMD"));
		UE_LOG(LogHapticsSDK, Log, TEXT("Using OVRPlugin backend"));

		HapticsModule->HapticsSDKInitializeWithOvrPlugin(
			GameEngine, TCHAR_TO_ANSI(*UnrealVersion), TCHAR_TO_ANSI(*SdkVersion));
	}

#if !WITH_EDITOR
	HeadsetRemovedHandle = FCoreDelegates::VRHeadsetRemovedFromHead.AddStatic(&OnHeadsetRemoved);
	HeadsetPutOnHandle = FCoreDelegates::VRHeadsetPutOnHead.AddStatic(&OnHeadsetPutOn);
#endif
}

void UMetaXRHapticsGameInstanceSubsystem::Deinitialize()
{
	FMetaXRHapticsModule* const HapticsModule = FMetaXRHapticsModule::GetIfLibraryLoaded();
	if (HapticsModule == nullptr)
	{
		return;
	}

	if (HapticsModule->GetOpenXRExtension())
	{
		HapticsModule->GetOpenXRExtension()->DestroyActionSet();
	}
	UE_LOG(LogHapticsSDK, Log, TEXT("Uninitializing Native SDK"));
	HapticsModule->HapticsSDKUninitialize();

#if !WITH_EDITOR
	FCoreDelegates::VRHeadsetRemovedFromHead.Remove(HeadsetRemovedHandle);
	FCoreDelegates::VRHeadsetPutOnHead.Remove(HeadsetPutOnHandle);
	HeadsetRemovedHandle.Reset();
	HeadsetPutOnHandle.Reset();
#endif
}

void UMetaXRHapticsGameInstanceSubsystem::OnHeadsetRemoved()
{
	FMetaXRHapticsModule* const HapticsModule = FMetaXRHapticsModule::GetIfLibraryLoaded();
	if (HapticsModule == nullptr)
	{
		return;
	}

	HapticsModule->HapticsSDKSetSuspended(true);
}

void UMetaXRHapticsGameInstanceSubsystem::OnHeadsetPutOn()
{
	FMetaXRHapticsModule* const HapticsModule = FMetaXRHapticsModule::GetIfLibraryLoaded();
	if (HapticsModule == nullptr)
	{
		return;
	}

	HapticsModule->HapticsSDKSetSuspended(false);
}
