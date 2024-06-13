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

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Meta Haptics Native SDK
// =======================
//
// SDK initialization
// ------------------
//
// This header provides a C interface to the Meta Haptics Native SDK.
// 
// To enable the SDK for use with OVRPlugin, call haptics_sdk_initialize_with_ovr_plugin.
//
// When haptics are no longer required, SDK resources can be cleaned up by calling
// haptics_sdk_uninitialize().
//
//
// Thread Safety
// -------------
//
// All SDK functions can be called from any thread. If you call multiple functions from more than
// one thread at the same time, subsequent functions will block and wait until the first function
// has finished. This is because the SDK uses a global mutex internally.
//
//
// Errors
// ------
//
// Functions return a value from the HapticsSdkResult enum. The HAPTICS_SDK_SUCCEEDED and
// HAPTICS_SDK_FAILED macros are available for checking the result.
// When an error is encountered, call haptics_sdk_error_message() to retrieve an error message
// with information about what went wrong.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __haptics_sdk_h__
#define __haptics_sdk_h__

#pragma once

#define HAPTICS_SDK_SUCCEEDED(result) ((result) >= 0)
#define HAPTICS_SDK_FAILED(result) ((result) < 0)

#include <stdint.h>

/**
 * Provides a constant for functions using identifiers, such as player and clip instances.
 * On error, the identifier is set to this constant to provide a defined invalid state.
 */
#define HAPTICS_SDK_INVALID_ID -1

/**
 * A selector for specifying on which controller a haptic should play
 */
typedef enum HapticsSdkController {
  HAPTICS_SDK_CONTROLLER_LEFT = 0,
  HAPTICS_SDK_CONTROLLER_RIGHT = 1,
  HAPTICS_SDK_CONTROLLER_BOTH = 2,
} HapticsSdkController;

/**
 * The log level that is passed to HapticsSdkLogCallback
 *
 * The higher the value, the more important the message.
 */
enum HapticsSdkLogLevel
#ifdef __cplusplus
  : int32_t
#endif // __cplusplus
 {
  HAPTICS_SDK_LOG_LEVEL_TRACE = 0,
  HAPTICS_SDK_LOG_LEVEL_DEBUG = 1,
  HAPTICS_SDK_LOG_LEVEL_INFO = 2,
  HAPTICS_SDK_LOG_LEVEL_WARN = 3,
  HAPTICS_SDK_LOG_LEVEL_ERROR = 4,
};
#ifndef __cplusplus
typedef int32_t HapticsSdkLogLevel;
#endif // __cplusplus

/**
 * HapticsSdkResult contains the success and error values used by the Haptics SDK.
 * haptics_sdk_error_message() can be called for more information.
 */
enum HapticsSdkResult
#ifdef __cplusplus
  : int32_t
#endif // __cplusplus
 {
  /**
   * Returned on success.
   */
  HAPTICS_SDK_SUCCESS = 0,
  /**
   * unspecified errors, not covered by the other error codes below.
   */
  HAPTICS_SDK_ERROR = -1,
  /**
   * instance failed to be created.
   */
  HAPTICS_SDK_INSTANCE_INITIALIZATION_FAILED = -2,
  /**
   * instance already initialized.
   */
  HAPTICS_SDK_INSTANCE_ALREADY_INITIALIZED = -3,
  /**
   * instance already uninitialized.
   */
  HAPTICS_SDK_INSTANCE_ALREADY_UNINITIALIZED = -4,
  /**
   * instance has not been initialized.
   */
  HAPTICS_SDK_INSTANCE_NOT_INITIALIZED = -5,
  /**
   * haptic clip data was not valid UTF-8.
   */
  HAPTICS_SDK_INVALID_UTF8 = -6,
  /**
   * failed to load a clip.
   */
  HAPTICS_SDK_LOAD_CLIP_FAILED = -7,
  /**
   * instance failed to create a player.
   */
  HAPTICS_SDK_CREATE_PLAYER_FAILED = -8,
  /**
   * clip ID is invalid.
   */
  HAPTICS_SDK_CLIP_ID_INVALID = -9,
  /**
   * player ID is invalid.
   */
  HAPTICS_SDK_PLAYER_ID_INVALID = -10,
  /**
   * value for setting the amplitude is negative.
   */
  HAPTICS_SDK_PLAYER_INVALID_AMPLITUDE = -11,
  /**
   * value for setting the frequency shift is invalid (out of range).
   */
  HAPTICS_SDK_PLAYER_INVALID_FREQUENCY_SHIFT = -12,
  /**
   * value for setting the player priority is invalid (out of range).
   */
  HAPTICS_SDK_PLAYER_INVALID_PRIORITY = -13,
  /**
   * player has no clip loaded
   */
  HAPTICS_SDK_NO_CLIP_LOADED = -14,
};
#ifndef __cplusplus
typedef int32_t HapticsSdkResult;
#endif // __cplusplus

