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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <streamrecorder_private.h>
#include <streamrecorder.h>
#include <mm_streamrecorder.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dlog.h>
#include <tbm_type.h>
#include <tbm_surface.h>
#include <tbm_bufmgr.h>
#include <tbm_surface_internal.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TIZEN_N_STREAMRECORDER"

static int __mm_streamrecorder_msg_cb(int message, void *param, void *user_data);


int streamrecorder_create(streamrecorder_h *recorder)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	streamrecorder_s *handle = NULL;
	handle = (streamrecorder_s *)malloc(sizeof(streamrecorder_s));
	if (handle == NULL) {
		LOGE("[%s] malloc error", __func__);
		return STREAMRECORDER_ERROR_OUT_OF_MEMORY;
	}

	memset(handle, 0 , sizeof(streamrecorder_s));

	handle->pkt = (streamrecorder_mediapacket *)malloc(sizeof(streamrecorder_mediapacket));
	memset(handle->pkt, 0, sizeof(streamrecorder_mediapacket));

	ret = mm_streamrecorder_create(&handle->mm_handle);
	if (ret != MM_ERROR_NONE) {
		free(handle);
		LOGE("[%s] mm_streamrecorder_create fail", __func__);
		return __convert_streamrecorder_error_code(__func__, ret);
	}

	mm_streamrecorder_set_message_callback(handle->mm_handle, __mm_streamrecorder_msg_cb, (void *)handle);

	*recorder = (streamrecorder_h)handle;
	return ret;
}

int streamrecorder_get_state(streamrecorder_h recorder, streamrecorder_state_e *state)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	ret = _streamrecorder_get_state(recorder, state);

	return ret;
}

