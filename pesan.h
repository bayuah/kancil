/* 
 * `pesan.h`
 * Fungsi pesan.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014 - 2015
 * Lisensi: lihat LICENCE.txt
 */
#ifndef _KANCIL_PESAN_
#define _KANCIL_PESAN_
// Fungsi.
char* arti_panji(int panji);
char *arti_status(int status);
char* buat_pesan_start(
	char *pecahan,
	size_t panjang_pecahan,
	char *identifikasi_berkas,
	char *nama_berkas,
	double ukuran_berkas
	);
int ambil_pesan_start(
	char *pesan,
	size_t panjang_pecahan,
	char **identifikasi_berkas,
	char **nama_berkas,
	char **ukuran_berkas
	);
char* buat_pesan_peladen(
	char *pesan,
	size_t panjang_pecahan,
	char* identifikasi_berkas,
	double ukuran_berkas,
	double ukuran_diterima
);
char* buat_pengepala(
	char *pecahan,
	unsigned int identifikasi,
	int panji,
	int bit_paritas,
	int status_gerbang,
	int status_peladen
	);
void ambil_pengepala(
	char *pecahan,
	int *versi,
	unsigned int *identifikasi,
	int *panji,
	int *bit_paritas,
	int *status_gerbang,
	int *status_peladen
	);
int ambil_pesan_peladen(
	char *pesan,
	size_t panjang_pesan,
	char** identifikasi_berkas,
	char** ukuran_berkas,
	char** ukuran_diterima,
	char** unix_time
);
char *buat_pesan(
	char *pecahan,
	unsigned int identifikasi,
	int *paritas,
	char* pesan);
char *ambil_pesan(const char* pecahan);
int pilih_gerbang(
	int maksimal_gerbang,
	unsigned char *kunci,
	int basis_waktu,
	double waktu_unix,
	unsigned char *pubkey
);
#endif /* _KANCIL_PESAN_ */
