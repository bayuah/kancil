/* 
 * `lingkungan.h`
 * Menentukan lingkungan kompiler.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */
#ifndef _KANCIL_ARCH_
#define _KANCIL_ARCH_

// Arsitektur.
#if defined (__x86_64__) || defined (_____LP64_____) || defined (_LP64)
	# define __x86_64___
#elif defined (__ILP32__) || defined (_ILP32)
	# define __x86_32___
#else
	# define __x86_32___
#endif

// Sistem Operasi.
#if defined(WIN32) \
	|| defined(_WIN32) \
	|| defined(__WIN32) \
	|| defined(__CYGWIN__) \
	|| defined (__CYGWIN32__) \
	|| defined (_MSC_VER)
		#define __WIN32__
		#define DIR_SEPARATOR '\\'
	#ifdef _WIN64
		#define __WIN64__
	#endif
#elif __APPLE__
	#define __IOS__
	#define DIR_SEPARATOR '/'
	#include "TargetConditionals.h"
	#if TARGET_IPHONE_SIMULATOR
		# define __IOS_SIM__
	#elif TARGET_OS_IPHONE
		# define __IOS_DEVC__
	#elif TARGET_OS_MAC
		# define __IOS_OTR__
	#else
		# define __IOS_UNS__
	#endif
#elif __linux
	#define __LINUX__
	#define DIR_SEPARATOR '/'
#elif __unix
	# define __UNIX__
	#define DIR_SEPARATOR '/'
#elif __posix
	#define __POSIX__
	#define DIR_SEPARATOR '/'
#endif

#endif /*_KANCIL_ARCH_*/