/**
 * The version information returned by haptics_sdk_version()
 */
typedef struct HapticsSdkVersion {
  uint16_t major;
  uint16_t minor;
  uint16_t patch;
} HapticsSdkVersion;

/**
 * The function for the log callback
 *
 * If this callback was set during initialization, the callback will be invoked for each log
 * message the Haptics SDK would otherwise send to the default logging mechanism. The default
 * logging mechanism is the Android log on Android, and stdout/stderr on all other platforms.
 *
 * The second parameter is the log message, which is UTF-8 encoded and null terminated. The Haptics
 * SDK retains ownership of the message, the callback must not free its memory. The message is only
 * valid during the callback invocation.
 *
 * The callback will be invoked from different threads, potentially concurrently. Therefore the
 * callback must be thread safe.
 */
typedef void (*HapticsSdkLogCallback)(HapticsSdkLogLevel, const char*);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Returns the Haptics SDK's version
 */
struct HapticsSdkVersion haptics_sdk_version(void);

/**
 * Initializes the SDK with the 'null' backend
 *
 * The null backend is useful when developing on a platform without haptics.
 *
 * log_callback specifies a callback function that will be invoked for every log message. It is
 * optional and can be null, in which case the default logging mechanism will be used instead.
 * When calling this function multiple times, only the log_callback from the first call will be
 * used, the log_callback in subsequent calls will be ignored.
 *
 * See also:
 *   haptics_sdk_uninitialize()
 *   haptics_sdk_initialized()
 *   HapticsSdkLogCallback
 */
HapticsSdkResult haptics_sdk_initialize_with_null_backend(HapticsSdkLogCallback log_callback);

/**
 * Uninitializes the SDK
 *
 * This will free up any resources used by the SDK instance, including clips and clip players.
 *
 * This will wait until the internal threads have exited, to ensure a deterministic shutdown
 * process.
 *
 * See also:
 *   haptics_sdk_initialized()
 */
HapticsSdkResult haptics_sdk_uninitialize(void);

/**
 * Puts whether the SDK is initialized into the initialized_out argument
 *
 * Returns HAPTICS_SDK_SUCCESS if the call was successful, and an error code otherwise.
 * If there was an error, the value pointed to by initialized_out will not be changed.
 *
 * See also:
 *   haptics_sdk_uninitialize()
 */
HapticsSdkResult haptics_sdk_initialized(bool *initialized_out);

/**
 * Loads a haptic clip from JSON string data
 *
 * - The data argument is expected to point to UTF-8 encoded JSON data.
 * - The data_size argument is the size in bytes of the JSON data,
 *   not including any null-terminator that may or may not be at the end of the buffer.
 * - After the clip has been created, the provided JSON data can be discarded.
 * - Returns HAPTICS_SDK_SUCCESS if the data is parsed successfully, and an error code otherwise.
 * - On success, puts the clip identifier into the clip_id_out argument.
 * - On failure, puts HAPTICS_SDK_INVALID_ID into the clip_id_out argument.
 *
 * See also:
 *   haptics_sdk_release_clip()
 */
HapticsSdkResult haptics_sdk_load_clip(const char *data, uint32_t data_size, int32_t *clip_id_out);

/**
 * Puts the clip duration, in seconds, into the duration_out argument
 *
 * The clip_id argument is the clip identifier returned by haptics_sdk_load_clip().
 *
 * Returns HAPTICS_SDK_SUCCESS if the clip duration was successfully retrieved, and an error code otherwise.
 */
HapticsSdkResult haptics_sdk_clip_duration(int32_t clip_id,
                                           float *duration_out);

/**
 * Releases ownership of a loaded haptic clip
 *
 * Releasing the clip means that the API no longer maintains a handle to the clip,
 * however resources won't be freed until all clip players that are playing the clip have also
 * been destroyed.
 *
 * Returns HAPTICS_SDK_SUCCESS if the clip was successfully released, and an error code otherwise.
 */
HapticsSdkResult haptics_sdk_release_clip(int32_t clip_id);

/**
 * Creates a haptic clip player
 *
 * - By default a player has no haptic clip assigned to play back.
 * - To assign a clip to the player, use haptics_sdk_player_set_clip().
 * - The loaded clip can later be changed using haptics_sdk_player_set_clip() again.
 * - Returns HAPTICS_SDK_SUCCESS if successful, and an error code if making the player failed.
 * - On success, puts the player identifier into the player_id_out argument.
 * - On failure, puts HAPTICS_SDK_INVALID_ID into the player_id_out argument.
 *
 * See also:
 *   haptics_sdk_release_player()
 *   haptics_sdk_player_set_clip()
 */