int streamrecorder_enable_source_buffer(streamrecorder_h recorder, streamrecorder_source_e type)
{
	int ret = STREAMRECORDER_ERROR_NONE;
	if (recorder == NULL) {
		LOGE("NULL video pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	if (type == STREAMRECORDER_SOURCE_VIDEO) {
		ret = _streamrecorder_set_videosource_buffer(recorder);
	} else if (type == STREAMRECORDER_SOURCE_AUDIO) {
		ret = _streamrecorder_set_audiosource_buffer(recorder);
	} else {
		ret = _streamrecorder_set_videosource_buffer(recorder);
		if (ret == STREAMRECORDER_ERROR_NONE) {
			ret = _streamrecorder_set_audiosource_buffer(recorder);
		}
	}
	return ret;
}

int streamrecorder_push_stream_buffer(streamrecorder_h recorder, media_packet_h packet)
{
	int ret = MM_ERROR_NONE;
	uint64_t pts = 0;
	bool flag = FALSE;
	tbm_surface_h surface = NULL;
	int width;
	int height;
	int i = 0;
	streamrecorder_mediapacket *pkt = NULL;

	if ((recorder == NULL) || (packet == NULL)) {
		LOGE("[%s] INVALID_PARAMETER(0x%08x)", __func__, STREAMRECORDER_ERROR_INVALID_PARAMETER);
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	streamrecorder_s *handle = (streamrecorder_s *)recorder;
	pkt = handle->pkt;

	/* pts */
	media_packet_get_pts(packet, &pts);
	media_packet_is_video(packet, &flag);
	if (flag) {
		media_format_h fmt = NULL;
		media_format_mimetype_e mimetype = 0;
		media_packet_get_format(packet, &fmt);
		media_format_get_video_info(fmt, &mimetype, &width, &height, NULL, NULL);
		media_format_unref(fmt);

		if ((mimetype == MEDIA_FORMAT_NV12) || (mimetype == MEDIA_FORMAT_NV21)) {
			MMVideoBuffer *video_buf = NULL;
			void *dataPtr = NULL;
			video_buf = (MMVideoBuffer *)malloc(sizeof(MMVideoBuffer));
			if (!video_buf) {
				LOGE("Failed to alloc MMVideoBuffer");
				return STREAMRECORDER_ERROR_OUT_OF_MEMORY;
			}
			memset(video_buf, 0x00, sizeof(MMVideoBuffer));
			media_packet_get_buffer_data_ptr(packet, (void **)&dataPtr);
			media_packet_get_tbm_surface(packet, &surface);
			if (surface == NULL) {
				LOGE("Failed to get surface");
				free(video_buf);
				return STREAMRECORDER_ERROR_INVALID_PARAMETER;
			}
			video_buf->handle_num = tbm_surface_internal_get_num_bos(surface);
			video_buf->type = MM_VIDEO_BUFFER_TYPE_TBM_BO;
			//video_buf->handle.paddr[0] = dataPtr;
			video_buf->data[0] =  dataPtr;
			video_buf->width[0] = width;
			video_buf->height[0] = height;
			media_packet_get_video_stride_width(packet, 0, &video_buf->stride_width[0]);
			media_packet_get_video_stride_width(packet, 0, &video_buf->stride_height[0]);
			video_buf->size[0] = width*height*3/2;

			for (i = 0; i < MAX_MPACKET_DATA; i++) {
				if (pkt->packet[i] == NULL) {
					pkt->packet[i] = packet;
					pkt->consumed_buf[i] = video_buf;
					pkt->is_video[i] = TRUE;
					break;
				} else {
					if (i == MAX_MPACKET_DATA -1) {
						free(video_buf);
						return STREAMRECORDER_ERROR_OUT_OF_MEMORY;
					}
				}
			}

			ret = mm_streamrecorder_push_stream_buffer(handle->mm_handle, MM_STREAM_TYPE_VIDEO, pts, video_buf, video_buf->size[0]);
		} else if (mimetype == MEDIA_FORMAT_I420) {
			void *buf_data = NULL;
			uint64_t buf_size = 0;
			ret = media_packet_get_buffer_size(packet, &buf_size);
			if (ret != MEDIA_PACKET_ERROR_NONE) {
				LOGW("buffer size get fail");
				return STREAMRECORDER_ERROR_INVALID_PARAMETER;
			}
			ret = media_packet_get_buffer_data_ptr(packet, (void **)&buf_data);
			if (ret != MEDIA_PACKET_ERROR_NONE) {
				LOGW("buffer size get fail");
				return STREAMRECORDER_ERROR_INVALID_PARAMETER;
			}
			ret = mm_streamrecorder_push_stream_buffer(handle->mm_handle, MM_STREAM_TYPE_VIDEO, pts, buf_data, buf_size);
		}
	} else {
		media_packet_is_audio(packet, &flag);
		if (flag) {
			void *buf_data = NULL;
			uint64_t buf_size = 0;

			ret = media_packet_get_buffer_size(packet, &buf_size);
			if (ret != MEDIA_PACKET_ERROR_NONE) {
				LOGW("buffer size get fail");
				return STREAMRECORDER_ERROR_INVALID_PARAMETER;
			}
			ret = media_packet_get_buffer_data_ptr(packet, &buf_data);
			if (ret != MEDIA_PACKET_ERROR_NONE) {
				LOGW("buffer size get fail");
				return STREAMRECORDER_ERROR_INVALID_PARAMETER;
			}

			for (i = 0; i < MAX_MPACKET_DATA; i++) {
				if (pkt->packet[i] == NULL) {
					pkt->packet[i] = packet;
					pkt->consumed_buf[i] = buf_data;
					pkt->is_video[i] = FALSE;
					break;
				} else {
					if (i == MAX_MPACKET_DATA -1) {
						return STREAMRECORDER_ERROR_OUT_OF_MEMORY;
					}
				}
			}
			ret = mm_streamrecorder_push_stream_buffer(handle->mm_handle, MM_STREAM_TYPE_AUDIO, pts, buf_data, buf_size);
		}

	}
	return __convert_streamrecorder_error_code(__func__, ret);
}


int streamrecorder_destroy(streamrecorder_h recorder)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_destroy(recorder);

	return ret;
}


int streamrecorder_prepare(streamrecorder_h recorder)
{
	int ret = STREAMRECORDER_ERROR_NONE;
	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	ret = _streamrecorder_prepare(recorder);

	return ret;
}


int streamrecorder_unprepare(streamrecorder_h recorder)
{
	int ret = STREAMRECORDER_ERROR_NONE;
	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	ret = _streamrecorder_unprepare(recorder);

	return ret;

}


int streamrecorder_start(streamrecorder_h recorder)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL startointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_start(recorder);
	return ret;
}


