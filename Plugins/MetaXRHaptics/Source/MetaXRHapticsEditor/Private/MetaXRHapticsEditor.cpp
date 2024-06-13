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

#include "MetaXRHapticsEditor.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FMetaXRHapticsEditorModule"

void FMetaXRHapticsEditorModule::StartupModule()
{
	HapticClipAssetTypeActions = MakeShared<FMetaXRHapticClipAssetTypeActions>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(HapticClipAssetTypeActions.ToSharedRef());

	SetupStyle();

	if (Style.IsValid())
	{
		FSlateStyleRegistry::RegisterSlateStyle(*Style);
	}
}

void FMetaXRHapticsEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(HapticClipAssetTypeActions.ToSharedRef());
	}
	HapticClipAssetTypeActions.Reset();

	if (Style.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(Style->GetStyleSetName());
		Style.Reset();
	}
}

void FMetaXRHapticsEditorModule::SetupStyle()
{
	const FString PluginBaseDir = IPluginManager::Get().FindPlugin("MetaXRHaptics")->GetBaseDir();
	const FString IconPath = FPaths::Combine(*PluginBaseDir, TEXT("Resources/HapticClipIcon128.png"));

	if (!FPaths::FileExists(IconPath))
	{
		return;
	}

	Style = MakeShareable(new FSlateStyleSet("MetaXRHapticsStyle"));
	const FVector2D Icon128x128(128.0f, 128.0f);
	FSlateImageBrush* SlateImageBrush = new FSlateImageBrush(IconPath, Icon128x128);
	Style->Set("ClassThumbnail.MetaXRHapticClip", SlateImageBrush);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMetaXRHapticsEditorModule, MetaXRHapticsEditor)
