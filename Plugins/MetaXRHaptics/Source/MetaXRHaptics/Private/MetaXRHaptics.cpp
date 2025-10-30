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
#include "MetaXRHapticsOpenXRExtension.h"
#include "Interfaces/IPluginManager.h"
#include "OpenXRCore.h"
#include "Misc/Paths.h"

METAXRHAPTICS_API DEFINE_LOG_CATEGORY(LogHapticsSDK);

#define LOCTEXT_NAMESPACE "FMetaXRHapticsModule"

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

FMetaXRHapticsModule::FMetaXRHapticsModule() = default;

FMetaXRHapticsModule::~FMetaXRHapticsModule() = default;

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

FMetaXRHapticsOpenXRExtension* FMetaXRHapticsModule::GetOpenXRExtension() const
{
	return OpenXRExtension.Get();
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
	HapticsSDKInitializeLogging = LoadFunction<HapticsSdkInitializeLoggingPtr>("haptics_sdk_initialize_logging");
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
	HapticsSDKPlayerPause = LoadFunction<HapticsSdkPlayerPausePtr>("haptics_sdk_player_pause");
	HapticsSDKPlayerResume = LoadFunction<HapticsSdkPlayerResumePtr>("haptics_sdk_player_resume");
	HapticsSDKPlayerStop = LoadFunction<HapticsSdkPlayerStopPtr>("haptics_sdk_player_stop");
	HapticsSdkPlayerSeek = LoadFunction<HapticsSdkPlayerSeekPtr>("haptics_sdk_player_seek");
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
	HapticsSDKGetOpenXrExtensionCount = LoadFunction<HapticsSdkGetOpenXrExtensionCountPtr>("haptics_sdk_get_openxr_extension_count");
	HapticsSDKGetOpenXrExtension = LoadFunction<HapticsSdkGetOpenXrExtensionPtr>("haptics_sdk_get_openxr_extension");
	HapticsSDKInitializeWithOpenXr = LoadFunction<HapticsSDKInitializeWithOpenXrPtr>("haptics_sdk_initialize_with_openxr_from_game_engine");
	HapticsSDKSetOpenXrSession = LoadFunction<HapticsSdkSetOpenXrSessionPtr>("haptics_sdk_set_openxr_session");
	HapticsSDKSetOpenXrActionSet = LoadFunction<HapticsSdkSetOpenXrActionSetPtr>("haptics_sdk_set_openxr_action_set");
	HapticsSDKCreateOpenXrActionSet = LoadFunction<HapticsSdkCreateOpenXrActionSetPtr>("haptics_sdk_create_openxr_action_set");
	HapticsSDKDestroyOpenXrActionSet = LoadFunction<HapticsSdkDestroyOpenXrActionSetPtr>("haptics_sdk_destroy_openxr_action_set");
	HapticsSDKGetOpenXrSuggestedBindingCount = LoadFunction<HapticsSdkGetOpenXrSuggestedBindingCountPtr>("haptics_sdk_get_openxr_suggested_binding_count");
	HapticsSDKGetOpenXrSuggestedBinding = LoadFunction<HapticsSdkGetOpenXrSuggestedBindingPtr>("haptics_sdk_get_openxr_suggested_binding");
	HapticsSDKSetOpenXrSessionState = LoadFunction<HapticsSdkSetOpenXrSessionStatePtr>("haptics_sdk_set_openxr_session_state");

	if (HapticsSDKInitializeLogging)
	{
		HapticsSDKInitializeLogging(NativeSdkLogCallback);
	}

	// We create the FMetaXRHapticsOpenXRExtension here and not in UMetaXRHapticsGameInstanceSubsystem.
	// This is because it is needed earlier, Unreal calls FMetaXRHapticsOpenXRExtension::PostCreateInstance() early during startup.
	// If we were to create FMetaXRHapticsOpenXRExtension in UMetaXRHapticsGameInstanceSubsystem, we would miss that call to
	// PostCreateInstance() as it would be too late.
	// This is also the reason why the LoadingPhase in MetaXRHaptics.uplugin is "PostConfigInit" - anything later would be too
	// late.
	OpenXRExtension.Reset(new FMetaXRHapticsOpenXRExtension());
}

void FMetaXRHapticsModule::ShutdownModule()
{
	OpenXRExtension.Reset();

	if (HapticsSDKLibraryHandle != nullptr)
	{
		FPlatformProcess::FreeDllHandle(HapticsSDKLibraryHandle);
		UE_LOG(LogHapticsSDK, Log, TEXT("Released native library"));
		HapticsSDKLibraryHandle = nullptr;
	}

	HapticsSDKVersion = nullptr;
	HapticsSDKInitializeLogging = nullptr;
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
	HapticsSDKPlayerPause = nullptr;
	HapticsSDKPlayerResume = nullptr;
	HapticsSDKPlayerStop = nullptr;
	HapticsSdkPlayerSeek = nullptr;
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
	HapticsSDKGetOpenXrExtensionCount = nullptr;
	HapticsSDKGetOpenXrExtension = nullptr;
	HapticsSDKInitializeWithOpenXr = nullptr;
	HapticsSDKSetOpenXrSession = nullptr;
	HapticsSDKSetOpenXrActionSet = nullptr;
	HapticsSDKCreateOpenXrActionSet = nullptr;
	HapticsSDKDestroyOpenXrActionSet = nullptr;
	HapticsSDKGetOpenXrSuggestedBindingCount = nullptr;
	HapticsSDKGetOpenXrSuggestedBinding = nullptr;
	HapticsSDKSetOpenXrSessionState = nullptr;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMetaXRHapticsModule, MetaXRHaptics)
