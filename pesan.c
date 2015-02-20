/* 
 * `pesan.c`
 * Fungsi pesan.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014 - 2015
 * Lisensi: lihat LICENCE.txt
 */

#include <math.h>
#include "kancil.h"
#include "faedah.h"
#include "rsa.h"

/*
 * arti_panji()
 * Mengartikan panji.
 */
char* arti_panji(int panji){
	int penyangga_ukuran=64;
	char *penyangga;
	penyangga=malloc(sizeof(penyangga)*penyangga_ukuran);
	
	switch(panji){
		case START_FLAG:
			snprintf(penyangga, sizeof(penyangga)*penyangga_ukuran, _("Mulai"));
		break;
		case STOP_FLAG:
			snprintf(penyangga, sizeof(penyangga)*penyangga_ukuran, _("Henti"));
		break;
		case INTRANSFER_FLAG:
			snprintf(penyangga, sizeof(penyangga)*penyangga_ukuran, _("Transfer"));
		break;
		case INVALID_FLAG:
			snprintf(penyangga, sizeof(penyangga)*penyangga_ukuran, _("Taksah"));
		break;
		default:
			snprintf(penyangga, sizeof(penyangga)*penyangga_ukuran, _("Tak Dikenal (%1$i)"), panji);
		break;
	};
	
	// Hasil.
	return penyangga;
}

char *arti_status(int status){
	int penyangga_ukuran=64;
	char *penyangga;
	penyangga=malloc(sizeof(penyangga)*penyangga_ukuran);
	
	switch(status){
		case 0:
			snprintf(penyangga, sizeof(penyangga)*penyangga_ukuran, _("NOPE"));
		break;
		case 1:
			snprintf(penyangga, sizeof(penyangga)*penyangga_ukuran, _("OKE"));
		break;
		default:
			snprintf(penyangga, sizeof(penyangga)*penyangga_ukuran, _("Tak Dikenal (%1$i)"), status);
		break;
	};
	
	// Hasil.
	return penyangga;
}

/*
 * buat_pecahan_start()
 * Membuat pecahan awal sebagai identifikasi berkas terkirim.
 * Setiap bagian dibatasi oleh karakter [CR][LF].
 * @param (char*) pecahan, tujuan;
 * @param (size_t) panjang_pecahan, panjang maksimal;
 * @param (char*) berkas_identifikasi, penanda berkas terkirim.
 *  Digunakan agar peladen mengetahui bahwa berkas
 *  yang dikirim merupakan bagian dari berkas lain;
 * @param (char*) berkas_nama, nama berkas tujuan. Maksimal BERKAS_MAX_STR karakter;
 * @param (double) berkas_ukuran, ukuran berkas.
 *  Angkat bulat yang akan diubah menjadi string (char*)
 */
