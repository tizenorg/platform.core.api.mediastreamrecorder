/*
* Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef __TIZEN_MEDIA_STREAMRECORDER_H__
#define __TIZEN_MEDIA_STREAMRECORDER_H__

#include <tizen.h>
#include <media_packet.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file streamrecorder.h
 * @brief This file contains the Streamrecorder API, related structures and enumerations.
 * @since_tizen 3.0
 */

/**
 * @addtogroup CAPI_MEDIA_STREAMRECORDER_MODULE
 * @{
 */

#define STREAMRECORDER_ERROR_CLASS        -0x03100000

/**
 * @brief The Streamrecorder handle.
 * @since_tizen 3.0
 */
typedef struct streamrecorder_s *streamrecorder_h;

/**
 * @brief The buffer type of application that pushes.
 * @since_tizen 3.0
 */
typedef enum {
	STREAMBUFFER_TYPE_VIDEO, /**< Video buffer */
	STREAMBUFFER_TYPE_AUDIO, /**< Audio buffer */
} streamrecorder_buffer_type_e;

/**
 * @brief Enumeration for the pixel format.
 * @since_tizen 3.0
 */
typedef enum {
	STREAMRECORDER_INPUT_FORMAT_INVALID = -1,   /**< Invalid pixel format */
	STREAMRECORDER_INPUT_FORMAT_NV12,           /**< NV12 pixel format */
	STREAMRECORDER_INPUT_FORMAT_NV21,           /**< NV12 pixel format */
	STREAMRECORDER_INPUT_FORMAT_I420,           /**< I420 pixel format */
	STREAMRECORDER_INPUT_FORMAT_NUM             /**< Number of the pixel format */
} streamrecorder_video_source_format_e;

/**
 * @brief Enumeration for the file container format.
 * @since_tizen 3.0
 */
typedef enum {
	STREAMRECORDER_FILE_FORMAT_3GP,    /**< 3GP file format */
	STREAMRECORDER_FILE_FORMAT_MP4,    /**< MP4 file format */
	STREAMRECORDER_FILE_FORMAT_AMR,    /**< AMR file format */
	STREAMRECORDER_FILE_FORMAT_ADTS,   /**< ADTS file format */
	STREAMRECORDER_FILE_FORMAT_WAV,    /**< WAV file format */
} streamrecorder_file_format_e;

/**
 * @brief Enumeration for the audio codec.
 * @since_tizen 3.0
 */
typedef enum {
	STREAMRECORDER_AUDIO_CODEC_DISABLE = -1, /**< Disable audio track */
	STREAMRECORDER_AUDIO_CODEC_AMR = 0,      /**< AMR codec */
	STREAMRECORDER_AUDIO_CODEC_AAC,          /**< AAC codec */
	STREAMRECORDER_AUDIO_CODEC_PCM           /**< PCM codec */
} streamrecorder_audio_codec_e;

/**
 * @brief Enumeration for the video codec.
 * @since_tizen 3.0
 */
typedef enum {
	STREAMRECORDER_VIDEO_CODEC_H263,    /**< H263 codec */
	STREAMRECORDER_VIDEO_CODEC_H264,    /**< H264 codec */
	STREAMRECORDER_VIDEO_CODEC_MPEG4,   /**< MPEG4 codec */
} streamrecorder_video_codec_e;

/**
 * @brief Enumeration for the recording limit.
 * @since_tizen 3.0
 */
typedef enum {
	STREAMRECORDER_RECORDING_LIMIT_TIME,        /**< Time limit (second) of recording file */
	STREAMRECORDER_RECORDING_LIMIT_SIZE,        /**< Size limit (kilo bytes [KB]) of recording file */
	STREAMRECORDER_RECORDING_LIMIT_FREE_SPACE,  /**< No free space in storage */
} streamrecorder_recording_limit_type_e;

/**
 * @brief Enumeration for Streamrecorder error type.
 * @since_tizen 3.0
 */
typedef enum {
	STREAMRECORDER_ERROR_NONE                  = TIZEN_ERROR_NONE,                /**< Successful */
	STREAMRECORDER_ERROR_INVALID_PARAMETER     = TIZEN_ERROR_INVALID_PARAMETER,   /**< Invalid parameter */
	STREAMRECORDER_ERROR_INVALID_STATE         = STREAMRECORDER_ERROR_CLASS | 0x02,     /**< Invalid state */
	STREAMRECORDER_ERROR_OUT_OF_MEMORY         = TIZEN_ERROR_OUT_OF_MEMORY ,      /**< Out of memory */
	STREAMRECORDER_ERROR_DEVICE                = STREAMRECORDER_ERROR_CLASS | 0x04,     /**< Device error */
	STREAMRECORDER_ERROR_INVALID_OPERATION     = TIZEN_ERROR_INVALID_OPERATION,   /**< Internal error */
	STREAMRECORDER_ERROR_SOUND_POLICY          = STREAMRECORDER_ERROR_CLASS | 0x06,     /**< Blocked by Audio Session Manager */
	STREAMRECORDER_ERROR_SOUND_POLICY_BY_CALL  = STREAMRECORDER_ERROR_CLASS | 0x07,     /**< Blocked by Audio Session Manager - CALL */
	STREAMRECORDER_ERROR_SOUND_POLICY_BY_ALARM = STREAMRECORDER_ERROR_CLASS | 0x08,     /**< Blocked by Audio Session Manager - ALARM */
	STREAMRECORDER_ERROR_SECURITY_RESTRICTED   = STREAMRECORDER_ERROR_CLASS | 0x09,     /**< Restricted by security system policy */
	STREAMRECORDER_ERROR_OUT_OF_STORAGE        = STREAMRECORDER_ERROR_CLASS | 0x0a,     /**< Out of storage */
	STREAMRECORDER_ERROR_PERMISSION_DENIED     = TIZEN_ERROR_PERMISSION_DENIED,   /**< The access to the resources can not be granted */
	STREAMRECORDER_ERROR_NOT_SUPPORTED         = TIZEN_ERROR_NOT_SUPPORTED,       /**< The feature is not supported */
} streamrecorder_error_e;

