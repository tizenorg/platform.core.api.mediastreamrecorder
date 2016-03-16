/*
* recorder_testsuite
*
* Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd. All rights reserved.
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
*
*/

/*=======================================================================================
|  INCLUDE FILES                                                                        |
=======================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gst/gst.h>
#include <sys/time.h>
#include <media_format.h>
#include <streamrecorder.h>
#include <streamrecorder_private.h>

#define LOGD g_print
#define LOGE g_print
#define LOGW g_print
/*-----------------------------------------------------------------------
|    GLOBAL VARIABLE DEFINITIONS:                                       |
-----------------------------------------------------------------------*/
#define EXPORT_API __attribute__((__visibility__("default")))

#define PACKAGE "recorder_testsuite"

GMainLoop *g_loop;
GIOChannel *stdin_channel;
int resolution_set;
int g_current_state;
int src_w, src_h;
GstCaps *filtercaps;
int recorder_state;
static GTimer *timer = NULL;
void *display;

/*-----------------------------------------------------------------------
|    GLOBAL CONSTANT DEFINITIONS:                                       |
-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------
|    IMPORTED VARIABLE DECLARATIONS:                                    |
-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------
|    IMPORTED FUNCTION DECLARATIONS:                                    |
-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------
|    LOCAL #defines:                                                    |
-----------------------------------------------------------------------*/


#define DISPLAY_W_320                       320                 /*for direct FB*/
#define DISPLAY_H_240                       240                 /*for direct FB*/


#define SRC_VIDEO_FRAME_RATE_15         15    /* video input frame rate */
#define SRC_VIDEO_FRAME_RATE_30         30    /* video input frame rate */
#define IMAGE_ENC_QUALITY               85    /* quality of jpeg */

#define MAX_FILE_SIZE_FOR_MMS           (250 * 1024)

#define EXT_JPEG                        "jpg"
#define EXT_MP4                         "mp4"
#define EXT_3GP                         "3gp"
#define EXT_AMR                         "amr"
#define EXT_MKV                         "mkv"

#define TARGET_FILENAME_PATH            "/opt/usr/media/"
#define IMAGE_CAPTURE_EXIF_PATH         TARGET_FILENAME_PATH"exif.raw"
#define TARGET_FILENAME_VIDEO           TARGET_FILENAME_PATH"test.mp4"
#define TARGET_FILENAME_AUDIO           TARGET_FILENAME_PATH"test_rec_audio.m4a"
#define CAPTURE_FILENAME_LEN            256

#define AUDIO_SOURCE_SAMPLERATE_AAC     44100
#define AUDIO_SOURCE_SAMPLERATE_AMR     8000
#define AUDIO_SOURCE_FORMAT             MM_STREAMRECORDER_AUDIO_FORMAT_PCM_S16_LE
#define AUDIO_SOURCE_CHANNEL_AAC        2
#define AUDIO_SOURCE_CHANNEL_AMR        1
#define VIDEO_ENCODE_BITRATE            40000000 /* bps */

#define CHECK_MM_ERROR(expr) \
do {\
	int ret = 0; \
	ret = expr; \
	if (ret != 0) {\
		printf("[%s:%d] error code : %x \n", __func__, __LINE__, ret); \
		return; \
	} \
} while (0)


#ifndef SAFE_FREE
#define SAFE_FREE(x)       if (x) { g_free(x); x = NULL; }
#endif


GTimeVal previous;
GTimeVal current;
GTimeVal result;
/*temp*/

/**
* Enumerations for command
*/
#define SENSOR_WHITEBALANCE_NUM     10
#define SENSOR_COLOR_TONE_NUM       31
#define SENSOR_FLIP_NUM         3
#define SENSOR_PROGRAM_MODE_NUM     15
#define SENSOR_FOCUS_NUM        6
#define SENSOR_INPUT_ROTATION       4
#define SENSOR_AF_SCAN_NUM      4
#define SENSOR_ISO_NUM          8
#define SENSOR_EXPOSURE_NUM     9
#define SENSOR_IMAGE_FORMAT     9