char* buat_pesan_start(
	char *pesan,
	size_t panjang_pecahan,
	char *identifikasi_berkas,
	char *nama_berkas,
	double ukuran_berkas
	){
	int panjang=0;
	
	// Memeriksa identifikasi.
	panjang += strlen(identifikasi_berkas);
	
	// Bila lebih dari BERKAS_MAX_STR karakter,
	// nama berkas dipotong.
	char* nama;
	nama=malloc(sizeof (nama) * BERKAS_MAX_STR+1);
	memset(nama, 0, BERKAS_MAX_STR+1);
	memcpy(nama, nama_berkas, BERKAS_MAX_STR);
	
	// Memeriksa panjang nama.
	panjang += strlen(nama);
	
	// Mengubah angka jadi huruf.
	char* ukuran;
	ukuran=malloc(sizeof (ukuran) * BERKAS_MAX_STR+1);
	memset(ukuran, 0, BERKAS_MAX_STR+1);
	sprintf(ukuran, "%1$.0lf", ukuran_berkas);
	
	// Memeriksa panjang ukuran.
	panjang += strlen(ukuran);
	
	// Memeriksa panjang semua.
	if(panjang+6>(int)panjang_pecahan){
		// Bila lebih dari ukuran,
		// keluar dari program.
		FAIL(
			_("Panjang %1$s (%2$i) terlalu besar. Maksimal %3$i."),
			_("pecahan awal"),panjang, panjang_pecahan
			);
		exit(EXIT_FAILURE_MESSAGES);
	};
	
	// Buat komposisi.
	sprintf(
		pesan,
		"%1$s\r\n%2$s\r\n%3$s\r\n",
		identifikasi_berkas,
		nama,
		ukuran
	);
	
	// Bersihkan.
	free(nama);
	free(ukuran);
	
	// Hasil.
	return pesan;
}
int ambil_pesan_start(
	char *pesan,
	size_t panjang_pesan,
	char **identifikasi_berkas,
	char **nama_berkas,
	char **ukuran_berkas
	){
	
	// Pesan.
	DEBUG4(_("Panjang maksimal pesan: %1$i."), panjang_pesan);
	
	/*
	Tidak digunakan
	sebab sering menyebabkan kesalahan segmentasi memori
	bila nilai adalah NOL.
	
	// Memeriksa panjang.
	int panjang = strlen(pesan);
	
	// Memeriksa panjang semua.
	if(panjang>(int)panjang_pesan){
		// Bila lebih dari ukuran,
		// keluar dari program.
		WARN(
			_("Panjang %1$s (%2$i) terlalu besar. Maksimal %3$i."),
			_("pecahan awal"),panjang, panjang_pesan
			);
		return EXIT_FAILURE_CHUNK;
	};
	*/
	
	// Memecah nilai.
	// Identifikasi||Nama||Ukuran||
	
	// Memeriksa apakah ada [CR][LF]
	if (strstr(pesan, "\r\n") == NULL){
		WARN(_("%1$s tidak sah."),_("Pesan"));
		return EXIT_FAILURE_CHUNK;
	}else{
		char* tok;
		int panjang_tok=0;
		tok = strtok (pesan,"\r\n");
		if (tok==NULL|| strchr(tok, 0)==NULL){
			WARN(_("%1$s tidak sah."),_("Pesan"));
			return EXIT_FAILURE_CHUNK;
		}else{
			panjang_tok=strlen(tok);
			strncpy(*identifikasi_berkas,tok,panjang_tok+1);
			
			tok = strtok (NULL, "\r\n");
			if (tok==NULL|| strchr(tok, 0)==NULL){
				WARN(_("%1$s tidak sah."),_("Pesan"));
				return EXIT_FAILURE_CHUNK;
			}else{
				panjang_tok=strlen(tok);
				strncpy(*nama_berkas,tok,panjang_tok+1);
				
				tok = strtok (NULL, "\r\n");
				if (tok==NULL|| strchr(tok, 0)==NULL){
					WARN(_("%1$s tidak sah."),_("Pesan"));
					return EXIT_FAILURE_CHUNK;
				}else{
					panjang_tok=strlen(tok);
					strncpy(*ukuran_berkas,tok,panjang_tok+1);
				};
			};
		};
	};
	
	// Tidak ada masalah.
	return EXIT_SUCCESS;
}

/*
 * buat_pesan_peladen()
 * Membuat pesan balasan kepada Klien.
 * Setiap bagian dibatasi oleh karakter [CR][LF].
 * @param (char*) pesan, tujuan;
 * @param (size_t) panjang_pecahan, panjang maksimal;
 * @param (char*) berkas_identifikasi, penanda berkas terkirim.
 *  Digunakan agar peladen mengetahui bahwa berkas
 *  yang dikirim merupakan bagian dari berkas lain;
 * @param (double) berkas_ukuran, ukuran berkas;
 *  Angkat bulat yang akan diubah menjadi string (char*)
 * @param (double) ukuran_diterima, ukuran berkas diterima.
 *  Angkat bulat yang akan diubah menjadi string (char*)
 */
