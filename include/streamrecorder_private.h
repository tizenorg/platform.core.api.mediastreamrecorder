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



#ifndef __TIZEN_MEDIA_STREAMRECORDER_PRIVATE_H__
#define	__TIZEN_MEDIA_STREAMRECORDER_PRIVATE_H__

#include <streamrecorder.h>
#include <mm.h>
#include <mm_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	_STREAMRECORDER_EVENT_TYPE_STATE_CHANGE,
	_STREAMRECORDER_EVENT_TYPE_RECORDING_LIMITED,
	_STREAMRECORDER_EVENT_TYPE_RECORDING_STATUS,
	_STREAMRECORDER_EVENT_TYPE_INTERRUPTED,
	_STREAMRECORDER_EVENT_TYPE_AUDIO_STREAM,
	_STREAMRECORDER_EVENT_TYPE_ERROR,
	_STREAMRECORDER_EVENT_TYPE_CONSUME_COMPLETE,
	_STREAMRECORDER_EVENT_TYPE_NUM
}_streamrecorder_event_e;


typedef enum {
	_STREAMRECORDER_TYPE_AUDIO = 0,
	_STREAMRECORDER_TYPE_VIDEO,
}_streamrecorder_type_e;


typedef struct {
	MMHandleType mm_handle;
	void* user_cb[_STREAMRECORDER_EVENT_TYPE_NUM];
	void* user_data[_STREAMRECORDER_EVENT_TYPE_NUM];
	unsigned int state;
	_streamrecorder_event_e type;
	double last_max_input_level;
}streamrecorder_s;

int __convert_streamrecorder_error_code(const char *func, int code);
int _streamrecorder_set_videosource_buffer(streamrecorder_h recorder);
int _streamrecorder_set_audiosource_buffer(streamrecorder_h recorder);
int _streamrecorder_get_state(streamrecorder_h recorder, streamrecorder_state_e *state);
int _streamrecorder_destroy(streamrecorder_h recorder);
int _streamrecorder_prepare(streamrecorder_h recorder);
int _streamrecorder_unprepare(streamrecorder_h recorder);
int _streamrecorder_start(streamrecorder_h recorder);
int _streamrecorder_pause(streamrecorder_h recorder);
int _streamrecorder_commit(streamrecorder_h recorder);
int _streamrecorder_cancel(streamrecorder_h recorder);
int _streamrecorder_set_video_framerate(streamrecorder_h recorder , int framerate);
int _streamrecorder_set_video_source_format(streamrecorder_h recorder , int format);
int _streamrecorder_set_video_resolution(streamrecorder_h recorder, int width, int height);
int _streamrecorder_get_video_resolution(streamrecorder_h recorder, int *width, int *height);
int _streamrecorder_foreach_supported_video_resolution(streamrecorder_h recorder,streamrecorder_supported_video_resolution_cb foreach_cb, void *user_data);
int _streamrecorder_set_filename(streamrecorder_h recorder,  const char *filename);
int _streamrecorder_get_filename(streamrecorder_h recorder,  char **filename);
int _streamrecorder_set_file_format(streamrecorder_h recorder, streamrecorder_file_format_e format);
int _streamrecorder_get_file_format(streamrecorder_h recorder, streamrecorder_file_format_e *format);
int _streamrecorder_foreach_supported_file_format(streamrecorder_h recorder, streamrecorder_supported_file_format_cb foreach_cb, void *user_data);
int _streamrecorder_set_size_limit(streamrecorder_h recorder, int kbyte);
int _streamrecorder_set_time_limit(streamrecorder_h recorder, int second);
int _streamrecorder_set_audio_encoder(streamrecorder_h recorder, streamrecorder_audio_codec_e codec);
int _streamrecorder_get_audio_encoder(streamrecorder_h recorder, streamrecorder_audio_codec_e *codec);
int _streamrecorder_set_video_encoder(streamrecorder_h recorder, streamrecorder_video_codec_e codec);
int _streamrecorder_get_video_encoder(streamrecorder_h recorder, streamrecorder_video_codec_e *codec);
int _streamrecorder_set_audio_samplerate(streamrecorder_h recorder, int samplerate);
int _streamrecorder_set_audio_encoder_bitrate(streamrecorder_h recorder, int bitrate);
int _streamrecorder_set_video_encoder_bitrate(streamrecorder_h recorder, int bitrate);
int _streamrecorder_get_size_limit(streamrecorder_h recorder, int *kbyte);
int _streamrecorder_get_time_limit(streamrecorder_h recorder, int *second);
int _streamrecorder_get_audio_samplerate(streamrecorder_h recorder, int *samplerate);
int _streamrecorder_get_audio_encoder_bitrate(streamrecorder_h recorder, int *bitrate);
int _streamrecorder_get_video_encoder_bitrate(streamrecorder_h recorder, int *bitrate);
int _streamrecorder_foreach_supported_audio_encoder(streamrecorder_h recorder, streamrecorder_supported_audio_encoder_cb foreach_cb, void *user_data);
int _streamrecorder_foreach_supported_video_encoder(streamrecorder_h recorder, streamrecorder_supported_video_encoder_cb foreach_cb, void *user_data);
bool _streamrecorder_attr_is_muted(streamrecorder_h recorder);
int _streamrecorder_set_audio_channel(streamrecorder_h recorder, int channel_count);
int _streamrecorder_get_audio_channel(streamrecorder_h recorder, int *channel_count);
#ifdef __cplusplus
}
#endif

#endif //__TIZEN_MEDIA_STREAMRECORDER_PRIVATE_H__


