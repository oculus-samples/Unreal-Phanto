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

#include "MetaXRHaptics.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"

METAXRHAPTICS_API DEFINE_LOG_CATEGORY(LogHapticsSDK);

#define LOCTEXT_NAMESPACE "FMetaXRHapticsModule"

template <typename Func>
Func FMetaXRHapticsModule::LoadFunction(const FString& Name)
{
	if (HapticsSDKLibraryHandle == nullptr)
	{
		return nullptr;
	}

	const Func func = reinterpret_cast<Func>(FPlatformProcess::GetDllExport(HapticsSDKLibraryHandle, *Name));
	if (func == nullptr)
	{
		UE_LOG(LogHapticsSDK, Error, TEXT("Failed to load function '%s'"), *Name);
		HapticsSDKLibraryHandle = nullptr;
	}

	return func;
}

FMetaXRHapticsModule* FMetaXRHapticsModule::GetIfLibraryLoaded()
{
	if (!FModuleManager::Get().IsModuleLoaded("MetaXRHaptics"))
	{
		UE_LOG(LogHapticsSDK, Error, TEXT("Haptics SDK module is not available"));
		return nullptr;
	}

	FMetaXRHapticsModule* const MetaHapticsModule = &Get();
	if (MetaHapticsModule->HapticsSDKLibraryHandle == nullptr)
	{
		return nullptr;
	}

	return MetaHapticsModule;
}

void FMetaXRHapticsModule::StartupModule()
{
#if PLATFORM_WINDOWS
	const FString BaseDir = IPluginManager::Get().FindPlugin(TEXT("MetaXRHaptics"))->GetBaseDir();
	const FString NativeLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/lib/Win64/haptics_sdk.dll"));
	if (!FPaths::FileExists(NativeLibraryPath))
	{
		UE_LOG(LogHapticsSDK, Error, TEXT("Could not find native library '%s'"), *NativeLibraryPath);
		return;
	}
#elif PLATFORM_ANDROID
	const FString NativeLibraryPath = TEXT("libhaptics_sdk.so");
#endif

	HapticsSDKLibraryHandle = FPlatformProcess::GetDllHandle(*NativeLibraryPath);
	if (HapticsSDKLibraryHandle != nullptr)
	{
		UE_LOG(LogHapticsSDK, Log, TEXT("Loaded native library '%s'"), *NativeLibraryPath);
	}
	else
	{
		UE_LOG(LogHapticsSDK, Error, TEXT("Failed to load native library '%s'"), *NativeLibraryPath);
		return;
	}

	HapticsSDKVersion = LoadFunction<HapticsSdkVersionPtr>("haptics_sdk_version");
	HapticsSDKInitializeWithNullBackend = LoadFunction<HapticsSdkInitializeWithNullBackendPtr>(
		"haptics_sdk_initialize_with_null_backend");
	HapticsSDKUninitialize = LoadFunction<HapticsSdkUninitializePtr>("haptics_sdk_uninitialize");
	HapticsSDKInitialized = LoadFunction<HapticsSdkInitializedPtr>("haptics_sdk_initialized");
	HapticsSDKLoadClip = LoadFunction<HapticsSdkLoadClipPtr>("haptics_sdk_load_clip");
	HapticsSDKClipDuration = LoadFunction<HapticsSdkClipDurationPtr>("haptics_sdk_clip_duration");
	HapticsSDKReleaseClip = LoadFunction<HapticsSdkReleaseClipPtr>("haptics_sdk_release_clip");
	HapticsSDKCreatePlayer = LoadFunction<HapticsSdkCreatePlayerPtr>("haptics_sdk_create_player");
	HapticsSDKReleasePlayer = LoadFunction<HapticsSdkReleasePlayerPtr>("haptics_sdk_release_player");
	HapticsSDKPlayerSetClip = LoadFunction<HapticsSdkPlayerSetClipPtr>("haptics_sdk_player_set_clip");
	HapticsSDKPlayerPlay = LoadFunction<HapticsSdkPlayerPlayPtr>("haptics_sdk_player_play");
	HapticsSDKPlayerStop = LoadFunction<HapticsSdkPlayerStopPtr>("haptics_sdk_player_stop");
	HapticsSDKPlayerSetAmplitude =
		LoadFunction<HapticsSdkPlayerSetAmplitudePtr>("haptics_sdk_player_set_amplitude");
	HapticsSDKPlayerAmplitude =
		LoadFunction<HapticsSdkPlayerAmplitudePtr>("haptics_sdk_player_amplitude");
	HapticsSDKPlayerSetFrequencyShift =
		LoadFunction<HapticsSdkPlayerSetFrequencyShiftPtr>("haptics_sdk_player_set_frequency_shift");
	HapticsSDKPlayerFrequencyShift =
		LoadFunction<HapticsSdkPlayerFrequencyShiftPtr>("haptics_sdk_player_frequency_shift");
	HapticsSDKPlayerSetLoopingEnabled =
		LoadFunction<HapticsSdkPlayerSetLoopingEnabledPtr>("haptics_sdk_player_set_looping_enabled");
	HapticsSDKPlayerLoopingEnabled =
		LoadFunction<HapticsSdkPlayerLoopingEnabledPtr>("haptics_sdk_player_looping_enabled");
	HapticsSDKPlayerSetPriority =
		LoadFunction<HapticsSdkPlayerSetPriorityPtr>("haptics_sdk_player_set_priority");
	HapticsSDKPlayerPriority =
		LoadFunction<HapticsSdkPlayerPriorityPtr>("haptics_sdk_player_priority");
	HapticsSDKErrorMessage = LoadFunction<HapticsSdkErrorMessagePtr>("haptics_sdk_error_message");
	HapticsSDKInitializeWithOvrPlugin =
		LoadFunction<HapticsSdkInitializeWithOvrPluginPtr>("haptics_sdk_initialize_with_ovr_plugin");
	HapticsSDKSetSuspended = LoadFunction<HapticsSdkSetSuspendedPtr>("haptics_sdk_set_suspended");
	HapticsSDKSuspended = LoadFunction<HapticsSdkSuspendedPtr>("haptics_sdk_suspended");
	HapticsSdkNullBackendStats = LoadFunction<HapticsSdkNullBackendStatsPtr>("haptics_sdk_get_null_backend_statistics");
}