/*-----------------------------------------------------------------------
|    LOCAL CONSTANT DEFINITIONS:                                        |
-----------------------------------------------------------------------*/
enum {
	MODE_LIVE_BUFFER_I420_VIDEO , /*video capturing mode from live buffer I420 using software encoder*/
	MODE_LIVE_BUFFER_NV12_VIDEO_SW , /*video capturing mode from live buffer NV12 using software encoder */
	MODE_LIVE_BUFFER_NV12_VIDEO_HW , /*video capturing mode from live buffer NV12 using hardware encoder*/
	MODE_NUM,
};

enum {
	MENU_STATE_MAIN,
	MENU_STATE_NUM,
};

/*-----------------------------------------------------------------------
|    LOCAL DATA TYPE DEFINITIONS:                   |
-----------------------------------------------------------------------*/
typedef struct _mstreamrecorder_handle {
	streamrecorder_h recorder;
	int mode;                       /*video/audio(recording) mode */
	int menu_state;
	int fps;
	bool isMute;
	unsigned long long elapsed_time;
} mstreamrecorder_handle_t;


/*---------------------------------------------------------------------------
|    LOCAL VARIABLE DEFINITIONS:                                            |
---------------------------------------------------------------------------*/
static mstreamrecorder_handle_t *hmstreamrecorder ;

/*---------------------------------------------------------------------------
|    LOCAL FUNCTION PROTOTYPES:                                             |
---------------------------------------------------------------------------*/
static void print_menu();
static gboolean cmd_input(GIOChannel *channel);
static gboolean init(int type);
static gboolean mode_change();


void _recording_status_cb(unsigned long long elapsed_time, unsigned long long file_size, void *user_data)
{
	printf("elapsed time :%lld, file_size :%lld\n", elapsed_time, file_size);
}

void _recording_limit_reached_cb(streamrecorder_recording_limit_type_e type, void *user_data)
{
	g_print("limited!! %d\n", type);
	int *ischeck = (int *)user_data;
	*ischeck = 1;
}
static inline void flush_stdin()
{
	int ch;
	while ((ch = getchar()) != EOF && ch != '\n');
}


static void print_menu()
{
	switch (hmstreamrecorder->menu_state) {
	case MENU_STATE_MAIN:
	if (hmstreamrecorder->mode == MODE_LIVE_BUFFER_I420_VIDEO || hmstreamrecorder->mode == MODE_LIVE_BUFFER_NV12_VIDEO_SW) {
	    g_print("\n\t=======================================\n");
	    if (recorder_state <= STREAMRECORDER_STATE_NONE) {
			g_print("\t   '1' Start Recording\n");
			g_print("\t   'b' back\n");
		} else if (recorder_state == STREAMRECORDER_STATE_RECORDING) {
			g_print("\t   'p' Pause Recording\n");
			g_print("\t   'c' Cancel\n");
			g_print("\t   's' Save\n");
		} else if (recorder_state == STREAMRECORDER_STATE_PAUSED) {
			g_print("\t   'r' Resume Recording\n");
			g_print("\t   's' Save\n");
		}
	}
	break;

	default:
		LOGE("unknow menu state !!\n");
		break;
	}
	return;
}

