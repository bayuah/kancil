/*
 * `gerbang-anak.c`
 * Sebagai gerbang dari kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014 - 2015
 * Lisensi: lihat LICENCE.txt
 */

#include "gerbang.h"
/*
 * anak_sambungan()
 * Proses yang menangani sambungan dari klien.
 */
void anak_gerbang(
	int sock,
	struct KIRIMBERKAS *kirim,
	struct INFOALAMAT *infoalamat
){
	// Variabel.
	int n;
	// int status;
	int diterima = 0;
	// int ukuberkas_panjang=12;
	
	// Perilaku.
	int maksimal_coba=aturan.tries;
	int ulang_tunggu=aturan.waitretry;
	// int urut_maksimal=aturan.maxqueue;
	// int urut_tunggu=aturan.waitqueue;
	// bool urut_jangan_tunggu=true;
	
	// Penyangga.
	// size_t panjang_pesan=0;
	char penyangga[ENCRYPTED_CONTAINER_SIZE+1];
	memset(penyangga, 0, ENCRYPTED_CONTAINER_SIZE+1);
	
	// Pengepala.
	int versi;
	int panji;
	int paritas;
	int status_gerbang;
	int status_peladen;
	unsigned int identifikasi;
	
	// Pesan.
	char pesan[CHUNK_MESSAGE_SIZE+1];
	memset(pesan, 0, CHUNK_MESSAGE_SIZE+1);
	
	// Inisiasi.
	char hostname[BERKAS_MAX_STR];
	char portno[BERKAS_MAX_STR];
	
	// RSA.
	unsigned char tujuan_ency[ENCRYPTED_CONTAINER_SIZE+1];
	unsigned char tujuan_deco[MAX_CHUNK_SIZE+1];
	unsigned char pesan_ency[MAX_CHUNK_SIZE+1];
	unsigned char pesan_deco[ENCRYPTED_CONTAINER_SIZE+1];;
	
	// Teruskan.
	strncpy(hostname, kirim->hostname, BERKAS_MAX_STR);
	strncpy(portno, kirim->portno, BERKAS_MAX_STR);
	
	// Penerima. --------------------------------------------------------------
	
	// Menerima pesan.
	while(diterima < MAX_CHUNK_SIZE){
		n = recv(sock, penyangga+diterima, diterima-MAX_CHUNK_SIZE, 0);
		if (n < 0){
			WARN(_("Gagal membaca soket"), 0);
			memset(penyangga, 0, ENCRYPTED_CONTAINER_SIZE+1);
			return;
		};
		diterima+=n;
	};
	
	// Pesan mentah.
	DEBUG4(_("Panjang pesan mentah diterima: %1$i."), diterima);
	DEBUG5(_("Pesan mentah diterima"), penyangga, 0, diterima);
	
	// Dekripsi.
	if(!aturan.rawtransfer){
	
		// Memecahkan pesan.
		memcpy(pesan_deco, penyangga, ENCRYPTED_CONTAINER_SIZE);
		memset(tujuan_deco, 0, MAX_CHUNK_SIZE+1);
		diterima=rsa_decrypt(
			pesan_deco,
			diterima,
			default_rsa_privatekey(),
			tujuan_deco,
			aturan.rsa_padding
		);
		
		// Buang.
		memset(pesan_deco, 0, ENCRYPTED_CONTAINER_SIZE);
		
		// Pesan mentah.
		DEBUG4(_("Panjang terpecahkan: %1$i."), diterima);
		DEBUG5(_("Pesan mentah diterima terpecahkan"), tujuan_deco, 0, diterima);
		
		// Penugasan.
		memcpy(penyangga, tujuan_deco, MAX_CHUNK_SIZE);
		
		// Buang.
		memset(tujuan_deco, 0, MAX_CHUNK_SIZE+1);
	};
	
	// Membaca pesan.
	// Mendapatkan pengepala.
	ambil_pengepala(
		penyangga,
		&versi,
		&identifikasi,
		&panji,
		&paritas,
		&status_gerbang,
		&status_peladen
		);
	
	// Mendapatkan pesan.
	memcpy(pesan, ambil_pesan(penyangga), CHUNK_MESSAGE_SIZE);
	
	// Keterangan.
	DEBUG2(_("Versi: %1$i."),versi);
	DEBUG2(_("Panji: %1$s."),arti_panji(panji));
	DEBUG2(_("Identifikasi: %1$i."),identifikasi);
	
	// Memeriksa paritas.
	int cek_paritas;
	cek_paritas=check_parity(pesan, 0, CHUNK_MESSAGE_SIZE);
	if(cek_paritas!=paritas){
		status_gerbang=0;
		DEBUG1(_("Paritas TIDAK SAMA."), 0);
		DEBUG1(_("Paritas Klien adalah %1$i tetapi ditemukan %2$i."), paritas, cek_paritas);
	}else{
		status_gerbang=1;
		DEBUG2(_("Paritas sama (%1$i)"), paritas);
		
		// Pengirim. ----------------------------------------------------------
		// Perilaku.
		bool do_ulang=false;
		int coba=0;
		do{
			
			// Bangun pesan.
			memcpy(penyangga, buat_pesan(
				penyangga,
				identifikasi,
				&cek_paritas,
				pesan
			), MAX_CHUNK_SIZE);
			
			// Bangun pengepala.
			DEBUG2(_("Panji %1$s."), arti_panji(panji));
			memcpy(penyangga, buat_pengepala(
				penyangga,
				identifikasi,
				panji,
				paritas,
				status_gerbang,
				status_peladen
			), MAX_CHUNK_SIZE);
			
			// ============= Enkripsi  =======
			
			int panjang_penyangga;
			
			
			// Enkripsi.
			if(!aturan.rawtransfer){
				// Pesan mentah.
				DEBUG5(_("Pesan mentah dikirim"), penyangga, 0, MAX_CHUNK_SIZE);
				
				// Penyandian.
				memset(pesan_ency, 0, MAX_CHUNK_SIZE+1);
				memcpy(pesan_ency, penyangga, MAX_CHUNK_SIZE);
				memset(tujuan_ency, 0, ENCRYPTED_CONTAINER_SIZE+1);
				panjang_penyangga=rsa_encrypt(
					pesan_ency,
					MAX_CHUNK_SIZE+1,
					default_rsa_pubkey(),
					tujuan_ency,
					aturan.rsa_padding
				);
				
				// Pesan mentah.
				DEBUG5(_("Pesan mentah dikirim tersandikan"), tujuan_ency, 0, panjang_penyangga);
				
				// Salin.
				memcpy(penyangga, tujuan_ency, panjang_penyangga);
				
			}else{
				panjang_penyangga=MAX_CHUNK_SIZE;
			};
			
			// Kirim.
			INFO(_("Menghubungi Peladen."), 0);
			int panjang_diterima;
			char *kirim_data=kirimdata(
				penyangga,
				panjang_penyangga,
				hostname,
				portno,
				infoalamat,
				&panjang_diterima
			);
			
			// Bila terjadi kesalahan.
			if(kirim_data == NULL){
				// Pesan kesalahan.
				FAIL(_("Kegagalan %1$s."), _("Soket"));
				strcpy(pesan, "Gerbang");
				// Kirim status.
				status_peladen=0;
				panji=0;
				paritas=cek_paritas;
				
				// Alokasi ulang memori.
				memset(penyangga, 0, MAX_CHUNK_SIZE+1);
				
				// Berhenti.
				break;
			};
			
			// Salin.
			memcpy(penyangga, kirim_data, ENCRYPTED_CONTAINER_SIZE);
			
			// Pesan mentah.
			DEBUG4(_("Panjang pesan mentah diterima: %1$i"), panjang_penyangga);
			DEBUG5(_("Pesan mentah diterima"), penyangga, 0, panjang_penyangga);
			
			
			// ============= Dekripsi  =======
			
			// Dekripsi.
			if(!aturan.rawtransfer){
				// Pemecah sandi.
				memcpy(pesan_deco, penyangga, ENCRYPTED_CONTAINER_SIZE);
				memset(tujuan_deco, 0, MAX_CHUNK_SIZE+1);
				panjang_penyangga=rsa_decrypt(
					pesan_deco,
					panjang_diterima,
					default_rsa_privatekey(),
					tujuan_deco,
					aturan.rsa_padding
				);
				
				// Buang.
				memset(pesan_deco, 0, ENCRYPTED_CONTAINER_SIZE);
				
				// Periksa.
				// print_unsigned_array(tujuan_deco, 100);
				
				// Pesan mentah.
				DEBUG4(_("Panjang pesan mentah diterima terpecahkan: %1$i"), panjang_penyangga);
				DEBUG5(_("Pesan mentah diterima terpecahkan"), tujuan_deco, 0, panjang_penyangga);
				
				// Ubah.
				memcpy(penyangga, tujuan_deco, MAX_CHUNK_SIZE);
				
				// Buang.
				memset(tujuan_deco, 0, MAX_CHUNK_SIZE+1);
			};
			
			// Mendapatkan pengepala.
			// Respons.
			int r_versi;
			int r_panji;
			int r_paritas;
			int r_status_gerbang;
			int r_status_peladen;
			unsigned int r_identifikasi;
			ambil_pengepala(
				penyangga,
				&r_versi,
				&r_identifikasi,
				&r_panji,
				&r_paritas,
				&r_status_gerbang,
				&r_status_peladen
				);
			
			// Ambil pesan.
			memcpy(pesan, ambil_pesan(penyangga), CHUNK_MESSAGE_SIZE);
			
			// Balasan
			DEBUG2(_("Balasan: Versi: %1$i."), r_versi);
			DEBUG2(_("Balasan: Identifikasi: %1$i."), r_identifikasi);
			DEBUG2(_("Balasan: Panji: %1$s."), arti_panji(r_panji));
			DEBUG2(_("Balasan: Paritas: %1$i."), r_paritas);
			DEBUG2(_("Balasan: Status Gerbang: %1$s."), arti_status(r_status_gerbang));
			DEBUG2(_("Balasan: Status Peladen: %1$s."), arti_status(r_status_peladen));
			
			// Memeriksa hasil.
			// Bila status peladen adalah NOL.
			if(!r_status_peladen){
				status_peladen=0;
				
				// Memeriksa apakah
				// telah terlalu banyak melakukan percobaan.
				if(coba>maksimal_coba){
					WARN(_("Telah melakukan percobaan sebanyak %1$i kali. Berhenti."), maksimal_coba);
					status_peladen=0;
					panji=0;
					paritas=cek_paritas;
					break;
				}else{
					WARN(_("Gagal mengirim ke %1$s."),_("Peladen"));
				};
				
				// Panji Taksah.
				if(r_panji==INVALID_FLAG){
					// Pesan.
					NOTICE(_("Panji Taksah ditemukan."), 0);
					NOTICE(_("Menunggu %1$i detik untuk mengirim ulang."), ulang_tunggu);
					sleep(ulang_tunggu);
				}else if(r_panji==START_FLAG){
					// Meminta Klien untuk mengirim ulang
					// informasi berkas.
					
					// Pesan.
					NOTICE(_("Meminta pengiriman ulang informasi berkas."), 0);
					
					// Berhenti.
					status_peladen=0;
					panji=r_panji;
					paritas=r_paritas;
					identifikasi=r_identifikasi;
					do_ulang=false;
					break;
				}else if(r_panji==INTRANSFER_FLAG){
					// Meminta pengiriman ulang
					// berkas berdasarkan identifikasi.
					NOTICE(_("Meminta pengiriman ulang pecahan identifikasi '%1$i'."), r_identifikasi);
					
					status_peladen=0;
					panji=r_panji;
					paritas=r_paritas;
					identifikasi=r_identifikasi;
					do_ulang=false;
					break;
				};
				
				// Meningkat.
				coba++;
				do_ulang=true;
			}else{
				// Berhenti.
				status_peladen=1;
				panji=r_panji;
				paritas=r_paritas;
				identifikasi=r_identifikasi;
				do_ulang=false;
				
				// Pesan.
				INFO(_("Meneruskan ke Klien."), 0);
				break;
			};
			
		}while(do_ulang);
	};
	
	// Balasan. ---------------------------------------------------------------
	
	// Menulis balasan.
	// Pesan Kosong.
	memcpy(penyangga, buat_pesan(
		penyangga, identifikasi, &paritas, pesan
	), MAX_CHUNK_SIZE);
	
	// Pengepala.
	memcpy(penyangga, buat_pengepala(
		penyangga, identifikasi, panji, paritas,
		status_gerbang, status_peladen
	), MAX_CHUNK_SIZE);
	
		// Pesan.
	DEBUG5(_("Pesan mentah dikirim"), penyangga, 0, CHUNK_HEADER_SIZE);
	
	// ============= Enkripsi  =======
	int panjang_penyangga;
	if(!aturan.rawtransfer){
		memset(pesan_ency, 0, MAX_CHUNK_SIZE+1);
		memcpy(pesan_ency, penyangga, MAX_CHUNK_SIZE);
		memset(penyangga, 0, MAX_CHUNK_SIZE+1);
		
		// Pesan mentah.
		DEBUG5(_("Pesan mentah dikirim terenkripsi"), pesan_ency, 0, MAX_CHUNK_SIZE);
		
		// Enkripsi.
		memset(tujuan_ency, 0, ENCRYPTED_CONTAINER_SIZE+1);
		panjang_penyangga=rsa_encrypt(
			pesan_ency,
			MAX_CHUNK_SIZE,
			default_rsa_pubkey(),
			tujuan_ency,
			aturan.rsa_padding
		);
		
		// Pesan mentah.
		DEBUG4(_("Panjang pesan mentah dikirim tersandikan: %1$i."), panjang_penyangga);
		DEBUG5(_("Pesan mentah dikirim tersandikan"), tujuan_ency, 0, panjang_penyangga);
		
		// Salin.
		memcpy(penyangga, tujuan_ency, panjang_penyangga);
	}else{
		panjang_penyangga=MAX_CHUNK_SIZE;
	};
	
	// Balasan.
	if (!sendall(sock, penyangga, &panjang_penyangga)){
	// if (!sendall(sock, "peladen", &len)){
		FAIL(_("Kesalahan dalam menulis ke soket: %1$s (%2$i)."),strerror(errno), errno);
		exit(EXIT_FAILURE_SOCKET);
	};
	
	// Membuang isi pesan.
	// free(pesan);
	close(sock);
}