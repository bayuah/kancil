/* 
 * `faedah.h`
 * Alat bantu dalam program kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014 - 2015
 * Lisensi: lihat LICENCE.txt
 */

#ifndef _KANCIL_FAEDAH_
#define _KANCIL_FAEDAH_

#include "lingkungan.h"

// Memastikan dapat membaca
// berkas ukuran besar.
#ifndef __USE_LARGEFILE64
	#define __USE_LARGEFILE64 1
#endif
#define _FILE_OFFSET_BITS 64
#include <sys/types.h>

// Bila tak ada.
#ifdef __CYGWIN__
	#ifndef _off64_t_defined
		typedef _off64_t off64_t;
		# define _off64_t_defined
	#endif
#else
	#ifndef __off64_t_defined
		typedef __off64_t off64_t;
		# define __off64_t_defined
	#endif
#endif

// Tingkat kekutu.
#define MINI_DEBUG 1
#define MEDIUM_DEBUG 2
#define FULL_DEBUG 3

// Ubtuk waktu.
#define CURRENTTIME_SECONDS 0
#define CURRENTTIME_MICROSECONDS 1

void dec2bin(int num, char *str);
double current_time(int select);
int mod_int (int a, int b);
char *b2s(bool s);
bool s2b(char *s);
bool is_nonascii(char *s);
int int_tak_min(int nilai, int pengganti);
char *remove_ext (char* str, char dot, char sep);
void set_null(char **str, size_t length);
void print_array(char *array, int length);
void print_unsigned_array(unsigned char *array, size_t length);
void print_unsigned_array_nonsymbol(unsigned char *array, size_t length);
void print_char(char *array, size_t length);
void print_char_csv(char *array, size_t start, size_t maximal);
void print_unsigned_char_csv(unsigned char *array, size_t start, size_t maximal);
int check_parity(char *str, size_t start, size_t length);
void insert_string(char **target, size_t target_length,
	char *str, size_t start, size_t str_length);
char *basename (const char *filename);
off64_t fsize(const char *filename);
bool file_exist (char *filename);
char *readable_fs(double size, char *penyanga);
int killpid(pid_t pid, int sig);
char *unix_signal_code(int signal);
double kecepatan_rerata(
	double kecepatan,
	double kecepatan_sebelumnya,
	double penghalus
);

void throw_error(int type, const char * file, const int line,
	const char *msg, ...);
void _test(const char * file, const int line, const char *msg);

// Makro.
#define TEST(msg, ...) \
	throw_error(0, __FILE__, __LINE__, msg, __VA_ARGS__);
#define FAIL(msg, ...) \
	throw_error(1, __FILE__, __LINE__, msg, __VA_ARGS__);
#define WARN(msg, ...) \
	throw_error(2, __FILE__, __LINE__, msg, __VA_ARGS__);
#define NOTICE(msg, ...) \
	throw_error(3, __FILE__, __LINE__, msg, __VA_ARGS__);
#define INFO(msg, ...) \
	throw_error(4, __FILE__, __LINE__, msg, __VA_ARGS__);
#define DEBUG(msg, ...) \
	throw_error(5, __FILE__, __LINE__, msg, __VA_ARGS__);
#define DEBUG1(msg, ...) \
	throw_error(5, __FILE__, __LINE__, msg, __VA_ARGS__);
#define DEBUG2(msg, ...) \
	throw_error(6, __FILE__, __LINE__, msg, __VA_ARGS__);
#define DEBUG3(msg, ...) \
	throw_error(7, __FILE__, __LINE__, msg, __VA_ARGS__);
#define DEBUG4(msg, ...) \
	throw_error(8, __FILE__, __LINE__, msg, __VA_ARGS__);
#define DEBUG5(msg, ...) \
	throw_error(9, __FILE__, __LINE__, msg, __VA_ARGS__);

#endif /* _KANCIL_FAEDAH_ */