media_packet_h streamrecorder_make_media_packet(int video, void *data, int size)
{
	guint8 *pkt_data;
	media_packet_h out_pkt;
	media_format_h output_fmt;
	if (media_format_create(&output_fmt)) {
		g_print("media_format_create failed\n");
		return NULL;
	}

	if (video) {
		if (hmstreamrecorder->mode == MODE_LIVE_BUFFER_NV12_VIDEO_SW) {
			media_format_set_video_mime(output_fmt, MEDIA_FORMAT_NV12);
		} else if (hmstreamrecorder->mode == MODE_LIVE_BUFFER_I420_VIDEO) {
			media_format_set_video_mime(output_fmt, MEDIA_FORMAT_I420);
		}
		media_format_set_video_width(output_fmt, 1280);
		media_format_set_video_height(output_fmt, 720);
		if (media_format_set_video_frame_rate(output_fmt, 30)) {
			g_print("media_format_set_video_frame_rate failed\n");
			return NULL;
		}
	} else {
		media_format_set_audio_mime(output_fmt, MEDIA_FORMAT_PCM);
		media_format_set_audio_channel(output_fmt, 1);
		media_format_set_audio_samplerate(output_fmt, 44100);
		/*media_format_set_audio_bit(core->output_fmt, info->bit);*/
	}
	if (media_packet_create(output_fmt, NULL, NULL, &out_pkt)) {
		g_print("create video media_packet failed\n");
		return NULL;
	}
	if (media_packet_alloc(out_pkt)) {
		g_print("video media_packet alloc failed\n");
		return NULL;
	}
	media_packet_get_buffer_data_ptr(out_pkt, (void **)&pkt_data);
	memcpy((char *)pkt_data, data, size);
	if (media_packet_set_buffer_size(out_pkt, (uint64_t)(size))) {
		g_print("video set_buffer_size failed\n");
		return NULL;
	}

	return out_pkt;
}

void feed_video_es(GstElement *element, GstBuffer *buffer, GstPad *pad, gpointer data)
{
	guint n;
	GstMemory *mem;
	GstMapInfo map = GST_MAP_INFO_INIT;
	media_packet_h out_pkt = NULL;

	gst_buffer_ref(buffer);

	n = gst_buffer_n_memory(buffer);

	mem = gst_buffer_peek_memory(buffer, n-1);

	gst_memory_map(mem, &map, GST_MAP_READ);
	out_pkt = streamrecorder_make_media_packet(1, map.data, map.size);
	gst_memory_unmap(mem, &map);
	if (out_pkt) {
		media_packet_set_pts(out_pkt, buffer->pts);
		media_packet_set_extra(out_pkt, buffer);
		streamrecorder_push_stream_buffer(hmstreamrecorder->recorder, out_pkt);
	}
	return;
}

GstElement *vpipeline, *vconvert, *vsrc, *vfakesink, *vcaps;
void close_pipeline(void)
{
	if (vpipeline)
		gst_object_unref(vpipeline);
	if (vsrc)
		gst_object_unref(vsrc);
	if (vfakesink)
		gst_object_unref(vfakesink);
	if (vcaps)
		gst_object_unref(vcaps);
	if (vconvert)
		gst_object_unref(vconvert);
}


