/*
* Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <stdio.h>
#include <stdlib.h>
#include <mm.h>
#include <mm_types.h>
#include <mm_streamrecorder.h>
#include <streamrecorder_private.h>
#include <dlog.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>


#define LOWSET_DECIBEL -300.0

int __convert_streamrecorder_error_code(const char *func, int code)
{
	int ret = STREAMRECORDER_ERROR_INVALID_OPERATION;
	const char *errorstr = NULL;

	switch (code) {
	case STREAMRECORDER_ERROR_INVALID_PARAMETER:
		ret = STREAMRECORDER_ERROR_INVALID_PARAMETER;
		errorstr = "INVALID_PARAMETER";
		break;
	case MM_ERROR_NONE:
		ret = STREAMRECORDER_ERROR_NONE;
		errorstr = "ERROR_NONE";
		break;
	case MM_ERROR_STREAMRECORDER_INVALID_ARGUMENT:
	case MM_ERROR_COMMON_INVALID_ATTRTYPE:
		ret = STREAMRECORDER_ERROR_INVALID_PARAMETER;
		errorstr = "INVALID_PARAMETER";
		break;
	case MM_ERROR_COMMON_INVALID_PERMISSION:
		ret = STREAMRECORDER_ERROR_PERMISSION_DENIED;
		errorstr = "ERROR_PERMISSION_DENIED";
		break;
	case MM_ERROR_STREAMRECORDER_NOT_INITIALIZED:
		ret = STREAMRECORDER_ERROR_INVALID_STATE;
		errorstr = "INVALID_STATE";
		break;
	case MM_ERROR_STREAMRECORDER_GST_CORE:
	case MM_ERROR_STREAMRECORDER_GST_LIBRARY:
	case MM_ERROR_STREAMRECORDER_GST_RESOURCE:
	case MM_ERROR_STREAMRECORDER_GST_STREAM:
	case MM_ERROR_STREAMRECORDER_GST_STATECHANGE:
	case MM_ERROR_STREAMRECORDER_GST_NEGOTIATION:
	case MM_ERROR_STREAMRECORDER_GST_LINK:
	case MM_ERROR_STREAMRECORDER_GST_FLOW_ERROR:
	case MM_ERROR_STREAMRECORDER_ENCODER:
	case MM_ERROR_STREAMRECORDER_ENCODER_BUFFER:
	case MM_ERROR_STREAMRECORDER_ENCODER_WRONG_TYPE:
	case MM_ERROR_STREAMRECORDER_ENCODER_WORKING:
	case MM_ERROR_STREAMRECORDER_RESPONSE_TIMEOUT:
	case MM_ERROR_STREAMRECORDER_CMD_IS_RUNNING:
		ret = STREAMRECORDER_ERROR_INVALID_OPERATION;
		errorstr = "INVALID_OPERATION";
		break;
	case MM_ERROR_STREAMRECORDER_RESOURCE_CREATION:
	case MM_ERROR_COMMON_OUT_OF_MEMORY:
		ret = STREAMRECORDER_ERROR_OUT_OF_MEMORY;
		errorstr = "OUT_OF_MEMORY";
		break;
	case MM_ERROR_OUT_OF_STORAGE:
		ret = STREAMRECORDER_ERROR_OUT_OF_STORAGE;
		errorstr = "OUT_OF_STORAGE";
		break;
	case MM_ERROR_COMMON_OUT_OF_ARRAY:
	case MM_ERROR_COMMON_OUT_OF_RANGE:
	case MM_ERROR_COMMON_ATTR_NOT_EXIST:
		ret = STREAMRECORDER_ERROR_NOT_SUPPORTED;
		errorstr = "NOT_SUPPORTED";
		break;
	default:
		ret = STREAMRECORDER_ERROR_INVALID_OPERATION;
		errorstr = "INVALID_OPERATION";
		break;
	}


	LOGE("[%s] %s(0x%08x) : core frameworks error code(0x%08x)", func, errorstr, ret, code);

	return ret;
}

static streamrecorder_state_e __streamrecorder_state_convert(void *mm_state)
{
	streamrecorder_state_e state = STREAMRECORDER_STATE_NONE;
	MMStreamRecorderStateType srstate = (MMStreamRecorderStateType)mm_state;

	switch (srstate) {
	case MM_STREAMRECORDER_STATE_NONE:
		state = STREAMRECORDER_STATE_NONE;
		break;
	case MM_STREAMRECORDER_STATE_CREATED:
		state = STREAMRECORDER_STATE_CREATED;
		break;
	case MM_STREAMRECORDER_STATE_PREPARED:
		state = STREAMRECORDER_STATE_PREPARED;
		break;
	case MM_STREAMRECORDER_STATE_RECORDING:
		state = STREAMRECORDER_STATE_RECORDING;
		break;
	case MM_STREAMRECORDER_STATE_PAUSED:
		state = STREAMRECORDER_STATE_PAUSED;
		break;
	default:
		state = STREAMRECORDER_STATE_NONE;
		break;
	}

	return state;
}

static int _streamrecorder_check_and_set_attribute(streamrecorder_h recorder, const char *attribute_name, int set_value)
{
	int ret = MM_ERROR_NONE;

	MMStreamRecorderStateType mmstate;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	mm_streamrecorder_get_state(handle->mm_handle, &mmstate);
	if (mmstate >= MM_STREAMRECORDER_STATE_RECORDING) {
		LOGE("invalid state %d", mmstate);
		return STREAMRECORDER_ERROR_INVALID_STATE;
	}

	ret = mm_streamrecorder_set_attributes(handle->mm_handle, NULL,
												attribute_name, set_value,
												NULL);

	if (ret != MM_ERROR_NONE) {
		LOGE("set [%s] failed 0x%x", attribute_name, ret);
	}



	return __convert_streamrecorder_error_code(attribute_name, ret);
}


int _streamrecorder_set_videosource_buffer(streamrecorder_h recorder)
{
	int ret = STREAMRECORDER_ERROR_NONE;
	if (recorder == NULL)
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;

	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	mm_streamrecorder_set_attributes(handle->mm_handle, NULL,
								MMSTR_RECORDER_MODE, MM_STREAMRECORDER_MODE_MEDIABUFFER,
								NULL);

	mm_streamrecorder_set_attributes(handle->mm_handle, NULL,
								MMSTR_VIDEO_ENABLE, true,
								NULL);


	return ret;
}

int _streamrecorder_set_audiosource_buffer(streamrecorder_h recorder)
{
	int ret = STREAMRECORDER_ERROR_NONE;
	if (recorder == NULL)
		return	STREAMRECORDER_ERROR_INVALID_PARAMETER;

	streamrecorder_s *handle;
	handle = (streamrecorder_s *) recorder;
	mm_streamrecorder_set_attributes(handle->mm_handle, NULL,
									MMSTR_RECORDER_MODE, MM_STREAMRECORDER_MODE_MEDIABUFFER,
									NULL);

	mm_streamrecorder_set_attributes(handle->mm_handle, NULL,
								MMSTR_AUDIO_ENABLE, true,
								NULL);

	return ret;
}

int _streamrecorder_get_state(streamrecorder_h recorder, streamrecorder_state_e *state)
{
	int ret = STREAMRECORDER_ERROR_NONE;
	MMStreamRecorderStateType srstate;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	if (state == NULL) {
		LOGE("NULL pointer state");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	streamrecorder_s *handle = (streamrecorder_s *)recorder;


	ret = mm_streamrecorder_get_state(handle->mm_handle, &srstate);
	if (ret != MM_ERROR_NONE) {
		return __convert_streamrecorder_error_code(__func__, ret);
	}
	*state = __streamrecorder_state_convert((void *)srstate);

	return STREAMRECORDER_ERROR_NONE;
}

int _streamrecorder_destroy(streamrecorder_h recorder)
{
	streamrecorder_s *handle = NULL;
	int ret = MM_ERROR_NONE;
	int state = MM_STREAMRECORDER_STATE_NONE;
	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	handle = (streamrecorder_s *)recorder;

	ret = streamrecorder_get_state(recorder, &state);
	if (ret != MM_ERROR_NONE) {
		LOGE("stramrecorder_unrealize fail");
		return __convert_streamrecorder_error_code(__func__, ret);
	}

	if (state < MM_STREAMRECORDER_STATE_CREATED) {
		LOGE("STREAMRECORDER_ERROR_INVALID_STATE (state:%d)", state);
		return STREAMRECORDER_ERROR_INVALID_STATE;
	}

	ret = mm_streamrecorder_destroy(handle->mm_handle);

	if (ret == MM_ERROR_NONE) {
		free(handle);
	}

	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_prepare(streamrecorder_h recorder)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;
	int state = MM_STREAMRECORDER_STATE_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	ret = streamrecorder_get_state(recorder, &state);
	if (ret != MM_ERROR_NONE) {
		return __convert_streamrecorder_error_code(__func__, ret);
	}

	if (state != MM_STREAMRECORDER_STATE_CREATED) {
		LOGE("STREAMRECORDER_ERROR_INVALID_STATE (state:%d)", state);
		return STREAMRECORDER_ERROR_INVALID_STATE;
	}

	ret = mm_streamrecorder_realize(handle->mm_handle);
	if (ret != MM_ERROR_NONE) {
		LOGE("prepare fail");
		return __convert_streamrecorder_error_code(__func__, ret);
	}

	return STREAMRECORDER_ERROR_NONE;
}

int _streamrecorder_unprepare(streamrecorder_h recorder)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;
	int state = MM_STREAMRECORDER_STATE_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = streamrecorder_get_state(recorder, &state);
	if (ret != MM_ERROR_NONE) {
		return __convert_streamrecorder_error_code(__func__, ret);
	}

	if (state < MM_STREAMRECORDER_STATE_PREPARED) {
		LOGE("STREAMRECORDER_ERROR_INVALID_STATE (state:%d)", state);
		return STREAMRECORDER_ERROR_INVALID_STATE;
	}

	ret = mm_streamrecorder_unrealize(handle->mm_handle);
	if (ret != MM_ERROR_NONE) {
		LOGE("stramrecorder_unrealize fail");
	}
	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_start(streamrecorder_h recorder)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;
	int state = MM_STREAMRECORDER_STATE_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = streamrecorder_get_state(recorder, &state);
	if (ret != MM_ERROR_NONE) {
		return __convert_streamrecorder_error_code(__func__, ret);
	}

	if (!(state == MM_STREAMRECORDER_STATE_PREPARED || state == MM_STREAMRECORDER_STATE_PAUSED)) {
		LOGE("STREAMRECORDER_ERROR_INVALID_STATE (state:%d)", state);
		return STREAMRECORDER_ERROR_INVALID_STATE;
	}

	return __convert_streamrecorder_error_code(__func__, mm_streamrecorder_record(handle->mm_handle));
}

int _streamrecorder_pause(streamrecorder_h recorder)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;
	int state = MM_STREAMRECORDER_STATE_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = streamrecorder_get_state(recorder, &state);
	if (ret != MM_ERROR_NONE) {
		return __convert_streamrecorder_error_code(__func__, ret);
	}

	if (state != MM_STREAMRECORDER_STATE_RECORDING) {
		LOGE("STREAMRECORDER_ERROR_INVALID_STATE (state:%d)", state);
		return STREAMRECORDER_ERROR_INVALID_STATE;
	}


	return __convert_streamrecorder_error_code(__func__, mm_streamrecorder_pause(handle->mm_handle));
}

int _streamrecorder_commit(streamrecorder_h recorder)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;
	int state = MM_STREAMRECORDER_STATE_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = streamrecorder_get_state(recorder, &state);
	if (ret != MM_ERROR_NONE) {
		return __convert_streamrecorder_error_code(__func__, ret);
	}
	
	if (state != MM_STREAMRECORDER_STATE_RECORDING) {
		LOGE("STREAMRECORDER_ERROR_INVALID_STATE (state:%d)", state);
		return STREAMRECORDER_ERROR_INVALID_STATE;
	}

	return __convert_streamrecorder_error_code(__func__, mm_streamrecorder_commit(handle->mm_handle));
}

int _streamrecorder_cancel(streamrecorder_h recorder)
{
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	return __convert_streamrecorder_error_code(__func__, mm_streamrecorder_cancel(handle->mm_handle));
}

int _streamrecorder_set_video_framerate(streamrecorder_h recorder , int framerate)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_set_attributes(handle->mm_handle, NULL,
								MMSTR_VIDEO_FRAMERATE, framerate,
								NULL);

	return __convert_streamrecorder_error_code(__func__, ret);

}

int _streamrecorder_get_video_framerate(streamrecorder_h recorder, int *framerate)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if (framerate == NULL) {
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_get_attributes(handle->mm_handle, NULL,
								MMSTR_VIDEO_FRAMERATE, framerate,
								NULL);

	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_set_video_source_format(streamrecorder_h recorder , int format)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	ret = mm_streamrecorder_set_attributes(handle->mm_handle, NULL,
								MMSTR_VIDEO_SOURCE_FORMAT, format,
								NULL);

	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_get_video_source_format(streamrecorder_h recorder, int *format)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (format == NULL) {
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_get_attributes(handle->mm_handle, NULL,
								MMSTR_VIDEO_SOURCE_FORMAT, format,
								NULL);

	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_set_video_resolution(streamrecorder_h recorder, int width, int height)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;
	streamrecorder_state_e state;

	if (handle == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	streamrecorder_get_state(recorder, &state);
	if (state > STREAMRECORDER_STATE_CREATED) {
		LOGE("STREAMRECORDER_ERROR_INVALID_STATE (state:%d)", state);
		return STREAMRECORDER_ERROR_INVALID_STATE;
	}

	if (width == 0 || height == 0) {
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_set_attributes(handle->mm_handle, NULL,
								MMSTR_VIDEO_RESOLUTION_WIDTH, width,
								MMSTR_VIDEO_RESOLUTION_HEIGHT, height,
								NULL);

	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_get_video_resolution(streamrecorder_h recorder, int *width, int *height)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (!handle) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}


	if (!width || !height) {
		LOGE("NULL pointer width = [%p], height = [%p]", width, height);
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_get_attributes(handle->mm_handle, NULL,
								MMSTR_VIDEO_RESOLUTION_WIDTH , width,
								MMSTR_VIDEO_RESOLUTION_HEIGHT , height,
								NULL);
	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_foreach_supported_video_resolution(streamrecorder_h recorder, streamrecorder_supported_video_resolution_cb foreach_cb, void *user_data)
{
	int i = 0;
	int ret = MM_ERROR_NONE;
	streamrecorder_s * handle = (streamrecorder_s *)recorder;

	if (!handle) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}


	if (!foreach_cb) {
		LOGE("NULL pointer callback");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	MMStreamRecorderAttrsInfo video_width;
	MMStreamRecorderAttrsInfo video_height;
	ret = mm_streamrecorder_get_attribute_info(handle->mm_handle, MMSTR_VIDEO_RESOLUTION_WIDTH, &video_width);
	ret |= mm_streamrecorder_get_attribute_info(handle->mm_handle, MMSTR_VIDEO_RESOLUTION_HEIGHT, &video_height);

	if (ret != MM_ERROR_NONE) {
		return __convert_streamrecorder_error_code(__func__, ret);
	 }
	for (i = 0 ; i < video_width.int_array.count ; i++) {
		if (!foreach_cb(video_width.int_array.array[i], video_height.int_array.array[i], user_data)) {
			break;
		}
	}

	return STREAMRECORDER_ERROR_NONE;
}


int _streamrecorder_set_filename(streamrecorder_h recorder,  const char *filename)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if (filename == NULL) {
		LOGE("filename is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	MMStreamRecorderStateType mmstate = MM_STREAMRECORDER_STATE_NONE;
	mm_streamrecorder_get_state(handle->mm_handle, &mmstate);
	if (mmstate >= MM_STREAMRECORDER_STATE_RECORDING) {
		LOGE("invalid state %d", mmstate);
		return STREAMRECORDER_ERROR_INVALID_STATE;
	}
	ret = mm_streamrecorder_set_attributes(handle->mm_handle, NULL,
								MMSTR_FILENAME, filename, strlen(filename),
								NULL);
	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_get_filename(streamrecorder_h recorder,  char **filename)
{
	int ret = MM_ERROR_NONE;
	char *record_filename = NULL;
	int record_filename_size;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if (filename == NULL) {
		LOGE("filename is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_get_attributes(handle->mm_handle, NULL,
								MMSTR_FILENAME, &record_filename, &record_filename_size,
								NULL);
	if (ret == MM_ERROR_NONE && record_filename) {
		*filename = strdup(record_filename);
	} else {
		LOGE("internal return (0x%08x), get filename p:%p", ret, record_filename);
		*filename = NULL;
	}
	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_set_file_format(streamrecorder_h recorder, streamrecorder_file_format_e format)
{
	int format_table[5] = {MM_FILE_FORMAT_3GP, /* STREAMRECORDER_FILE_FORMAT_3GP */
				MM_FILE_FORMAT_MP4, /* STREAMRECORDER_FILE_FORMAT_MP4 */
				MM_FILE_FORMAT_AMR, /* STREAMRECORDER_FILE_FORMAT_AMR */
				MM_FILE_FORMAT_AAC, /* STREAMRECORDER_FILE_FORMAT_ADTS */
				MM_FILE_FORMAT_WAV, /* STREAMRECORDER_FILE_FORMAT_WAV */
	};
	if (recorder == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if (format < STREAMRECORDER_FILE_FORMAT_3GP || format > STREAMRECORDER_FILE_FORMAT_WAV) {
		LOGE("invalid format %d", format);
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	return _streamrecorder_check_and_set_attribute(recorder, MMSTR_FILE_FORMAT, format_table[format]);

}

int _streamrecorder_get_file_format(streamrecorder_h recorder, streamrecorder_file_format_e *format)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;
	int mm_format;

	if (recorder == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if (format == NULL) {
		LOGE("format is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_get_attributes(handle->mm_handle, NULL,
								MMSTR_FILE_FORMAT, &mm_format,
								NULL);
	if (ret == MM_ERROR_NONE) {
		switch (mm_format) {
		case MM_FILE_FORMAT_3GP:
			*format = STREAMRECORDER_FILE_FORMAT_3GP;
			break;
		case MM_FILE_FORMAT_MP4:
			*format = STREAMRECORDER_FILE_FORMAT_MP4;
			break;
		case MM_FILE_FORMAT_AMR:
			*format = STREAMRECORDER_FILE_FORMAT_AMR;
			break;
		case MM_FILE_FORMAT_AAC:
			*format = STREAMRECORDER_FILE_FORMAT_ADTS;
			break;
		case MM_FILE_FORMAT_WAV:
			*format = STREAMRECORDER_FILE_FORMAT_WAV;
			break;
		default:
			ret = MM_ERROR_STREAMRECORDER_INTERNAL;
			break;
		}
	}

	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_foreach_supported_file_format(streamrecorder_h recorder, streamrecorder_supported_file_format_cb foreach_cb, void *user_data)
{
	int i = 0;
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;
	int format;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	if (foreach_cb == NULL) {
		LOGE("NULL pointer foreach_cb");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	MMStreamRecorderAttrsInfo info;
	ret = mm_streamrecorder_get_attribute_info(handle->mm_handle, MMSTR_FILE_FORMAT, &info);
	if (ret != MM_ERROR_NONE) {
		LOGE("mm_streamrecorder_get_attribute_info failed 0x%x", ret);
		return __convert_streamrecorder_error_code(__func__, ret);
	}

	for (i = 0 ; i < info.int_array.count ; i++) {
		switch (info.int_array.array[i]) {
		case MM_FILE_FORMAT_3GP:
			format = STREAMRECORDER_FILE_FORMAT_3GP;
			break;
		case MM_FILE_FORMAT_MP4:
			format = STREAMRECORDER_FILE_FORMAT_MP4;
			break;
		case MM_FILE_FORMAT_AMR:
			format = STREAMRECORDER_FILE_FORMAT_AMR;
			break;
		case MM_FILE_FORMAT_AAC:
			format = STREAMRECORDER_FILE_FORMAT_ADTS;
			break;
		case MM_FILE_FORMAT_WAV:
			format = STREAMRECORDER_FILE_FORMAT_WAV;
			break;
		default:
			format = -1;
			break;
		}

		if (format != -1 && !foreach_cb(format, user_data)) {
			break;
		}
	}

	return STREAMRECORDER_ERROR_NONE;
}

int _streamrecorder_set_size_limit(streamrecorder_h recorder, int kbyte)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if (kbyte < 0) {
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_set_attributes(handle->mm_handle, NULL,
								MMSTR_TARGET_MAX_SIZE, kbyte,
								NULL);
	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_set_time_limit(streamrecorder_h recorder, int second)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if (second < 0) {
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_set_attributes(handle->mm_handle, NULL,
								MMSTR_TARGET_TIME_LIMIT, second,
								NULL);
	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_set_audio_encoder(streamrecorder_h recorder, streamrecorder_audio_codec_e codec)
{
	streamrecorder_s *handle = (streamrecorder_s *)recorder;
	int audio_table[3] = { MM_AUDIO_CODEC_AMR,      /* STREAMRECORDER_AUDIO_CODEC_AMR */
				   MM_AUDIO_CODEC_AAC,      /* STREAMRECORDER_AUDIO_CODEC_AAC */
				   MM_AUDIO_CODEC_WAVE      /* STREAMRECORDER_AUDIO_CODEC_PCM */
	};

	if (recorder == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if ((codec < STREAMRECORDER_AUDIO_CODEC_AMR || codec > STREAMRECORDER_AUDIO_CODEC_PCM)) {
		LOGE("invalid parameter : codec %d", codec);
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	return _streamrecorder_check_and_set_attribute(recorder, MMSTR_AUDIO_ENCODER, audio_table[codec]);
}

int _streamrecorder_get_audio_encoder(streamrecorder_h recorder, streamrecorder_audio_codec_e *codec)
{
	int ret = MM_ERROR_NONE;
	int mm_codec = 0;
	int audio_enable = 0;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if (codec == NULL) {
		LOGE("codec is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_get_attributes(handle->mm_handle, NULL,
								MMSTR_AUDIO_ENCODER, &mm_codec,
								MMSTR_AUDIO_ENABLE, &audio_enable,
								NULL);

	if (ret == MM_ERROR_NONE && audio_enable != 0) {
		switch (mm_codec) {
		case MM_AUDIO_CODEC_AMR:
			*codec = STREAMRECORDER_AUDIO_CODEC_AMR;
			break;
		case MM_AUDIO_CODEC_AAC:
			*codec = STREAMRECORDER_AUDIO_CODEC_AAC;
			break;
		case MM_AUDIO_CODEC_WAVE:
			*codec = STREAMRECORDER_AUDIO_CODEC_PCM;
			break;
		default:
			ret = MM_ERROR_STREAMRECORDER_INTERNAL;
			break;
		}
	} else {
		ret = MM_ERROR_STREAMRECORDER_INTERNAL;
	}

	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_set_video_encoder(streamrecorder_h recorder, streamrecorder_video_codec_e codec)
{
	int ret = MM_ERROR_NONE;
	int video_table[2] = {MM_VIDEO_CODEC_H263,     /* STREAMRECORDER_VIDEO_CODEC_H263 */
				MM_VIDEO_CODEC_MPEG4,    /* STREAMRECORDER_VIDEO_CODEC_MPEG4 */
							};
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (handle == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}


	if (codec < STREAMRECORDER_VIDEO_CODEC_H263 || codec > STREAMRECORDER_VIDEO_CODEC_MPEG4) {
		LOGE("invalid codec %d", codec);
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_set_attributes(handle->mm_handle, NULL,
								MMSTR_VIDEO_ENCODER, video_table[codec],
								NULL);
	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_get_video_encoder(streamrecorder_h recorder, streamrecorder_video_codec_e *codec)
{
	int ret = MM_ERROR_NONE;
	int mm_codec = 0;
	int video_enable = 0;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (handle == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	if (codec == NULL) {
		LOGE("codec is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_get_attributes(handle->mm_handle, NULL,
								MMSTR_VIDEO_ENABLE, &video_enable,
								MMSTR_VIDEO_ENCODER, &mm_codec,
								NULL);
	if (ret == MM_ERROR_NONE && video_enable != 0) {
		switch (mm_codec) {
		case MM_VIDEO_CODEC_H263:
			*codec = STREAMRECORDER_VIDEO_CODEC_H263;
			break;
		case MM_VIDEO_CODEC_MPEG4:
			*codec = STREAMRECORDER_VIDEO_CODEC_MPEG4;
			break;
		default:
			ret = MM_ERROR_STREAMRECORDER_INTERNAL;
			break;
		}
	} else {
		ret = MM_ERROR_STREAMRECORDER_INTERNAL;
	}

	return __convert_streamrecorder_error_code(__func__, ret);
}


int _streamrecorder_set_audio_samplerate(streamrecorder_h recorder, int samplerate)
{
	if (samplerate < 1) {
		LOGE("invalid samplerate %d", samplerate);
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	return _streamrecorder_check_and_set_attribute(recorder, MMSTR_AUDIO_SAMPLERATE, samplerate);

}

int _streamrecorder_set_audio_encoder_bitrate(streamrecorder_h recorder, int bitrate)
{
	if (bitrate <= 0) {
		LOGE("invalid bitrate %d", bitrate);
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	return _streamrecorder_check_and_set_attribute(recorder, MMSTR_AUDIO_BITRATE, bitrate);

}

int _streamrecorder_set_video_encoder_bitrate(streamrecorder_h recorder, int bitrate)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (handle == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if (bitrate < 0) {
		LOGE("Invalid bitrate %d", bitrate);
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_set_attributes(handle->mm_handle, NULL, MMSTR_VIDEO_BITRATE, bitrate, NULL);

	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_get_size_limit(streamrecorder_h recorder, int *kbyte)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if (kbyte == NULL) {
		LOGE("Size limit is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_get_attributes(handle->mm_handle, NULL, MMSTR_TARGET_MAX_SIZE, kbyte, NULL);

	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_get_time_limit(streamrecorder_h recorder, int *second)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if (second == NULL) {
		LOGE("Time limit is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_get_attributes(handle->mm_handle, NULL, MMSTR_TARGET_TIME_LIMIT, second, NULL);

	return __convert_streamrecorder_error_code(__func__, ret);
}


int _streamrecorder_get_audio_samplerate(streamrecorder_h recorder, int *samplerate)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if (samplerate == NULL) {
		LOGE("samplerate is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_get_attributes(handle->mm_handle, NULL,
								MMSTR_AUDIO_SAMPLERATE, samplerate,
								NULL);

	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_get_audio_encoder_bitrate(streamrecorder_h recorder, int *bitrate)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if (bitrate == NULL) {
		LOGE("bitrate is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_get_attributes(handle->mm_handle, NULL,
								MMSTR_AUDIO_BITRATE, bitrate,
								NULL);

	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_get_video_encoder_bitrate(streamrecorder_h recorder, int *bitrate)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (handle == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if (bitrate == NULL) {
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	ret = mm_streamrecorder_get_attributes(handle->mm_handle, NULL,
								MMSTR_VIDEO_BITRATE, bitrate,
								NULL);
	return __convert_streamrecorder_error_code(__func__, ret);
}

int _streamrecorder_foreach_supported_audio_encoder(streamrecorder_h recorder, streamrecorder_supported_audio_encoder_cb foreach_cb, void *user_data)
{
	int i = 0;
	int ret = MM_ERROR_NONE;
	int codec;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	if (foreach_cb == NULL) {
		LOGE("foreach_cb is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	MMStreamRecorderAttrsInfo info;
	ret = mm_streamrecorder_get_attribute_info(handle->mm_handle, MMSTR_AUDIO_ENCODER, &info);
	if (ret != MM_ERROR_NONE) {
		return __convert_streamrecorder_error_code(__func__, ret);
	}

	for (i = 0 ; i < info.int_array.count ; i++) {
		switch (info.int_array.array[i]) {
		case MM_AUDIO_CODEC_AMR:
			codec = STREAMRECORDER_AUDIO_CODEC_AMR;
			break;
		case MM_AUDIO_CODEC_AAC:
			codec = STREAMRECORDER_AUDIO_CODEC_AAC;
			break;
		case MM_AUDIO_CODEC_WAVE:
			codec = STREAMRECORDER_AUDIO_CODEC_PCM;
			break;
		default:
			codec = -1;
			break;
		}
		if (codec != -1 && !foreach_cb(codec, user_data)) {
			break;
		}
	}

	return STREAMRECORDER_ERROR_NONE;
}


int _streamrecorder_foreach_supported_video_encoder(streamrecorder_h recorder, streamrecorder_supported_video_encoder_cb foreach_cb, void *user_data)
{
	int i = 0;
	int ret = MM_ERROR_NONE;
	int codec;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (handle == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	if (foreach_cb == NULL) {
		LOGE("foreach_cb is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	MMStreamRecorderAttrsInfo info;
	ret = mm_streamrecorder_get_attribute_info(handle->mm_handle, MMSTR_VIDEO_ENCODER, &info);
	if (ret != MM_ERROR_NONE) {
		return __convert_streamrecorder_error_code(__func__, ret);
	}

	for (i = 0 ; i < info.int_array.count ; i++) {
		switch (info.int_array.array[i]) {
		case MM_VIDEO_CODEC_H263:
			codec = STREAMRECORDER_VIDEO_CODEC_H263;
			break;
		case MM_VIDEO_CODEC_MPEG4:
			codec = STREAMRECORDER_VIDEO_CODEC_MPEG4;
			break;
		default:
			codec = -1;
			break;
		}

		if (codec != -1 && !foreach_cb(codec, user_data)) {
			break;
		}
	}

	return STREAMRECORDER_ERROR_NONE;
}



int _streamrecorder_set_audio_channel(streamrecorder_h recorder, int channel_count)
{
	if (channel_count < 1) {
		LOGE("invalid channel %d", channel_count);
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	return _streamrecorder_check_and_set_attribute(recorder, MMSTR_AUDIO_CHANNEL, channel_count);
}

int _streamrecorder_get_audio_channel(streamrecorder_h recorder, int *channel_count)
{
	int ret = MM_ERROR_NONE;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("handle is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	if (channel_count == NULL) {
		LOGE("channel_count is NULL");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = mm_streamrecorder_get_attributes(handle->mm_handle, NULL,
								MMSTR_AUDIO_CHANNEL, channel_count,
								NULL);

	return  __convert_streamrecorder_error_code(__func__, ret);
}

