/* 
 * `tampilan.c`
 * Memberikan tampilan untuk Kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */
#include "lingkungan.h"

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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Posix
#define _POSIX_SOURCE
#include <sys/types.h>

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
			NOTICE(
				_("Berkas '%1$s' terkirim %2$s/%3$s (%4$.0lf/%5$.0lf bita)."),
				basename(berkas),
				ukuberkas_dikirim, ukuberkas_ukuran,
				sekarang, tujuan
				);
		break;
		case 1:
			NOTICE(
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
 