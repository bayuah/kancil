/*
 * `argumen.h`
 * Pengurai argumen dari kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */

#ifndef _KANCIL_ARGUMEN_H_
#define _KANCIL_ARGUMEN_H_

void urai_argumen(int argc, char *argv[]);
void info_bangun();
void info_versi();
void lisensi();
void bantuan_param_standar();
void bantuan();
bool is_klien();
bool is_gerbang();
bool is_peladen();
int baca_konfigurasi();

#endif /*_KANCIL_ARGUMEN_H_*/
