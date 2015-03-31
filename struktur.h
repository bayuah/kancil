/*
 * `struktur.h`
 * Definisi struktur data Kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014 - 2015
 * Lisensi: lihat LICENCE.txt
 */

#ifndef _KANCIL_STRUKTUR_H_
#define _KANCIL_STRUKTUR_H_

// Bila belum dimuat.
#ifndef _TR1_STDBOOL_H
	#include <stdbool.h>
#endif
#ifndef _INC_TYPES
	#include <sys/types.h>
#endif

#ifndef MAX_STR
	#define MAX_STR 2048
#endif

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
	#define INFOALAMAT_MAX_STR 128
#endif

struct INFOALAMAT{
	char inang           [INFOALAMAT_MAX_ID+1][INFOALAMAT_MAX_STR];
	int ipcount          [INFOALAMAT_MAX_ID+1];
	int ai_family        [INFOALAMAT_MAX_ID+1][INFOALAMAT_MAX_IP];
	int ai_socktype      [INFOALAMAT_MAX_ID+1][INFOALAMAT_MAX_IP];
	int ai_protocol      [INFOALAMAT_MAX_ID+1][INFOALAMAT_MAX_IP];
	unsigned int ai_addrlen [INFOALAMAT_MAX_ID+1][INFOALAMAT_MAX_IP];
	char ai_canonname    [INFOALAMAT_MAX_ID+1][INFOALAMAT_MAX_IP][INFOALAMAT_MAX_STR];
	unsigned short sockaddr_sa_family [INFOALAMAT_MAX_ID+1][INFOALAMAT_MAX_IP];
	char sockaddr_sa_data             [INFOALAMAT_MAX_ID+1][INFOALAMAT_MAX_IP][14];
	bool kunci;
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

// Terkirim.
#ifndef BERKAS_ID_NOTTRANSFERED
	#define BERKAS_ID_NOTTRANSFERED 0
#endif
#ifndef BERKAS_ID_INTRANSFER
	#define BERKAS_ID_INTRANSFER 1
#endif
#ifndef BERKAS_ID_TRANSFERED
	#define BERKAS_ID_TRANSFERED 2
#endif

// Kirim.
struct KIRIMBERKAS{
	/*
	 * `identifikasi` dan `identifikasi_sebelumnya`
	 * Memberikan informasi identifikasi dalam pengiriman berkas.
	 * `kelompok_kirim`
	 * Memberikan informasi kelompok pengiriman berkas.
	 */
	unsigned int identifikasi;
	unsigned int identifikasi_sebelumnya;
	unsigned int kelompok_kirim;
	
	/*
	 * urut_kali.
	 * Informasi cacah kali pengiriman berurutan. 
	 */
	int urut_kali;
	
	/*
	 * `paksa_panji`.
	 * Memaksa penggunaan panji. Bila diaktifkan (memberi nilai panji sahih),
	 * maka dalam sambungan pengiriman, panji sambungan sesuai dengan nilai.
	 */
	int paksa_panji;
	
	/*
	 * Ukuran.
	 * Ukuran berkas dan yang telah dikirim.
	 */
	double ukuran_berkas;
	double ukuran_kirim;
	double ukuran_kirim_sebelumnya;
	
	/*
	 * Informasi inang dan berkas.
	 */
	char hostname[BERKAS_MAX_STR];
	char portno[BERKAS_MAX_STR];
	char berkas[BERKAS_MAX_STR];
	char berkas_lajur[BERKAS_MAX_STR];
	char berkas_identifikasi[BERKAS_MAX_STR];
	
	/*
	 * Penghitung kecepatan.
	 * Membantu dalam melakukan menghitung kecepatan.
	 */
	double waktu_terkirim;
	double detik;
	double kecepatan;
	
	/*
	 * Status dan keadaan.
	 */
	int coba;
	bool do_kirim;
	bool kunci;
	int data_terkirim[MAX_CHUNK_ID];
} KIRIMBERKAS;

// Berkas.
struct TERIMABERKAS{
	/*
	 * Identifikasi.
	 * Identifikasi pecahan yang diterima.
	 */
	char identifikasi[BERKAS_MAX_STR];
	
	/*
	 * Nama berkas.
	 */
	char nama[BERKAS_MAX_STR];
	
	
	/*
	 * Ukuran berkas akan diterima,
	 * telah diterima, dan ofset ukuran.
	 */
	double ofset;
	double ukuran;
	double diterima;
	
