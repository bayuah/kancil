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

#define __USE_LARGEFILE64
#define _BSD_SOURCE
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE

// Standar.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
// Untuk isalpha()
// dan isdigit().
#include <ctype.h>  

// Kekutu.
// Ada masalah
// di Ms. Windows. 
#ifndef __WIN32__
	#define EXECINFO_COMPATIBLE
	#include <execinfo.h>
#endif

// Posix
#define _POSIX_SOURCE
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

// Pemgturan kompilasi.
#pragma GCC diagnostic ignored "-Wformat"

// Mendapatkan versi kompilasi.
#ifdef COMPILE_MODE_BUILD
	#define COMPILE_MODE "built"
#elif COMPILE_MODE_BUILD_INTERNAL
	#define COMPILE_MODE "built-internal"
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
#ifndef CHUNK_HEADER_SIZE
	#define CHUNK_HEADER_SIZE 4
#endif
#ifndef CHUNK_MESSAGE_SIZE
	#define CHUNK_MESSAGE_SIZE 1020
#endif
#ifndef MAX_CHUNK_ID
	#define MAX_CHUNK_ID 65025 // 16-bit.
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

// Variabel global.
struct GLOBAL_KANCIL {
	bool show_error;
	bool show_warning;
	bool show_notice;
	bool show_info;
	bool show_debug1;
	bool show_debug2;
	bool show_debug3;
	bool show_debug4;
	bool show_debug5;
	int tries;
	int waitretry;
	int maxqueue;
	int waitqueue;
	bool nowaitqueue;
	char* tempdir;
} GLOBAL_KANCIL;
#define kancil GLOBAL_KANCIL
#define aturan GLOBAL_KANCIL

struct DAFTAR_ALAMAT{
	int identifikasi;
	char nama_inang[INET6_ADDRSTRLEN+1];
	struct addrinfo *info;
	struct DAFTAR_ALAMAT *awal;
	struct DAFTAR_ALAMAT *lanjut;
} DAFTAR_ALAMAT;

struct KIRIMBERKAS{
	unsigned int identifikasi;
	unsigned int identifikasi_sebelumnya;
	unsigned int kelompok_kirim;
	int urut_kali;
	double ukuran_berkas;
	double ukuran_kirim;
	bool do_kirim;
	char *hostname;
	char *portno;
	char *berkas;
	int coba;
	struct DAFTAR_ALAMAT *alamat;
} KIRIMBERKAS;

// Global.
struct KIRIMBERKAS *GLOBAL_kirim;

#endif /* _KANCIL_H_ */
