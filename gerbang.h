/*
 * `gerbang.h`
 * Sebagai gerbang dari kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */
#ifndef _KANCIL_GERBANG_H_
#define _KANCIL_GERBANG_H_

// Kancil.
#define PROGNAME "Gerbang Kancil"
#define PROGCODE "KANCIL_GERBANG"
#include "kancil.h"

// Lokal.
#include "faedah.h"
#include "soket.h"
#include "pesan.h"
#include "tampilan.h"

// Fungsi lokal.
void info_kancil();
void signal_callback_handler(int signum);
void anak_gerbang(
	int sock,
	struct KIRIMBERKAS *kirim,
	struct INFOALAMAT *infoalamat
);
void free_shm();

int sockid;

#endif /* _KANCIL_GERBANG_H_ */
 