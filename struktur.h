/*
 * `struktur.h`
 * Definisi struktur kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */

#ifndef _KANCIL_STRUKTUR_H_
#define _KANCIL_STRUKTUR_H_

// Variabel global.
struct GLOBAL_KANCIL {
	bool show_error;
	bool show_warning;
	bool show_notice;
	bool show_info;
	bool show_debug1;
	bool show_debug2;
	bool show_debug3;
	bool show_debug4;
	bool show_debug5;
	int tries;
	int waitretry;
	int maxqueue;
	int waitqueue;
	bool nowaitqueue;
	char* tempdir;
} GLOBAL_KANCIL;
#define kancil GLOBAL_KANCIL
#define aturan GLOBAL_KANCIL

// Alamat.
#ifndef INET_ADDRSTRLEN
	#define INET_ADDRSTRLEN 16
#endif
#ifndef INET6_ADDRSTRLEN
	#define INET6_ADDRSTRLEN 46
#endif

struct DAFTAR_ALAMAT{
	int identifikasi;
	char nama_inang[INET6_ADDRSTRLEN+1];
	struct addrinfo *info;
	struct DAFTAR_ALAMAT *awal;
	struct DAFTAR_ALAMAT *lanjut;
} DAFTAR_ALAMAT;

// Kirim.
struct KIRIMBERKAS{
	unsigned int identifikasi;
	unsigned int identifikasi_sebelumnya;
	unsigned int kelompok_kirim;
	int urut_kali;
	double ukuran_berkas;
	double ukuran_kirim;
	bool do_kirim;
	char *hostname;
	char *portno;
	char *berkas;
	int coba;
	struct DAFTAR_ALAMAT *alamat;
} KIRIMBERKAS;

#endif /* _KANCIL_STRUKTUR_H_ */
