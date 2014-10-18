#ifndef LOGGER_H
#define LOGGER_H

#include "../Utilities/String.h"

// Call the BE_ERROR() macro to send out an error message
//		Example -> BE_ERROR("Something Blew Up!");
#define BE_ERROR(str) \
	std::wstring s(s2ws(str)); \
	if(g_pApp->GetHwnd() != NULL) { \
		MessageBox(g_pApp->GetHwnd(), s.c_str(), L"Error", MB_OK); \
	} else { \
		OutputDebugString(s.c_str()); \
	} \

#define BE_ASSERT(expr) \
do \
{ \
	if (!(expr)) \
	{ \
		assert(expr); \
	} \
} \
while (0) \

#define BE_ASSERT_MESSAGE(expr, message) \
do \
{ \
	if (!(expr)) \
		{ \
		assert(expr && message); \
		} \
} \
while (0) \

#endif