int streamrecorder_pause(streamrecorder_h recorder)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_pause(recorder);
	return ret;
}


int streamrecorder_commit(streamrecorder_h recorder)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_commit(recorder);
	return ret;
}


int streamrecorder_cancel(streamrecorder_h recorder)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_cancel(recorder);
	return ret;
}

int streamrecorder_set_video_framerate(streamrecorder_h recorder, int framerate)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_set_video_framerate(recorder, framerate);
	return ret;

}

int streamrecorder_get_video_framerate(streamrecorder_h recorder, int *framerate)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_get_video_framerate(recorder, framerate);
	return ret;
}

int streamrecorder_set_video_source_format(streamrecorder_h recorder, streamrecorder_video_source_format_e format)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_set_video_source_format(recorder, format);
	return ret;
}

int streamrecorder_get_video_source_format(streamrecorder_h recorder, streamrecorder_video_source_format_e *format)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_get_video_source_format(recorder, format);
	return ret;
}


int streamrecorder_set_video_resolution(streamrecorder_h recorder, int width, int height)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_set_video_resolution(recorder, width, height);
	return ret;
}

int streamrecorder_get_video_resolution(streamrecorder_h recorder, int *width, int *height)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_get_video_resolution(recorder, width , height);
	return ret;
}



int streamrecorder_foreach_supported_video_resolution(streamrecorder_h recorder,
								streamrecorder_supported_video_resolution_cb foreach_cb, void *user_data)
{

	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	ret = _streamrecorder_foreach_supported_video_resolution(recorder, foreach_cb, user_data);
	return ret;


}

int streamrecorder_set_filename(streamrecorder_h recorder,  const char *filename)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_set_filename(recorder, filename);
	return ret;
}


int streamrecorder_get_filename(streamrecorder_h recorder,  char **filename)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_get_filename(recorder, filename);
	return ret;
}


int streamrecorder_set_file_format(streamrecorder_h recorder, streamrecorder_file_format_e format)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_set_file_format(recorder, format);
	return ret;
}


int streamrecorder_get_file_format(streamrecorder_h recorder, streamrecorder_file_format_e *format)
{

	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_get_file_format(recorder, format);
	return ret;

}