char *buat_pesan_peladen(
	char *pesan,
	size_t panjang_pecahan,
	char* identifikasi_berkas,
	double ukuran_berkas,
	double ukuran_diterima
){
	int panjang=0;
	
	// Memeriksa identifikasi.
	panjang += strlen(identifikasi_berkas);
	
	/*Bagian Kedua*/
	
	// Mengubah angka jadi huruf.
	char* ukuran_berkas_str;
	ukuran_berkas_str=malloc(sizeof (ukuran_berkas_str) * BERKAS_MAX_STR+1);
	memset(ukuran_berkas_str, 0, BERKAS_MAX_STR+1);
	sprintf(ukuran_berkas_str, "%.0f", ukuran_berkas);
	
	// Memeriksa panjang ukuran.
	panjang += strlen(ukuran_berkas_str);
	
	/*Bagian Ketiga*/
	
	// Mengubah angka jadi huruf.
	char* ukuran_diterima_str;
	ukuran_diterima_str=malloc(sizeof (ukuran_diterima_str) * BERKAS_MAX_STR+1);
	memset(ukuran_diterima_str, 0, BERKAS_MAX_STR+1);
	sprintf(ukuran_diterima_str, "%.0f", ukuran_diterima);
	
	// Memeriksa panjang ukuran.
	panjang += strlen(ukuran_diterima_str);
	
	/*Bagian Keempat*/
	
	// Ambil waktu sekarang.
	double waktu_unix, waktu_unix_detik, waktu_unix_mikrodetik;
	waktu_unix=current_time(CURRENTTIME_MICROSECONDS);
	waktu_unix_mikrodetik=modf(waktu_unix, &waktu_unix_detik);
	
	// Mengubah angka jadi huruf.
	char* waktu_unix_detik_str;
	waktu_unix_detik_str=malloc(sizeof (waktu_unix_detik_str) * BERKAS_MAX_STR+1);
	memset(waktu_unix_detik_str, 0, BERKAS_MAX_STR+1);
	sprintf(waktu_unix_detik_str, "%1$.0f", waktu_unix_detik);
	
	// Memeriksa panjang ukuran.
	panjang += strlen(waktu_unix_detik_str);
	
	// Mengubah angka jadi huruf.
	char*waktu_unix_mikrodetik_str;
	waktu_unix_mikrodetik_str=malloc(sizeof (waktu_unix_mikrodetik_str) * BERKAS_MAX_STR+1);
	memset(waktu_unix_mikrodetik_str, 0, BERKAS_MAX_STR+1);
	sprintf(waktu_unix_mikrodetik_str, "%f", waktu_unix_mikrodetik);
	
	// Buang tanda desimal.
	memmove(
		waktu_unix_mikrodetik_str+0,
		waktu_unix_mikrodetik_str+2,
		BERKAS_MAX_STR+1-2
	);
	
	// Memeriksa panjang ukuran.
	panjang += strlen(waktu_unix_mikrodetik_str);
	
	
	// Memeriksa panjang semua.
	if(panjang+6>(int)panjang_pecahan){
		// Bila lebih dari ukuran,
		// keluar dari program.
		FAIL(
			_("Panjang %1$s (%2$i) terlalu besar. Maksimal %3$i."),
			_("pecahan awal"),panjang, panjang_pecahan
			);
		exit(EXIT_FAILURE_MESSAGES);
	};
	
	
	// Buat komposisi.
	sprintf(
		pesan,
		"%1$s\r\n%2$s\r\n%3$s\r\n%4$s.%5$s\r\n",
		identifikasi_berkas,
		ukuran_berkas_str,
		ukuran_diterima_str,
		waktu_unix_detik_str,
		waktu_unix_mikrodetik_str
	);
	
	// Bersihkan.
	free(ukuran_berkas_str);
	free(ukuran_diterima_str);
	free(waktu_unix_detik_str);
	free(waktu_unix_mikrodetik_str);
	
	// Hasil.
	return pesan;
}
int ambil_pesan_peladen(
	char *pesan,
	size_t panjang_pesan,
	char** identifikasi_berkas,
	char** ukuran_berkas,
	char** ukuran_diterima,
	char** unix_time
){
	
	// Pesan.
	DEBUG4(_("Panjang maksimal pesan: %1$i."), panjang_pesan);
	
	/*
	Tidak digunakan
	sebab sering menyebabkan kesalahan segmentasi memori
	bila nilai adalah NOL.
	
	// Memeriksa panjang.
	int panjang = strlen(pesan);
	
	// Memeriksa panjang semua.
	if(panjang>(int)panjang_pesan){
		// Bila lebih dari ukuran,
		// keluar dari program.
		WARN(
			_("Panjang %1$s (%2$i) terlalu besar. Maksimal %3$i."),
			_("pecahan awal"),panjang, panjang_pesan
			);
		return EXIT_FAILURE_CHUNK;
	};
	*/
	
	// Memecah nilai.
	// Identifikasi||Ukuran berkas||Ukuran diterima||
	// Waktu UNIX detik[.]Waktu UNIX milidetik||
	
	// Memeriksa apakah ada [CR][LF]
	DEBUG4(_("Memeriksa apakah terdapat simbol %1$s."), "[CR][LF]");
	if (strstr(pesan, "\r\n") == NULL){
		WARN(_("%1$s tidak sah."),_("Pesan"));
		return EXIT_FAILURE_CHUNK;
	}else{
		// Pesan.
		DEBUG4(_("Memecah pesan."), 0);
		
		char* tok;
		int panjang_tok=0;
		
		DEBUG4(_("Memeriksa apakah kosong."), 0);
		tok = strtok (pesan,"\r\n");
		if (tok==NULL|| strchr(tok, 0)==NULL){
			WARN(_("%1$s tidak sah."),_("Pesan"));
			return EXIT_FAILURE_CHUNK;
		}else{
			DEBUG4(_("Tidak kosong."), 0);
			
			// Identifikasi.
			panjang_tok=strlen(tok);
			DEBUG4(_("Mendapatkan identifikasi berkas dengan panjang %1$i bita."), panjang_tok);
			strncpy(*identifikasi_berkas, tok, panjang_tok);
			DEBUG4(_("Berhasil mendapatkan identifikasi berkas %i ."), strlen(*identifikasi_berkas));
			tok = strtok (NULL, "\r\n");
			
			DEBUG4(_("Memeriksa apakah kosong."), 0);
			if (tok==NULL|| strchr(tok, 0)==NULL){
				WARN(_("%1$s tidak sah."),_("Pesan"));
				return EXIT_FAILURE_CHUNK;
			}else{
				DEBUG4(_("Tidak kosong."), 0);
				
				// Ukuran berkas.
				panjang_tok=strlen(tok);
				DEBUG4(_("Mendapatkan ukuran berkas panjang %1$i bita."), panjang_tok);
				strncpy(*ukuran_berkas,tok,panjang_tok);
				DEBUG4(_("Berhasil mendapatkan ukuran berkas."), 0);
				tok = strtok (NULL, "\r\n");
				
				DEBUG4(_("Memeriksa apakah kosong."), 0);
				if (tok==NULL|| strchr(tok, 0)==NULL){
					WARN(_("%1$s tidak sah."),_("Pesan"));
					return EXIT_FAILURE_CHUNK;
				}else{
					DEBUG4(_("Tidak kosong."), 0);
					
					// Ukuran diterima.
					panjang_tok=strlen(tok);
					DEBUG4(_("Mendapatkan ukuran diterima panjang %1$i bita."), panjang_tok);
					strncpy(*ukuran_diterima,tok,panjang_tok);
					DEBUG4(_("Berhasil mendapatkan ukuran diterima."), 0);
					tok = strtok (NULL, "\r\n");
					
					DEBUG4(_("Memeriksa apakah kosong."), 0);
					if (tok==NULL|| strchr(tok, 0)==NULL){
						WARN(_("%1$s tidak sah."),_("Pesan"));
						return EXIT_FAILURE_CHUNK;
					}else{
						DEBUG4(_("Tidak kosong."), 0);
						
						// Waktu unix.
						panjang_tok=strlen(tok);
						DEBUG4(_("Mendapatkan waktu peladen panjang %1$i bita."), panjang_tok);
						strncpy(*unix_time,tok,panjang_tok);
						DEBUG4(_("Berhasil mendapatkan waktu peladen."), 0);
					};
				};
			};
		};
	};
	
	// Tidak ada masalah.
	return EXIT_SUCCESS;
}