HapticsSdkResult haptics_sdk_create_player(int32_t *player_id_out);

/**
 * Releases a clip player that was previously created with haptics_sdk_create_player()
 *
 * Returns HAPTICS_SDK_SUCCESS if the call was successful, and an error code if the
 * player ID was invalid or releasing the player failed.
 *
 * See also:
 *   haptics_sdk_create_player()
 */
HapticsSdkResult haptics_sdk_release_player(int32_t player_id);

/**
 * Sets the clip that is used by the given player
 *
 * - If the player is currently playing, it will stop. Other properties like amplitude, frequency
 *   shift, looping and priority are kept.
 * - The clip must have been previously loaded by the API by calling haptics_sdk_load_clip().
 * - A clip can be loaded by multiple players.
 * - Returns HAPTICS_SDK_SUCCESS if successful. Otherwise an error code is returned, for example when
 *   an invalid player_id or clip_id is passed.
 *
 * See also:
 *   haptics_sdk_create_player()
 *   haptics_sdk_load_clip()
 */
HapticsSdkResult haptics_sdk_player_set_clip(int32_t player_id,
                                             int32_t clip_id);

/**
 * Starts playback on a clip player
 *
 * Returns HAPTICS_SDK_SUCCESS if successful.
 * Returns an error code if the player ID is invalid, the player has no clip set,
 * or if there was an error starting the playback.
 *
 * See also:
 *   haptics_sdk_create_player()
 *   haptics_sdk_player_stop()
 */
HapticsSdkResult haptics_sdk_player_play(int32_t player_id, enum HapticsSdkController controller);

/**
 * Stops playback on a clip player
 *
 * Returns HAPTICS_SDK_SUCCESS if successful.
 * Returns an error code if the player ID is invalid, the player has no clip set,
 * or if there was an error stopping the playback.
 *
 * See also:
 *   haptics_sdk_player_play()
 */
HapticsSdkResult haptics_sdk_player_stop(int32_t player_id);

/**
 * Sets a clip player's amplitude
 *
 * During playback, the amplitudes from the clip will be multiplied by the player's amplitude,
 * changing the overall strength of the vibration.
 *
 * Amplitude values in a clip are in the range of 0.0 to 1.0,
 * and the result after applying the amplitude scale will be clipped to that range.
 *
 * The amplitude argument must be 0.0 or larger, and the default value is 1.0.
 * An amplitude of 0.0 means that no vibration will be triggered,
 * and an amplitude of 0.5 will result in the clip being played back at half of its amplitude.
 *
 * This method can be called during active playback, in which case the amplitude is applied
 * immediately, with a small delay in the tens of milliseconds.
 *
 * Returns HAPTICS_SDK_SUCCESS if successful, and an error code if the player ID is invalid or if the
 * amplitude argument is not within the valid range.
 *
 * See also:
 *   haptics_sdk_player_amplitude()
 */
HapticsSdkResult haptics_sdk_player_set_amplitude(int32_t player_id,
                                                  float amplitude);

/**
 * Puts the clip player's amplitude into the amplitude_out argument
 *
 * This is the getter for the associated haptics_sdk_player_set_amplitude setter().
 *
 * Returns HAPTICS_SDK_SUCCESS if the call was successful, and an error code otherwise.
 * If there was an error, the value pointed to by amplitude_out will not be changed.
 */
HapticsSdkResult haptics_sdk_player_amplitude(int32_t player_id, float *amplitude_out);

/**
 * Sets a clip player's frequency shift amount
 *
 * During playback, the shift_amount argument is added to the frequencies of the clip.
 * Frequency values in a clip are in the range from 0.0 to 1.0,
 * and the result after applying the frequency shift will be clipped to that range.
 *
 * The shift_amount arguments must be between -1.0 and 1.0, and the default value is 0.0.
 * A shift amount of 0.0 means that the frequencies will not be changed.
 *
 * This method can be called during active playback, in which case the frequency shift is applied
 * immediately, with a small delay in the tens of milliseconds.
 *
 * Returns HAPTICS_SDK_SUCCESS if successful, and an error code if the player ID is invalid or if
 * the shift_amount argument is not within the valid range.
 *
 * See also:
 *   haptics_sdk_player_frequency_shift()
 */
HapticsSdkResult haptics_sdk_player_set_frequency_shift(int32_t player_id, float shift_amount);

/**
 * Puts a player's frequency shift value into the frequency_shift_out argument
 *
 * This is the getter for the associated haptics_sdk_player_set_frequency_shift() setter.
 *
 * Returns HAPTICS_SDK_SUCCESS if the call was successful, and an error code otherwise.
 * If there was an error, the value pointed to by frequency_shift_out will not be changed.
 */
HapticsSdkResult haptics_sdk_player_frequency_shift(int32_t player_id, float *frequency_shift_out);