/**
 * @brief Enumeration for the recorder policy.
 * @since_tizen 3.0
 */
typedef enum {
	STREAMRECORDER_POLICY_NONE = 0,       /**< None */
	STREAMRECORDER_POLICY_SOUND,          /**< Sound policy */
	STREAMRECORDER_POLICY_SOUND_BY_CALL,  /**< Sound policy by CALL */
	STREAMRECORDER_POLICY_SOUND_BY_ALARM, /**< Sound policy by ALARM */
	STREAMRECORDER_POLICY_SECURITY        /**< Security policy */
} streamrecorder_policy_e;

/**
 * @brief Enumeration for Streamrecorder states.
 * @since_tizen 3.0
 */
typedef enum {
	STREAMRECORDER_STATE_NONE,      /**< StreamRecorder is not created */
	STREAMRECORDER_STATE_CREATED,   /**< StreamRecorder is created, but not prepared */
	STREAMRECORDER_STATE_PREPARED,     /**< StreamRecorder is prepared to record */
	STREAMRECORDER_STATE_RECORDING, /**< StreamRecorder is recording media */
	STREAMRECORDER_STATE_PAUSED,    /**< StreamRecorder is paused while recording media */
} streamrecorder_state_e;

/**
 * @brief Enumeration for the recorder rotation type.
 * @since_tizen 3.0
 */
