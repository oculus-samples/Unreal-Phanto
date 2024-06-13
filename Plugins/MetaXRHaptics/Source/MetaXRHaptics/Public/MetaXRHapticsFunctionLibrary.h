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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MetaXRHapticsFunctionLibrary.generated.h"

class UMetaXRHapticsPlayerComponent;

UCLASS()
class METAXRHAPTICS_API UMetaXRHapticsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Spawns a UMetaXRHapticsPlayerComponent and attaches it to an Actor.
	 *
	 * Always spawn new player components with this method, Unreal's built-in "Add Haptics SDK Player"
	 * will lead to initialization errors.
	 *
	 * When playback finishes, the component will *not* automatically destroy itself.
	 *
	 * @param ActorToAttachTo Actor to which this component will be attached to.
	 * @param HapticClip Haptic clip to load.
	 * @param Controller Controller used to play haptics.
	 * @param Priority Playback priority, ranging from 0 (low priority) to 1024 (high priority).
	 * @param Amplitude Amplitude scale, ranging from 0.0 to infinite.
	 * @param FrequencyShift Frequency shift, ranging from -1.0 to 1.0.
	 * @param bIsLooping Whether playback should loop.
	 *
	 * @return A UMetaXRHapticsPlayerComponent that plays back the haptic clip, and can be used to
	 *         control playback.
	 */
	UFUNCTION(BlueprintCallable, Category = "MetaXR|Haptics", meta = (AdvancedDisplay = "3"))
	static UMetaXRHapticsPlayerComponent* SpawnHapticsPlayerComponent(
		AActor* ActorToAttachTo,
		UMetaXRHapticClip* HapticClip,
		const EMetaXRHapticController Controller,
		const int32 Priority = 512,
		const float Amplitude = 1.0f,
		const float FrequencyShift = 0.0f,
		const bool bIsLooping = false);
};
