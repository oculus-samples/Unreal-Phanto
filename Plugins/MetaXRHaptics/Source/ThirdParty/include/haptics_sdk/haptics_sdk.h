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
// The Haptics SDK has three backends, an OpenXR backend, an OVRPlugin backend and a callback backend.
//
//
// To initialize the Haptics SDK with the OpenXR backend, perform the following steps in order.
// Performing the initialization steps in the wrong order will most likely result in errors.
//
// 1. Set up logging by calling haptics_sdk_initialize_logging().
// 2. When you create the OpenXR instance with xrCreateInstance(), enable the optional extensions
//    that are supported by the OpenXR runtime.
//    If the optional extensions that are supported by the runtime are not enabled, the SDK may
//    fall back to lower-quality haptic playback and some features may not be available.
//   a. You can get the names of the optional extensions with
//      haptics_sdk_get_openxr_extension_count() and haptics_sdk_get_openxr_extension().
//   b. You can check which of those optional extensions are supported by the OpenXR runtime 
//      with xrEnumerateInstanceExtensionProperties().
// 3. After creating the OpenXR instance, initialize the SDK and pass it the instance with
//    haptics_sdk_initialize_with_openxr().
// 4. After creating the OpenXR session, pass it to the SDK by calling
//    haptics_sdk_set_openxr_session().
// 5. When you set up your OpenXR actions, pass an action set to the SDK by calling
//    haptics_sdk_set_openxr_action_set(). The SDK will add a new action to it, which the SDK will
//    use as an output action for vibrations. You can either use a separate action set that is only
//    used by the SDK, or use the action set that also contains other application actions.
// 6. Bind the output action by including the SDK’s suggested bindings in your call to
//    xrSuggestInteractionProfileBindings().
//   a. Use haptics_sdk_get_openxr_suggested_binding_count() and
//      haptics_sdk_get_openxr_suggested_binding() to get the suggested bindings.
// 7. Attach the action set to the OpenXR session with xrAttachSessionActionSets().
// 8. To save power when the system is idle, whenever the OpenXR session state changes,
//    call haptics_sdk_set_openxr_session_state() with the updated state.
//
// Playback operations will only be available when the OpenXR session's state is in the
// XR_SESSION_STATE_FOCUSED state.
//
//
// To initialize the Haptics SDK with the OVRPlugin backend, perform the following steps in order:
// 1. Set up logging by calling haptics_sdk_initialize_logging().
// 2. Initialize the SDK by calling haptics_sdk_initialize_with_ovr_plugin().
//
//
// To initialize the Haptics SDK with the callback backend, perform the following steps in order:
// 1. Define a callback function adhering to the signature provided by HapticsSdkPlayCallback.
//    The body of the callback function is intended to handle haptic playback data by forwarding
//    the playback data (duration, amplitude) to haptic play calls provided by your engine's or 
//    application's haptic playback API.
// 3. Initialize the SDK by calling haptics_sdk_initialize_with_callback_backend() with a pointer
//    to the callback function defined in the first step. 
//   a. You must ensure that the provided pointers are valid for the lifetime of the application
//      and are thread-safe.
//   b. If needed, also provide a context pointer. Otherwise, set the context pointer to a nullptr.
//
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
#include <openxr/openxr.h>

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
  /**
   * invalid play callback pointer provided
   */
  HAPTICS_SDK_INVALID_PLAY_CALLBACK_POINTER = -15,
  /**
   * invalid playback position
   */
  HAPTICS_SDK_PLAYER_INVALID_SEEK_POSITION = -16,
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

