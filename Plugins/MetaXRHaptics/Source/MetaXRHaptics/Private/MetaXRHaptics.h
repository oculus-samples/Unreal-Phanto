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

#pragma once

#include "Modules/ModuleManager.h"
#include "haptics_sdk/haptics_sdk.h"
#include "haptics_sdk/haptics_sdk_internal.h"

METAXRHAPTICS_API DECLARE_LOG_CATEGORY_EXTERN(LogHapticsSDK, Log, All);

class FMetaXRHapticsOpenXRExtension;

using HapticsSdkVersionPtr = decltype(haptics_sdk_version)*;
using HapticsSdkInitializeLoggingPtr = decltype(haptics_sdk_initialize_logging)*;
using HapticsSdkInitializeWithNullBackendPtr = decltype(haptics_sdk_initialize_with_null_backend)*;
using HapticsSdkUninitializePtr = decltype(haptics_sdk_uninitialize)*;
using HapticsSdkInitializedPtr = decltype(haptics_sdk_initialized)*;
using HapticsSdkLoadClipPtr = decltype(haptics_sdk_load_clip)*;
using HapticsSdkClipDurationPtr = decltype(haptics_sdk_clip_duration)*;
using HapticsSdkReleaseClipPtr = decltype(haptics_sdk_release_clip)*;
using HapticsSdkCreatePlayerPtr = decltype(haptics_sdk_create_player)*;
using HapticsSdkReleasePlayerPtr = decltype(haptics_sdk_release_player)*;
using HapticsSdkPlayerSetClipPtr = decltype(haptics_sdk_player_set_clip)*;
using HapticsSdkPlayerPlayPtr = decltype(haptics_sdk_player_play)*;
using HapticsSdkPlayerPausePtr = decltype(haptics_sdk_player_pause)*;
using HapticsSdkPlayerResumePtr = decltype(haptics_sdk_player_resume)*;
using HapticsSdkPlayerStopPtr = decltype(haptics_sdk_player_stop)*;
using HapticsSdkPlayerSeekPtr = decltype(haptics_sdk_player_seek)*;
using HapticsSdkPlayerSetAmplitudePtr = decltype(haptics_sdk_player_set_amplitude)*;
using HapticsSdkPlayerAmplitudePtr = decltype(haptics_sdk_player_amplitude)*;
using HapticsSdkPlayerSetFrequencyShiftPtr = decltype(haptics_sdk_player_set_frequency_shift)*;
using HapticsSdkPlayerFrequencyShiftPtr = decltype(haptics_sdk_player_frequency_shift)*;
using HapticsSdkPlayerSetLoopingEnabledPtr = decltype(haptics_sdk_player_set_looping_enabled)*;
using HapticsSdkPlayerLoopingEnabledPtr = decltype(haptics_sdk_player_looping_enabled)*;
using HapticsSdkPlayerSetPriorityPtr = decltype(haptics_sdk_player_set_priority)*;
using HapticsSdkPlayerPriorityPtr = decltype(haptics_sdk_player_priority)*;
using HapticsSdkErrorMessagePtr = decltype(haptics_sdk_error_message)*;
using HapticsSdkInitializeWithOvrPluginPtr = decltype(haptics_sdk_initialize_with_ovr_plugin)*;
using HapticsSdkSetSuspendedPtr = decltype(haptics_sdk_set_suspended)*;
using HapticsSdkSuspendedPtr = decltype(haptics_sdk_suspended)*;
using HapticsSdkNullBackendStatsPtr = decltype(haptics_sdk_get_null_backend_statistics)*;
using HapticsSdkGetOpenXrExtensionCountPtr = decltype(haptics_sdk_get_openxr_extension_count)*;
using HapticsSdkGetOpenXrExtensionPtr = decltype(haptics_sdk_get_openxr_extension)*;
using HapticsSDKInitializeWithOpenXrPtr = decltype(haptics_sdk_initialize_with_openxr_from_game_engine)*;
using HapticsSdkSetOpenXrSessionPtr = decltype(haptics_sdk_set_openxr_session)*;
using HapticsSdkSetOpenXrActionSetPtr = decltype(haptics_sdk_set_openxr_action_set)*;
using HapticsSdkCreateOpenXrActionSetPtr = decltype(haptics_sdk_create_openxr_action_set)*;
using HapticsSdkDestroyOpenXrActionSetPtr = decltype(haptics_sdk_destroy_openxr_action_set)*;
using HapticsSdkGetOpenXrSuggestedBindingCountPtr = decltype(haptics_sdk_get_openxr_suggested_binding_count)*;
using HapticsSdkGetOpenXrSuggestedBindingPtr = decltype(haptics_sdk_get_openxr_suggested_binding)*;
using HapticsSdkSetOpenXrSessionStatePtr = decltype(haptics_sdk_set_openxr_session_state)*;

/**
 * The haptics module is responsible for loading the native library (haptics_sdk.dll or
 * libhaptics_sdk.so) and its functions at runtime.
 *
 * Once loaded, the function pointers are accessible as public member variables.
 */
