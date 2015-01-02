/* 
 * `tampilan.h`
 * Memberikan tampilan untuk Kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */
#ifndef _KANCIL_TAMPILAN_
#define _KANCIL_TAMPILAN_
#define PROGRES_KIRIM 0
#define PROGRES_TERIMA 1
void tampil_info_progres_berkas(
	int tipe, char* berkas,
	double sekarang, double tujuan,
	int ukuberkas_panjang
	);
#endif