/**
 * Type definition for the Haptics SDK play callback function
 *
 * This callback is invoked by the callback backend when a sample is rendered,
 * providing information about the event's target location, duration, and amplitude.
 *
 * - context: A pointer to the context associated with the haptic callback.
 * - location: An enumerated value of type [HapticsSdkController] indicating the location of
 *   the haptic feedback (e.g., left controller, or right controller).
 * - duration: A floating-point value representing the duration of the haptic event in seconds.
 * - amplitude: A floating-point value representing the amplitude (intensity) of the haptic event.
 *     The value range is 0.0 to 1.0 (inclusive).
 *
 * See also:
 *   haptics_sdk_initialize_with_callback_backend()
 */
typedef void (*HapticsSdkPlayCallback)(void *context,
                                       enum HapticsSdkController location,
                                       float duration,
                                       float amplitude);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Returns the Haptics SDK's version
 */
struct HapticsSdkVersion haptics_sdk_version(void);

/**
 * Sets up logging
 *
 * This function initializes the logging feature of the Haptics SDK. Once activated, the SDK will
 * start generating log messages.
 *
 * The logging feature can only be initialized once. Any subsequent attempts to initialize logging
 * will be ignored.
 * If this function is not explicitly called, the logging feature will be automatically initialized
 * during the SDK initialization, with the default logging mechanism. In this case, function calls
 * before SDK initialization will not log any messages. It is therefore recommended to explicitly
 * call haptics_sdk_initialize_logging() early, before SDK initialization and before any other
 * function calls, to ensure those early function calls generate log messages as well.
 *
 * log_callback specifies a callback function that will be invoked for every log message. It is
 * optional and can be null, in which case the default logging mechanism will be used instead.
 *
 * The default logging mechanism is:
 * - On Android, the SDK logs to the Android log (logcat)
 * - On Windows, the SDK logs to the standard output
 *
 * See also:
 *   HapticsSdkLogCallback
 */
HapticsSdkResult haptics_sdk_initialize_logging(HapticsSdkLogCallback log_callback);

/**
 * Initializes the SDK with the 'null' backend
 *
 * The null backend is useful when developing on a platform without haptics.
 * See also:
 *   haptics_sdk_uninitialize()
 *   haptics_sdk_initialized()
 */
HapticsSdkResult haptics_sdk_initialize_with_null_backend(void);

/**
 * Initializes the SDK with the callback backend
 *
 * This backend is used to initialize the SDK with a callback adhering to the signature of
 * [HapticsSdkPlayCallback]. The callback is set up to provide haptic playback data to the
 * application.
 * The playback data is intended to be passed to haptic API calls capable of duration and
 * amplitude control of a haptic vibration event.
 *
 * To use the callback, the callback backend needs to be initialized with a valid
 * pointer by calling [haptics_sdk_initialize_with_callback_backend()].
 * You must ensure that the provided pointers are valid for the lifetime of the application
 * and that they are thread-safe.
 *
 * - context_pointer (void*): Pointer to a user-provided context object. This is the exact
 *     same value passed to [haptics_sdk_initialize_with_callback_backend()].
 * - callback_pointer [HapticsSdkPlayCallback]: The haptic play callback function.
 * - The callback function is expected to have the signature of type [HapticsSdkPlayCallback]
 * - Playback data is being generated at a rate of 50 Hz, consisting of duration and amplitude data.
 * - Returns HAPTICS_SDK_SUCCESS if initializing the callback backend was successful
 * - Returns HAPTICS_SDK_INVALID_PLAY_CALLBACK_POINTER if the provided play callback pointer
 *     was invalid (i.e. a null pointer)
 * - Returns HAPTICS_SDK_INSTANCE_INITIALIZATION_FAILED if the initialization failed
 * - Returns HAPTICS_SDK_INSTANCE_ALREADY_INITIALIZED if an instance of the SDK is already initialized
 *
 * See also:
 *   HapticsSdkPlayCallback
 *   haptics_sdk_uninitialize()
 *   haptics_sdk_initialized()
 */
