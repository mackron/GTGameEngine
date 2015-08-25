// An improved windows.h header that #undef's those annoying #defines of common function names.

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)

#if !defined(WINVER)
#define WINVER 0x0501
#elif WINVER < 0x0501
#undef  WINVER
#define WINVER 0x0501
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef CreateWindow
#undef DeleteFile
#undef CopyFile
#undef SetCurrentDirectory
#undef GetCurrentDirectory
#undef CreateDirectory
#undef CreateFont
#undef LoadImage
#undef GetClassName
#undef GetCommandLine
#undef PostMessage
#undef DrawText
#undef min
#undef max
#undef MIN
#undef MAX

#endif