int __feed_buffer_test(MMHandleType handle, int mode)
{

	GstCaps *caps = NULL;
	g_print("creating __feed_buffer_test pipeline\n");
	if (!handle)
		return FALSE;
	/* create video pipeline */
	vpipeline = gst_pipeline_new(NULL);
	if (!vpipeline) {
		g_print("pipeline create fail\n");
		return FALSE;
	}
	if (mode == MODE_LIVE_BUFFER_NV12_VIDEO_SW) {
		vsrc = gst_element_factory_make("videotestsrc", NULL);
		if (!vsrc) {
			g_print("src element creation failed\n");
			return FALSE;
		}
	} else {
		vsrc = gst_element_factory_make("videotestsrc", NULL);
		if (!vsrc) {
			g_print("src element creation failed\n");
			return FALSE;
		}
	}
	if (mode == MODE_LIVE_BUFFER_NV12_VIDEO_SW) {
	vconvert = gst_element_factory_make("videoconvert", NULL);
		if (!vconvert) {
			g_print("convert element creation failed\n");
			return FALSE;
		}
	}
	vcaps = gst_element_factory_make("capsfilter", NULL);
	if (!vcaps) {
	    g_print("capsfilter element creation failed\n");
	    return FALSE;
	}
	if (mode == MODE_LIVE_BUFFER_I420_VIDEO) {
	caps = gst_caps_new_simple("video/x-raw",
				"format", G_TYPE_STRING, "I420",
				"width", G_TYPE_INT, 1280,
				"height", G_TYPE_INT, 720, NULL);
	} else if (mode == MODE_LIVE_BUFFER_NV12_VIDEO_SW) {
	caps = gst_caps_new_simple("video/x-raw",
				"format", G_TYPE_STRING, "NV12",
				"width", G_TYPE_INT, 1280,
				"height", G_TYPE_INT, 720,
				"framerate", GST_TYPE_FRACTION, 30, 1,  NULL);
	}
	g_object_set(vcaps, "caps", caps, NULL);
	gst_caps_unref(caps);

	vfakesink = gst_element_factory_make("fakesink", NULL);
	if (!vfakesink) {
	    g_print("video fakesink creation failed\n");
	    return FALSE;
	}
	g_object_set(GST_OBJECT(vfakesink), "signal-handoffs", TRUE, NULL);
	g_signal_connect(vfakesink, "handoff", G_CALLBACK(feed_video_es), handle);

	if (mode == MODE_LIVE_BUFFER_I420_VIDEO) {
	gst_bin_add_many(GST_BIN(vpipeline), vsrc, vcaps, vfakesink, NULL);
		/* link elements */
		if (!gst_element_link_many(vsrc, vcaps, vfakesink, NULL)) {
			GST_WARNING("Can't link elements video pipeline");
			return FALSE;
		}
	} else if (mode == MODE_LIVE_BUFFER_NV12_VIDEO_SW) {
	gst_bin_add_many(GST_BIN(vpipeline), vsrc, vconvert, vcaps, vfakesink, NULL);
	/* link elements */
		if (!gst_element_link_many(vsrc , vconvert, vcaps, vfakesink, NULL)) {
			GST_WARNING("Can't link elements video pipeline");
			return FALSE;
		}
	}
	g_print("Linking done video pipeline\n");

#if 0
	GstElement *a_pipeline, *asrc, *afakesink;
	/* create common elements */
	a_pipeline = gst_pipeline_new(NULL);
	if (v_pipeline) {
	    g_print("pipeline create fail\n");
	    return FALSE;
	}
	asrc = gst_element_factory_make("audiotestsrc", NULL);
	if (!asrc) {
	    g_print("src element creation failed\n");
	    return FALSE;
	}
	afakesink = gst_element_factory_make("fakesink", NULL);
	if (!afakesink) {
	    g_print("audio fakesink creation failed\n");
	    return FALSE;
	}
	/* connect handoff */
	g_object_set(GST_OBJECT(afakesink), "signal-handoffs", TRUE, NULL);
	g_signal_connect(fakesink, "handoff", G_CALLBACK(feed_audio_es), handle);

	gst_bin_add_many(GST_BIN(v_pipeline), asrc, afakesink, NULL);

	/* link elements */
	if (!gst_element_link_many(asrc, afakesink, NULL)) {
	    GST_WARNING("Can't link elements audio pipeline");
	    return FALSE;
	}
	g_print("Linking done audio pipeline\n");
	gst_element_set_state(apipeline, GST_STATE_PLAYING);
#endif
	gst_element_set_state(vpipeline, GST_STATE_PLAYING);
	g_print("feed_task pipeline is playing\n");

	return 0;
}