/* pengepala()
 * Pengepala mengambil EMPAT bita pertama dari POTONGAN
 * sebagai:
 *  1 bita pertama sebagai versi dan panji
 *         (3 bit versi, 2 bit panji, 3 bit cadangan)
 *  2 bita kedua identifikasi/nomor pecahan
 *  1 bita terakhir dicadangkan.
 * @param (char*) pecahan;
 * @param (int) identifikasi (maksimal 254)
 * @param (int) panji:
 *  0 (00) INVALID
 *  1 (10) STOP
 *  2 (01) START
 *  3 (11) IN-TRANSFER
 * @param (int) bit_paritas, nilai bit paritas pesan (sahih 0 dan 1).
 * @param (int) status_gerbang,
 *  status penerimaan pengiriman pecahan di gerbang.
 *  0 Pesan tidak diterima    
 *  1 Pesan diterima
 *  Pengiriman klien harus bernilai 0 (NOL)
 *  0 dan 1 dikirimkan oleh gerbang dan peladen
 * @param (int) status_peladen,
 *  status penerimaan pengiriman pecahan di peladen.
 *  0 Pesan tidak diterima    
 *  1 Pesan diterima
 *  Pengiriman klien harus bernilai 0 (NOL)
 *  0 dan 1 dikirimkan oleh gerbang dan peladen
 */