typedef enum
{
	STREAMRECORDER_ROTATION_NONE, /**< No rotation */
	STREAMRECORDER_ROTATION_90,   /**< 90 degree rotation */
	STREAMRECORDER_ROTATION_180,  /**< 180 degree rotation */
	STREAMRECORDER_ROTATION_270,  /**< 270 degree rotation */
} streamrecorder_rotation_e;

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_STREAMRECORDER_CALLBACK_MODULE
 * @{
 */

/**
 * @brief Called when limitation error occurs while recording.
 * @details The callback function is possible to receive three types of limits: time, size and no-space.
 * @since_tizen 3.0
 * @remarks After being called, recording data is discarded and not written in the recording file. Also the state of streamrecorder is not changed.
 * @param[in] type The imitation type
 * @param[in] user_data The user data passed from the callback registration function
 * @pre You have to register a callback using streamrecorder_set_recording_limit_reached_cb().
 * @see streamrecorder_set_recording_status_cb()
 * @see streamrecorder_set_recording_limit_reached_cb()
 * @see streamrecorder_unset_recording_limit_reached_cb()
 */
typedef void (*streamrecorder_recording_limit_reached_cb)(streamrecorder_recording_limit_type_e type, void *user_data);

/**
 * @brief Called to indicate the recording status.
 * @since_tizen 3.0
 * @remarks This callback function is repeatedly invoked during the #STREAMRECORDER_STATE_RECORDING state.
 * @param[in] elapsed_time  The time of the recording (milliseconds)
 * @param[in] file_size     The size of the recording file (KB)
 * @param[in] user_data     The user data passed from the callback registration function
 * @pre streamrecorder_start() will invoke this callback if you register it using streamrecorder_set_recording_status_cb().
 * @see	streamrecorder_set_recording_status_cb()
 * @see	streamrecorder_unset_recording_status_cb()
 * @see	streamrecorder_start()
 */
typedef void (*streamrecorder_recording_status_cb)(unsigned long long elapsed_time, unsigned long long file_size, void *user_data);

/**
 * @brief Called when the record state is changed.
 * @since_tizen 3.0
 * @param[in] previous	The previous state of the recorder
 * @param[in] current	The current state of the recorder
 * @param[in] by_policy     @c true if the state is changed by policy, otherwise @c false if the state is not changed
 * @param[in] user_data	The user data passed from the callback registration function
 * @pre This function is required to register a callback using streamrecorder_set_state_changed_cb().
 * @see	streamrecorder_set_state_changed_cb()
 * @see	streamrecorder_prepare()
 * @see	streamrecorder_unprepare()
 * @see	streamrecorder_start()
 * @see	streamrecorder_pause()
 * @see	streamrecorder_commit()
 * @see	streamrecorder_cancel()
 */
typedef void (*streamrecorder_state_changed_cb)(streamrecorder_state_e previous , streamrecorder_state_e current , bool by_policy, void *user_data);

/**
 * @brief Called when the recorder is interrupted by a policy.
 * @since_tizen 3.0
 * @param[in] policy        The policy that is interrupting the recorder
 * @param[in] previous      The previous state of the recorder
 * @param[in] current       The current state of the recorder
 * @param[in] user_data     The user data passed from the callback registration function
 * @see	streamrecorder_set_interrupted_cb()
 */
typedef void (*streamrecorder_interrupted_cb)(streamrecorder_policy_e policy, streamrecorder_state_e previous, streamrecorder_state_e current, void *user_data);

/**
 * @brief Called when the error occurred.
 * @since_tizen 3.0
 * @remarks This callback informs about the critical error situation. \n
 *          When being invoked, user should release the resource and terminate the application. \n
 *          This error code will be reported.
 *          #STREAMRECORDER_ERROR_DEVICE \n
 *          #STREAMRECORDER_ERROR_INVALID_OPERATION \n
 *          #STREAMRECORDER_ERROR_OUT_OF_MEMORY.
 * @param[in] error          The error code
 * @param[in] current_state  The current state of the recorder
 * @param[in] user_data      The user data passed from the callback registration function
 * @pre	This callback function is invoked if you register this callback using streamrecorder_set_error_cb().
 * @see	streamrecorder_set_error_cb()
 * @see	streamrecorder_unset_error_cb()
 */
typedef void (*streamrecorder_error_cb)(streamrecorder_error_e error, streamrecorder_state_e current_state, void *user_data);

/**
 * @brief Called iteratively to notify about how much buffer has been consumed
 * @since_tizen 3.0
 * @remarks The user needs to release consumed buffer if buffer is allocated by user including media packet
 * @param[in] buffer  The consumed buffer that user pushed
 * @param[in] user_data	The user data passed
 * @see streamrecorder_set_buffer_consume_completed_cb() will invoke this callback.
 * @see streamrecorder_unset_buffer_consume_completed_cb()
 */
typedef void (*streamrecorder_consume_completed_cb)(void *buffer, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_STREAMRECORDER_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Called once for each supported video resolution.
 * @since_tizen 3.0
 * @param[in] width         The video image width
 * @param[in] height        The video image height
 * @param[in] user_data     The user data passed from the foreach function
 * @return    @c true to continue with the next iteration of the loop, \n otherwise @c false to break out of the loop
 * @pre	streamrecorder_foreach_supported_video_resolution() will invoke this callback.
 * @see	streamrecorder_foreach_supported_video_resolution()
 */
typedef bool (*streamrecorder_supported_video_resolution_cb)(int width, int height, void *user_data);

/**
 * @brief Called iteratively to notify about the supported file formats.
 * @since_tizen 3.0
 * @param[in] format   The format of recording files
 * @param[in] user_data	The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n otherwise @c false to break out of the loop
 * @pre streamrecorder_foreach_supported_file_format() will invoke this callback.
 * @see	streamrecorder_foreach_supported_file_format()
 */
typedef bool (*streamrecorder_supported_file_format_cb)(streamrecorder_file_format_e format, void *user_data);

/**
 * @brief Called iteratively to notify about the supported audio encoders.
 * @since_tizen 3.0
 * @param[in] codec	The codec of audio encoder
 * @param[in] user_data	The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n otherwise @c false to break out of the loop
 * @pre streamrecorder_foreach_supported_audio_encoder() will invoke this callback.
 * @see	streamrecorder_foreach_supported_audio_encoder()
 */
typedef bool (*streamrecorder_supported_audio_encoder_cb)(streamrecorder_audio_codec_e codec, void *user_data);

/**
 * @brief Called iteratively to notify about the supported video encoders.
 * @since_tizen 3.0
 * @param[in] codec	The codec of video encoder
 * @param[in] user_data	The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n otherwise @c false to break out of the loop
 * @pre streamrecorder_foreach_supported_video_encoder() will invoke this callback.
 * @see	streamrecorder_foreach_supported_video_encoder()
 */
typedef bool (*streamrecorder_supported_video_encoder_cb)(streamrecorder_video_codec_e codec, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_STREAMRECORDER_MODULE
 * @{
 */

/**
 * @brief Creates a streamrecorder handle to record a video or audio
 * @since_tizen 3.0
 * @remarks You must release @a recorder using streamrecorder_destroy().
 * @param[out]  recorder	A handle to the streamrecorder
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #STREAMRECORDER_ERROR_SOUND_POLICY Sound policy error
 * @retval #STREAMRECORDER_ERROR_INVALID_OPERATION Invalid operation
 */
int streamrecorder_create(streamrecorder_h *recorder);

/**
 * @brief Destroys the streamrecorder handle
 * @since_tizen 3.0
 * @param[in]	recorder    The handle to the streamrecorder
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_INVALID_OPERATION Invalid operation
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @pre  The recorder state should be #STREAMRECORDER_STATE_CREATED.
 * @post The recorder state will be #STREAMRECORDER_STATE_NONE.
 */
int streamrecorder_destroy(streamrecorder_h recorder);

/**
 * @brief Prepares the streamrecorder for recording.
 * @since_tizen 3.0
 * @remarks Before calling the function, it is required to properly set videosource (streamrecorder_set_videosource_buffer()),
 *          audiosource (streamrecorder_set_audiosource_buffer()),
 *          audio encoder (streamrecorder_set_audio_encoder()),
 *          video encoder(streamrecorder_set_video_encoder()) and file format (streamrecorder_set_file_format()).
 * @param[in]	recorder  The handle to the streamrecorder
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_SOUND_POLICY Sound policy error
 * @retval #STREAMRECORDER_ERROR_INVALID_OPERATION Invalid operation
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @pre  The recorder state should be #STREAMRECORDER_STATE_CREATED
 * @pre  The recorder should call streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() or both before this function.
 *          streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @post The recorder state will be #STREAMRECORDER_STATE_PREPARED
 * @see	streamrecorder_unprepare()
 * @see	streamrecorder_set_audio_encoder()
 * @see	streamrecorder_set_video_encoder()
 * @see	streamrecorder_set_file_format()
 */
int streamrecorder_prepare(streamrecorder_h recorder);

/**
 * @brief Resets the streamrecorder.
 * @since_tizen 3.0
 * @param[in]  recorder  The handle to the streamrecorder
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_INVALID_OPERATION Invalid operation
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @pre  The recorder state should be #STREAMRECORDER_STATE_PREPARED set by streamrecorder_prepare(), streamrecorder_cancel() or streamrecorder_commit().
 * @post The recorder state will be #STREAMRECORDER_STATE_CREATED.
 * @see	streamrecorder_prepare()
 * @see	streamrecorder_cancel()
 * @see	streamrecorder_commit()
 */
int streamrecorder_unprepare(streamrecorder_h recorder);

/**
 * @brief Starts the recording.
 * @since_tizen 3.0
 * @remarks If file path has been set to an existing file, this file is removed automatically and updated by new one. \n
 *          In the video recorder, some format does not support record mode. It will return #STREAMRECORDER_ERROR_INVALID_OPERATION error. \n
 *          You should use default format or #STREAMRECORDER_INPUT_FORMAT_NV12 in the record mode. \n
 *          When you want to record audio or video file, you need to add privilege according to rules below additionally. \n
 *          If you want to save contents to internal storage, you should add mediastorage privilege. \n
 *          If you want to save contents to external storage, you should add externalstorage privilege. \n
 *          The filename should be set before this function is invoked.
 * @param[in]  recorder  The handle to the streamrecorder
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_INVALID_OPERATION Invalid operation
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @retval #STREAMRECORDER_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @pre The recorder state must be #STREAMRECORDER_STATE_PREPARED by streamrecorder_prepare() or #STREAMRECORDER_STATE_PAUSED by streamrecorder_pause(). \n
 * The filename should be set by streamrecorder_set_filename().
 * @post The recorder state will be #STREAMRECORDER_STATE_RECORDING.
 * @see	streamrecorder_pause()
 * @see	streamrecorder_commit()
 * @see	streamrecorder_cancel()
 * @see	streamrecorder_set_audio_encoder()
 * @see	streamrecorder_set_filename()
 * @see	streamrecorder_set_file_format()
 * @see	streamrecorder_recording_status_cb()
 * @see	streamrecorder_set_filename()
 */
int streamrecorder_start(streamrecorder_h recorder);

/**
 * @brief Pauses the recording.
 * @since_tizen 3.0
 * @remarks Recording can be resumed with streamrecorder_start().
 * @param[in]  recorder  The handle to the streamrecorder
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_INVALID_OPERATION Invalid operation
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @pre The recorder state must be #STREAMRECORDER_STATE_RECORDING.
 * @post The recorder state will be #STREAMRECORDER_STATE_PAUSED.
 * @see streamrecorder_pause()
 * @see streamrecorder_commit()
 * @see streamrecorder_cancel()
 */
int streamrecorder_pause(streamrecorder_h recorder);

/**
 * @brief Stops recording and saves the result.
 * @since_tizen 3.0
 * @remarks When you want to record audio or video file, you need to add privilege according to rules below additionally. \n
 *          If you want to save contents to internal storage, you should add mediastorage privilege. \n
 *          If you want to save contents to external storage, you should add externalstorage privilege.
 * @param[in]  recorder  The handle to the streamrecorder
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_INVALID_OPERATION Invalid operation
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @retval #STREAMRECORDER_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @pre The recorder state must be #STREAMRECORDER_STATE_RECORDING set by streamrecorder_start() or #STREAMRECORDER_STATE_PAUSED by streamrecorder_pause().
 * @post The recorder state will be #STREAMRECORDER_STATE_PREPARED.
 * @see streamrecorder_pause()
 * @see streamrecorder_cancel()
 * @see streamrecorder_set_filename()
 * @see streamrecorder_start()
 */
int streamrecorder_commit(streamrecorder_h recorder);

/**
 * @brief Cancels the recording.
 * @details The recording data is discarded and not written in the recording file.
 * @since_tizen 3.0
 * @remarks When you want to record audio or video file, you need to add privilege according to rules below additionally. \n
 *          If you want to save contents to internal storage, you should add mediastorage privilege. \n
 *          If you want to save contents to external storage, you should add externalstorage privilege.
 * @param[in]  recorder  The handle to the streamrecorder
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_INVALID_OPERATION Invalid operation
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @retval #STREAMRECORDER_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @pre The recorder state must be #STREAMRECORDER_STATE_RECORDING set by streamrecorder_start() or #STREAMRECORDER_STATE_PAUSED by streamrecorder_pause().
 * @post The recorder state will be #STREAMRECORDER_STATE_PREPARED.
 * @see streamrecorder_pause()
 * @see streamrecorder_commit()
 * @see streamrecorder_cancel()
 * @see streamrecorder_start()
 */
int streamrecorder_cancel(streamrecorder_h recorder);

/**
 * @brief pushes buffer to StreamRecorder to record audio/video
 * @since_tizen 3.0
 * @remarks When you want to record audio or video file, you need to add privilege according to rules below additionally. \n
 *          If you want to save contents to internal storage, you should add mediastorage privilege. \n
 *          If you want to save contents to external storage, you should add externalstorage privilege.
 * @param[in]	recorder	The handle to the streamrecorder
 * @param[in]	inbuf The media packet containing buffer and other associated values
 * @return  @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 */
int streamrecorder_push_stream_buffer(streamrecorder_h recorder, media_packet_h inbuf);

/**
 * @brief sets the video source as live buffer to be used for recording
 * @since_tizen 3.0
 * @remarks if you want to enable video recording, call before streamrecorder_prepare()
 * @param[in]  recorder  A handle to the streamrecorder
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre The recorder state must be only #STREAMRECORDER_STATE_CREATED
 */
int streamrecorder_set_videosource_buffer(streamrecorder_h recorder);

/**
 * @brief sets the audio source as buffer to be used for recording
 * @since_tizen 3.0
 * @remarks if you want to enable audio recording, call this function before streamrecorder_prepare()
 * @param[in]  recorder  A handle to the streamrecorder
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre The recorder state must be only #STREAMRECORDER_STATE_CREATED
 */
int streamrecorder_set_audiosource_buffer(streamrecorder_h recorder);

/**
 * @brief Gets the recorder's current state.
 * @since_tizen 3.0
 * @param[in]  recorder The handle to the streamrecorder
 * @param[out]	state  The current state of the streamrecorder
 * @return  @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 */
int streamrecorder_get_state(streamrecorder_h recorder, streamrecorder_state_e *state);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_STREAMRECORDER_ATTRIBUTES_MODULE
 * @{
 */


/**
 * @brief Sets the file path to record.
 * @details This function sets file path which defines where newly recorded data should be stored.
 * @since_tizen 3.0
 * @remarks If the same file already exists in the file system, then old file will be overwritten.
 * @param[in]	recorder	The handle to the streamrecorder
 * @param[in]	path The recording file path
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @pre The recorder state must be #STREAMRECORDER_STATE_CREATED.
 *          streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see streamrecorder_get_filename()
 */
int streamrecorder_set_filename(streamrecorder_h recorder, const char *path);

/**
 * @brief Gets the file path to record.
 * @since_tizen 3.0
 * @remarks You must release @a path using free().
 * @param[in]	recorder    The handle to the streamrecorder
 * @param[out]	path    The recording file path
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 *          streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	streamrecorder_set_filename()
 */
int streamrecorder_get_filename(streamrecorder_h recorder, char **path);

/**
 * @brief Sets the file format for recording media stream.
 * @since_tizen 3.0
 * @param[in] recorder The handle to the streamrecorder
 * @param[in] format   The media file format
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @retval #STREAMRECORDER_ERROR_INVALID_OPERATION Invalid operation
 * @pre The streamrecorder state must be #STREAMRECORDER_STATE_CREATED
 *          streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see streamrecorder_get_file_format()
 * @see streamrecorder_foreach_supported_file_format()
 */
int streamrecorder_set_file_format(streamrecorder_h recorder, streamrecorder_file_format_e format);


/**
 * @brief Gets the file format for recording media stream.
 * @since_tizen 3.0
 * @param[in] recorder The handle to the streamrecorder
 * @param[out] format   The media file format
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre The streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 *          streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see streamrecorder_set_file_format()
 * @see streamrecorder_foreach_supported_file_format()
 */
int streamrecorder_get_file_format(streamrecorder_h recorder, streamrecorder_file_format_e *format);


/**
 * @brief Sets the audio codec for encoding an audio stream.
 * @since_tizen 3.0
 * @remarks You can get available audio encoders by using streamrecorder_foreach_supported_audio_encoder(). \n
 *          If set to #STREAMRECORDER_AUDIO_CODEC_DISABLE, the audio track is not created in recording files.\n
 * @param[in] recorder The handle to the streamrecorder
 * @param[in] codec    The audio codec
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @retval #STREAMRECORDER_ERROR_NOT_SUPPORTED The feature is not supported
 * @retval #STREAMRECORDER_ERROR_INVALID_OPERATION Invalid operation
 * @pre The recorder state must be #STREAMRECORDER_STATE_CREATED.
 *          streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	streamrecorder_get_audio_encoder()
 * @see streamrecorder_foreach_supported_audio_encoder()
 */
int streamrecorder_set_audio_encoder(streamrecorder_h recorder, streamrecorder_audio_codec_e codec);

/**
 * @brief Gets the audio codec for encoding an audio stream.
 * @since_tizen 3.0
 * @param[in] recorder The handle to the streamrecorder
 * @param[out] codec   The audio codec
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre The streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	streamrecorder_set_audio_encoder()
 * @see streamrecorder_foreach_supported_audio_encoder()
 */
int streamrecorder_get_audio_encoder(streamrecorder_h recorder, streamrecorder_audio_codec_e *codec);

/**
 * @brief Sets the resolution of the video recording.
 * @since_tizen 3.0
 * @remarks This function should be called before recording (streamrecorder_start()).
 * @param[in] recorder	The handle to the streamrecorder
 * @param[in] width	The input width
 * @param[in] height	The input height
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @retval #STREAMRECORDER_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre    The recorder state must be #STREAMRECORDER_STATE_CREATED.
 *          streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	streamrecorder_start()
 * @see	streamrecorder_get_video_resolution()
 * @see	streamrecorder_foreach_supported_video_resolution()
 */
int streamrecorder_set_video_resolution(streamrecorder_h recorder, int width, int height);

/**
 * @brief Gets the resolution of the video recording.
 * @since_tizen 3.0
 * @param[in] recorder	The handle to the streamrecorder
 * @param[out] width	The video width
 * @param[out] height	The video height
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre The streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	streamrecorder_set_video_resolution()
 * @see	streamrecorder_foreach_supported_video_resolution()
 */
int streamrecorder_get_video_resolution(streamrecorder_h recorder, int *width, int *height);

/**
 * @brief Sets the video codec for encoding video stream.
 * @remarks You can get available video encoders by using recorder_foreach_supported_video_encoder().
 * @param[in] recorder The handle to the streamrecorder
 * @param[in] codec    The video codec
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @retval #STREAMRECORDER_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre The recorder state must be #STREAMRECORDER_STATE_CREATED.
 *          streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see streamrecorder_get_video_encoder()
 * @see streamrecorder_foreach_supported_video_encoder()
 */
int streamrecorder_set_video_encoder(streamrecorder_h recorder, streamrecorder_video_codec_e codec);

/**
 * @brief Gets the video codec for encoding video stream.
 * @param[in] recorder The handle to the streamrecorder
 * @param[out] codec   The video codec
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre The streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see streamrecorder_set_video_encoder()
 * @see streamrecorder_foreach_supported_video_encoder()
 */
int streamrecorder_get_video_encoder(streamrecorder_h recorder, streamrecorder_video_codec_e *codec);

/**
 * @brief Sets the maximum size of a recording file.
 * @remarks After reaching the limitation, the recording data is discarded and not written in the recording file.
 * @param[in] recorder The handle to the streamrecorder
 * @param[in] kbyte    The maximum size of the recording file(KB) \n
 *                     @c 0 means unlimited recording size.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @pre The recorder state must be #STREAMRECORDER_STATE_CREATED.
 *          streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	streamrecorder_get_size_limit()
 * @see	streamrecorder_set_time_limit()
 */
int streamrecorder_set_size_limit(streamrecorder_h recorder, int kbyte);

/**
 * @brief Gets the maximum size of a recording file.
 * @param[in]  recorder The handle to the streamrecorder
 * @param[out] kbyte    The maximum size of recording file (KB) \n
 *                      @c 0 means unlimited recording size.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre The streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	streamrecorder_set_size_limit()
 * @see	streamrecorder_get_time_limit()
 */
int streamrecorder_get_size_limit(streamrecorder_h recorder, int *kbyte);

/**
 * @brief Sets the time limit of a recording file.
 * @remarks After reaching the limitation, the recording data is discarded and not written in the recording file.
 * @param[in] recorder The handle to the streamrecorder
 * @param[in] second   The time limit of the recording file (in seconds) \n
 *                     @c 0 means unlimited recording size.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @pre The recorder state must be #STREAMRECORDER_STATE_CREATED.
 *          streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	streamrecorder_get_time_limit()
 * @see	streamrecorder_set_size_limit()
 */
int streamrecorder_set_time_limit(streamrecorder_h recorder, int second);


/**
 * @brief Gets the time limit of a recording file.
 * @param[in]  recorder  The handle to the streamrecorder
 * @param[out] second    The time limit of the recording file (in seconds) \n
 *                       @c 0 means unlimited recording time.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre The streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	streamrecorder_set_time_limit()
 * @see	streamrecorder_get_size_limit()
 */
int streamrecorder_get_time_limit(streamrecorder_h recorder, int *second);

/**
 * @brief Sets the sampling rate of an audio stream.
 * @param[in] recorder    The handle to the streamrecorder
 * @param[in] samplerate The sample rate in Hertz
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @pre The recorder state must be #STREAMRECORDER_STATE_CREATED.
 *          streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	streamrecorder_get_audio_samplerate()
 */
int streamrecorder_set_audio_samplerate(streamrecorder_h recorder, int samplerate);

/**
 * @brief Gets the sampling rate of an audio stream.
 * @param[in]  recorder    The handle to the streamrecorder
 * @param[out] samplerate  The sample rate in Hertz
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre The streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	streamrecorder_set_audio_samplerate()
 */
int streamrecorder_get_audio_samplerate(streamrecorder_h recorder, int *samplerate);

/**
 * @brief Sets the bitrate of an audio encoder.
 * @param[in] recorder  The handle to the streamrecorder
 * @param[in] bitrate   The bitrate (for mms : 12200[bps], normal : 288000[bps])
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @pre The recorder state must be #STREAMRECORDER_STATE_CREATED.
 *          streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	streamrecorder_get_audio_encoder_bitrate()
 */
int streamrecorder_set_audio_encoder_bitrate(streamrecorder_h recorder, int bitrate);

/**
 * @brief Sets the bitrate of a video encoder.
 * @param[in] recorder  The handle to the streamrecorder
 * @param[in] bitrate   The bitrate in bits per second
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #STREAMRECORDER_ERROR_INVALID_STATE Invalid state
 * @pre The recorder state must be #STREAMRECORDER_STATE_CREATED.
 *          streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	streamrecorder_get_video_encoder_bitrate()
 */
int streamrecorder_set_video_encoder_bitrate(streamrecorder_h recorder, int bitrate);

/**
 * @brief Gets the bitrate of an audio encoder.
 * @param[in]  recorder  The handle to the streamrecorder
 * @param[out] bitrate   The bitrate in bits per second
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre The streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	recorder_set_audio_encoder_bitrate()
 */
int streamrecorder_get_audio_encoder_bitrate(streamrecorder_h recorder, int *bitrate);

/**
 * @brief Gets the bitrate of a video encoder.
 * @param[in]  recorder  The handle to the streamrecorder
 * @param[out] bitrate   The bitrate in bits per second
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre The streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	streamrecorder_set_audio_encoder_bitrate()
 */
int streamrecorder_get_video_encoder_bitrate(streamrecorder_h recorder, int *bitrate);

/**
 * @brief Sets the number of the audio channel.
 * @remarks This attribute is applied only in STREAMRECORDER_STATE_CREATED state. \n
 *          For mono recording, setting channel to @c 1. \n
 *          For stereo recording, setting channel to @c 2.
 * @param[in] recorder       The handle to the streamrecorder
 * @param[in] channel_count  The number of the audio channel
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre The recorder state must be #STREAMRECORDER_STATE_CREATED (for video recorder only).\n
 *          streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	streamrecorder_get_audio_channel()
 */
int streamrecorder_set_audio_channel(streamrecorder_h recorder, int channel_count);

/**
 * @brief Gets the number of the audio channel.
 * @param[in] recorder  The handle to the streamrecorder
 * @param[out] channel_count  The number of the audio channel
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre The streamrecorder_set_videosource_buffer() or streamrecorder_set_audiosource_buffer() should be invoked before thie function.
 * @see	streamrecorder_set_audio_channel()
 */
int streamrecorder_get_audio_channel(streamrecorder_h recorder, int *channel_count);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_STREAMRECORDER_CAPABILITY_MODULE
 * @{
 */


/**
 * @brief Retrieves all supported file formats by invoking a specific callback for each supported file format.
 * @since_tizen 3.0
 * @param[in] recorder  The handle to the streamrecorder
 * @param[in] callback The iteration callback
 * @param[in] user_data	The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @post  streamrecorder_supported_file_format_cb() will be invoked.
 * @see streamrecorder_get_file_format()
 * @see streamrecorder_set_file_format()
 * @see streamrecorder_supported_file_format_cb()
 */
int streamrecorder_foreach_supported_file_format(streamrecorder_h recorder, streamrecorder_supported_file_format_cb callback, void *user_data);

/**
 * @brief Retrieves all supported audio encoders by invoking a specific callback for each supported audio encoder.
 * @since_tizen 3.0
 * @param[in] recorder  The handle to the streamrecorder
 * @param[in] callback	The iteration callback
 * @param[in] user_data	The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @post  streamrecorder_supported_audio_encoder_cb() will be invoked.
 * @see	streamrecorder_set_audio_encoder()
 * @see	streamrecorder_get_audio_encoder()
 * @see	streamrecorder_supported_audio_encoder_cb()
 */
int streamrecorder_foreach_supported_audio_encoder(streamrecorder_h recorder, streamrecorder_supported_audio_encoder_cb callback, void *user_data);

/**
 * @brief Retrieves all supported video encoders by invoking a specific callback for each supported video encoder.
 * @param[in] recorder	The handle to the streamrecorder
 * @param[in] callback	The iteration callback
 * @param[in] user_data	The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @post  streamrecorder_supported_video_encoder_cb() will be invoked.
 * @see streamrecorder_set_video_encoder()
 * @see streamrecorder_get_video_encoder()
 * @see	streamrecorder_supported_video_encoder_cb()
 */
int streamrecorder_foreach_supported_video_encoder(streamrecorder_h recorder, streamrecorder_supported_video_encoder_cb callback, void *user_data);

/**
 * @brief Retrieves all supported video resolutions by invoking callback function once for each supported video resolution.
 * @param[in] recorder	The handle to the streamrecorder
 * @param[in] foreach_cb	The callback function to be invoked
 * @param[in] user_data	The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @post	This function invokes streamrecorder_supported_video_resolution_cb() repeatedly to retrieve each supported video resolution.
 * @see	streamrecorder_set_video_resolution()
 * @see	streamrecorder_get_video_resolution()
 * @see	streamrecorder_supported_video_resolution_cb()
 */
int streamrecorder_foreach_supported_video_resolution(streamrecorder_h recorder,
                                                streamrecorder_supported_video_resolution_cb foreach_cb, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_STREAMRECORDER_CALLBACK_MODULE
 * @{
 */


/**
 * @brief Registers the callback function that will be invoked when the recorder state changes.
 * @param[in] recorder	The handle to the streamrecorder
 * @param[in] callback	The function pointer of user callback
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @post  streamrecorder_state_changed_cb() will be invoked.
 * @see streamrecorder_unset_state_changed_cb()
 * @see streamrecorder_state_changed_cb()
 */
int streamrecorder_set_state_changed_cb(streamrecorder_h recorder, streamrecorder_state_changed_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function.
 * @param[in]  recorder The handle to the streamrecorder
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @see streamrecorder_set_state_changed_cb()
 */
int streamrecorder_unset_state_changed_cb(streamrecorder_h recorder);

/**
 * @brief Registers a callback function to be called when the media recorder is interrupted according to a policy.
 * @param[in] recorder	The handle to the streamrecorder
 * @param[in] callback	  The callback function to register
 * @param[in] user_data   The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @see	streamrecorder_unset_interrupted_cb()
 * @see	streamrecorder_interrupted_cb()
 */
int streamrecorder_set_interrupted_cb(streamrecorder_h recorder, streamrecorder_interrupted_cb callback,
	    void *user_data);

/**
 * @brief Unregisters the callback function.
 * @param[in]	recorder	The handle to the streamrecorder
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @see	streamrecorder_set_interrupted_cb()
 */
int streamrecorder_unset_interrupted_cb(streamrecorder_h recorder);

/**
 * @brief Registers a callback function to be invoked when the recording information changes.
 * @param[in]  recorder   The handle to the streamrecorder
 * @param[in]  callback   The function pointer of user callback
 * @param[in]  user_data  The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @post  streamrecorder_recording_status_cb() will be invoked.
 * @see	streamrecorder_unset_recording_status_cb()
 * @see	streamrecorder_recording_status_cb()
 */
int streamrecorder_set_recording_status_cb(streamrecorder_h recorder, streamrecorder_recording_status_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function.
 * @param[in]  recorder    The handle to the streamrecorder
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @see	streamrecorder_set_recording_status_cb()
 */
int streamrecorder_unset_recording_status_cb(streamrecorder_h recorder);

/**
 * @brief Registers the callback function to be run when reached the recording limit.
 * @param[in]	recorder	The handle to treamrecorder
 * @param[in]	callback	The function pointer of user callback
 * @param[in]	user_data	The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @post  streamrecorder_recording_limit_reached_cb() will be invoked.
 * @see	streamrecorder_unset_recording_limit_reached_cb()
 * @see	streamrecorder_set_size_limit()
 * @see	streamrecorder_set_time_limit()
 * @see	streamrecorder_recording_limit_reached_cb()
 */
int streamrecorder_set_recording_limit_reached_cb(streamrecorder_h recorder, streamrecorder_recording_limit_reached_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function.
 * @param[in]  recorder  The handle to the streamrecorder
 * @return @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @see	streamrecorder_set_recording_limit_reached_cb()
 */
int streamrecorder_unset_recording_limit_reached_cb(streamrecorder_h recorder);

/**
 * @brief Registers a callback function to be called when an asynchronous operation error occurred.
 * @remarks This callback informs critical error situation.\n
 *          When this callback is invoked, user should release the resource and terminate the application. \n
 *          These error codes will occur. \n
 *          #STREAMRECORDER_ERROR_DEVICE \n
 *          #STREAMRECORDER_ERROR_INVALID_OPERATION \n
 *          #STREAMRECORDER_ERROR_OUT_OF_MEMORY
 * @param[in]	recorder	The handle to the streamrecorder
 * @param[in]	callback	The callback function to register
 * @param[in]	user_data	The user data to be passed to the callback function
 * @return  @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @post	This function will invoke recorder_error_cb() when an asynchronous operation error occur.
 * @see	streamrecorder_unset_error_cb()
 * @see	streamrecorder_error_cb()
 */
int streamrecorder_set_error_cb(streamrecorder_h recorder, streamrecorder_error_cb callback, void *user_data);


/**
 * @brief Unregisters the callback function.
 * @param[in]	recorder	The handle to the streamrecorder
 * @return  @c on success, otherwise a negative error value
 * @retval    #STREAMRECORDER_ERROR_NONE Successful
 * @retval    #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @see	streamrecorder_set_error_cb()
 */
int streamrecorder_unset_error_cb(streamrecorder_h recorder);

/**
 * @brief Registers a callback function to be called when an asynchronous operation error occurred.
 * @remarks This callback informs critical error situation.\n
 *          When this callback is invoked, user should release the resource and terminate the application. \n
 *          These error codes will occur. \n
 *          #STREAMRECORDER_ERROR_DEVICE \n
 *          #STREAMRECORDER_ERROR_INVALID_OPERATION \n
 *          #STREAMRECORDER_ERROR_OUT_OF_MEMORY
 * @param[in]	recorder	The handle to the streamrecorder
 * @param[in]	callback	The callback function to register
 * @param[in]	user_data	The user data to be passed to the callback function
 * @return  @c 0 on success, otherwise a negative error value
 * @retval #STREAMRECORDER_ERROR_NONE Successful
 * @retval #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @post	This function will invoke recorder_error_cb() when an asynchronous operation error occur.
 * @see	streamrecorder_unset_error_cb()
 * @see	streamrecorder_error_cb()
 */
int streamrecorder_set_buffer_consume_completed_cb(streamrecorder_h recorder, streamrecorder_consume_completed_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function.
 * @param[in]	recorder	The handle to the streamrecorder
 * @return  @c on success, otherwise a negative error value
 * @retval    #STREAMRECORDER_ERROR_NONE Successful
 * @retval    #STREAMRECORDER_ERROR_INVALID_PARAMETER Invalid parameter
 * @see	streamrecorder_set_error_cb()
 */
int streamrecorder_unset_buffer_consume_completed_cb(streamrecorder_h recorder);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MEDIA_STREAMRECORDER_H__ */