/**
 * Enables or disables looped playback for a clip player
 *
 * When looping is enabled, clip playback will be repeated indefinitely.
 *
 * Looping can be enabled or disabled during playback.
 *
 * Returns HAPTICS_SDK_SUCCESS if the call was successful, and an error code
 * if setting the loop status failed.
 *
 * See also:
 *   haptics_sdk_player_looping_enabled()
 */
HapticsSdkResult haptics_sdk_player_set_looping_enabled(int32_t player_id, bool enabled);

/**
 * Puts the clip player's looping enabled state into the looping_enabled_out argument
 *
 * This is the getter for the associated haptics_sdk_player_set_looping_enabled() setter.
 *
 * Returns HAPTICS_SDK_SUCCESS if the call was successful, and an error code otherwise.
 * If there was an error, the value pointed to by looping_enabled_out will not be changed.
 */
HapticsSdkResult haptics_sdk_player_looping_enabled(int32_t player_id, bool *looping_enabled_out);

/**
 * Sets the playback priority of a clip player
 *
 * When multiple players are playing at the same time on an actuator,
 * only the player with the highest priority will trigger vibrations.
 *
 * If multiple players have the same priority level,
 * then the player that was started most recently will trigger vibrations.
 *
 * The priority can be any integer value between 0 (low priority) and
 * 1024 (high priority). The default value is 512.
 *
 * The player's priority can be changed before and during playback.
 *
 * Returns HAPTICS_SDK_SUCCESS if the call was successful, and an error code otherwise.
 *
 * See also:
 *   haptics_sdk_player_priority()
 */
HapticsSdkResult haptics_sdk_player_set_priority(int32_t player_id, uint32_t priority);

/**
 * Puts the clip player's priority into the priority_out argument
 *
 * This is the getter for the associated haptics_sdk_player_set_priority setter().
 *
 * Returns HAPTICS_SDK_SUCCESS if the call was successful, and an error code otherwise.
 * If there was an error, the value pointed to by priority_out will not be changed.
 */
HapticsSdkResult haptics_sdk_player_priority(int32_t player_id, uint32_t *priority_out);

/**
 * Returns a message that describes the error that occurred most recently in the SDK
 *
 * - The error message will be UTF-8 encoded and null-terminated.
 * - The message buffer is owned internally, and is only guaranteed to be valid
 *   until the next call to a haptics_sdk function.
 * - Subsequent successful SDK calls aren't guaranteed to clear the message,
 *   so the message will be stale if it isn't retrieved immediately following an error.
 * - Returns NULL if no error message is available.
 */
const char *haptics_sdk_error_message(void);

/**
 * Initializes the SDK using the OVRPlugin backend
 *
 * This dynamically loads the OVRPlugin library, which requires that the library is in the runtime
 * linker search path. The minimum supported version is v46.
 *
 * The provided string pointers should point to null-terminated UTF-8 encoded string data,
 * and the corresponding data can be discarded after the SDK is initialized.
 *
 * An error code will be returned in the following cases:
 *   - the required symbols for the PCM haptic playback API can not be found.
 *   - any of the provided information string pointers are invalid.
 *
 * log_callback specifies a callback function that will be invoked for every log message. It is
 * optional and can be null, in which case the default logging mechanism will be used instead.
 * When calling this function multiple times, only the log_callback from the first call will be
 * used, the log_callback in subsequent calls will be ignored.
 *
 * See also:
 *   haptics_sdk_uninitialize()
 *   haptics_sdk_initialized()
 *   HapticsSdkLogCallback
 */
HapticsSdkResult haptics_sdk_initialize_with_ovr_plugin(const char *game_engine_name,
                                                        const char *game_engine_version,
                                                        const char *game_engine_haptics_sdk_version,
                                                        HapticsSdkLogCallback log_callback);

/**
 * Suspends or wakes up the haptic runtime
 *
 * A suspended runtime will not run regular updates in the haptics thread, therefore not
 * advancing playback and not triggering vibrations. A suspended runtime will use less power.
 *
 * This should be called whenever the VR focus changes, to reduce power consumption
 * while the user isn't wearing the headset.
 *
 * Returns HAPTICS_SDK_SUCCESS if successful, and an error code if suspending the haptic runtime
 * failed.
 */
HapticsSdkResult haptics_sdk_set_suspended(bool suspended);

/**
 * Puts whether the haptic runtime is suspended into the suspended_out argument
 *
 * This is the getter for the associated haptics_sdk_set_suspended() setter.
 *
 * Returns HAPTICS_SDK_SUCCESS if the call was successful, and an error code otherwise.
 * If there was an error, the value pointed to by suspended_out will not be changed.
 */
HapticsSdkResult haptics_sdk_suspended(bool *suspended_out);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* __haptics_sdk_h__ */
