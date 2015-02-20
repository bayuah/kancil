/*
 * `peladen.h`
 * Sebagai peladen dari Kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014 - 2015
 * Lisensi: lihat LICENCE.txt
 */
#ifndef _KANCIL_PELADEN_H_
#define _KANCIL_PELADEN_H_

// Kancil.
#define PROGNAME "Peladen Kancil"
#define PROGCODE "KANCIL_PELADEN"
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
void stop_listening(int sock);
void anak_tulis(struct TERIMABERKAS *berkas);
void signal_callback_handler(int signum);
void anak_sambungan (
	int sock,
	struct TERIMABERKAS *berkas,
	RSA *rsapub,
	RSA *rsapriv
);
void free_shm();

int sockid;

#endif /* _KANCIL_PELADEN_H_ */
