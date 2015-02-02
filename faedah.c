/* 
 * `faedah.c`
 * Alat bantu kemanfaatan dalam program kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */

#include "lingkungan.h"

// Memastikan dapat membaca
// berkas ukuran besar.
#define __USE_LARGEFILE64
#define _BSD_SOURCE
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

// Standar.
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>

// Soket.
// #include <sys/socket.h>

// Posix.
#define _POSIX_SOURCE
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#ifndef _LOCALE_H
	#include <libintl.h>
	#include <locale.h>
	#define _(String) gettext (String) // Makro.
#endif

// Ambil struktur.
#include "struktur.h"

// Fungsi lokal.
char *remove_ext (char* str, char dot, char sep);
void print_unsigned_char_csv(
	unsigned char *array,
	size_t start,
	size_t maximal
);

/*
 * @param: type (int)
 * 1 Galat (Error)
 * 2 Peringatan (Warning)
 * 3 Pesan (Notice)
 * 4 Kekutu 1 (Debug1)
 * 5 Kekutu 2 (Debug2)
 */
void throw_error(int type, const char * file, const int line,
	const char *msg, ...){
	const bool boleh[] = {1,
		aturan.show_error,
		aturan.show_warning,
		aturan.show_notice,
		aturan.show_info,
		aturan.show_debug1,
		aturan.show_debug2,
		aturan.show_debug3,
		aturan.show_debug4,
		aturan.show_debug5
		};
	
	// Memastikan berhenti
	//sebelum inisiasi.
	if(!boleh[type])
		return;
	
	// Aksi.
	int penyangga_ukuran=1024;
	char penyangga[penyangga_ukuran];
	// penyangga=malloc(sizeof(penyangga)*penyangga_ukuran);
	
	// Tampilan.
	// Antara 1 hingga 8.
	if(type>=1&&type<=8){
		va_list args;
		va_start(args, msg);
		vsnprintf(penyangga, penyangga_ukuran, msg, args);
		va_end(args);
	};
	
	// Berkas.
	char filestr[strlen(file)];
	// filestr=malloc(sizeof(filestr)*strlen(file));
	strcpy(filestr, file);
	// filestr = remove_ext (filestr, '.', DIR_SEPARATOR);
	
	// Pilih.
	switch(type){
		case 0:
			printf(
				"%s:%d %s: %s\r\n",
				filestr,
				line, _("TES"), penyangga
				);
		break;
		case 1:
			if(boleh[1])
			fprintf(
				stderr, "%s:%d %s: %s\r\n",
				filestr,
				line, _("GALAT"), penyangga
				);
		break;
		case 2:
			if(boleh[2])
			fprintf(
				stderr, "%s: %s: %s\r\n",
				filestr,
				_("PERINGATAN"), penyangga
				);
		break;
		case 3:
			if(boleh[3])
			printf(
				"%s: %s: %s\r\n",
				filestr,
				_("MAKLUMAT"),
				penyangga
				);
		break;
		case 4:
			if(boleh[4])
			printf(
				"%s: %s\r\n",
				_("INFO"),
				penyangga
				);
		break;
		case 5:
			if(boleh[5])
			printf(
				"%s:%d %s: %s\r\n",
				filestr,
				line, _("KEKUTU 1"), penyangga
				);
		break;
		case 6:
			if(boleh[6])
			printf(
				"%s:%d %s: %s\r\n",
				filestr,
				line, _("KEKUTU 2"), penyangga
				);
		break;
		case 7:
			if(boleh[7])
			printf(
				"%s:%d %s: %s\r\n",
				filestr,
				line, _("KEKUTU 3"), penyangga
				);
		break;
		case 8:
			if(boleh[8])
			printf(
				"%s:%d %s: %s\r\n",
				filestr,
				line, _("KEKUTU 4"), penyangga
				);
		break;
		case 9:
			if(boleh[9]){
				int va_argstart=0;
				int va_argmax=0;
				va_list args;
				va_start(args, msg);
				unsigned char* va_argmsg = va_arg(args, unsigned char*);
				va_argstart = va_arg(args, int);
				va_argmax   = va_arg(args, int);
				va_end(args);
				printf(
					"%s:%d %s: %s: %i-%i ",
					filestr,
					line, _("KEKUTU 5"),
					msg, va_argstart, va_argmax
				);
				printf("{");
				print_unsigned_char_csv(va_argmsg, (size_t)va_argstart, (size_t)va_argmax);
				printf("}");
				printf("\r\n");
			};
		break;
	};
	
	// membersihkan memori.
	// free(filestr);
	// free(penyangga);
}