HapticsSdkResult haptics_sdk_initialize_with_callback_backend(void *context_pointer,
                                                              HapticsSdkPlayCallback callback_pointer);

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
 * - A player can be in a Stopped, Playing, or Paused state.
 *   - By default a player is in a Stopped state. A player returns to the Stopped state when
 *     the loaded clip reaches its end during play back, or by calling haptics_sdk_player_stop().
 *   - When calling haptics_sdk_player_play() the player enters the Playing state.
 *   - A player in the Playing state can enter a Paused state using haptics_sdk_player_pause().
 *   - Playback can be resumed (i.e. Playing) from the current Paused play back position by
 *     calling haptics_sdk_player_play() or haptics_sdk_player_resume().
 *     - Calling haptics_sdk_player_resume() on a Playing player is a no-op.
 *     - Calling haptics_sdk_player_play() on a Playing player makes it play again from the start.
 * - Returns HAPTICS_SDK_SUCCESS if successful, and an error code if making the player failed.
 * - On success, puts the player identifier into the player_id_out argument.
 * - On failure, puts HAPTICS_SDK_INVALID_ID into the player_id_out argument.
 *
 * See also:
 *   haptics_sdk_release_player()
 *   haptics_sdk_player_set_clip()
 *   haptics_sdk_player_play()
 *   haptics_sdk_player_pause()
 *   haptics_sdk_player_resume()
 *   haptics_sdk_player_stop()
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
 * - If the clip player is currently playing or stopped, calling haptics_sdk_player_play()
 *   starts playback on the clip player from the beginning.
 * - If the clip player is currently paused, calling haptics_sdk_player_play() will
 *   resume playback at the current playback position, and on the playback location
 *   defined by the controller argument.
 *
 * See also:
 *   haptics_sdk_create_player()
 *   haptics_sdk_player_stop()
 */
HapticsSdkResult haptics_sdk_player_play(int32_t player_id, enum HapticsSdkController controller);

/**
 * Pauses playback on a clip player
 *
 * Returns HAPTICS_SDK_SUCCESS if successful.
 * Returns an error code if the player ID is invalid, the player has no clip set,
 * or if there was an error pausing the playback.
 *
 * - A clip player in a paused state does not produce any haptic output, but maintains its
 *   current playback position as long as the player's clip is not changed by calling
 *   haptics_sdk_player_set_clip(), or upon being stopped by calling haptics_sdk_player_stop().
 * - The playback position of a paused player can be shifted by calling haptics_sdk_player_seek().
 * - Setting the amplitude and frequency shift still works and will be picked up as soon
 *   as the player resumes playback or is restarted.
 * - A clip player in a paused state can:
 *   - resume playback from the playback position it was paused at by calling haptics_sdk_player_resume()
 *     or haptics_sdk_player_play(), which allows to change the target playback location.
 *   - be stopped by calling haptics_sdk_player_stop()
 *
 * See also:
 *   haptics_sdk_create_player()
 *   haptics_sdk_player_play()
 *   haptics_sdk_player_resume()
 *   haptics_sdk_player_stop()
 */
HapticsSdkResult haptics_sdk_player_pause(int32_t player_id);

/**
 * Resumes playback on a clip player
 *
 * Returns HAPTICS_SDK_SUCCESS if successful.
 * Returns an error code if the player ID is invalid, the player has no clip set,
 * or if there was an error resuming the playback.
 *
 * - Only a player that was previously paused by calling haptics_sdk_player_pause()
 *   can be resumed.
 * - Resuming a player that is not paused (e.g. stopped or playing) has no effect (no-op).
 *
 * See also:
 *   haptics_sdk_create_player()
 *   haptics_sdk_player_play()
 *   haptics_sdk_player_pause()
 *   haptics_sdk_player_stop()
 */
