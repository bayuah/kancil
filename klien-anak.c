/* 
 * `kancil.h`
 * Sebagai klien dari kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt 
 */

#include "klien.h"

void anak_kirim(
	FILE *pberkas,
	struct KIRIMBERKAS *kirim,
	struct INFOALAMAT *infoalamat,
	int ukuberkas_panjang
){
	char *pesan="";
	char *pecahan="";
	size_t panjang_pesan=0;
	
	// Perilaku.
	int penunjuk_berkas=0;
	int maksimal_coba=aturan.tries;
	int ulang_tunggu=aturan.waitretry;
	int urut_maksimal=aturan.maxqueue;
	int urut_tunggu=aturan.waitqueue;
	bool urut_jangan_tunggu=true;
	unsigned int kelompok_kirim=kirim->kelompok_kirim;
	char *berkas=kirim->berkas;
	
	// Pengepala Pecahan.
	bool identifikasi_awal=true;
	unsigned int identifikasi=kirim->identifikasi;
	unsigned int identifikasi_sebelumnya=kirim->identifikasi_sebelumnya;
	int paritas=0;
	int panji=0;
	int status_gerbang=0;
	int status_peladen=0;
	
	// Tujuan.
	char *hostname=kirim->hostname;
	char *portno=kirim->portno;
	
	// Alokasi memori.
	// pecahan=malloc(sizeof(pecahan)*MAX_CHUNK_SIZE);
	pecahan=malloc(sizeof(pecahan)*ENCRYPTED_CONTAINER_SIZE);
	pesan=malloc((sizeof pesan)* CHUNK_MESSAGE_SIZE);
	
	// Berurutan kirim.
	kirim->urut_kali++;
	
	// Membersihkan isi.
	memset(pesan, 0, CHUNK_MESSAGE_SIZE);
	memset(pecahan, 0, MAX_CHUNK_SIZE);
	
	// Pesan.
	if (pesan == NULL){
		FAIL(_("Gagal mengalokasikan memori."),0);
		exit(EXIT_FAILURE_MEMORY);
	};
	
	// Apakah pertama.
	if(identifikasi==0){
		// Panji.
		panji=START_FLAG;
		
		// Memastikan nama berkas ada.
		if(!strlen(berkas)){
			// Kesalahan.
			FAIL(_("Gagal memperoleh nama berkas."), 0);
			exit(EXIT_FAILURE_MEMORY);
		};
		
		// Pecahan pertama adalah
		// pecahan dengan informasi berkas.
		pesan=buat_pesan_start(
			pesan,
			CHUNK_MESSAGE_SIZE,
			"sate",
			basename(kirim->berkas),
			kirim->ukuran_berkas
			);
	// Apakah hampir akhir.
	}else if(identifikasi==MAX_CHUNK_ID){
		// Panji.
		panji=STOP_FLAG;
		
	}else{
		// Panji.
		panji=INTRANSFER_FLAG;
		
		// Menggeser penunjuk berkas.
		penunjuk_berkas=kelompok_kirim*(identifikasi-1)*CHUNK_MESSAGE_SIZE;
		fseek(pberkas, penunjuk_berkas, SEEK_SET);
		
		// Baca berkas untuk pesan.
		panjang_pesan=fread(pesan, 1, CHUNK_MESSAGE_SIZE, pberkas);
		
		// Menambah bita terkirim.
		if(identifikasi_awal ||
			identifikasi!=identifikasi_sebelumnya){
			// Menambah.
			identifikasi_awal=false;
			kirim->ukuran_kirim+=(double)panjang_pesan;
			identifikasi_sebelumnya=identifikasi;
			kirim->identifikasi_sebelumnya=identifikasi;
		};
		
		// Bila telah selesai.
		if (!panjang_pesan){
			if(feof(pberkas)!=0){
				// Selesai kirim.
				char *penyangga_feof;
				penyangga_feof=malloc(sizeof(penyangga_feof)*ukuberkas_panjang);
				INFO(
					_("Berkas '%1$s' dengan ukuran %2$s (%3$.0lf bita) telah selesai dikirim."),
					basename(kirim->berkas), readable_fs(kirim->ukuran_berkas, penyangga_feof), kirim->ukuran_berkas
					);
				free(penyangga_feof);
			}else if(ferror(pberkas)!=0){
				WARN(_("Gagal membaca berkas '%1$s': %2$s (%3$i)."), basename(kirim->berkas), strerror(errno), errno);
			}else{
				WARN(_("Kesalahan berkas yang tidak diketahui: %1$s (%2$i)."), strerror(errno), errno);
			};
			
			// Pesan.
			DEBUG1(_("Mengirim panji '%1$s'."),_("Henti"));
			
			// Mengirim panji STOP
			// dan keluar dari perulangan.
			kirim->do_kirim=false;
			panji=STOP_FLAG;
		};
	};
	
	// Bangun pesan.
	pecahan=buat_pesan(
		pecahan,
		identifikasi,
		&paritas,
		pesan);
	
	// Bangun pengepala.
	DEBUG2(_("Panji %1$s."), arti_panji(panji));
	pecahan=buat_pengepala(
		pecahan,
		identifikasi,
		panji,
		paritas,
		status_gerbang,
		status_peladen
	);
	
	// Pesan.
	if(kelompok_kirim>1){
		DEBUG1(
			_("Mengirim pesan %1$i kelompok %2$i ."),
			identifikasi, kelompok_kirim);
	}else{
		DEBUG1(
			_("Mengirim pesan %1$i."),
			identifikasi);
	};
	
	// ============= Enkripsi  =======
	
	int panjang_pecahan;
	
	// Pesan mentah.
	DEBUG5(_("Pesan mentah dikirim"), pecahan, 0, MAX_CHUNK_SIZE);
	
	// Penyandian.
	unsigned char *pesan_ency;
	pesan_ency=malloc(sizeof(pesan_ency)*MAX_CHUNK_SIZE+2);
	pesan_ency=(unsigned char*)pecahan;
	unsigned char *tujuan_ency;
	tujuan_ency=malloc((sizeof tujuan_ency)*MAX_CHUNK_SIZE+5);
	panjang_pecahan=rsa_encrypt(
		pesan_ency,
		MAX_CHUNK_SIZE+1,
		default_rsa_pubkey(),
		tujuan_ency,
		RSA_PKCS1_OAEP_PADDING
	);
	
	// Pesan mentah.
	DEBUG5(_("Pesan mentah dikirim tersandikan"), tujuan_ency, 0, MAX_CHUNK_SIZE);
	// int panjang_pecahan=0;
	// Kirim.
	int panjang_diterima;
	pecahan=kirimdata(
		(char*)tujuan_ency,
		panjang_pecahan,
		hostname,
		portno,
		infoalamat,
		&panjang_diterima
		);
	
	// Pesan mentah.
	DEBUG4(_("Panjang pesan mentah diterima: %1$i"), panjang_pecahan);
	DEBUG5(_("Pesan mentah diterima"), pecahan, 0, panjang_pecahan);
	
	// Bila kosong.
	if(pecahan == NULL){
		// Pesan kesalahan.
		FAIL(_("Kegagalan %1$s."), _("Soket"));
		
		// Alokasi ulang memori.
		// pecahan=malloc((sizeof pecahan) * MAX_CHUNK_SIZE );
		
		// Keluar.
		exit(EXIT_FAILURE_SOCKET);
	};
	
	// ============= Dekripsi  =======
	
	// Pemecah sandi.
	unsigned char *pesan_deco=(unsigned char*)pecahan;
	unsigned char *tujuan_deco;
	tujuan_deco=malloc((sizeof tujuan_deco)*MAX_CHUNK_SIZE);
	panjang_pecahan=rsa_decrypt(
		pesan_deco,
		panjang_diterima,
		default_rsa_privatekey(),
		tujuan_deco,
		RSA_PKCS1_OAEP_PADDING
	);
	
	// Periksa.
	// print_unsigned_array(tujuan_deco, 100);
	
	// Pesan mentah.
	DEBUG4(_("Panjang pesan mentah diterima terpecahkan: %1$i"), panjang_pecahan);
	DEBUG5(_("Pesan mentah diterima terpecahkan"), tujuan_deco, 0, panjang_pecahan);
	
	// Ubah.
	pecahan=(char*)tujuan_deco;
	
	// Mendapatkan pengepala.
	// Respons.
	int r_versi;
	int r_panji;
	int r_paritas;
	int r_status_gerbang;
	int r_status_peladen;
	unsigned int r_identifikasi;
	ambil_pengepala(
		pecahan,
		&r_versi,
		&r_identifikasi,
		&r_panji,
		&r_paritas,
		&r_status_gerbang,
		&r_status_peladen
		);
	
	// Balasan
	DEBUG2(_("Balasan: Versi: %1$i."), r_versi);
	DEBUG2(_("Balasan: Identifikasi: %1$i."), r_identifikasi);
	DEBUG2(_("Balasan: Panji: %1$s."), arti_panji(r_panji));
	DEBUG2(_("Balasan: Paritas: %1$i."), r_paritas);
	DEBUG2(_("Balasan: Status Gerbang: %1$s."), arti_status(r_status_gerbang));
	DEBUG2(_("Balasan: Status Peladen: %1$s."), arti_status(r_status_peladen));
	
	// Memeriksa hasil.
	// Bila status gerbang atau peladen adalah NOL.
	if(!r_status_gerbang || !r_status_peladen){
		
		// Memeriksa apakah
		// telah terlalu banyak melakukan percobaan.
		if(kirim->coba>maksimal_coba){
			FAIL(_("Telah melakukan percobaan sebanyak %1$i kali. Berhenti."), maksimal_coba);
			exit(EXIT_FAILURE);
		}else if(!r_status_gerbang && !r_status_peladen){
			WARN(_("Gagal mengirim ke %1$s dan %2$s."),_("Gerbang"),_("Peladen"));
		}else if(!r_status_peladen){
			WARN(_("Gagal mengirim ke %1$s."),_("Peladen"));
		}else{
			WARN(_("Gagal mengirim ke %1$s."),_("Gerbang"));
		};
		
		// Panji Taksah.
		if(r_panji==INVALID_FLAG){
			// Pesan.
			NOTICE(_("Panji Taksah ditemukan."), 0);
			NOTICE(_("Menunggu %1$i detik untuk mengirim ulang."), ulang_tunggu);
			sleep(ulang_tunggu);
		}else if(r_panji==START_FLAG){
			// Tunggu berkas
			// adalah seperempat ulang tunggu.
			int tunggu=ulang_tunggu/2;
			NOTICE(_("Meminta pengiriman ulang informasi berkas."), 0);
			NOTICE(_("Menunggu %1$i detik untuk mengirim ulang."), tunggu);
			sleep(tunggu);
			
			// Mengatur ulang ukuran berkas terkirim.
			kirim->ukuran_kirim=0;
		};
		
		// Mengirim ulang.
		NOTICE(_("Percobaan ke-%1$i. Mengulangi pengiriman pecahan %2$i."),kirim->coba, r_identifikasi);
		identifikasi=r_identifikasi;
		kirim->identifikasi=identifikasi;
		
		// Menambah
		// percobaan pengiriman.
		kirim->coba++;
	}else{
		// Berhasil.
		char *penyangga_succ;
		penyangga_succ=malloc(sizeof(penyangga_succ)*ukuberkas_panjang);
		INFO(
			_("Berhasil mengirim %1$s (%2$.0lf bita)."),
			readable_fs(kirim->ukuran_kirim, penyangga_succ), kirim->ukuran_kirim
			);
		free(penyangga_succ);
		
		// Bila belum selesai.
		if(kirim->ukuran_kirim<=kirim->ukuran_berkas){
			// Menambahkan.
			identifikasi++;
			kirim->identifikasi=identifikasi;
			
			// Bila lebih dari spesifikasi,
			// mengulangi dari NOL.
			if(identifikasi>MAX_CHUNK_ID){
				// Pesan.
				WARN(_("Telah melebihi maksimal identifikasi %1$i."), MAX_CHUNK_ID);
				DEBUG1(_("Nilai identifikasi adalah %1$i."), identifikasi);
				
				// Perkembangan.
				tampil_info_progres_berkas(
					PROGRES_KIRIM, kirim->berkas,
					kirim->ukuran_kirim, kirim->ukuran_berkas,
					ukuberkas_panjang
				);
				
				// Menambah kelompok.
				kelompok_kirim++;
				kirim->kelompok_kirim=kelompok_kirim;
				
				// Pesan.
				NOTICE(_("Menunggu %1$i detik untuk melanjutkan."), urut_tunggu);
				DEBUG1(_("Kelompok pecahan selanjutnya adalah '%1$i'."), kelompok_kirim);
				
				// Tunggu.
				sleep(urut_tunggu);
				
				// Mengulangi identifikasi.
				identifikasi=0;
				kirim->identifikasi=identifikasi;
			};
		}else{
			kirim->do_kirim=false;
		};
		kirim->coba=1;
	};
	
	// Bila lebih dari maksimal kali kirim,
	// menunggu sebanyak waktu untuk mengirim ulang.
	if(kirim->urut_kali>=urut_maksimal && !urut_jangan_tunggu){
		// Pesan.
		WARN(_("Telah mengirim berurutan sebanyak %1$i kali."),kirim->urut_kali);
		
		// Perkembangan.
		tampil_info_progres_berkas(
			PROGRES_KIRIM, kirim->berkas,
			kirim->ukuran_kirim, kirim->ukuran_berkas,
			ukuberkas_panjang
		);
		
		// Pesan.
		NOTICE(_("Menunggu %1$i detik untuk mengirim ulang."), urut_tunggu);
		sleep(urut_tunggu);
		kirim->urut_kali=0;
	};
	
	// Memastikan nilai kelompok benar.
	kirim->kelompok_kirim=kelompok_kirim;
	
	// Mengembalikan data.
	// kirim->berkas=berkas;
	
	// if(identifikasi>3)
		// kirim->do_kirim=false;
}