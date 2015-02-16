/*
 * `klien.h`
 * Sebagai klien dari kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */
#ifndef _KANCIL_KLIEN_H_
#define _KANCIL_KLIEN_H_

// Kancil.
#define PROGNAME "Klien Kancil"
#define PROGCODE "KANCIL_KLIEN"
#include "kancil.h"

// Lokal.
#include "faedah.h"
#include "rsa.h"
#include "soket.h"
#include "pesan.h"
#include "tampilan.h"
#include "argumen.h"

// Fungsi lokal.
void info_kancil();
void signal_callback_handler(int signum);
unsigned int anak_kirim(
	unsigned int identifikasi,
	FILE *pberkas,
	struct KIRIMBERKAS *kirim,
	struct INFOALAMAT *info_alamat,
	int ukuberkas_panjang
);
void free_shm();

#endif /* _KANCIL_KLIEN_H_ */