int streamrecorder_set_notify_cb(streamrecorder_h recorder, streamrecorder_notify_cb callback, void *user_data)
{
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if (callback == NULL) {
		LOGE("NULL pointer callback");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	handle->user_cb[_STREAMRECORDER_EVENT_TYPE_NOTIFY] = callback;
	handle->user_data[_STREAMRECORDER_EVENT_TYPE_NOTIFY] = user_data;

	return STREAMRECORDER_ERROR_NONE;
}


int streamrecorder_unset_notify_cb(streamrecorder_h recorder)
{
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	handle->user_cb[_STREAMRECORDER_EVENT_TYPE_NOTIFY] = NULL;
	handle->user_data[_STREAMRECORDER_EVENT_TYPE_NOTIFY] = NULL;

	return STREAMRECORDER_ERROR_NONE;
}

int streamrecorder_set_buffer_consume_completed_cb(streamrecorder_h recorder, streamrecorder_consume_completed_cb callback, void *user_data)
{
	if (recorder == NULL)
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;
	if (callback == NULL)
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;

	handle->user_cb[_STREAMRECORDER_EVENT_TYPE_CONSUME_COMPLETE] = callback;
	handle->user_data[_STREAMRECORDER_EVENT_TYPE_CONSUME_COMPLETE] = user_data;
	return STREAMRECORDER_ERROR_NONE;
}

int streamrecorder_unset_buffer_consume_completed_cb(streamrecorder_h recorder)
{
	if (recorder == NULL)
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	handle->user_cb[_STREAMRECORDER_EVENT_TYPE_CONSUME_COMPLETE] = NULL;
	handle->user_data[_STREAMRECORDER_EVENT_TYPE_CONSUME_COMPLETE] = NULL;
	return STREAMRECORDER_ERROR_NONE;
}

int streamrecorder_set_error_cb(streamrecorder_h recorder, streamrecorder_error_cb callback, void *user_data)
{
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	if (callback == NULL) {
		LOGE("NULL pointer callback");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	handle->user_cb[_STREAMRECORDER_EVENT_TYPE_ERROR] = callback;
	handle->user_data[_STREAMRECORDER_EVENT_TYPE_ERROR] = user_data;

	return STREAMRECORDER_ERROR_NONE;
}


int streamrecorder_unset_error_cb(streamrecorder_h recorder)
{
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	handle->user_cb[_STREAMRECORDER_EVENT_TYPE_ERROR] = NULL;
	handle->user_data[_STREAMRECORDER_EVENT_TYPE_ERROR] = NULL;

	return STREAMRECORDER_ERROR_NONE;
}


int streamrecorder_set_recording_status_cb(streamrecorder_h recorder, streamrecorder_recording_status_cb callback, void *user_data)
{
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	if (callback == NULL) {
		LOGE("NULL pointer callback");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	handle->user_cb[_STREAMRECORDER_EVENT_TYPE_RECORDING_STATUS] = callback;
	handle->user_data[_STREAMRECORDER_EVENT_TYPE_RECORDING_STATUS] = user_data;

	return STREAMRECORDER_ERROR_NONE;
}


int streamrecorder_unset_recording_status_cb(streamrecorder_h recorder)
{
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	handle->user_cb[_STREAMRECORDER_EVENT_TYPE_RECORDING_STATUS] = NULL;
	handle->user_data[_STREAMRECORDER_EVENT_TYPE_RECORDING_STATUS] = NULL;

	return STREAMRECORDER_ERROR_NONE;
}


int streamrecorder_set_recording_limit_reached_cb(streamrecorder_h recorder, streamrecorder_recording_limit_reached_cb callback, void *user_data)
{
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	if (callback == NULL) {
		LOGE("NULL pointer callback");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	handle->user_cb[_STREAMRECORDER_EVENT_TYPE_RECORDING_LIMITED] = callback;
	handle->user_data[_STREAMRECORDER_EVENT_TYPE_RECORDING_LIMITED] = user_data;

	return STREAMRECORDER_ERROR_NONE;
}


int streamrecorder_unset_recording_limit_reached_cb(streamrecorder_h recorder)
{
	streamrecorder_s *handle = (streamrecorder_s *)recorder;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	handle->user_cb[_STREAMRECORDER_EVENT_TYPE_RECORDING_LIMITED] = NULL;
	handle->user_data[_STREAMRECORDER_EVENT_TYPE_RECORDING_LIMITED] = NULL;

	return STREAMRECORDER_ERROR_NONE;
}


int streamrecorder_foreach_supported_file_format(streamrecorder_h recorder, streamrecorder_supported_file_format_cb foreach_cb, void *user_data)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_foreach_supported_file_format(recorder, foreach_cb, user_data);
	return ret;

}


int streamrecorder_set_recording_limit(streamrecorder_h recorder, streamrecorder_recording_limit_type_e type, int limit)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	if (type == STREAMRECORDER_RECORDING_LIMIT_TYPE_SIZE)
		ret = _streamrecorder_set_size_limit(recorder, limit);
	else
		ret = _streamrecorder_set_time_limit(recorder, limit);
	return ret;
}

int streamrecorder_get_recording_limit(streamrecorder_h recorder, streamrecorder_recording_limit_type_e type, int *limit)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}
	if (type == STREAMRECORDER_RECORDING_LIMIT_TYPE_SIZE)
		ret = _streamrecorder_get_size_limit(recorder, limit);
	else
		ret = _streamrecorder_get_time_limit(recorder, limit);
	return ret;
}

int streamrecorder_set_audio_encoder(streamrecorder_h recorder, streamrecorder_audio_codec_e codec)
{

	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_set_audio_encoder(recorder, codec);
	return ret;

}


int streamrecorder_get_audio_encoder(streamrecorder_h recorder, streamrecorder_audio_codec_e *codec)
{

	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_get_audio_encoder(recorder, codec);
	return ret;

}


int streamrecorder_set_video_encoder(streamrecorder_h recorder, streamrecorder_video_codec_e codec)
{

	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_set_video_encoder(recorder, codec);
	return ret;

}


