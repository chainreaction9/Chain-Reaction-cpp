#pragma once
/* This file serves as a precompiled header for all the cpp files. It must be included in every cpp source file. */
/* If a '.cpp' source file contains classes and other object definitions declared inside a header file, this file must be included 
/* before the header file.*/

#ifndef _PRECOMPILED_H

/*Macro definition to detect memory leaks*/
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS //To make wxWidget work in debug-x64 configuration.
#endif //!_CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <crtdbg.h>
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#endif

#ifndef STRING_STREAM_H
#include <sstream>
#define STRING_STREAM_H
#endif //!STRING_STREAM_H

#ifndef STRING_H
#define STRING_H
#include <string>
#endif //!STRING_H

#ifndef REGEX_H
#include <regex>
#define REGEX_H
#endif //!REGEX_H

#ifndef FSTREAM_H
#define FSTREAM_H
#include <fstream>
#endif //!FSTREAM_H

#ifndef _VECTOR_
#include <vector>
#endif //!_VECTOR_

#ifndef _UNORDERED_MAP_
#include <unordered_map>
#endif //!_UNORDERED_MAP_

#ifndef _SET_
#include <set>
#define _SET_
#endif //!_SET_

#ifndef _ALGORITHM_
#include <algorithm>
#define _ALGORITHM_
#endif //!_ALGORITHM_

#ifndef _CTIME_
#include <ctime>
#define _CTIME_
#endif //!_CTIME_

#ifndef _INC_MATH
#define _INC_MATH
#define _USE_MATH_DEFINES
#include <math.h>
#endif //!_INC_MATH

#ifndef __GLEW_H__
#include <GL/glew.h>
#endif //!__GLEW_H__

#ifndef AL_H
#define AL_H
#include <AL/alc.h>
#include <AL/al.h>
#endif //!AL_H

#ifndef GLM_H
//minimal inclusion
#include <ext/vector_float1.hpp>
#include <ext/vector_float2.hpp>
#include <ext/vector_float3.hpp>
#include <ext/vector_float4.hpp>

#include <trigonometric.hpp>
#include <ext/matrix_float4x4.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>  //perspective, translate, rotate
#define GLM_H
#endif //!GLM_H

#ifndef MEMORY_H
#define MEMORY_H
#include <memory.h>
#endif //!MEMORY_H

#ifndef WX_PREC_H
#include <wx/wxprec.h>
#define WX_PREC_H

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //!WX_PRECOMP

#ifndef WX_LAYWIN_H
#include <wx/laywin.h>
#define WX_LAYWIN_H
#endif //WX_LAYWIN_H

#ifndef WX_GLCANVAS_H
#include <wx/glcanvas.h>
#define _WX_GLCANVAS_H
#endif //!WX_GLCANVAS_H

#ifndef WX_MEMORY_STREAM_H
#include <wx/mstream.h>
#define WX_MEMORY_STREAM_H
#endif //!WX_MEMORY_STREAM_H

#ifndef WX_ZIP_STREAM_H
#include <wx/zipstrm.h>
#define WX_ZIP_STREAM_H
#endif //!WX_ZIP_STREAM_H

#ifndef WX_FILE_H
#include <wx/file.h>
#include <wx/wfstream.h>
#endif //!WX_FILE_H

#ifndef WX_SOCKET_H
#include <wx/socket.h>
#define WX_SOCKET_H
#endif //!WX_SOCKET_H

#ifndef WX_SOUND_H
#define SOUND_H
#include <wx/sound.h>
#endif //!WX_SOUND_H

#ifndef WX_PROGDLG_H
#include <wx/progdlg.h>
#define WX_PROGDLG_H
#endif //!WX_PROGDLG_H

#endif //!WX_PREC_H
#if defined (_WIN32) //include windows header only in windows environment.
#ifndef WINDOWS_H
#include <Windows.h>
#define WINDOWS_H
#endif //!WINDOWS_H
#endif //!_WIN32
#define _PRECOMPILED_H
#endif //!_PRECOMPILED_H