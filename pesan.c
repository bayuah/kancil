/* 
 * `pesan.c`
 * Fungsi pesan.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */

#include "kancil.h"
#include "faedah.h"

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
 * @param (char*) berkas_nama, nama berkas tujuan. Maksimal 64 karakter;
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
	
	// Bila lebih dari 64 karakter, nama berkas dipotong.
	char* nama;
	nama=malloc(sizeof (nama) * 65);
	memset(nama, 0, 65);
	memcpy(nama, nama_berkas, 64);
	
	// Memeriksa panjang nama.
	panjang += strlen(nama);
	
	// Mengubah angka jadi huruf.
	char* ukuran;
	ukuran=malloc(sizeof (ukuran) * 64);
	memset(ukuran, 0, 64);
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
		if (strchr(tok, 0)==NULL){
			WARN(_("%1$s tidak sah."),_("Pesan"));
			return EXIT_FAILURE_CHUNK;
		}else{
			panjang_tok=strlen(tok);
			strncpy(*identifikasi_berkas,tok,panjang_tok+1);
			
			tok = strtok (NULL, "\r\n");
			if (strchr(tok, 0)==NULL){
				WARN(_("%1$s tidak sah."),_("Pesan"));
				return EXIT_FAILURE_CHUNK;
			}else{
				panjang_tok=strlen(tok);
				strncpy(*nama_berkas,tok,panjang_tok+1);
				
				tok = strtok (NULL, "\r\n");
				if (strchr(tok, 0)==NULL){
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
