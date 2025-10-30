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

#include "IOpenXRExtensionPlugin.h"
#include "Misc/EngineVersionComparison.h"

class FMetaXRHapticsModule;

/**
 * An implementation of IOpenXRExtensionPlugin, used to hook the Native SDK into Unreal's OpenXR objects.
 */
class FMetaXRHapticsOpenXRExtension : public IOpenXRExtensionPlugin
{
public:
	FMetaXRHapticsOpenXRExtension();
	~FMetaXRHapticsOpenXRExtension();

	/**
	 * Returns whether the OpenXR plugin is used as the HMD module.
	 *
	 * This is different to just checking if the OpenXR plugin is enabled and loaded. The plugin might be enabled
	 * and loaded, but if the Meta XR plugin is also enabled and loaded, the Meta XR plugin might be used as the
	 * HMD module instead, depending on settings.
	 */
	bool IsOpenXRPluginUsed() const;

	XrInstance GetOpenXRInstance() const;
	void SetupActionSet();
	void DestroyActionSet();

private:
	/** IOpenXRExtensionPlugin implementation */
	virtual bool GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
	virtual void PostCreateInstance(XrInstance InInstance) override;
	virtual void PostCreateSession(XrSession InSession) override;
#if !UE_VERSION_OLDER_THAN(5, 0, 0)
	virtual void AttachActionSets(TSet<XrActionSet>& OutActionSets) override;
	virtual void GetActiveActionSetsForSync(TArray<XrActiveActionSet>& OutActiveSets) override;
	virtual void BindExtensionPluginDelegates(class IOpenXRExtensionPluginDelegates& OpenXRHMD) override;
#if !UE_VERSION_OLDER_THAN(5, 4, 0)
	virtual bool GetSuggestedBindings(XrPath InInteractionProfile, TArray<XrActionSuggestedBinding>& OutBindings) override;
#endif
#endif

	// We're not overriding OnEvent() here, and not calling haptics_sdk_set_openxr_session_state() when the session state
	// changes. This is not needed, as in UMetaXRHapticsGameInstanceSubsystem we're already calling
	// haptics_sdk_set_suspended() when the headset is removed or put on, which has the same effect.

	// The OpenXR instance, owned by the OpenXRHMD module.
	XrInstance Instance = XR_NULL_HANDLE;

	// The OpenXR session, owned by the OpenXRHMD module.
	XrSession Session = XR_NULL_HANDLE;

	// The action set which contains the haptic output action. This action set is created and owned by this class.
	XrActionSet ActionSet = XR_NULL_HANDLE;

	FMetaXRHapticsModule* const Module = nullptr;
#if !UE_VERSION_OLDER_THAN(5, 0, 0)
	// The plugin delegate. We don't use this for anything apart from checking if the OpenXR plugin is actually
	// used, in IsOpenXRPluginUsed().
	IOpenXRExtensionPluginDelegates* Delegate = nullptr;
#endif
};
