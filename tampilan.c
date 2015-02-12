/* 
 * `tampilan.c`
 * Memberikan tampilan untuk Kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */
#include "lingkungan.h"
#include "return.h"

#ifdef __x86_64___
	#define __USE_LARGEFILE64
	#define _LARGEFILE_SOURCE
	#define _LARGEFILE64_SOURCE
#endif

// Bila stat64 tidak ada.
#ifndef stat64
	#ifdef _stat64
		#define stat64 _stat64
	#endif
#endif

// standar.
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>

// Posix
#define _POSIX_SOURCE
#include <sys/types.h>
#include <sys/ioctl.h>

#ifndef _LOCALE_H
	#include <libintl.h>
	#include <locale.h>
	#define _(String) gettext (String) // Makro.
#endif

#include "faedah.h"

/*
 * tampil_info_progres_berkas()
 * Menampilkan perkembangan pengiriman/penerimaan berkas.
 * @param (int) tipe
 *  PROGRES_KIRIM menampilkan pengiriman
 *  PROGRES_TERIMA menampilkan penerimaan;
 * @param (double) sekarang, ukuran yang telah diterima/dikirim;
 * @param (double) tujuan, ukuran akhir;
 * @param (int) ukuberkas_panjang, panjang string.
 * @hasil: (takada) Langsung ke STDOUT.
 */
void tampil_info_progres_berkas(
	int tipe, char* berkas,
	double sekarang, double tujuan,
	int ukuberkas_panjang
	){
	// Mempersiapkan tampilan ukuran.
	char *ukuberkas_dikirim;
	ukuberkas_dikirim=malloc(sizeof(char*)*ukuberkas_panjang);
	ukuberkas_dikirim=readable_fs(sekarang, ukuberkas_dikirim);
	char *ukuberkas_ukuran;
	ukuberkas_ukuran=malloc(sizeof(char*)*ukuberkas_panjang);
	ukuberkas_ukuran=readable_fs(tujuan, ukuberkas_ukuran);
	
	// Pesan.
	switch(tipe){
		case 0:
			INFO(
				_("Berkas '%1$s' terkirim %2$s/%3$s (%4$.0lf/%5$.0lf bita)."),
				basename(berkas),
				ukuberkas_dikirim, ukuberkas_ukuran,
				sekarang, tujuan
				);
		break;
		case 1:
			INFO(
				_("Berkas '%1$s' diterima %2$s/%3$s (%4$.0lf/%5$.0lf bita)."),
				basename(berkas),
				ukuberkas_dikirim, ukuberkas_ukuran,
				sekarang, tujuan
				);
		break;
	};
	
	// Membersihkan.
	free(ukuberkas_ukuran);
	free(ukuberkas_dikirim);
}

/*
 * kancil_signal_code()
 * Menampilan sinyal kancil.
 * Bila sinyal tidak diketahui, mencoba memanggil sinyal unix.
 * @param (int) sinyal
 * @hasil (char*) NULL bila tidak ditemukan.
 */
char *kancil_signal_code(int sinyal){
	switch(sinyal){
	case EXIT_SUCCESS: return "EXIT_SUCCESS"; break;
	case EXIT_FAILURE: return "EXIT_FAILURE"; break;
	case EXIT_FAILURE_ARGS: return "EXIT_FAILURE_ARGS"; break;
	case EXIT_FAILURE_VARS: return "EXIT_FAILURE_VARS"; break;
	case EXIT_FAILURE_ENV: return "EXIT_FAILURE_ENV"; break;
	case EXIT_FAILURE_IO: return "EXIT_FAILURE_IO"; break;
	case EXIT_FAILURE_MEMORY: return "EXIT_FAILURE_MEMORY"; break;
	case EXIT_FAILURE_SOCKET: return "EXIT_FAILURE_SOCKET"; break;
	case EXIT_FAILURE_MESSAGES: return "EXIT_FAILURE_MESSAGES"; break;
	case EXIT_FAILURE_FORK: return "EXIT_FAILURE_FORK"; break;
	case EXIT_FAILURE_CHUNK: return "EXIT_FAILURE_CHUNK"; break;
	case EXIT_FAILURE_CRYPT: return "EXIT_FAILURE_CRYPT"; break;
	case EXIT_TEST: return "EXIT_TEST"; break;
	default: return unix_signal_code(sinyal); break;
	};
}