void dec2bin(int num, char *str){
	*(str+5) = '\0';
	int mask = 0x10 << 1;
	while(mask >>= 1)
	*str++ = !!(mask & num) + '0';
}


/*
 * remove_ext()
 * Menghapus akhiran berkas.
 * @url: http://stackoverflow.com/a/2736841/1705800
 * @penulis-asli: Paxdiablo (http://stackoverflow.com/users/14860)
 * @param (char*) str, string terproses;
 * @param (char*) dot, pemisah akhiran;
 * @param (char*) sep, pemisah lajur, 0 berarti diabaikan.
 * @hasil: (char*) NULL bila gagal.
 * @Catatan: Memori hasil harus dibersihkan.
 * @Contoh:
 *  char *s;
 *  printf ("[%s]\n", (s = remove_ext ("halo.txt", '.', '/')));
 *  free (s);
 */
char *remove_ext (char* str, char dot, char sep) {
	char *retstr, *lastdot, *lastsep;

	// Memeriksa galat dan alokasi untaian.
	if (str == NULL)
		return NULL;
	if ((retstr = malloc (strlen (str) + 1)) == NULL)
		return NULL;

	// Menyalin dan mencari karakter tercari.
	strcpy (retstr, str);
	lastdot = strrchr (retstr, dot);
	lastsep = (sep == 0) ? NULL : strrchr (retstr, sep);

	// Bila pemisah akhiran.
	if (lastdot != NULL) {
		
		// Bila sebelum pemisah akhiran.
		if (lastsep != NULL) {
			if (lastsep < lastdot) {
			
				// then remove it.
				*lastdot = '\0';
			}
		} else {
		
			// Has extension separator with no path separator.
			*lastdot = '\0';
		}
	}

	// Return the modified string.
	return retstr;
}

/*
 * mod_int()
 * Melakukan operasi modulus bilangan bulat.
 * @url: http://stackoverflow.com/a/4003287/1705800
 * @penulis-asli: Armen Tsirunyan (http://stackoverflow.com/users/469935)
 * @param: (int) a Nilai asal;
 * @param: (int) a Nilai modulo;
 * @hasil: (int) hasil.
 */
int mod_int (int a, int b){
	if(b < 0)
		return mod_int(-a, -b);  
	int ret = a % b;
	if(ret < 0)
		ret+=b;
	return ret;
}

/*
 * int_tak_min()
 * Memastikan bahwa nilai int tidak minus.
 * @param: nilai (int) Nilai terperiksa.
 * @param: pengganti (int) Pengganti batas nilai NOL
 */
int int_tak_min(int nilai, int pengganti){
	if(nilai < 0)
		return (pengganti+nilai);
	else return nilai;
}

/*
 * set_null()
 * Memberikan nilai null terhadap variabel.
 * @param: (char*) str;
 * @hasil: takada.
 */
void set_null(char **str, size_t length){
	memset(*str,0, length);
}

/*
 * print_array()
 * Mencetak nilai larik (array).
 * @param: (char*) array;
 * @param: (int) length, panjang;
 * @hasil: takada.
 */
void print_array(char *array, size_t length){
	// int i;
	unsigned int i;
	for(i=0;i<length;i++){
		printf("index-%u:\"%c\"(%i)\r\n",i, array[i], array[i]);
	};
}
/*
 * print_unsigned_array()
 * Sama seperti print_array()
 * selain menggunakan unsigned char.
 */
