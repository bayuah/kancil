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
	int rsa_datasize;
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

// Memastikan alamat
// dapat tersimpan
// di memori bagi pakai.
// Maksimal 10 inang
// dan 5 alamat.
#ifndef INFOALAMAT_MAX_ID
	#define INFOALAMAT_MAX_ID 10
#endif
#ifndef INFOALAMAT_MAX_IP
	#define INFOALAMAT_MAX_IP 5
#endif
#ifndef INFOALAMAT_MAX_STR
	#define INFOALAMAT_MAX_STR 64
#endif

struct INFOALAMAT{
	char inang           [INFOALAMAT_MAX_ID][INFOALAMAT_MAX_STR];
	int ipcount          [INFOALAMAT_MAX_ID];
	int ai_family        [INFOALAMAT_MAX_ID][INFOALAMAT_MAX_IP];
	int ai_socktype      [INFOALAMAT_MAX_ID][INFOALAMAT_MAX_IP];
	int ai_protocol      [INFOALAMAT_MAX_ID][INFOALAMAT_MAX_IP];
	unsigned int ai_addrlen [INFOALAMAT_MAX_ID][INFOALAMAT_MAX_IP];
	char ai_canonname    [INFOALAMAT_MAX_ID][INFOALAMAT_MAX_IP][INFOALAMAT_MAX_STR];
	unsigned short sockaddr_sa_family [INFOALAMAT_MAX_ID][INFOALAMAT_MAX_IP];
	char sockaddr_sa_data             [INFOALAMAT_MAX_ID][INFOALAMAT_MAX_IP][14];
} INFOALAMAT;

// Kancil.
#ifndef CHUNK_MESSAGE_SIZE
	#define CHUNK_MESSAGE_SIZE 1020
#endif
#ifndef MAX_CHUNK_ID
	#define MAX_CHUNK_ID 65025 // 16-bit.
#endif
#ifndef BERKAS_MAX_STR
	#define BERKAS_MAX_STR 255
#endif

// Kirim.
struct KIRIMBERKAS{
	unsigned int identifikasi;
	unsigned int identifikasi_sebelumnya;
	unsigned int kelompok_kirim;
	int urut_kali;
	double ukuran_berkas;
	double ukuran_kirim;
	double ukuran_kirim_sebelumnya;
	bool do_kirim;
	char hostname[BERKAS_MAX_STR];
	char portno[BERKAS_MAX_STR];
	char berkas[BERKAS_MAX_STR];
	char berkas_identifikasi[BERKAS_MAX_STR];
	bool data_terkirim[MAX_CHUNK_ID];
	double waktu_terkirim;
	int coba;
} KIRIMBERKAS;

// Berkas.
struct BERKAS{
	char identifikasi[BERKAS_MAX_STR];
	char nama[BERKAS_MAX_STR];
	double ofset;
	double ukuran;
	double diterima;
	pid_t pid_tulis;
	bool sedang_sibuk;
	bool terima_berkas;
	char data_pesan[MAX_CHUNK_ID][CHUNK_MESSAGE_SIZE+1];
	bool data_terima[MAX_CHUNK_ID];
} BERKAS;
#endif /* _KANCIL_STRUKTUR_H_ */
