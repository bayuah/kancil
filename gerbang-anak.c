/*
 * `gerbang-anak.c`
 * Sebagai gerbang dari kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
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
	char *penyangga;
	penyangga=malloc((sizeof penyangga) * MAX_CHUNK_SIZE );
	set_null(&penyangga, MAX_CHUNK_SIZE);
	
	// Pengepala.
	int versi;
	int panji;
	int paritas;
	int status_gerbang;
	int status_peladen;
	unsigned int identifikasi;
	
	// Pesan.
	char *pesan;
	pesan=malloc((sizeof pesan) * CHUNK_MESSAGE_SIZE);
	
	// Inisiasi.
	char *hostname;
	char *portno;
	hostname=malloc((sizeof hostname) * KIRIMBERKAS_MAX_STR);
	portno=malloc((sizeof portno) * KIRIMBERKAS_MAX_STR);
	
	// Teruskan.
	strncpy(hostname, kirim->hostname, KIRIMBERKAS_MAX_STR);
	strncpy(portno, kirim->portno, KIRIMBERKAS_MAX_STR);
	
	// Penerima. --------------------------------------------------------------
	
	// Menerima pesan.
	while(diterima < MAX_CHUNK_SIZE){
		n = recv(sock, penyangga+diterima, diterima-MAX_CHUNK_SIZE, 0);
		if (n < 0){
			WARN(_("Gagal membaca soket"), 0);
			free(penyangga);
			return;
		};
		diterima+=n;
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
	pesan=ambil_pesan(penyangga);
	
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
			penyangga=buat_pesan(
				penyangga,
				identifikasi,
				&cek_paritas,
				pesan);
			
			// Bangun pengepala.
			DEBUG2(_("Panji %1$s."), arti_panji(panji));
			penyangga=buat_pengepala(
				penyangga,
				identifikasi,
				panji,
				paritas,
				status_gerbang,
				status_peladen
			);
			// Kirim.
			INFO(_("Menghubungi Peladen."), 0);
			penyangga=kirimdata(
				penyangga,
				hostname,
				portno,
				infoalamat
				);
			
			// Bila terjadi kesalahan.
			if(penyangga == NULL){
				// Pesan kesalahan.
				FAIL(_("Kegagalan %1$s."), _("Soket"));
				pesan="Gerbang";
				// Kirim status.
				status_peladen=0;
				panji=0;
				paritas=cek_paritas;
				
				// Alokasi ulang memori.
				penyangga=malloc((sizeof penyangga) * MAX_CHUNK_SIZE );
				
				// Berhenti.
				break;
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
	penyangga=buat_pesan(
		penyangga, identifikasi, &paritas, pesan);
	
	// Pengepala.
	penyangga=buat_pengepala(
		penyangga, identifikasi, panji, paritas,
		status_gerbang, status_peladen);
		
	// Balasan.
	int len=MAX_CHUNK_SIZE;
	if (!sendall(sock, penyangga, &len)){
	// if (!sendall(sock, "peladen", &len)){
		FAIL(_("Kesalahan dalam menulis ke soket: %1$s (%2$i)."),strerror(errno), errno);
		exit(EXIT_FAILURE_SOCKET);
	};
	
	// Membuang isi pesan.
	// free(pesan);
	close(sock);
}