void print_unsigned_array(unsigned char *array, size_t length){
	// int i;
	unsigned int i;
	for(i=0;i<length;i++){
		printf("index-%i:\"%c\"(%i)\r\n",i, array[i], array[i]);
	};
}
/*
 * print_unsigned_array_nosymbols()
 * Sama seperti print_unsigned_array()
 * selain tanpa simbol, hanya indeks ASCII.
 */
void print_unsigned_array_nonsymbol(unsigned char *array, size_t length){
	// int i;
	unsigned int i;
	for(i=0;i<length;i++){
		printf("index-%i: %i\r\n",i, array[i]);
	};
}
/*
 * print_char()
 * Sama seperti print_array()
 * selain hasil ditampilkan mendatar.
 */
void print_char(char *array, size_t length){
	// int i;
	unsigned int i;
	for(i=0;i<length;i++){
		printf("%c",array[i]);
	};
}
/*
 * print_char_csv()
 * Sama seperti print_char()
 * selain hasil ditampilkan
 * menurut nilai indeks dan CSV.
 */
void print_char_csv(char *array, size_t start, size_t maximal){
	// int i;
	unsigned int i;
	for(i=start;i<maximal;i++){
		printf("%i,",array[i]);
	};
}
/*
 * print_unsigned_char_csv()
 * Sama seperti print_char_csv()
 * selain menggunakan unsigned char.
 */
void print_unsigned_char_csv(
	unsigned char *array,
	size_t start,
	size_t maximal
){
	// int i;
	unsigned int i;
	for(i=start;i<maximal;i++){
		printf("%i,",array[i]);
	};
}

/*
 * check_parity()
 * Memeriksa bit paritas dari larik (array).
 * @param: (char*) str;
 * @param: (int) start, awal;
 * @param: (int) length, panjang;
 * @hasil: (int) paritas.
 */
int check_parity(char *str, size_t start, size_t length) {
	int n, parity = 0;
	unsigned int i;
	for(i=start;i<length;i++){
		n=str[i];
		// Operasi AND
		// dengan nilai dikurangi SATU.
		while (n){
			parity = !parity;
			n      = n & (n - 1);
		};
	};
	return parity;
}

/*
 * insert_string()
 * Menyisipkan string dalam string lain.
 * @param: (char*) target, tujuan string;
 * @param: (char*) str, string penyisip;
 * @param: (int) start, dimulai dari NOL penyisipan;
 * @param: (size_t) length, panjang pemyisipan;
 * @hasil: takada.
 */
void insert_string(char **target, size_t target_length,
	char *str, size_t start, size_t str_length){
	char *penyangga;
	size_t i_size;
	size_t j_size;
	
	// Alokasi penyangga sebesar `target`.
	penyangga=malloc(sizeof(*target)*target_length);
	set_null(&penyangga, target_length);
	
	// Memasukkan nilai penyangga
	// dari `target` dan `str`
	j_size=0;
	for(i_size=0; i_size<target_length;i_size++){
		// Bila di antara `start`
		// dan `length`.
		if(i_size>=start && i_size < (start+str_length)){
			penyangga[i_size]=*(str+j_size++);
		}else{
			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
				penyangga[i_size]=(int)(target+i_size);
			#pragma GCC diagnostic pop
		};
	};
	penyangga[target_length]=0;
	
	// Kembali.
	memcpy(*target, penyangga, target_length);
	
}

/*
 * basename()
 * Mencari nama dasar dari berkas.
 * @url: https://www.cygwin.com/ml/cygwin/2003-06/msg01312.html
 * @penulis-asli: Vikram Mehta <vm6580@hotmail.com>
 * @param: (char) filename;
 * @hasil: (char) hasil.
 */
char *basename (const char *filename){
	#ifdef DIR_SEPARATOR
		char *p = strrchr (filename, DIR_SEPARATOR);
	#else
		char *p = strrchr (filename, '/');
	#endif
	return p ? p + 1 : (char *) filename;
}

/* 
 * fsize()
 * Mencari ukuran berkas.
 * @url: http://stackoverflow.com/a/8384/1705800
 * @penulis-asli: Ted Percival (stackoverflow.com/users/954)
 */