static void main_menu(gchar buf)
{
	int err = 0;

	if (hmstreamrecorder->mode == MODE_LIVE_BUFFER_I420_VIDEO || hmstreamrecorder->mode == MODE_LIVE_BUFFER_NV12_VIDEO_SW) {
	    if (recorder_state == STREAMRECORDER_STATE_NONE) {
			switch (buf) {
			case '1': /* Start Recording*/
				hmstreamrecorder->elapsed_time = 0;
				err = streamrecorder_start(hmstreamrecorder->recorder);
				sleep(1);
				g_timer_reset(timer);
				err = __feed_buffer_test(hmstreamrecorder->recorder, hmstreamrecorder->mode);
				if (err != MM_ERROR_NONE)

				g_print("Rec star in live buffer 0x%x", err);
				if (err == 0)
				    LOGE("Rec start live buffer 0x%x", err);

				recorder_state = STREAMRECORDER_STATE_RECORDING;
			break;

			case 'b': /* back*/
				hmstreamrecorder->menu_state = MENU_STATE_MAIN;
				mode_change();
			break;

			default:
				g_print("\t Invalid input \n");
			break;
			}
		} else if (recorder_state == STREAMRECORDER_STATE_RECORDING || recorder_state == STREAMRECORDER_STATE_PAUSED) {
			switch (buf) {
			    if (recorder_state == STREAMRECORDER_STATE_RECORDING) {
					case 'p': /* Pause Recording*/
						g_print("*Pause!\n");
						err = streamrecorder_pause(hmstreamrecorder->recorder);

						if (err < 0)
							LOGE("Rec pause streamrecorder_pause  = %x", err);

						recorder_state = STREAMRECORDER_STATE_PAUSED;
					break;

			    } else {
					case 'r': /* Resume Recording*/
						g_print("*Resume!\n");
						err = streamrecorder_start(hmstreamrecorder->recorder);
						if (err < 0)
							LOGE("Rec start streamrecorder_record  = %x", err);

						recorder_state = STREAMRECORDER_STATE_RECORDING;
						break;
			    }

			case 'c': /* Cancel*/
				g_print("*Cancel Recording !\n");

				err = streamrecorder_cancel(hmstreamrecorder->recorder);

				if (err < 0)
				    LOGE("Cancel recording streamrecorder_cancel  = %x", err);

				recorder_state = STREAMRECORDER_STATE_NONE;
				break;

			case 's': /* Save*/
				g_print("*Save Recording!\n");
				g_timer_reset(timer);

				err = streamrecorder_commit(hmstreamrecorder->recorder);

				if (err < 0)
				    LOGE("Save recording streamrecorder_commit  = %x", err);

				recorder_state = STREAMRECORDER_STATE_NONE;
				break;

			default:
				g_print("\t Invalid input \n");
				break;
			} /*switch*/
	    } else {
			LOGE("Wrong streamrecorder state, check status!!");
		}
	} else {
	    g_print("\t Invalid mode, back to upper menu \n");
	    hmstreamrecorder->menu_state = MENU_STATE_MAIN;
	    mode_change();
	}
}


/**
* This function is to execute command.
*
* @param    channel [in]    1st parameter
*
* @return   This function returns TRUE/FALSE
* @remark
* @see
*/
static gboolean cmd_input(GIOChannel *channel)
{
	gchar *buf = NULL;
	gsize read_size;
	GError *g_error = NULL;

	LOGD("ENTER");

	g_io_channel_read_line(channel, &buf, &read_size, NULL, &g_error);
	if (g_error) {
	    LOGD("g_io_channel_read_chars error");
	    g_error_free(g_error);
	    g_error = NULL;
	}

	if (buf) {
	    g_strstrip(buf);

	    LOGD("Menu Status : %d", hmstreamrecorder->menu_state);
	    switch (hmstreamrecorder->menu_state) {
	    case MENU_STATE_MAIN:
			main_menu(buf[0]);
		break;
	    default:
		break;
	    }

	    g_free(buf);
	    buf = NULL;

	    print_menu();
	} else {
	    LOGD("No read input");
	}

	return TRUE;
}

