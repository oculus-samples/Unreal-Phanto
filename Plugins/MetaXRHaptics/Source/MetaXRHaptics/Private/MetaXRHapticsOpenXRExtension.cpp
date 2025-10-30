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

#include "MetaXRHapticsOpenXRExtension.h"
#include "MetaXRHaptics.h"

FMetaXRHapticsOpenXRExtension::FMetaXRHapticsOpenXRExtension()
	: Module(FMetaXRHapticsModule::GetIfLibraryLoaded())
{
	ensure(Module);
	RegisterOpenXRExtensionModularFeature();
}

FMetaXRHapticsOpenXRExtension::~FMetaXRHapticsOpenXRExtension()
{
	DestroyActionSet();
}

bool FMetaXRHapticsOpenXRExtension::IsOpenXRPluginUsed() const
{
#if UE_VERSION_OLDER_THAN(5, 4, 0)
	// We don't support OpenXR for UE < 5.4, as IOpenXRExtensionPlugin::GetSuggestedBindings() is required and only
	// available for UE >= 5.4
	return false;
#else
	// If the OpenXR plugin is enabled and loaded, PostCreateInstance() gets called and Instance will be set.
	// However, that is not enough to check, as the OpenXR plugin might not be used as the HMD module. Therefore also
	// check that BindExtensionPluginDelegates() got called, which is only the case if the plugin gets used as
	// the HMD module.
	return Instance != XR_NULL_HANDLE && Delegate != nullptr;
#endif
}

XrInstance FMetaXRHapticsOpenXRExtension::GetOpenXRInstance() const
{
	return Instance;
}

void FMetaXRHapticsOpenXRExtension::SetupActionSet()
{
	if (!Module)
	{
		return;
	}

	// The initialization order between VR live preview in the editor and a standalone game differs:
	// - In the editor, the OpenXR session is started first, then UMetaXRHapticsGameInstanceSubsystem::Initialize()
	//   is called
	// - In a standalone game, UMetaXRHapticsGameInstanceSubsystem::Initialize() is called first, then the OpenXR
	//   session is started.
	//
	// Two things need to complete before we can continue setting up the Native SDK:
	// - The Native SDK must be initialized (done in UMetaXRHapticsGameInstanceSubsystem::Initialize())
	// - The OpenXR session must be started (done when FMetaXRHapticsOpenXRExtension::PostCreateSession() gets called)
	// We want to continue setting up the Native SDK after both these steps are complete, but we don't know in which
	// order the steps will run. Therefore we place a call to this function (SetupActionSet()) after both steps.
	// After the first step finishes, we return early, and after the second step finishes, we'll continue setting up
	// the Native SDK.
	// Additionally, Unreal can call PostCreateSession() twice in a row, for the same session. In this case, the last
	// call to SetupActionSet() is redundant, as ActionSet has already been created. We also return early in that case.
	//
	// Setting up the Native SDK means passing it the session and action set to use.

	bool NativeSDKInitalized = false;
	Module->HapticsSDKInitialized(&NativeSDKInitalized);
	const bool OpenXRSessionStarted = (Session != XR_NULL_HANDLE);
	if (!NativeSDKInitalized || !OpenXRSessionStarted || ActionSet != XR_NULL_HANDLE)
	{
		return;
	}

	Module->HapticsSDKSetOpenXrSession(Session);
	Module->HapticsSDKCreateOpenXrActionSet(&ActionSet);
	Module->HapticsSDKSetOpenXrActionSet(ActionSet);
}

void FMetaXRHapticsOpenXRExtension::DestroyActionSet()
{
	if (Module && ActionSet != XR_NULL_HANDLE)
	{
		Module->HapticsSDKDestroyOpenXrActionSet(ActionSet);
		ActionSet = XR_NULL_HANDLE;
	}
	Session = XR_NULL_HANDLE;
}

bool FMetaXRHapticsOpenXRExtension::GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions)
{
	if (!Module)
	{
		return false;
	}

	int32_t ExtensionCount = 0;
	Module->HapticsSDKGetOpenXrExtensionCount(&ExtensionCount);
	for (auto i = 0; i < ExtensionCount; i++)
	{
		OutExtensions.Push(Module->HapticsSDKGetOpenXrExtension(i));
	}
	return true;
}

void FMetaXRHapticsOpenXRExtension::PostCreateInstance(XrInstance InInstance)
{
	UE_LOG(LogHapticsSDK, Log, TEXT("OpenXR instance set - OpenXR plugin is enabled and loaded"));

	ensure(InInstance != XR_NULL_HANDLE);
	Instance = InInstance;

	// We have an OpenXR instance now, and in theory we could now call Module->HapticsSDKInitializeWithOpenXr().
	// However, we don't initialize the Native SDK here, but in UMetaXRHapticsGameInstanceSubsystem::Initialize(),
	// for multiple reasons:
	// 1. To stay consistent how the Native SDK is initialized when using the OVRPlugin backend
	// 2. To have both the initialization and the deinitialization in the same class
	// 3. At this point, the decision to use OpenXR instead of OVRPlugin has not been made yet.
	//    IOpenXRExtensionPlugin::PostCreateInstance() also gets called when both the OpenXR plugin and the Meta XR plugin
	//    are enabled at the same time. The Meta XR plugin has a higher default priority than the OpenXR plugin. And if the
	//    XR API setting in the Meta XR plugin is set to OVRPlugin, then OVRPlugin instead of OpenXR gets used.
	//    Only when IOpenXRExtensionPlugin::BindExtensionPluginDelegates() gets called we can be sure that OpenXR instead
	//    of OVRPlugin is used.
}

void FMetaXRHapticsOpenXRExtension::PostCreateSession(XrSession InSession)
{
	UE_LOG(LogHapticsSDK, Verbose, TEXT("OpenXR session started"));

	ensure(InSession != XR_NULL_HANDLE);
	Session = InSession;
	SetupActionSet();
}

#if !UE_VERSION_OLDER_THAN(5, 0, 0)

void FMetaXRHapticsOpenXRExtension::AttachActionSets(TSet<XrActionSet>& OutActionSets)
{
	if (ActionSet != XR_NULL_HANDLE)
	{
		OutActionSets.Add(ActionSet);
	}
}

void FMetaXRHapticsOpenXRExtension::GetActiveActionSetsForSync(TArray<XrActiveActionSet>& OutActiveSets)
{
	if (ActionSet != XR_NULL_HANDLE)
	{
		const XrActiveActionSet activeActionSet{ ActionSet, XR_NULL_PATH };
		OutActiveSets.Push(activeActionSet);
	}
}

void FMetaXRHapticsOpenXRExtension::BindExtensionPluginDelegates(IOpenXRExtensionPluginDelegates& OpenXRHMD)
{
	UE_LOG(LogHapticsSDK, Log, TEXT("Extension plugin delegate set - OpenXR plugin is used as the HMD module"));
	Delegate = &OpenXRHMD;
}

#if !UE_VERSION_OLDER_THAN(5, 4, 0)
bool FMetaXRHapticsOpenXRExtension::GetSuggestedBindings(XrPath InInteractionProfile, TArray<XrActionSuggestedBinding>& OutBindings)
{
	if (!Module)
	{
		return false;
	}

	int32_t BindingCount = 0;
	Module->HapticsSDKGetOpenXrSuggestedBindingCount(&BindingCount);

	for (int i = 0; i < BindingCount; i++)
	{
		const XrActionSuggestedBinding Binding = Module->HapticsSDKGetOpenXrSuggestedBinding(i);
		OutBindings.Push(Binding);
	}

	return true;
}
#endif
#endif
