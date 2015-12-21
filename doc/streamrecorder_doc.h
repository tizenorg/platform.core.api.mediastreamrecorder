/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. 
 */


#ifndef __TIZEN_MEDIA_STREAMRECORDER_DOC_H__
#define __TIZEN_MEDIA_STREAMRECORDER_DOC_H__


/**
 * @file streamrecorder_doc.h
 * @brief The file contains the high level documentation for the StreamRecorder API.
 */

/**
 * @ingroup CAPI_MEDIA_FRAMEWORK
 * @defgroup CAPI_MEDIA_STREAMRECORDER_MODULE StreamRecorder
 * @brief The @ref CAPI_MEDIA_STREAMRECORDER_MODULE API provides functions for StreamRecorder with buffer including media packet.
 *
 * @section CAPI_MEDIA_STREAMRECORDER_MODULE_HEADER Required Header
 *   \#include <streamrecorder.h>
 *
 * @section CAPI_MEDIA_STREAMRECORDER_MODULE_OVERVIEW Overview
 * The StreamRecorder API allows application developers to support using the video or audio recorder. It includes functions that record video or audio and supports to set up
 * notifications for state changes of creation, ready, record, pause, information about resolution and binary data format, and functions for artistic.
 *
 * The StreamRecorder API allows creation of components required in recording video or audio including:
 * - selecting a proper output format
 * - controlling the StreamRecorder state
 * - getting supported formats and video resolutions
 *
 * The StreamRecorder API also notifies you (by callback mechanism) when a significant parameter changes.
 *
 * @subsection CAPI_MEDIA_STREAMRECORDER_LIFE_CYCLE_CALLBACK_OPERATIONS Callback(Event) Operations
 * The callback mechanism is used to notify the application about significant StreamRecorder events.
 * <div><table class="doxtable" >
 *     <tr>
 *        <th><b>REGISTER</b></th>
 *        <th><b>UNREGISTER</b></th>
 *        <th><b>CALLBACK</b></th>
 *        <th><b>DESCRIPTION</b></th>
 *     </tr>
 *     <tr>
 *        <td>streamrecorder_set_error_cb()</td>
 *        <td>streamrecorder_unset_error_cb()</td>
 *        <td>streamrecorder_error_cb()</td>
 *        <td>This callback is used to notify error has occurred</td>
 *     </tr>
 *</table></div>
 *
 * @subsection CAPI_MEDIA_STREAMRECORDER_FOREACH_OPERATIONS Foreach Operations
 * <div><table class="doxtable" >
 *     <tr>
 *        <th><b>FOREACH</b></th>
 *        <th><b>CALLBACK</b></th>
 *        <th><b>DESCRIPTION</b></th>
 *     </tr>
 *     <tr>
 *        <td>streamrecorder_foreach_supported_audio_encoder()</td>
 *        <td>streamrecorder_supported_audio_encoder_cb()</td>
 *        <td>Supported audio encoders</td>
 *     </tr>
 *     <tr>
 *        <td>streamrecorder_foreach_supported_video_encoder()</td>
 *        <td>streamrecorder_supported_video_encoder_cb()</td>
 *        <td>Supported video encoders</td>
 *     </tr>
 *     <tr>
 *        <td>streamrecorder_foreach_supported_file_format()</td>
 *        <td>streamrecorder_supported_file_format_cb()</td>
 *        <td>Supported file formats</td>
 *     </tr>
 *     <tr>
 *        <td>streamrecorder_foreach_supported_video_resolution()</td>
 *        <td>streamrecorder_supported_video_resolution_cb()</td>
 *        <td>Supported video resolutions</td>
 *     </tr>
 *</table></div>
 *
 */


/**
 * @defgroup CAPI_MEDIA_STREAMRECORDER_ATTRIBUTES_MODULE Attributes
 * @brief The @ref CAPI_MEDIA_STREAMRECORDER_ATTRIBUTES_MODULE API provides functions to fetch StreamRecorder attributes.
 * @ingroup CAPI_MEDIA_STREAMRECORDER_MODULE
 *
 * @section CAPI_MEDIA_STREAMRECORDER_ATTRIBUTES_MODULE_HEADER Required Header
 *   \#include <streamrecorder.h>
 *
 * @section CAPI_MEDIA_STREAMRECORDER_ATTRIBUTES_MODULE_OVERVIEW Overview
 * The StreamRecorder Attributes API provides functions to get/set basic StreamRecorder attributes.
 *
 * With the StreamRecorder Attributes API you are able to set and get StreamRecorder attributes like:
 * <ul>
 *  <li>File name</li>
 *  <li>File format</li>
 *  <li>Audio encoder</li>
 *  <li>Video encoder</li>
 *  <li>Recording size limit</li>
 *  <li>Recording time limit</li>
 *  <li>Audio samplerate</li>
 *  <li>Audio encoder bitrate</li>
 *  <li>Video encoder bitrate</li>
 *  <li>Audio channel</li>
 * </ul>
 *
 */

/**
 * @defgroup CAPI_MEDIA_STREAMRECORDER_CALLBACK_MODULE Callback
 * @brief The @ref CAPI_MEDIA_STREAMRECORDER_CALLBACK_MODULE API Callbacks from the StreamRecorder frameworks.
 * @ingroup CAPI_MEDIA_STREAMRECORDER_MODULE
 *
 * @section CAPI_MEDIA_STREAMRECORDER_CALLBACK_MODULE_HEADER Required Header
 *   \#include <streamrecorder.h>
 *
 * @section CAPI_MEDIA_STREAMRECORDER_CALLBACK_MODULE_OVERVIEW Overview
 * The StreamRecorder Callback API provides functions status.
 *
 */

/**
 * @ingroup CAPI_MEDIA_STREAMRECORDER_MODULE
 * @defgroup CAPI_MEDIA_STREAMRECORDER_CAPABILITY_MODULE Capability
 * @brief The @ref CAPI_MEDIA_STREAMRECORDER_CAPABILITY_MODULE API provides capability information of the StreamRecorder.
 * @section CAPI_MEDIA_STREAMRECORDER_CAPABILITY_MODULE_HEADER Required Header
 *   \#include <streamrecorder.h>
 *
 * @section CAPI_MEDIA_CAMERA_CAPABILITY_MODULE_OVERVIEW Overview
 * The Camera Capability API provides functions to obtain capability information of the camera device.
 *
 */

#endif /* __TIZEN_MEDIA_STREAMRECORDER_DOC_H__ */


