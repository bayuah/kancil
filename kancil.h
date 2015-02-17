/* 
 * `kancil.h`
 * Pengepala standar untuk kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */
 
#include "lingkungan.h"

#ifndef _KANCIL_H_
#define _KANCIL_H_

#ifndef __USE_LARGEFILE64
	#define __USE_LARGEFILE64
#endif
#ifndef _BSD_SOURCE
	#define _BSD_SOURCE
#endif
#ifndef _LARGEFILE_SOURCE
	#define _LARGEFILE_SOURCE
#endif
#ifndef _LARGEFILE64_SOURCE
	#define _LARGEFILE64_SOURCE
#endif
#ifndef _POSIX_SOURCE
	#define _POSIX_SOURCE
#endif

// Standar.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
// Untuk isalpha()
// dan isdigit().
#include <ctype.h>  
#include <time.h>

// Kekutu.
// Ada masalah
// di Ms. Windows. 
#ifndef __WIN32__
	#define EXECINFO_COMPATIBLE
	#include <execinfo.h>
#endif

// Posix
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

// #include <netdb.h>

// Posix komunikasi antar cabang.
#include <sys/mman.h>
#include <sys/stat.h> /* Untuk konstan mode */
#include <fcntl.h>    /* Untuk konstan O_* */

// Variabel memori bersama
#ifdef _SYS_MMAN_H
	#define SHM
#endif
#if defined(PROGCODE) && defined (SHM)
	#define SHM_FILE "/SHM_" PROGCODE ".memory"
#endif

// Soket.
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// Lokalisasi
#include <libintl.h>
#include <locale.h>
#define _(String) gettext (String) // Makro.

// Ekstra.
#include <unistd.h>
#include <errno.h>

// Nilai keluaran.
#include "return.h"

// Kompabilitas.
#define h_addr h_addr_list[0]

// Pengaturan kompilasi.
#pragma GCC diagnostic ignored "-Wformat"

// Ubah makro jadi karakter.
#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

// Mendapatkan versi kompilasi.
#ifdef __COMPILER_FLAGS
	#define COMPILER_FLAGS __COMPILER_FLAGS
#endif
#ifdef __COMPILER_MACHINE
	#define COMPILER_MACHINE __COMPILER_MACHINE
#endif
#ifdef __COMPILER_VERSION
	#define COMPILER_VERSION __COMPILER_VERSION
#endif
#ifdef COMPILE_MODE_STABLE
	#define COMPILE_MODE "stable"
#elif COMPILE_MODE_PREVIEW
	#define COMPILE_MODE "preview"
#elif COMPILE_MODE_DEVEL
	#define COMPILE_MODE "devel"
#else
	#define COMPILE_MODE_DEVEL
	#define COMPILE_MODE "devel"
#endif

// Pengaturan program.
#ifdef NOFORK
	#define KANCIL_NOFORK
#endif

// Versi.
#ifndef __VERSION_MAJOR
	#define __VERSION_MAJOR 0
#endif
#ifndef __VERSION_MINOR
	#define __VERSION_MINOR 0
#endif
#ifndef __VERSION_PATCH
	#define __VERSION_PATCH 0
#endif
#ifndef __BUILT_NUMBER
	#define __BUILT_NUMBER 0
#endif
#ifndef __BUILT_TIME
	#define __BUILT_TIME 1416920916
#endif

// Pengaturan umum program.
#ifndef PROTOCOL_VERSION
	#define PROTOCOL_VERSION 1
#endif
#ifndef MAX_CHUNK_SIZE
	#define MAX_CHUNK_SIZE 1024
#endif
#ifndef ENCRYPTED_CONTAINER_SIZE
	#define ENCRYPTED_CONTAINER_SIZE 1500 // Sebenarnya adalah 1280-bita.
#endif
#ifndef CHUNK_HEADER_SIZE
	#define CHUNK_HEADER_SIZE 4
#endif
#ifndef CHUNK_MESSAGE_SIZE
	#define CHUNK_MESSAGE_SIZE 1020
#endif
#ifndef MAX_CHUNK_ID
	#define MAX_CHUNK_ID 65025 // 16-bit.
	// #define MAX_CHUNK_ID 10 // 16-bit.
#endif

// Nilai panji.
#ifndef STOP_FLAG
	#define STOP_FLAG 1
#endif
#ifndef START_FLAG
	#define START_FLAG 2
#endif
#ifndef INTRANSFER_FLAG
	#define INTRANSFER_FLAG 3
#endif
#ifndef INVALID_FLAG
	#define INVALID_FLAG 0
#endif
#ifndef UNDEFINED_FLAG
	#define UNDEFINED_FLAG -1
#endif

// Ambil struktur.
#include "struktur.h"

#endif /* _KANCIL_H_ */