HapticsSdkResult haptics_sdk_player_resume(int32_t player_id);

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
 * Seek the playback position on a clip player
 *
 * Returns HAPTICS_SDK_SUCCESS if successful.
 * Returns an error code if the player ID is invalid, the player has no clip set,
 * the time is out of the clip's playback range, or if there was an error seeking playback.
 *
 * The time argument is used to set the current playback position of the clip in seconds, and must be
 * between 0.0 and the loaded clip's duration.
 * To get the duration of the currently loaded clip, see haptics_sdk_clip_duration().
 * For example, setting time to 0.0 means that the clip is set to its initial playback position at the
 * beginning of the clip.
 *
 * - Calling seek() on an already playing player will immediately continue playback
 *   from the seeked playback position.
 * - A paused player will stay paused, but the playback position will be moved to the
 *   seeked time value.
 * - If a player is stopped, calling seek() will move the player to a paused state
 *   and set the default playback location to both controllers.
 * - For more information on playback states, see haptics_sdk_create_player().
 *
 * See also:
 *   haptics_sdk_create_player()
 *   haptics_sdk_player_play()
 *   haptics_sdk_player_pause()
 *   haptics_sdk_player_resume()
 */
HapticsSdkResult haptics_sdk_player_seek(int32_t player_id,
                                         float time);

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
 * The shift_amount argument must be between -1.0 and 1.0, and the default value is 0.0.
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
 * Puts the number of optional OpenXR extensions into the extension_count_out parameter
 *
 * After getting this count, haptics_sdk_get_openxr_extension() should be called repeatedly to
 * retrieve the optional extension names.
 *
 * Returns HAPTICS_SDK_SUCCESS if successful, and an error code if the function failed to retrieve the
 * list of optional extensions.
 *
 * See also:
 *   haptics_sdk_get_openxr_extension()
 */
HapticsSdkResult haptics_sdk_get_openxr_extension_count(int32_t *extension_count_out);

/**
 * Returns a pointer to one of the optional OpenXR extensions
 *
 * The number of optional extensions should be retrieved by calling
 * haptics_sdk_get_openxr_extension_count().
 *
 * Extension indices start at 0, with the intention that the extensions can be retrieved and
 * enabled using a for loop.
 *
 * The optional extensions that are supported by the OpenXR runtime should be enabled in the
 * call to xrCreateInstance().
 *
 * NULL will be returned if an invalid extension index is provided.
 * The returned pointer is guaranteed to be valid for the lifetime of the application.
 *
 * See also:
 *   haptics_sdk_get_openxr_extension_count()
 */
const char *haptics_sdk_get_openxr_extension(uint32_t extension_index);

/**
 * Initializes the SDK with the OpenXR backend
 *
 * Initializing the SDK will start the haptics thread and load the OpenXR loader library.
 *
 * This also sets the OpenXR instance to be used by the Haptics SDK. Before setting the OpenXR
 * instance, appropriate OpenXR extensions should be enabled if supported by the OpenXR runtime.
 * The list of optional extensions can be retrieved via calls to haptics_sdk_get_openxr_extension().
 *
 * If the optional extensions that are supported by the runtime are not enabled, the SDK may
 * fall back to lower-quality haptic playback and some features may not be available.
 *
 * After this initialization, playback operations won't be available until
 * haptics_sdk_set_openxr_session() and haptics_sdk_set_openxr_action_set() have been called.
 *
 * An error code will be returned if the SDK failed to initialize its OpenXR backend.
 *
 * See also:
 *   haptics_sdk_initialize_with_null_backend()
 *   haptics_sdk_uninitialize()
 *   haptics_sdk_initialized()
 *   haptics_sdk_get_openxr_extension()
 *   haptics_sdk_set_openxr_action_set()
 *   haptics_sdk_set_openxr_session()
 */
HapticsSdkResult haptics_sdk_initialize_with_openxr(XrInstance openxr_instance);

