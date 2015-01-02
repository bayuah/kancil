/* 
 * `faedah.h`
 * Alat bantu dalam program kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */

#ifndef _KANCIL_FAEDAH_
#define _KANCIL_FAEDAH_

#include "lingkungan.h"

void dec2bin(int num, char *str);
int mod_int (int a, int b);
int int_tak_min(int nilai, int pengganti);
char *remove_ext (char* str, char dot, char sep);
void set_null(char **str, size_t length);
void print_array(char *array, int length);
void print_unsigned_array(unsigned char *array, size_t length);
void print_char(char *array, size_t length);
void print_char_csv(char *array, size_t start, size_t maximal);
int check_parity(char *str, size_t start, size_t length);
void insert_string(char **target, size_t target_length,
	char *str, size_t start, size_t str_length);
char *basename (const char *filename);
off_t fsize(const char *filename);
bool file_exist (char *filename);
char *readable_fs(double size, char *penyanga);
int killpid(pid_t pid, int sig);
// int cari_karakter(
	// char* jarum, char* jerami,
	// size_t awal, size_t akhir
	// );

void throw_error(int type, const char * file, const int line,
	const char *msg, ...);
void _test(const char * file, const int line, const char *msg);

// Makro.
#define FAIL(msg, ...) \
	throw_error(1, __FILE__, __LINE__, msg, __VA_ARGS__);
#define WARN(msg, ...) \
	throw_error(2, __FILE__, __LINE__, msg, __VA_ARGS__);
#define NOTICE(msg, ...) \
	throw_error(3, __FILE__, __LINE__, msg, __VA_ARGS__);
#define INFO(msg, ...) \
	throw_error(4, __FILE__, __LINE__, msg, __VA_ARGS__);
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
#define TEST(msg) \
	_test(__FILE__, __LINE__, msg);

#endif /* _KANCIL_FAEDAH_ */
