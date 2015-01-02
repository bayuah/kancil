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
#include "kancil.h"

// Lokal.
#include "faedah.h"
#include "soket.h"
#include "pesan.h"
#include "tampilan.h"

// Fungsi lokal.
void info_kancil();
void signal_callback_handler(int signum);
void anak_kirim(
	FILE *pberkas,
	struct KIRIMBERKAS *kirim,
	int ukuberkas_panjang
);

#endif /* _KANCIL_KLIEN_H_ */
