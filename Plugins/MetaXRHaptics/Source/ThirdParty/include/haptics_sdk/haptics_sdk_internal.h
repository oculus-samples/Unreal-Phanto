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

// Declarations for internal functions of the Haptics SDK that are not in the public header
extern "C"
{
  typedef struct HapticsSdkNullBackendStats
  {
    int64_t stream_count;
    int64_t play_call_count;
  } HapticsSdkNullBackendStats;

  HapticsSdkNullBackendStats haptics_sdk_get_null_backend_statistics(void);

  HapticsSdkResult haptics_sdk_initialize_with_openxr_from_game_engine(
      XrInstance openxr_instance,
      const char *game_engine_name,
      const char *game_engine_version,
      const char *game_engine_haptics_sdk_version);

  HapticsSdkResult haptics_sdk_create_openxr_action_set(XrActionSet *action_set_out);

  HapticsSdkResult haptics_sdk_destroy_openxr_action_set(XrActionSet action_set);
}