off64_t fsize(const char *filename) {
	struct stat64 st; 
	
	if (stat64(filename, &st) == 0)
		return st.st_size;
	
	return -1; 
}
/* 
 * file_exist()
 * Mengetahui apakah berkas tersedia.
 * @url: http://stackoverflow.com/a/230070/1705800
 * @penulis-asli: Codebunny (stackoverflow.com/users/13667)
 */
bool file_exist (char *filename){
	struct stat64 st;
	return (stat64(filename, &st) == 0)? true:false;
}

/*
 * `readable_fs()`
 * Mendapatkan ukuran berkas yang dapat dibaca manusia.
 * @url: http://programanddesign.com/?p=182
 * @penulis-asli: Mark Bayazit (programanddesign.com)
 * @param: ukuran (double) dalam bita;
 * @param: penyanga  (char*).
 * @hasil: (char*) ukuran
 */
char *readable_fs(double ukuran, char *penyanga) {
	int i = 0;
	const char* satuan[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
	while (ukuran > 1024) {
		ukuran /= 1024;
		i++;
	};
	sprintf(penyanga, "%.*f %s", i, ukuran, satuan[i]);
	return penyanga;
}
/*
 * killpid()
 * Membunuh program berdasarkan pid.
 * @param: pid (pid_t) pid program;
 * @param: sig (int) sinyal pembunuh:
 *    SIGHUP  1    Hangup
 *    SIGINT  2    Interrupt from keyboard
 *    SIGKILL 9    Kill signal
 *    SIGTERM 15   Termination signal
 *    SIGSTOP 17,19,23   Stop the process.
 * @hasil: (int)
 */
int killpid(pid_t pid, int sig){
	int rturn;
	rturn=0;
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
		rturn=kill(pid, sig);
	#pragma GCC diagnostic pop
	return rturn;
}

/*
 * unix_signal_code()
 * Mengubah kode sinyal unix dari angka.
 * @param (int) sinyal
 * @hasil (cahr*) NULL bila tidak ditemukan.
 */
char *unix_signal_code(int sinyal){
	switch(sinyal){
		case 1: return "SIGHUP"; break;
		case 2: return "SIGINT"; break;
		case 3: return "SIGQUIT"; break;
		case 4: return "SIGILL"; break;
		case 5: return "SIGTRAP"; break;
		case 6: return "SIGABRT"; break;
		case 8: return "SIGFPE"; break;
		case 9: return "SIGKILL"; break;
		case 10: return "SIGBUS"; break;
		case 11: return "SIGSEGV"; break;
		case 13: return "SIGSYS"; break;
		case 14: return "SIGALRM"; break;
		case 15: return "SIGTERM"; break;
		case 16: return "SIGUSR1"; break;
		case 17: return "SIGUSR2"; break;
		case 18: return "SIGCHLD"; break;
		case 20: return "SIGTSTP"; break;
		case 21: return "SIGURG"; break;
		case 22: return "SIGPOLL"; break;
		case 23: return "SIGSTOP"; break;
		case 25: return "SIGCONT"; break;
		case 26: return "SIGTTIN"; break;
		case 27: return "SIGTTOU"; break;
		case 28: return "SIGVTALRM"; break;
		case 29: return "SIGPROF"; break;
		case 30: return "SIGXCPU"; break;
		case 31: return "SIGXFSZ"; break;
		default: return "UNKNOW"; break;
	};
}

/*
 * cari_karakter()
 * Mencari karakter dalam string.
 * @param (char*) jarum, tercari;
 * @param (char*) jerami, dicari;
 * @param (char*) awal, awal pencarian;
 * @param (char*) akhir, akhir pencarian.
 * @hasil (int) posisi. Dimulai dari NOL.
 */
// int cari_karakter(
	// char* jarum, char* jerami,
	// size_t awal, size_t akhir
	// ){
	// int i, posisi;
	// int ukuran_jarum=strlen(jarum);
	// for(i=awal; i<=akhir; i++){
		
	// };
	
	// return ukuran_jarum;
// }
