/******************************************************************************
    Copyright (C) 2016-2019 by Streamlabs (General Workings Inc)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/

#if defined(_WIN32)
#include "Shlobj.h"
#endif

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <napi.h>
#include <obs.h>

using namespace std;

class StreamToTwitch
{
    obs_output_t* output = nullptr;

public:
    StreamToTwitch(const string& key) {
        bool rv = obs_startup("en-US", nullptr, nullptr);
        assert(rv);

        obs_load_all_modules();

        // Just for information about all entity types available 
        for (int n = 0;; ++n) {
            const char* inp_type;
            if (!obs_enum_input_types(n, &inp_type))
                break;
            cout << "Input type: " << inp_type << endl;
        }

        for (int n = 0;; ++n) {
            const char* out_type;
            if (!obs_enum_output_types(n, &out_type))
                break;
            cout << "Output type: " << out_type << endl;
        }

        for (int n = 0;; ++n) {
            const char* src_type;
            if (!obs_enum_source_types(n, &src_type))
                break;
            cout << "Source type: " << src_type << endl;
        }

        for (int n = 0;; ++n) {
            const char* enc_type;
            if (!obs_enum_encoder_types(n, &enc_type))
                break;
            cout << "Encoder type: " << enc_type << endl;
        }

        for (int n = 0;; ++n) {
            const char* svc_type;
            if (!obs_enum_service_types(n, &svc_type))
                break;
            cout << "Service type: " << svc_type << endl;
        }

        // Video
        obs_video_info ovi = {
            .graphics_module = "libobs-opengl",
            .fps_num = 30,
            .fps_den = 1,
            .base_width = 1920,
            .base_height = 1080,
            .output_width = 1920,
            .output_height = 1080,
            .output_format = VIDEO_FORMAT_I420,
            .gpu_conversion = true,
            .colorspace = VIDEO_CS_DEFAULT,
            .range = VIDEO_RANGE_DEFAULT,
            .scale_type = OBS_SCALE_BILINEAR
        };

        int err = obs_reset_video(&ovi);
        assert(err == OBS_VIDEO_SUCCESS);
        auto vSrc = obs_source_create("monitor_capture", "", nullptr, nullptr);
        assert(vSrc);
        obs_set_output_source(0, vSrc);
        auto vEncoder = obs_video_encoder_create("obs_x264", "", nullptr, nullptr);
        assert(vEncoder);
        obs_encoder_set_video(vEncoder, obs_get_video());


        // Audio
        obs_audio_info oai = {
            .samples_per_sec = 44100,
            .speakers = SPEAKERS_STEREO
        };

        rv = obs_reset_audio(&oai);
        assert(rv);
        auto aSrc = obs_source_create("wasapi_input_capture", "", nullptr, nullptr);
        assert(aSrc);
        obs_set_output_source(1, aSrc);
        auto aEncoder = obs_audio_encoder_create("ffmpeg_aac", "", nullptr, 0, nullptr);
        obs_encoder_set_audio(aEncoder, obs_get_audio());

        // Service
        auto service_data = obs_data_create();
        obs_data_set_string(service_data, "service", "Twitch");
        obs_data_set_string(service_data, "key", key.c_str());
        obs_data_set_string(service_data, "server", "auto");
        auto service = obs_service_create("rtmp_common", "Twitch", service_data, nullptr);
        assert(service);
        obs_data_release(service_data);

        // Output
        output = obs_output_create("rtmp_output", "", nullptr, nullptr);
        assert(output);
        obs_output_set_video_encoder(output, vEncoder);
        obs_output_set_audio_encoder(output, aEncoder, 0);
        obs_output_set_service(output, service);

        rv = obs_output_start(output);
        assert(rv);
    }

    ~StreamToTwitch() {
        if (output) {
            obs_output_stop(output);
            obs_output_release(output);
        }
        obs_shutdown();
    }
};

unique_ptr<StreamToTwitch> g_stream;


void obsStartStreamToTwitch(const Napi::CallbackInfo& info) {
    cout << "obs_twitch_stream_node: obsStartStreamToTwitch called" << endl;
    auto env = info.Env();
    if (info.Length() != 1) {
        Napi::TypeError::New(env, "obs_twitch_stream_node: Only 1 argument (stream key as string) should be specified")
            .ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "obs_twitch_stream_node: argument (stream key) is not string")
            .ThrowAsJavaScriptException();
        return;
    }

    if (info[0].ToString().IsEmpty()) {
        Napi::TypeError::New(env, "obs_twitch_stream_node: argument (stream key) is empty")
            .ThrowAsJavaScriptException();
        return;
    }

    if (g_stream) {
        cout << "obs_twitch_stream_node: streming already started" << endl;
        return;
    }

    g_stream = make_unique<StreamToTwitch>(info[0].ToString());

}

void obsStopStreamToTwitch(const Napi::CallbackInfo& info) {
    cout << "obs_twitch_stream_node: obsStopStreamToTwitch called" << endl;
    if (g_stream) {
        g_stream.reset();
    }
    else
        cout << "obs_twitch_stream_node: streaming not started" << endl;
}

Napi::Object main_node(Napi::Env env, Napi::Object exports) {
    cout << "obs_twitch_stream_node: Module init called" << endl;
    exports.Set(
        Napi::String::New(env, "obsStartStreamToTwitch"),
        Napi::Function::New(env, obsStartStreamToTwitch)
    );
    exports.Set(
        Napi::String::New(env, "obsStopStreamToTwitch"),
        Napi::Function::New(env, obsStopStreamToTwitch)
    );
    return exports;
};

NODE_API_MODULE(obs_twitch_stream_node, main_node);