	/*
	 * Identifikasi
	 * proses penulis.
	 */
	pid_t pid_tulis;
	
	/*
	 * Status penulisan berkas.
	 * Bila sedang melakukan penulisan,
	 * maka sambungan yang masuk akan ditolak.
	 */
	bool sedang_sibuk;
	bool terima_berkas;
	
	/*
	 * Data identifikasi
	 * pesan yang diterima.
	 */
	char data_pesan[MAX_CHUNK_ID]
		[CHUNK_MESSAGE_SIZE+1];
	bool data_terima[MAX_CHUNK_ID];
} TERIMABERKAS;

#ifndef MAX_GATE
	#define MAX_GATE 256
#endif

// Variabel global.
struct GLOBAL_CONFIG {
	/*
	 * Menampilkan
	 * tingkatan kekutu.
	 */
	bool show_error;
	bool show_warning;
	bool show_notice;
	bool show_info;
	bool show_debug1;
	bool show_debug2;
	bool show_debug3; 
	bool show_debug4;
	bool show_debug5;
	bool quiet;
	int  debuglevel;
	
	/*
	 * Transfer mentah.
	 * Bernilai `false`
	 * bila transfer terenkripsi.
	 */
	bool rawtransfer;
	
	/*
	 * Eksperimen.
	 * Menggeser ke awal dari
	 * bita terakhir
	 * dalam pembacaan berkas.
	 */
	bool shifteof;
	
	/*
	 * Memeriksa balasan
	 * dari Peladen.
	 */
	bool transferedcheck;
	
	/*
	 * Menampilkan rangkuman
	 * dari pengiriman berkas.
	 * BELUM diterapkan.
	 */
	bool summary;
	
	/*
	 * Kali dan tunggu
	 * mencoba pengiriman.
	 */
	int  tries;
	int  waitretry;
	
	/*
	 * Waktu (detik) tunggu
	 * sebelum melakukan
	 * pengiriman ulang
	 * kelompok pecahan selanjutnya.
	 */
	int  waitqueue;
	
	/*
	 * Jumlah maksimal sambungan.
	 * Penerimaan dan pengiriman.
	 */
	int  maxconnection;
	int  parallel;
	
	/*
	 * Pengaturan
	 * berkas.
	 */
	char completedir[BERKAS_MAX_STR];
	char tempdir[BERKAS_MAX_STR];
	char config[BERKAS_MAX_STR];
	
	/*
	 * Lajur Berkas dikirim.
	*/
	char inputfile[BERKAS_MAX_STR];
	
	/*
	 * Pengaturan
	 * garam.
	 */
	unsigned char salt[MAX_STR];
	unsigned char salt_send[MAX_STR];
	
	
	/*
	 * Pengaturan
	 * pemilihan IP.
	 */
	int  timebase;
	int  timetolerance;
	int  gateid;
	int  gates_c;
	
	/* 
	 * Cara Pengalutan.
	 * 0 Hanya XOR
	 * 1 Menggunakan kunci publik RSA
	 */
	int  gateshashing;
	
	/*
	 * Inang penerima.
	 */
	int  hostname_c;
	char hostname[MAX_GATE][INFOALAMAT_MAX_STR];
	
	/*
	 * Kunci publik dan privat
	 * serta bantalan RSA
	 */
	int  pubkeys_c;
	char pubkeys[MAX_GATE][BERKAS_MAX_STR];
	char privkey[BERKAS_MAX_STR];
	int rsa_padding;
	
	/*
	 * Peladen.
	 */
	char listening[INFOALAMAT_MAX_STR];
	char defaultport[INFOALAMAT_MAX_STR];
	
	/*
	 * Hanya simulasi pengiriman.
	 */
	int dummy;
} GLOBAL_CONFIG;
#define aturan GLOBAL_CONFIG

// Informasi kancil.
struct GLOBAL_KANCIL {
	int version_major;
	int version_minor;
	int version_patch;
	int built_number;
	char* progname;
	char* progcode;
	char* compile_mode;
	char* compiler_machine;
	char* compiler_version;
	char* compiler_flags;
	long built_time;
	char* executable;
	double unixtime;
} GLOBAL_KANCIL;
#define infokancil GLOBAL_KANCIL

#endif /* _KANCIL_STRUKTUR_H_ */
