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

// Fungsi lokal.
void info_kancil();
void stop_listening(int sock);
void anak_tulis(struct BERKAS *berkas);
void signal_callback_handler(int signum);
void anak_sambungan (int sock, struct BERKAS *berkas);

int sockid;

#endif /* _KANCIL_PELADEN_H_ */
