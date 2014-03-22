#ifndef __gtgl_windef_h_
#define __gtgl_windef_h_

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && defined(_M_IX86)
#define _X86_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && defined(_M_AMD64)
#define _AMD64_
#endif

#include <windef.h>

#endif

#endif