int streamrecorder_get_video_encoder(streamrecorder_h recorder, streamrecorder_video_codec_e *codec)
{

	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_get_video_encoder(recorder, codec);
	return ret;

}


int streamrecorder_set_audio_samplerate(streamrecorder_h recorder, int samplerate)
{

	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_set_audio_samplerate(recorder, samplerate);
	return ret;

}


int streamrecorder_set_audio_encoder_bitrate(streamrecorder_h recorder, int bitrate)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_set_audio_encoder_bitrate(recorder, bitrate);
	return ret;

}

int streamrecorder_set_video_encoder_bitrate(streamrecorder_h recorder, int bitrate)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_set_video_encoder_bitrate(recorder, bitrate);
	return ret;
}

int streamrecorder_get_audio_samplerate(streamrecorder_h recorder, int *samplerate)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_get_audio_samplerate(recorder, samplerate);
	return ret;
}

int streamrecorder_get_audio_encoder_bitrate(streamrecorder_h recorder, int *bitrate)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_get_audio_encoder_bitrate(recorder, bitrate);
	return ret;
}


int streamrecorder_get_video_encoder_bitrate(streamrecorder_h recorder, int *bitrate)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_get_video_encoder_bitrate(recorder, bitrate);
	return ret;
}


int streamrecorder_foreach_supported_audio_encoder(streamrecorder_h recorder, streamrecorder_supported_audio_encoder_cb foreach_cb, void *user_data)
{

	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_foreach_supported_audio_encoder(recorder, foreach_cb, user_data);
	return ret;

}


int streamrecorder_foreach_supported_video_encoder(streamrecorder_h recorder, streamrecorder_supported_video_encoder_cb foreach_cb, void *user_data)
{

	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_foreach_supported_video_encoder(recorder, foreach_cb, user_data);
	return ret;

}


int streamrecorder_set_audio_channel(streamrecorder_h recorder, int channel_count)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_set_audio_channel(recorder, channel_count);
	return ret;
}


int streamrecorder_get_audio_channel(streamrecorder_h recorder, int *channel_count)
{
	int ret = STREAMRECORDER_ERROR_NONE;

	if (recorder == NULL) {
		LOGE("NULL pointer handle");
		return STREAMRECORDER_ERROR_INVALID_PARAMETER;
	}

	ret = _streamrecorder_get_audio_channel(recorder, channel_count);
	return ret;

}