char *keterangan_soket(int apa, int nilai){
	switch(apa){
		// Keluarga Alamat dan Protokol
		case 1:
			switch(nilai){
				case 0: return "AF_UNSPEC";break;
				case 1: return "AF_UNIX";break;
				case 2: return "AF_INET";break;
				default:  return "AF_XXX";break;
			};
		break;
		// Keluarga Protokol.
		case 2:
			switch(nilai){
				case 1: return "SOCK_STREAM";break;
				case 2: return "SOCK_DGRAM";break;
				case 3: return "SOCK_RAW";break;
				case 4: return "SOCK_RDM";break;
				case 5: return "SOCK_SEQPACKET";break;
				case 6: return "SOCK_DCCP";break;
				case 10: return "SOCK_PACKET";break;
				default:  return "SOCK_XXX";break;
			};
		break;
		// Protokol IP.
		case 3:
			switch(nilai){
				case 0: return "IPPROTO_IP";break;
				case 1: return "IPPROTO_ICMP";break;
				case 2: return "IPPROTO_IGMP";break;
				case 4: return "IPPROTO_IPIP";break;
				case 6: return "IPPROTO_TCP";break;
				case 8: return "IPPROTO_EGP";break;
				case 12: return "IPPROTO_PUP";break;
				case 17: return "IPPROTO_UDP";break;
				case 22: return "IPPROTO_IDP";break;
				case 33: return "IPPROTO_DCCP";break;
				case 46: return "IPPROTO_RSVP";break;
				case 47: return "IPPROTO_GRE";break;
				case 41: return "IPPROTO_IPV6";break;
				case 50: return "IPPROTO_ESP";break;
				case 51: return "IPPROTO_AH";break;
				case 94: return "IPPROTO_BEETPH";break;
				case 103: return "IPPROTO_PIM";break;
				case 108: return "IPPROTO_COMP";break;
				case 132: return "IPPROTO_SCTP";break;
				case 136: return "IPPROTO_UDPLITE";break;
				case 255: return "IPPROTO_RAW";break;
				default: return "IPPROTO_MAX";break;
			};
		break;
	}
	return NULL;
}

/*
 * `_progress()`
 * Memberikan tampilan perkembangan.
 * @param (char*) Format pesan.
 * @param (char*) Pesan.
 */
void _progress(char *msg, ...){
	
	// Buang STDOUT.
	fflush(stdout);
	
	// Penyangga.
	int penyangga_ukuran=256;
	char penyangga[penyangga_ukuran];
	
	// Mendapatkan ukuran tty.
	struct winsize ws;
	ioctl(0, TIOCGWINSZ, &ws);
	int tty_kolom=ws.ws_col;
	
	// Ubah format.
	va_list args;
	va_start(args, msg);
	vsnprintf(penyangga, penyangga_ukuran, msg, args);
	va_end(args);
	
	// Panjang tampilan.
	int panjang_tampilan=strlen(penyangga);
	
	// Bila terlalu panjang.
	// if(panjang_tampilan>tty_kolom){
		// panjang_tampilan=tty_kolom;
		// penyangga[tty_kolom]=0;
	// };
	
	// Hasil.
	printf("\r");
	printf("%*.*s", 0, tty_kolom, penyangga);
	for(int i=0; i < (tty_kolom-panjang_tampilan); i++)
		printf(" ");
	printf("\r");
	
	memset(penyangga, 0, penyangga_ukuran);
	
	// Buang STDOUT.
	fflush(stdout);
	
}
