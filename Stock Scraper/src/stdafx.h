// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
// I've made this a forced included file
// (under project properties/Advanced in visual studio).
// This means that the compiler will automatically include this file
// at the beginning of every other file in the project.
//

#pragma once

#define _BIND_TO_CURRENT_VCLIBS_VERSION 1
//Curl:
//#define CURL_STATICLIB //Needed to link static library
#ifdef _WIN64
#define ARCHITECTURE "Win64"
#elif defined _WIN32
#define ARCHITECTURE "Win32"
#endif
#ifdef _DEBUG
#define  CONFIGURATION "DLL" "Debug/libcurld.lib"
#else
#define CONFIGURATION "DLL" "Release/libcurl.lib"
#endif
#ifdef _MSC_VER
    #if _MSC_VER == 1900
    #define VERSION "14"
    #elif _MSC_VER == 1800
    #define VERSION "12"
    #endif
#endif
#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)
#define PASTE2(x,y) x##y
#define PASTE(x,y) PASTE2(x,y)
//Concatenates the values defined above into a single path to the .lib file
#define CURLPATH STRINGIFY(PASTE(PASTE(ARCHITECTURE"/VC", VERSION), "/"CONFIGURATION))
//Adds the .lib file as a library for the compiler
#pragma comment(lib, CURLPATH)
#undef ARCHITECTURE
#undef VERSION
#undef CONFIGURATION
#undef STRINGIFY2
#undef STRINGIFY
#undef PASTE2
#undef PASTE
#undef CURLPATH
//#ifdef _WIN64
//    #ifdef _DEBUG
//    #pragma comment(lib, "Win64/VC14/DLL Debug/libcurld.lib")
//    #else
//    #pragma comment(lib, "Win64/VC14/DLL Release/libcurl.lib")
//    #endif
//#elif defined _WIN32
//    #ifdef _DEBUG
//    #pragma comment(lib, "Win32/VC14/DLL Debug/libcurld.lib")
//    #else
//    #pragma comment(lib, "Win32/VC14/DLL Release/libcurl.lib")
//    #endif
//#endif
//Libraries that curl uses from Windows:
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

#include "targetver.h"

#include <direct.h> //_mkdir
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm> //remove
#include <thread> //multi-threading
#include <mutex> //locking for exclusive access from thread

#include <curl/curl.h> //Contains curl functions

using namespace std;


// TODO: reference additional headers your program requires here