char* buat_pengepala(
	char *pecahan,
	unsigned int identifikasi,
	int panji,
	int bit_paritas,
	int status_gerbang,
	int status_peladen
	){
	
	// Bita 0
	// Menambah versi.
	pecahan[0]=0;
	pecahan[0] |= PROTOCOL_VERSION << 0;
	
	// Menambah panji
	// Memeriksa apakah nilai di antara 0-3
	if(panji<0||panji>3){
		FAIL ( 
			_("Nilai %1$s (%2$i) tidak sesuai. Harap di antara %3$i hingga %4$i."),
			_("panji"), panji, 0 , 3);
		exit(EXIT_FAILURE_MESSAGES);
	};
	pecahan[0] |= panji << 3;
	
	// Menambah bit paritas
	// Memeriksa apakah nilai di antara 0-1
	if(bit_paritas<0||bit_paritas>1){
		FAIL ( 
			_("Nilai %1$s (%2$i) tidak sesuai. Harap di antara %3$i hingga %4$i."),
			_("bit paritas"), bit_paritas, 0 , 1);
		exit(EXIT_FAILURE_MESSAGES);
	};
	pecahan[0] |= bit_paritas << 5;
	
	// Menambah status gerbang
	// Memeriksa apakah nilai di antara 0-1
	if(bit_paritas<0||bit_paritas>1){
		FAIL ( 
			_("Nilai %1$s (%2$i) tidak sesuai. Harap di antara %3$i hingga %4$i."),
			_("status gerbang"), status_gerbang, 0 , 1);
		exit(EXIT_FAILURE_MESSAGES);
	};
	pecahan[0] |= status_gerbang << 6;
	
	// Menambah status gerbang
	// Memeriksa apakah nilai di antara 0-1
	if(status_peladen<0||status_peladen>1){
		FAIL ( 
			_("Nilai %1$s (%2$i) tidak sesuai. Harap di antara %3$i hingga %4$i."),
			_("status peladen"), status_peladen, 0 , 1);
		exit(EXIT_FAILURE_MESSAGES);
	};
	pecahan[0] |= status_peladen << 7;
		
	// Bita 1 dan 2	
	// Memecah dua bila lebih dari 255.
	unsigned int identifikasi_1;
	unsigned int identifikasi_2;
	if(identifikasi>MAX_CHUNK_ID){
		// Nilai identifikasi terlalu besar.
		FAIL(
			_("Nilai %1$s (%2$i) terlalu besar. Maksimal %3$i."),
			_("identifikasi pengepala"),identifikasi, MAX_CHUNK_ID
			);
		exit(EXIT_FAILURE);
	}else if(identifikasi<=255){
		identifikasi_1=identifikasi;
		identifikasi_2=0;
	}else{
		identifikasi_1=mod_int(identifikasi, 255);
		identifikasi_2=identifikasi/255;
	};
	
	// Memasukkan nilai.
	pecahan[1] = identifikasi_1;
	pecahan[2] = identifikasi_2;
	
	// Kosong/dicadangkan,
	pecahan[3]=0;
	
	return pecahan;
}

/*
 * get_pesan()
 * Mendapatkan pengepala dari pecahan.
 */