static gboolean init(int type)
{
	int err;
	int ischeck = 0;

	if (!hmstreamrecorder)
	    return FALSE;

	if (!hmstreamrecorder->recorder)
	    return FALSE;

	/*================================================================================
	Video capture mode
	*=================================================================================*/
	if (type == MODE_LIVE_BUFFER_I420_VIDEO || type == MODE_LIVE_BUFFER_NV12_VIDEO_SW) {
		if (type == MODE_LIVE_BUFFER_I420_VIDEO) {
			err = streamrecorder_set_video_source_format(hmstreamrecorder->recorder, STREAMRECORDER_VIDEO_SOURCE_FORMAT_I420);
			if (err < 0) {
				LOGE("Init fail. (%x)", err);
				goto ERROR;
			}
		} else if (type == MODE_LIVE_BUFFER_NV12_VIDEO_SW) {
			err = streamrecorder_set_video_source_format(hmstreamrecorder->recorder, STREAMRECORDER_VIDEO_SOURCE_FORMAT_NV12);
			if (err < 0) {
				LOGE("Init fail. (%x)", err);
				goto ERROR;
			}
		}
	    err = streamrecorder_set_file_format(hmstreamrecorder->recorder, STREAMRECORDER_FILE_FORMAT_MP4);
	    if (err < 0) {
			LOGE("aInit fail. (%x)", err);
			goto ERROR;
	    }
	    err = streamrecorder_set_video_encoder(hmstreamrecorder->recorder, STREAMRECORDER_VIDEO_CODEC_MPEG4);
	    if (err < 0) {
			LOGE("bInit fail. (%x)", err);
			goto ERROR;
	    }
	    err = streamrecorder_set_video_resolution(hmstreamrecorder->recorder, 1280, 720);
	    if (err < 0) {
			LOGE("dInit fail. (%x)", err);
			goto ERROR;
	    }
	    err = streamrecorder_set_video_framerate(hmstreamrecorder->recorder, 30);
	    if (err < 0) {
			LOGE("dInit fail. (%x)", err);
			goto ERROR;
	    }
	    err = streamrecorder_set_filename(hmstreamrecorder->recorder, TARGET_FILENAME_VIDEO);
	    if (err < 0) {
			LOGE("dInit fail. (%x)", err);
			goto ERROR;
		}
	}

	streamrecorder_set_recording_status_cb(hmstreamrecorder->recorder, _recording_status_cb, NULL);
	streamrecorder_set_recording_limit_reached_cb(hmstreamrecorder->recorder, _recording_limit_reached_cb, &ischeck);

	LOGD("Init DONE.");

	return TRUE;

ERROR:
	LOGE("init failed.");
	return FALSE;
}

