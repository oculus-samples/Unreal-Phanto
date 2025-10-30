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

#include "CoreMinimal.h"
#include "MetaXRHapticClip.generated.h"

/**
 * This helper class allows the creation of HapticClip.uasset files by dragging and dropping a .haptic file in the editor.
 * Developers do not need to interact with this class directly.
 * Assign the created asset to a UMetaXRHapticsPlayerComponent to play it.
 */
UCLASS(BlueprintType)
class METAXRHAPTICS_API UMetaXRHapticClip : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Data from the imported .haptic file.
	 */
	UPROPERTY()
	TArray<uint8> ClipData;
};