void FMetaXRHapticsModule::ShutdownModule()
{
	if (HapticsSDKLibraryHandle == nullptr)
	{
		return;
	}

	FPlatformProcess::FreeDllHandle(HapticsSDKLibraryHandle);
	UE_LOG(LogHapticsSDK, Log, TEXT("Released native library"));
	HapticsSDKLibraryHandle = nullptr;

	HapticsSDKVersion = nullptr;
	HapticsSDKInitializeWithNullBackend = nullptr;
	HapticsSDKUninitialize = nullptr;
	HapticsSDKInitialized = nullptr;
	HapticsSDKLoadClip = nullptr;
	HapticsSDKClipDuration = nullptr;
	HapticsSDKReleaseClip = nullptr;
	HapticsSDKCreatePlayer = nullptr;
	HapticsSDKReleasePlayer = nullptr;
	HapticsSDKPlayerSetClip = nullptr;
	HapticsSDKPlayerPlay = nullptr;
	HapticsSDKPlayerStop = nullptr;
	HapticsSDKPlayerSetAmplitude = nullptr;
	HapticsSDKPlayerAmplitude = nullptr;
	HapticsSDKPlayerSetFrequencyShift = nullptr;
	HapticsSDKPlayerFrequencyShift = nullptr;
	HapticsSDKPlayerSetLoopingEnabled = nullptr;
	HapticsSDKPlayerLoopingEnabled = nullptr;
	HapticsSDKPlayerSetPriority = nullptr;
	HapticsSDKPlayerPriority = nullptr;
	HapticsSDKErrorMessage = nullptr;
	HapticsSDKInitializeWithOvrPlugin = nullptr;
	HapticsSDKSetSuspended = nullptr;
	HapticsSDKSuspended = nullptr;
	HapticsSdkNullBackendStats = nullptr;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMetaXRHapticsModule, MetaXRHaptics)
