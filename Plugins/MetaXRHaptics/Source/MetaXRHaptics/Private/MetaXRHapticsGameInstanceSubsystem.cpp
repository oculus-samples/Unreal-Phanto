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
#include "Misc/CoreDelegates.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/AutomationTest.h"

void NativeSdkLogCallback(HapticsSdkLogLevel Level, const char* Message)
{
	switch (Level)
	{
		case HAPTICS_SDK_LOG_LEVEL_TRACE:
			UE_LOG(LogHapticsSDK, Verbose, TEXT("%s"), UTF8_TO_TCHAR(Message));
			break;
		case HAPTICS_SDK_LOG_LEVEL_DEBUG:
			UE_LOG(LogHapticsSDK, Log, TEXT("%s"), UTF8_TO_TCHAR(Message));
			break;
		case HAPTICS_SDK_LOG_LEVEL_INFO:
			UE_LOG(LogHapticsSDK, Display, TEXT("%s"), UTF8_TO_TCHAR(Message));
			break;
		case HAPTICS_SDK_LOG_LEVEL_WARN:
			UE_LOG(LogHapticsSDK, Warning, TEXT("%s"), UTF8_TO_TCHAR(Message));
			break;
		case HAPTICS_SDK_LOG_LEVEL_ERROR:
			UE_LOG(LogHapticsSDK, Error, TEXT("%s"), UTF8_TO_TCHAR(Message));
			break;
	}
}

void UMetaXRHapticsGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	FMetaXRHapticsModule* const HapticsModule = FMetaXRHapticsModule::GetIfLibraryLoaded();
	if (HapticsModule == nullptr)
	{
		return;
	}

#if WITH_DEV_AUTOMATION_TESTS && !UE_BUILD_SHIPPING
	if (FAutomationTestFramework::GetInstance().GetCurrentTest())
	{
		UE_LOG(LogHapticsSDK, Log, TEXT("Initalizing Haptics SDK with null backend"));
		HapticsModule->HapticsSDKInitializeWithNullBackend(NativeSdkLogCallback);
		return;
	}
#endif

	UE_LOG(LogHapticsSDK, Log, TEXT("Initializing Haptics SDK"));
	HapticsModule->HapticsSDKInitializeWithOvrPlugin(
		"UnrealEngine",
		TCHAR_TO_ANSI(*UKismetSystemLibrary::GetEngineVersion()),
		"63.0.0",
		NativeSdkLogCallback);

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

	UE_LOG(LogHapticsSDK, Log, TEXT("Uninitializing Haptics SDK"));
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
