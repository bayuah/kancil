/* 
 * `pesan.h`
 * Fungsi pesan.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
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
char *buat_pesan(
	char *pecahan,
	unsigned int identifikasi,
	int *paritas,
	char* pesan);
char *ambil_pesan(const char* pecahan);

#endif /* _KANCIL_PESAN_ */
