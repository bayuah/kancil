/*
 * `peladen-anak.c`
 * Sebagai anak peladen dari Kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014 - 2015
 * Lisensi: lihat LICENCE.txt
 */

#include "peladen.h"

/*
 * anak_tulis()
 * Proses yang menangani penulisan berkas.
 */
void anak_tulis(struct TERIMABERKAS *berkas){
	// Berkas.
	double dipotong=0;
	double ukuran_berkas=berkas->ukuran;
	double ukuran_diterima=berkas->ofset+berkas->diterima;
	double ukuran_diterima_sekarang=berkas->diterima;
	// Perilaku.
	int status;
	
	// Memasang status sibuk.
	DEBUG1(_("Mulai melakukan tindakan penulisan berkas."), 0);
	berkas->sedang_sibuk=true;
	
	// Periksa apakah nama kosong.
	if(!strlen(berkas->nama)){
		// Nama kosong.
		FAIL(_("Nama berkas kosong."), 0);
		
		// Melepas status sibuk.
		berkas->sedang_sibuk=false;
		
		// Keluar.
		exit(EXIT_FAILURE_VARS);
	};
	
	// Apakah ukuran kosong.
	if(ukuran_berkas<=0){
		// Berkas kosong.
		FAIL(_("Berkas '%1$s' kosong."), berkas->nama);
		
		// Melepas status sibuk.
		berkas->sedang_sibuk=false;
		
		// Keluar.
		exit(EXIT_FAILURE_VARS);
	};
	
	// Apakah tidak diterima.
	if(ukuran_diterima<=0){
		// Tidak diterima.
		FAIL(_("Tidak menerima berkas '%1$s'."), berkas->nama);
		
		// Melepas status sibuk.
		berkas->sedang_sibuk=false;
		
		// Keluar.
		exit(EXIT_FAILURE_IO);
	};
	
	// Bila direktori sementara tidak ada.
	if(!file_exist(aturan.tempdir)){
		DEBUG1(_("Direktori sementara '%1$s' tidak ditemukan."), aturan.tempdir);
		status=mkdir(aturan.tempdir, 0775);
		if(status){
			FAIL(
				_("Gagal membuat direktori '%1$s': %2$s (%3$i)."),
				aturan.tempdir, strerror(errno), errno
				);
			
			// Melepas status sibuk.
			berkas->sedang_sibuk=false;
			
			// Keluar.
			exit(EXIT_FAILURE_IO);
		}else{
			// Pesan.
			DEBUG1(
				_("Berhasil membuat direktori sementara '%1$s'."),
				aturan.tempdir
				);
		};
	};
	
	// Bila direktori selesai tidak ada.
	if(!file_exist(aturan.completedir)){
		DEBUG1(_("Direktori tujuan '%1$s' tidak ditemukan."), aturan.completedir);
		status=mkdir(aturan.completedir, 0775);
		if(status){
			FAIL(
				_("Gagal membuat direktori '%1$s': %2$s (%3$i)."),
				aturan.completedir, strerror(errno), errno
				);
			
			// Melepas status sibuk.
			berkas->sedang_sibuk=false;
			
			// Keluar.
			exit(EXIT_FAILURE_IO);
		}else{
			// Pesan.
			DEBUG1(
				_("Berhasil membuat direktori sementara '%1$s'."),
				aturan.completedir
				);
		};
	};
	
	// Membangun lajur.
	char pembatas[2];
	pembatas[0]=DIR_SEPARATOR;
	pembatas[1]=0;
	
	// Nama berkas selesai.
	// Nama untuk berkas belum selesai.
	int panjang_tmp=strlen(aturan.tempdir)+strlen(berkas->identifikasi)+3;
	char berkas_lajur_tmp[panjang_tmp];
	strcpy(berkas_lajur_tmp,aturan.tempdir);
	strcat(berkas_lajur_tmp,pembatas);
	strcat(berkas_lajur_tmp,berkas->identifikasi);
	
	// Nama untuk berkas telah selesai.
	int panjang_lajur=strlen(aturan.completedir)+strlen(berkas->nama)+3;
	char berkas_lajur[panjang_lajur];
	strcpy(berkas_lajur,aturan.completedir);
	strcat(berkas_lajur,pembatas);
	strcat(berkas_lajur,berkas->nama);
	
	// Nama.
	int nama_panjang=CHUNK_MESSAGE_SIZE/2;
	char nama_berkas[nama_panjang];
	memset(nama_berkas, 0, sizeof(nama_berkas[0])*(nama_panjang));
	
	// Nama berkas.
	strcpy(nama_berkas, berkas->identifikasi);
	
			
	// Bila diterima lebih besar dari ukuran.
	if(ukuran_diterima >ukuran_berkas){
		dipotong=ukuran_diterima-ukuran_berkas;
		
		// Pesan.
		DEBUG1(
			_("Berkas diterima (%1$.0f bita) lebih besar dari ukuran (%2$.0f bita)."),
			ukuran_diterima, ukuran_berkas
			);
		DEBUG2(_("Memotong berkas sebesar %1$.0f bita."), dipotong);
	}else{
		// Bila ukuran tiap kelompok pengiriman
		// melebihi ukuran keseluruhan pesan.
		// Hal ini disebabkan terdapat NULL diakhir pesan.
		double ukumax=(double)((MAX_CHUNK_ID-1) * CHUNK_MESSAGE_SIZE);
		if(ukuran_diterima_sekarang > ukumax){
			dipotong=ukuran_diterima-ukumax;
			
			// Pesan.
			DEBUG1(
				_("Berkas diterima (%1$.0f bita) lebih besar dari ukuran (%2$.0f bita)."),
				ukuran_diterima_sekarang, ukumax
				);
			DEBUG2(_("Memotong berkas sebesar %1$.0f bita."), dipotong);
		};
	};
	// Memindah nama berkas.
	// strcpy(berkas_lajur, berkas_lajur_tmp);
	
	
	// Buang ukuran.
	berkas->diterima=0;
	berkas->ofset=0;
	
	// Pesan.
	DEBUG1(_("Menjalankan penulisan berkas '%1$s'."), nama_berkas);
	
	// Membuka berkas.
	FILE *pberkas=fopen(berkas_lajur_tmp, "a+b");
	
	// Periksa kesalahan.
	if(pberkas == NULL){
		FAIL ( 
			_("Gagal membuka berkas '%1$s': %2$s (%1$i)."),
			berkas_lajur_tmp, strerror(errno), errno
			);
		
		// Melepas status sibuk.
		DEBUG1(_("Selesai melakukan tindakan penulisan berkas."), 0);
		berkas->sedang_sibuk=false;
		
		// Keluar.
		exit(EXIT_FAILURE_IO);
	}else{
		
		// Mulai tulis.
		size_t pesan_tertulis;
		size_t panjang_tulis;
		size_t tulis_tersisa;
		size_t sudah_tertulis;
		pesan_tertulis=0;
		sudah_tertulis=0;
		unsigned int id;
		id=1;
		
		// Menulis.
		size_t ukuran_terpotong=(size_t)(ukuran_diterima_sekarang-dipotong);
		DEBUG4(_("Mulai menulis berkas '%1$s' dengan ukuran %2$i bita."), nama_berkas, ukuran_terpotong);
		while(sudah_tertulis<ukuran_terpotong){
			
			// Menentukan panjang penulisan.
			// Bila panjang tersisa
			// adalah lebih besar `CHUNK_MESSAGE_SIZE`
			// maka `CHUNK_MESSAGE_SIZE`.
			tulis_tersisa=ukuran_terpotong-sudah_tertulis;
			if(tulis_tersisa > CHUNK_MESSAGE_SIZE){
				panjang_tulis=CHUNK_MESSAGE_SIZE;
			}else{
				panjang_tulis=tulis_tersisa;
			};
			
			// Tulis.
			DEBUG4(_("Tersisa %1$lu bita."),(long unsigned int) tulis_tersisa);
			DEBUG4(_("Menulis berkas %1$lu bita."),(long unsigned int) panjang_tulis);
			pesan_tertulis=fwrite((berkas->data_pesan[id]), 1, panjang_tulis, pberkas);
			DEBUG4(_("Selesai menulis berkas."), 0);
			
			// Bila telah selesai.
			if (pesan_tertulis){
				// Tambah.
				sudah_tertulis+=pesan_tertulis;
				
				// Membuang isi.
				memset(berkas->data_pesan[id], 0, CHUNK_MESSAGE_SIZE+1);
				
			}else{
				if(errno==0){
					// Tidak terjadi kesalahan.
					// Tambah.
					sudah_tertulis+=pesan_tertulis;
					
					// Membuang isi.
					memset(berkas->data_pesan[id], 0, CHUNK_MESSAGE_SIZE+1);
					
				}else if(feof(pberkas)!=0){
					
					// Tambah.
					sudah_tertulis+=pesan_tertulis;
					
					// Membuang isi.
					memset(berkas->data_pesan[id], 0, CHUNK_MESSAGE_SIZE+1);
					
					
				}else if(ferror(pberkas)!=0){
					
					// Pesan.
					FAIL(_("Gagal menulis berkas '%1$s': %2$s (%3$i)."), nama_berkas, strerror(errno), errno);
					
					// Melepas status sibuk.
					DEBUG1(_("Selesai melakukan tindakan penulisan berkas."), 0);
					berkas->sedang_sibuk=false;
					
					// Keluar.
					fclose (pberkas);
					exit(EXIT_FAILURE_IO);
				}else{
					
					// Pesan.
					FAIL(_("Kesalahan berkas yang tidak diketahui: %1$s (%2$i)."), strerror(errno), errno);
					
					// Melepas status sibuk.
					DEBUG1(_("Selesai melakukan tindakan penulisan berkas."), 0);
					berkas->sedang_sibuk=false;
					
					// Keluar.
					fclose (pberkas);
					exit(EXIT_FAILURE_IO);
				};
			};
			
			// Naik.
			id++;
		};
		fclose (pberkas);
		// Pesan.
		DEBUG1(_("Selesai menulis berkas '%1$s' sebesar %2$i bita."), nama_berkas, sudah_tertulis);
		
		// Bila telah selesai.
		if(ukuran_diterima >= ukuran_berkas){
			
			// Nama berkas.
			strcpy(nama_berkas, berkas->nama);
			
			// Bila berkas tmp ada.
			if(!file_exist(berkas_lajur_tmp)){
				FAIL(
					_("Gagal membuka mendahkan berkas sementara dengan identifikasi '%1$s': %2$s (%3$i)."),
					berkas->identifikasi, strerror(errno), errno
					);
				
				// Melepas status sibuk.
				berkas->sedang_sibuk=false;
				
				// Keluar.
				exit(EXIT_FAILURE_IO);
			}else{
				// Memindahkan berkas.
				status=rename(berkas_lajur_tmp, berkas_lajur);
				if(status){
					FAIL(
						_("Gagal mendahkan berkas sementara dengan identifikasi '%1$s': %2$s (%3$i)."),
						berkas->identifikasi, strerror(errno), errno
						);
					
					// Melepas status sibuk.
					berkas->sedang_sibuk=false;
					
					// Keluar.
					exit(EXIT_FAILURE_IO);
				}else{
					// Pesan.
					DEBUG1(
						_("Berhasil memindahkan berkas sementara '%1$s' untuk berkas '%2$s'."),
						berkas->identifikasi, nama_berkas
						);
				};
			};
			
			// Buang ukuran.
			berkas->diterima=0;
			berkas->ofset=0;
			
		};
		
	};
	
	// Bersihkan.
	memset(berkas->data_pesan, 0, sizeof(berkas->data_pesan[0][0])*MAX_CHUNK_ID*(CHUNK_MESSAGE_SIZE+1));
	memset(berkas->data_terima, 0, sizeof(berkas->data_terima[0])*MAX_CHUNK_ID);
	
	// Melepas status sibuk.
	DEBUG1(_("Selesai melakukan tindakan penulisan berkas."), 0);
	berkas->sedang_sibuk=false;
	
	// Membersihkan.
	// free(berkas_lajur);
}