static int __mm_streamrecorder_msg_cb(int message, void *param, void *user_data)
{
	streamrecorder_s * handle = (streamrecorder_s *)user_data;
	MMMessageParamType *m = (MMMessageParamType *)param;
	streamrecorder_notify_e notification = STREAMRECORDER_NOTIFY_NONE;
	streamrecorder_state_e previous_state;
	streamrecorder_recording_limit_type_e type;
	int streamrecorder_error = 0;
	switch (message) {
	case MM_MESSAGE_READY_TO_RESUME:
		LOGW("not supported message");
		break;
	case MM_MESSAGE_STREAMRECORDER_STATE_CHANGED:
		previous_state = handle->state;
		notification = STREAMRECORDER_NOTIFY_STATE_CHANGED;
		if (previous_state != handle->state && handle->user_cb[_STREAMRECORDER_EVENT_TYPE_NOTIFY])
			((streamrecorder_notify_cb)handle->user_cb[_STREAMRECORDER_EVENT_TYPE_NOTIFY])(previous_state, handle->state, notification, handle->user_data[_STREAMRECORDER_EVENT_TYPE_NOTIFY]);

		break;
	case MM_MESSAGE_STREAMRECORDER_MAX_SIZE:
	case MM_MESSAGE_STREAMRECORDER_NO_FREE_SPACE:
	case MM_MESSAGE_STREAMRECORDER_TIME_LIMIT:
		if (MM_MESSAGE_STREAMRECORDER_MAX_SIZE == message) {
			type = STREAMRECORDER_RECORDING_LIMIT_TYPE_SIZE;
		} else {
			type = STREAMRECORDER_RECORDING_LIMIT_TYPE_TIME;
		}
		if (handle->user_cb[_STREAMRECORDER_EVENT_TYPE_RECORDING_LIMITED]) {
			((streamrecorder_recording_limit_reached_cb)handle->user_cb[_STREAMRECORDER_EVENT_TYPE_RECORDING_LIMITED])(type, handle->user_data[_STREAMRECORDER_EVENT_TYPE_RECORDING_LIMITED]);
		}
		break;
	case MM_MESSAGE_STREAMRECORDER_RECORDING_STATUS:
		if (handle->user_cb[_STREAMRECORDER_EVENT_TYPE_RECORDING_STATUS]) {
			((streamrecorder_recording_status_cb)handle->user_cb[_STREAMRECORDER_EVENT_TYPE_RECORDING_STATUS])(m->recording_status.elapsed, m->recording_status.filesize, handle->user_data[_STREAMRECORDER_EVENT_TYPE_RECORDING_STATUS]);
		}
		break;

	case MM_MESSAGE_STREAMRECORDER_VIDEO_CAPTURED:
	{
		MMStreamRecordingReport *report = (MMStreamRecordingReport *)m->data;
		if (report != NULL && report->recording_filename) {
			free(report->recording_filename);
			report->recording_filename = NULL;
		}
		if (report) {
			free(report);
			report = NULL;
		}
	}	break;
	case MM_MESSAGE_STREAMRECORDER_CONSUME_COMPLETE:
	{
		void *consume_data = (m->consumed_mediabuffer).consumed_buffer;
		void *consume = NULL;
		streamrecorder_mediapacket *pkt;
		int i = 0;

		if (consume_data == NULL)
			break;

		pkt = handle->pkt;

		for (i = 0; i < MAX_MPACKET_DATA; i++) {
			if (pkt->consumed_buf[i] == consume_data) {
				consume = pkt->packet[i];
				if (pkt->is_video[i] == TRUE) {
					free(pkt->consumed_buf[i]);		// MMVideoBuffer free
					pkt->consumed_buf[i] = NULL;
				}
			}
		}

		if (handle->user_cb[_STREAMRECORDER_EVENT_TYPE_CONSUME_COMPLETE]) {
			((streamrecorder_consume_completed_cb)handle->user_cb[_STREAMRECORDER_EVENT_TYPE_CONSUME_COMPLETE])(consume, handle->user_data[_STREAMRECORDER_EVENT_TYPE_CONSUME_COMPLETE]);
		}
	}	break;
	case MM_MESSAGE_STREAMRECORDER_ERROR:
		switch (m->code) {
		case MM_ERROR_STREAMRECORDER_GST_CORE:
		case MM_ERROR_STREAMRECORDER_GST_LIBRARY:
		case MM_ERROR_STREAMRECORDER_GST_RESOURCE:
		case MM_ERROR_STREAMRECORDER_GST_STREAM:
		case MM_ERROR_STREAMRECORDER_GST_NEGOTIATION:
		case MM_ERROR_STREAMRECORDER_GST_FLOW_ERROR:
		case MM_ERROR_STREAMRECORDER_ENCODER:
		case MM_ERROR_STREAMRECORDER_ENCODER_BUFFER:
		case MM_ERROR_STREAMRECORDER_ENCODER_WORKING:
		case MM_ERROR_STREAMRECORDER_INTERNAL:
		case MM_ERROR_FILE_NOT_FOUND:
		case MM_ERROR_FILE_READ:
			streamrecorder_error = STREAMRECORDER_ERROR_INVALID_OPERATION;
			break;
		case MM_ERROR_OUT_OF_STORAGE:
			streamrecorder_error = STREAMRECORDER_ERROR_OUT_OF_STORAGE;
			break;
		default:
			streamrecorder_error = STREAMRECORDER_ERROR_INVALID_OPERATION;
			break;
		}

		if (streamrecorder_error != 0 && handle->user_cb[_STREAMRECORDER_EVENT_TYPE_ERROR]) {
			((streamrecorder_error_cb)handle->user_cb[_STREAMRECORDER_EVENT_TYPE_ERROR])(streamrecorder_error, handle->state, handle->user_data[_STREAMRECORDER_EVENT_TYPE_ERROR]);
		}
		break;
	default:
		break;
		}
		return 1;
}