class METAXRHAPTICS_API FMetaXRHapticsModule : public IModuleInterface
{
public:
	FMetaXRHapticsModule();
	~FMetaXRHapticsModule();

	/**
	 * Returns the module, loading it on demand if needed.
	 */
	static FMetaXRHapticsModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FMetaXRHapticsModule>("MetaXRHaptics");
	}

	/**
	 * Returns the module, but only if the the native library could be loaded successfully.
	 *
	 * Returns nullptr if the native library or any of its functions could not be loaded.
	 */
	static FMetaXRHapticsModule* GetIfLibraryLoaded();

	FMetaXRHapticsOpenXRExtension* GetOpenXRExtension() const;

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	HapticsSdkVersionPtr HapticsSDKVersion = nullptr;
	HapticsSdkInitializeLoggingPtr HapticsSDKInitializeLogging = nullptr;
	HapticsSdkInitializeWithNullBackendPtr HapticsSDKInitializeWithNullBackend = nullptr;
	HapticsSdkUninitializePtr HapticsSDKUninitialize = nullptr;
	HapticsSdkInitializedPtr HapticsSDKInitialized = nullptr;
	HapticsSdkLoadClipPtr HapticsSDKLoadClip = nullptr;
	HapticsSdkClipDurationPtr HapticsSDKClipDuration = nullptr;
	HapticsSdkReleaseClipPtr HapticsSDKReleaseClip = nullptr;
	HapticsSdkCreatePlayerPtr HapticsSDKCreatePlayer = nullptr;
	HapticsSdkReleasePlayerPtr HapticsSDKReleasePlayer = nullptr;
	HapticsSdkPlayerSetClipPtr HapticsSDKPlayerSetClip = nullptr;
	HapticsSdkPlayerPlayPtr HapticsSDKPlayerPlay = nullptr;
	HapticsSdkPlayerPausePtr HapticsSDKPlayerPause = nullptr;
	HapticsSdkPlayerResumePtr HapticsSDKPlayerResume = nullptr;
	HapticsSdkPlayerStopPtr HapticsSDKPlayerStop = nullptr;
	HapticsSdkPlayerSeekPtr HapticsSdkPlayerSeek = nullptr;
	HapticsSdkPlayerSetAmplitudePtr HapticsSDKPlayerSetAmplitude = nullptr;
	HapticsSdkPlayerAmplitudePtr HapticsSDKPlayerAmplitude = nullptr;
	HapticsSdkPlayerSetFrequencyShiftPtr HapticsSDKPlayerSetFrequencyShift = nullptr;
	HapticsSdkPlayerFrequencyShiftPtr HapticsSDKPlayerFrequencyShift = nullptr;
	HapticsSdkPlayerSetLoopingEnabledPtr HapticsSDKPlayerSetLoopingEnabled = nullptr;
	HapticsSdkPlayerLoopingEnabledPtr HapticsSDKPlayerLoopingEnabled = nullptr;
	HapticsSdkPlayerSetPriorityPtr HapticsSDKPlayerSetPriority = nullptr;
	HapticsSdkPlayerPriorityPtr HapticsSDKPlayerPriority = nullptr;
	HapticsSdkErrorMessagePtr HapticsSDKErrorMessage = nullptr;
	HapticsSdkInitializeWithOvrPluginPtr HapticsSDKInitializeWithOvrPlugin = nullptr;
	HapticsSdkSetSuspendedPtr HapticsSDKSetSuspended = nullptr;
	HapticsSdkSuspendedPtr HapticsSDKSuspended = nullptr;
	HapticsSdkNullBackendStatsPtr HapticsSdkNullBackendStats = nullptr;
	HapticsSdkGetOpenXrExtensionCountPtr HapticsSDKGetOpenXrExtensionCount = nullptr;
	HapticsSdkGetOpenXrExtensionPtr HapticsSDKGetOpenXrExtension = nullptr;
	HapticsSDKInitializeWithOpenXrPtr HapticsSDKInitializeWithOpenXr = nullptr;
	HapticsSdkSetOpenXrSessionPtr HapticsSDKSetOpenXrSession = nullptr;
	HapticsSdkSetOpenXrActionSetPtr HapticsSDKSetOpenXrActionSet = nullptr;
	HapticsSdkCreateOpenXrActionSetPtr HapticsSDKCreateOpenXrActionSet = nullptr;
	HapticsSdkDestroyOpenXrActionSetPtr HapticsSDKDestroyOpenXrActionSet = nullptr;
	HapticsSdkGetOpenXrSuggestedBindingCountPtr HapticsSDKGetOpenXrSuggestedBindingCount = nullptr;
	HapticsSdkGetOpenXrSuggestedBindingPtr HapticsSDKGetOpenXrSuggestedBinding = nullptr;
	HapticsSdkSetOpenXrSessionStatePtr HapticsSDKSetOpenXrSessionState = nullptr;

private:
	/** Handle to Haptics Native SDK library */
	void* HapticsSDKLibraryHandle = nullptr;

	TUniquePtr<FMetaXRHapticsOpenXRExtension> OpenXRExtension;

	template <typename Func>
	Func LoadFunction(const FString& Name);
};
