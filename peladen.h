/*
 * `peladen.h`
 * Sebagai peladen dari Kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */
 
#ifndef _KANCIL_PELADEN_H_
#define _KANCIL_PELADEN_H_

// Kancil.
#define PROGNAME "Peladen Kancil"
#include "kancil.h"

// Lokal.
#include "faedah.h"
#include "tampilan.h"
#include "soket.h"
#include "pesan.h"

// Berkas.
struct DATABERKAS{
	unsigned int identifikasi;
	char pesan[CHUNK_MESSAGE_SIZE+1];
	struct DATABERKAS *lanjut;
} DATABERKAS;

typedef struct BERKAS{
	char identifikasi[64];
	char nama[64];
	double ofset;
	double ukuran;
	double diterima;
	pid_t pid_tulis;
	bool sedang_sibuk;
	bool terima_berkas;
	struct DATABERKAS data;
} BERKAS;

// Fungsi lokal.
void info_kancil();
void stop_listening(int sock);
void anak_tulis(struct BERKAS *berkas);
void signal_callback_handler(int signum);
void anak_sambungan (int sock, struct BERKAS *berkas);

int sockid;

#endif /* _KANCIL_PELADEN_H_ */