void ambil_pengepala(
	char *pecahan,
	int *versi,
	unsigned int *identifikasi,
	int *panji,
	int *paritas,
	int *status_gerbang,
	int *status_peladen
	){
	
	// Bita 0.
	// Mendapatkan versi.
	*versi = pecahan[0] & (3 << 0);
	*versi = *versi >> 0;
	
	// Mendapatkan panji.
	*panji = pecahan[0] & (3 << 3);
	*panji = *panji >> 3;
	
	// Mendapatkan paritas.
	*paritas = pecahan[0] & (1 << 5);
	*paritas = *paritas >> 5;
	
	// Mendapatkan Status Gerbang.
	*status_gerbang = pecahan[0] & (1 << 6);
	*status_gerbang = *status_gerbang >> 6;
	
	// Mendapatkan Status Peladen.
	*status_peladen = pecahan[0] & (1 << 7);
	*status_peladen = *status_peladen >> 7;
	
	// Bita 1 dan 2.
	// Mendapatkan nilai dari tiap bita.
	unsigned int identifikasi_1;
	unsigned int identifikasi_2;
	identifikasi_1=(unsigned int)int_tak_min(pecahan[1], 256);
	identifikasi_2=(unsigned int)int_tak_min(pecahan[2], 256);
	*identifikasi=(identifikasi_2*(unsigned int)255)+identifikasi_1;
	if(*identifikasi>(unsigned int)MAX_CHUNK_ID){
		WARN(_("Identifikasi pengepala (%1$i) terlalu besar. Maksimal %2$i."), identifikasi, MAX_CHUNK_ID);
	};
	
	// Bita 4.
	// Dicadangkan.
}

/* set_pesan()
 * Melakukan pemecahan pesan
 * dan memeriksa bila identifikasi
 * masih dalam batas pesan.
 * @param (char*) pecahan;
 * @param: (int) identifikasi, dimulai dari nomor NOL;
 * @param: (char*) pesan;
 * @kembalian: (bool) `true` bila masih dalam pesan.
 */
char *buat_pesan(
	char *pecahan,
	unsigned int identifikasi,
	int *paritas,
	char* pesan){
	/* 
	 * panjang pecahan pesan bita merupakan
	 * panjang maksimal pecahan bita dikurangi dengan
	 * panjang pengepala pesan bita.
	 */
	int panjang_pecahan_pesan=CHUNK_MESSAGE_SIZE;
	
	/*
	 * Memeriksa apakah identifikasi
	 * telah melampaui panjang pesan.
	 */
	if(identifikasi>(unsigned int)MAX_CHUNK_ID){
		FAIL(_("Identifikasi pengepala (%1$i) terlalu besar. Maksimal %2$i."), identifikasi, MAX_CHUNK_ID);
		exit(EXIT_FAILURE_MESSAGES);
	};
	
	/*
	 * insert_string
	 */
	insert_string(
		&pecahan,                 MAX_CHUNK_SIZE,
		pesan, CHUNK_HEADER_SIZE, panjang_pecahan_pesan);
	
	// Hasil adalah bit paritas.
	// printf("Paritas-MX: %i\n", check_parity(pecahan, 4, MAX_CHUNK_SIZE));
	// printf("Paritas-PS: %i\n", check_parity(pesan, 0, panjang_pecahan_pesan));
	*paritas=check_parity(pesan, 0, panjang_pecahan_pesan);
	// exit(255);
	return pecahan;
}

/*
 * get_pesan()
 * Mendapatkan pesan dari pecahan.
 */
char *ambil_pesan(char* pecahan){
	/* 
	 * panjang pecahan pesan bita merupakan
	 * panjang maksimal pecahan bita dikurangi dengan
	 * panjang pengepala pesan bita.
	 */
	int panjang_pecahan_pesan=CHUNK_MESSAGE_SIZE;
	
	// Iniisiasi.
	char *penyangga;
	penyangga=malloc(sizeof (penyangga) * panjang_pecahan_pesan);
	memset(penyangga, 0, panjang_pecahan_pesan);
	
	// Penyalin nilai.
	int i,j;j=0;
	
	for(i=CHUNK_HEADER_SIZE;i<CHUNK_HEADER_SIZE+panjang_pecahan_pesan;i++){
		penyangga[j++]=pecahan[i];
	};
	
	// Hasil.
	return penyangga;
}

/*
 * `pilih_gerbang()`
 * Memilih gerbang.
 * @param: maksimal_gerbang (int) Jumlah maksimal gerbang;
 * @param: kunci (unsigned char*) Kunci yang digunakan;
 * @param: basis_waktu (int)      Basis waktu pemilihan gerbang;
 */