static gboolean init_handle()
{
	hmstreamrecorder->mode = 0;  /* image(capture)/video(recording) mode */
	hmstreamrecorder->menu_state = MENU_STATE_MAIN;
	hmstreamrecorder->isMute = FALSE;
	hmstreamrecorder->elapsed_time = 0;
	hmstreamrecorder->fps = SRC_VIDEO_FRAME_RATE_15; /*SRC_VIDEO_FRAME_RATE_30;*/

	return TRUE;
}
/**
* This function is to change streamrecorder mode.
*
* @param    type    [in]    image(capture)/video(recording) mode
*
* @return   This function returns TRUE/FALSE
* @remark
* @see      other functions
*/
static gboolean mode_change()
{
	int err = STREAMRECORDER_ERROR_NONE;
	int state = STREAMRECORDER_STATE_NONE;
	char media_type = '\0';
	bool check = FALSE;

	err = streamrecorder_get_state(hmstreamrecorder->recorder, (streamrecorder_state_e *)&state);
	if (state != STREAMRECORDER_STATE_NONE) {
	    if ((state == STREAMRECORDER_STATE_RECORDING) || (state == STREAMRECORDER_STATE_PAUSED)) {
		    LOGD("streamrecorder_cancel");
		    err = streamrecorder_cancel(hmstreamrecorder->recorder);

		    if (err < 0) {
			    LOGE("exit streamrecorder_cancel  = %x", err);
			    return FALSE;
		    }
	    }

		err = streamrecorder_get_state(hmstreamrecorder->recorder, (streamrecorder_state_e *)&state);
		if (state == STREAMRECORDER_STATE_PREPARED) {
		    LOGD("streamrecorder_destroy");
		    streamrecorder_unprepare(hmstreamrecorder->recorder);
		}

		err = streamrecorder_get_state(hmstreamrecorder->recorder, (streamrecorder_state_e *)&state);
		if (state == STREAMRECORDER_STATE_CREATED) {
		    LOGD("streamrecorder_destroy");
		    streamrecorder_destroy(hmstreamrecorder->recorder);
		}
	}

	init_handle();
	streamrecorder_create(&hmstreamrecorder->recorder);
	g_get_current_time(&previous);
	g_timer_reset(timer);
	while (!check) {
		g_print("\n\t=======================================\n");
		g_print("\t   RECORDER_TESTSUIT\n");
		g_print("\t=======================================\n");
		g_print("\t   '1' i420 sw buffer - Video\n");
		g_print("\t   '2' nv12 sw buffer - Video\n");
		g_print("\t   'q' Exit\n");
		g_print("\t=======================================\n");

		g_print("\t  Enter the media type:\n\t");

		err = scanf("%c", &media_type);
		if (err == EOF) {
			g_print("\t!!!read input error!!!\n");
			continue;
		}

		switch (media_type) {
		case '1':
			hmstreamrecorder->mode = MODE_LIVE_BUFFER_I420_VIDEO;
			streamrecorder_enable_source_buffer(hmstreamrecorder->recorder, STREAMRECORDER_SOURCE_VIDEO);
			check = TRUE;
			break;
		case '2':
			hmstreamrecorder->mode = MODE_LIVE_BUFFER_NV12_VIDEO_SW;
			streamrecorder_enable_source_buffer(hmstreamrecorder->recorder, STREAMRECORDER_SOURCE_VIDEO);
			check = TRUE;
			break;
		case 'q':
			g_print("\t Quit Streamrecorder Testsuite!!\n");
			hmstreamrecorder->mode = -1;
			if (g_main_loop_is_running(g_loop))
			    g_main_loop_quit(g_loop);

			return FALSE;
		default:
			g_print("\t Invalid media type(%d)\n", media_type);
			continue;
		}
	}

	g_timer_reset(timer);

	if (!init(hmstreamrecorder->mode)) {
		g_print("hmstreamrecorder->mode = %d\n", hmstreamrecorder->mode);
		LOGE("testsuite init() failed.");
		return -1;
	}

	g_timer_reset(timer);

	err = streamrecorder_prepare(hmstreamrecorder->recorder);
	LOGD("streamrecorder_start()  : %12.6lfs", g_timer_elapsed(timer, NULL));

	if (err != 0) {
		LOGE("streamrecorder_prepare  = %x", err);
		return -1;
	}

	g_get_current_time(&current);
	timersub(&current, &previous, &result);
	LOGD("Streamrecorder Starting Time  : %ld.%lds", result.tv_sec, result.tv_usec);

	return TRUE;
}


/**
* This function is the example main function for recorder API.
*
* @param
*
* @return   This function returns 0.
* @remark
* @see      other functions
*/
int main(int argc, char **argv)
{
	int bret;

	timer = g_timer_new();

	gst_init(&argc, &argv);

	LOGD("gst_init() : %12.6lfs", g_timer_elapsed(timer, NULL));

	hmstreamrecorder = (mstreamrecorder_handle_t *) g_malloc0(sizeof(mstreamrecorder_handle_t));

	recorder_state = STREAMRECORDER_STATE_NONE;

	g_timer_reset(timer);

	bret = mode_change();
	if (!bret)
		return bret;

	print_menu();

	g_loop = g_main_loop_new(NULL, FALSE);

	stdin_channel = g_io_channel_unix_new(fileno(stdin));/* read from stdin */
	g_io_add_watch(stdin_channel, G_IO_IN, (GIOFunc)cmd_input, NULL);

	LOGD("RUN main loop");

	g_main_loop_run(g_loop);

	LOGD("STOP main loop");

	if (timer) {
		g_timer_stop(timer);
		g_timer_destroy(timer);
		timer = NULL;
	}

	g_free(hmstreamrecorder);
	g_main_loop_unref(g_loop);
	g_io_channel_unref(stdin_channel);

	return bret;
}

/*EOF*/
