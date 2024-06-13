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

#include "MetaXRHapticClipFactory.h"
#include "MetaXRHapticClip.h"
#include "MetaXRHaptics/Private/MetaXRHaptics.h"
#include "Misc/FileHelper.h"

UMetaXRHapticClipFactory::UMetaXRHapticClipFactory()
{
	SupportedClass = UMetaXRHapticClip::StaticClass();
	bEditorImport = true;
	Formats.Add("haptic;Haptic Clip Data");
}

bool UMetaXRHapticClipFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	return Cast<UMetaXRHapticClip>(Obj) != nullptr;
}

void UMetaXRHapticClipFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	ReimportPath.Empty();
	UMetaXRHapticClip* const clip = Cast<UMetaXRHapticClip>(Obj);
	if (!clip)
	{
		UE_LOG(LogHapticsSDK, Error, TEXT("Unable to reimport haptic clip '%s', unexpected object type."),
			*Obj->GetName());
		return;
	}

	if (NewReimportPaths.Num() != 1)
	{
		UE_LOG(LogHapticsSDK, Error, TEXT("Unable to reimport haptic clip '%s', unexpected amount of reimport paths."),
			*clip->GetName());
		return;
	}

	ReimportPath = NewReimportPaths[0];
}

EReimportResult::Type UMetaXRHapticClipFactory::Reimport(UObject* Obj)
{
	UMetaXRHapticClip* const clip = Cast<UMetaXRHapticClip>(Obj);
	if (!clip)
	{
		UE_LOG(LogHapticsSDK, Error, TEXT("Unable to reimport haptic clip '%s', unexpected object type."),
			*Obj->GetName());
		return EReimportResult::Failed;
	}

	if (ReimportPath.IsEmpty())
	{
		// If no reimport path has been set, SetReimportPaths() will already have logged an error.
		return EReimportResult::Failed;
	}

	if (!FFileHelper::LoadFileToArray(clip->ClipData, *ReimportPath))
	{
		UE_LOG(LogHapticsSDK, Error, TEXT("Unable to reimport haptic clip '%s', failed to load file '%s'."),
			*clip->GetName(), *ReimportPath);
		return EReimportResult::Failed;
	}

	clip->Modify();
	clip->MarkPackageDirty();
	ReimportPath.Empty();

	return EReimportResult::Succeeded;
}

UObject* UMetaXRHapticClipFactory::FactoryCreateBinary(UClass* InClass,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	UObject* Context,
	const TCHAR* Type,
	const uint8*& Buffer,
	const uint8* BufferEnd,
	FFeedbackContext* Warn)
{
	check(InClass->IsChildOf(UMetaXRHapticClip::StaticClass()));
	UMetaXRHapticClip* const HapticClip = NewObject<UMetaXRHapticClip>(InParent, InClass, InName, Flags, Context);

	const int32 BufferSize = BufferEnd - Buffer;

	if (BufferSize <= 0)
	{
		return nullptr;
	}

	HapticClip->ClipData.SetNumUninitialized(BufferSize);
	FMemory::Memcpy(HapticClip->ClipData.GetData(), Buffer, BufferSize);
	return HapticClip;
}
