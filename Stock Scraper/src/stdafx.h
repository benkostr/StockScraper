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
#ifdef _M_X64
    #ifdef _DEBUG
    #pragma comment(lib, "Win64/VC12/DLL Debug/libcurld.lib")
    #else
    #pragma comment(lib, "Win64/VC12/DLL Release/libcurl.lib")
    #endif
#else
    #ifdef _DEBUG
    #pragma comment(lib, "Win32/VC12/DLL Debug/libcurld.lib")
    #else
    #pragma comment(lib, "Win32/VC12/DLL Release/libcurl.lib")
    #endif
#endif
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
