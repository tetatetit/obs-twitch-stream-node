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
#include <string>
#include <napi.h>
#include <obs.h>

int main(int, char ** , char **){}

Napi::Object main_node(Napi::Env env, Napi::Object exports) {
	obs_reset_audio(NULL);	
	return exports;
};

NODE_API_MODULE(obs_twitch_stream_node, main_node);