int pilih_gerbang(
	int maksimal_gerbang,
	unsigned char *kunci,
	int basis_waktu,
	double waktu_unix,
	RSA *rsa
){
	int rturn=-1;
	
	// Apakah basis waktu melebihi 60.
	if(basis_waktu>60){
		FAIL(_("Nilai basis waktu %1$i lebih besar dari nilai maksimal %2$i."), basis_waktu, 60);
		return -1;
	}
	
	// Mendapatkan panjang kunci.
	int panjang_kunci=strlen((char *)kunci);
	
	if(panjang_kunci>GATE_MAX_KEYSIZE){
		WARN(_("Panjang Kunci Gerbang (%1$i) terlalu besar. Maksimal: %2$i."), panjang_kunci, GATE_MAX_KEYSIZE);
		DEBUG1(_("Memotong sehingga memiliki panjang %1$i."), GATE_MAX_KEYSIZE);
		panjang_kunci=GATE_MAX_KEYSIZE;
	};
	
	// Maksimal gerbang.
	DEBUG4(_("Panjang kunci adalah %1$i bita."), panjang_kunci);
	DEBUG4(_("Waktu UNIX adalah %1$.0f."), waktu_unix);
	DEBUG4(_("Basis waktu adalah %1$i."), basis_waktu);
	DEBUG4(_("Jumlah maksimal Gerbang adalah %1$i."), maksimal_gerbang);
	
	// Mencari nilai awal detik.
	double waktu_detik=fmod(waktu_unix, 60);
	DEBUG4(_("Waktu detik adalah %1$.0f."), waktu_detik);
	double awal_detik=waktu_unix-waktu_detik;
	double basis_detik=floor(waktu_detik/(double)basis_waktu)*basis_waktu;
	DEBUG4(_("Basis detik adalah %1$.0f."), basis_detik);
	
	// Mengubah nilai UNIX
	// sesuai dengan basis.
	waktu_unix=(awal_detik+basis_detik);
	DEBUG4(_("Waktu UNIX diubah menjadi %1$.0f."), waktu_unix);
	
	// Mengubah nilai Waktu UNIX
	// ke untaian (string).
	unsigned char waktu_unix_str[256];
	memset(waktu_unix_str, 0, 256);
	snprintf((char *)waktu_unix_str, 256, "%.0f", waktu_unix);
	
	// Mendapatkan panjang waktu.
	int panjang_waktu=strlen((char *)waktu_unix_str);
	DEBUG4(_("Panjang waktu adalah %1$i bita."), panjang_waktu);
	
	// Pindahkan.
	// unsigned char kunci_ency[panjang_kunci+1];
	// memset(kunci_ency, 0, panjang_kunci+1);
	// memcpy(kunci_ency, kunci, panjang_kunci);
	// DEBUG4(_("Kunci adalah '%1$s'."), kunci_ency);
	
	// Mendapatkan hasil enkripsi.
	DEBUG4(_("Mendapatkan nilai Kunci Gerbang."), 0);
	unsigned char tujuan_ency[256];
	rsa_encrypt(
		// (unsigned char*)"Sate enak",
		kunci,
		panjang_kunci,
		tujuan_ency,
		rsa,
		RSA_NO_PADDING
	);
	DEBUG4(_("Berhasil mendapatkan nilai Kunci Gerbang."), 0);
	DEBUG5(_("Kunci Gerbang"), tujuan_ency, 0, panjang_waktu);
	DEBUG5(_("Waktu UNIX"), waktu_unix_str, 0, panjang_waktu);
	
	// Operasi XOR hasil dan waktu UNIX.
	// Operasi mulai dari LSB.
	int hasil=0;
	int i, j;
	DEBUG4(_("Mencari nilai XOR dari Kunci Gerbang dan Waktu UNIX."), 0);
	for(i=(panjang_waktu-1),j=0;i>=0;i--,j++){
		hasil+=(int)tujuan_ency[j]^waktu_unix_str[i];
	};
	DEBUG4(_("Nilai operasi XOR adalah %1$i."), hasil);
	
	// Mencari modulus
	// dari hasil XOR dan jumlah Gerbang.
	DEBUG4(_("Mencari nilai modulus dari nilai XOR dan jumlah Gerbang."), 0);
	rturn=mod_int(hasil, maksimal_gerbang);
	DEBUG4(_("Hasil adalah %1$i."), rturn);
	
	// Kembali.
	return rturn;
}