/**
 * Sets the OpenXR session to be used by the Haptics SDK
 *
 * This must be called after initializing the SDK via haptics_sdk_initialize_with_openxr().
 *
 * After the session has been successfully set, playback operations will be unavailable until an
 * action set has been provided via haptics_sdk_set_openxr_action_set().
 *
 * Playback operations will also only be available when the session is in the
 * XR_SESSION_STATE_FOCUSED state.
 *
 * Returns HAPTICS_SDK_SUCCESS if successful, and an error code if the OpenXR instance hasn't yet
 * been provided to the Haptics SDK.
 *
 * See also:
 *   haptics_sdk_initialize_with_openxr()
 *   haptics_sdk_set_openxr_action_set()
 *   haptics_sdk_set_openxr_session_state()
 */
HapticsSdkResult haptics_sdk_set_openxr_session(XrSession openxr_session);

/**
 * Sets the Action Set to which the Haptics SDK should attach generated haptic actions
 *
 * Once the OpenXR session has been set via haptics_sdk_set_openxr_session(),
 * this can be called repeatedly during a session as the appropriate action set changes.
 *
 * After setting a new action set, suggested interaction profile bindings should be retrieved
 * via haptics_sdk_get_openxr_suggested_binding().
 *
 * After calling this function, the action set needs to be:
 *   - Included in the suggested bindings, by calling xrSuggestInteractionProfileBindings() with
 *     the bindings provided by repeated calls to haptics_sdk_get_openxr_suggested_binding().
 *   - Attached to the OpenXR session, by including the action set in xrAttachSessionActionSets().
 *
 * Returns HAPTICS_SDK_SUCCESS if successful, and an error code may be returned if an error occurs
 * while setting the new action set.
 *
 * See also:
 *   haptics_sdk_get_openxr_suggested_binding()
 */
HapticsSdkResult haptics_sdk_set_openxr_action_set(XrActionSet action_set);

/**
 * Puts the number of suggested bindings for use with the Haptics SDK into the suggested_binding_count_out argument.
 *
 * After getting the number of suggested bindings, haptics_sdk_get_openxr_suggested_binding()
 * should be called repeatedly to retrieve each suggested binding.
 *
 * Returns HAPTICS_SDK_SUCCESS if successful, and an error code if getting the bindings failed, or the backend
 * was not accessible.
 *
 * See also:
 *   haptics_sdk_get_openxr_suggested_binding()
 */
HapticsSdkResult haptics_sdk_get_openxr_suggested_binding_count(int32_t *suggested_binding_count_out);

/**
 * Returns a suggested OpenXR interaction profile binding
 *
 * After setting an action set via haptics_sdk_set_openxr_action_set(),
 * new bindings for use with the Haptics SDK should be included in a call to
 * xrSuggestInteractionProfileBindings().
 *
 * The number of suggested bindings should be retrieved via a call to
 * haptics_sdk_get_openxr_suggested_binding_count(). Binding indices start at 0,
 * with the intention that the extensions can be retrieved using a for loop.
 *
 * If an invalid binding index is provided, a binding containing a null path
 * and a null action handle will be returned.
 *
 * See also:
 *   haptics_sdk_get_openxr_suggested_binding_count()
 */
XrActionSuggestedBinding haptics_sdk_get_openxr_suggested_binding(uint32_t index);

/**
 * Suspends or wakes up the haptic runtime based on the OpenXR session state
 *
 * This can be used to save power by telling the Haptics SDK to suspend updates while the OpenXR
 * session isn't focused.
 *
 * This should be called whenever the OpenXR session state changes. A good place to call this is in
 * the handler of the XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED event, after polling it with
 * xrPollEvent().
 *
 * Returns HAPTICS_SDK_SUCCESS if successful, and an error code otherwise.
 */
HapticsSdkResult haptics_sdk_set_openxr_session_state(XrSessionState openxr_session_state);

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
 * See also:
 *   haptics_sdk_uninitialize()
 *   haptics_sdk_initialized()
 */
HapticsSdkResult haptics_sdk_initialize_with_ovr_plugin(const char *game_engine_name,
                                                        const char *game_engine_version,
                                                        const char *game_engine_haptics_sdk_version);

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
