/* 
 * `kancil.h`
 * Sebagai klien dari kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt 
 */

#include "klien.h"

unsigned int anak_kirim(
	unsigned int identifikasi,
	FILE *pberkas,
	struct KIRIMBERKAS *kirim,
	struct INFOALAMAT *infoalamat,
	int ukuberkas_panjang
){
	char pesan[CHUNK_MESSAGE_SIZE];
	char pecahan[ENCRYPTED_CONTAINER_SIZE];
	size_t panjang_pesan=0;
	
	// Perilaku.
	int status=0;;
	int maksimal_coba=aturan.tries;
	int ulang_tunggu=aturan.waitretry;
	// int urut_maksimal=aturan.maxqueue;
	int urut_tunggu=aturan.waitqueue;
	// bool urut_jangan_tunggu=true;
	unsigned int kelompok_kirim=kirim->kelompok_kirim;
	char *nama_berkas=kirim->berkas;
	
	// Identifikasi.
	bool identifikasi_awal=true;
	unsigned int identifikasi_sebelumnya=kirim->identifikasi_sebelumnya;
	
	// Pengepala Pecahan.
	int paritas=0;
	int panji=0;
	int status_gerbang=0;
	int status_peladen=0;
	
	// Tujuan.
	char *hostname=kirim->hostname;
	char *portno=kirim->portno;
	
	// Berurutan kirim.
	kirim->urut_kali++;
	
	// Membersihkan isi.
	memset(pesan, 0, CHUNK_MESSAGE_SIZE);
	memset(pecahan, 0, MAX_CHUNK_SIZE);
	
	// Bila identifikasi
	// lebih dari MAX_CHUNK_ID,
	// maka identifikasi adalah NOL.
	if(identifikasi>MAX_CHUNK_ID){
		DEBUG1(_("Telah melebihi maksimal identifikasi %1$i."), MAX_CHUNK_ID);
		DEBUG1(_("Nilai identifikasi adalah %1$i."), identifikasi);
		
		// Ke NOL.
		identifikasi=0;
		DEBUG1(_("Mengubah identifikasi ke nilai 0."), 0);
		
		// Keluar.
		DEBUG1(_("Tidak melanjutkan pengiriman."), 0);
		return identifikasi;
	};
	
	// Apakah pertama.
	if(kirim->paksa_panji!=UNDEFINED_FLAG){
		// Memaksa panji.
		panji=kirim->paksa_panji;
		kirim->paksa_panji=UNDEFINED_FLAG;
		
		// Pesan.
		DEBUG1(_("Memaksa panji '%1$s'."),arti_panji(panji));
		
		// Langsung berhenti bila panji STOP.
		if(panji==STOP_FLAG){
			
			// Mengirim panji STOP
			// dan keluar dari perulangan.
			DEBUG1(_("Menghentikan pengulangan pengiriman."), 0);
			kirim->do_kirim=false;
		};
		
	}else if(identifikasi==0){
		// Panji.
		panji=START_FLAG;
		
		// Memastikan nama berkas ada.
		if(!strlen(nama_berkas)){
			// Kesalahan.
			FAIL(_("Gagal memperoleh nama berkas."), 0);
			exit(EXIT_FAILURE_MEMORY);
		};
		
		// Pecahan pertama adalah
		// pecahan dengan informasi berkas.
		memcpy(pesan, buat_pesan_start(
			pesan,
			CHUNK_MESSAGE_SIZE,
			kirim->berkas_identifikasi,
			nama_berkas,
			kirim->ukuran_berkas
			), CHUNK_MESSAGE_SIZE);
	// Apakah hampir akhir.
	}else if(identifikasi==MAX_CHUNK_ID){
		// Panji.
		DEBUG1(_("Mengirim panji '%1$s'."),_("Henti"));
		panji=STOP_FLAG;
	/*
	}else if (kirim->ukuran_kirim>kirim->ukuran_berkas){
		// Panji STOP.
		DEBUG1(_("Mengirim panji '%1$s'."), _("Henti"));
		panji=STOP_FLAG;
		
		// Hentikan perulangan.
		// Keluar dari perulangan.
		DEBUG1(_("Menghentikan pengulangan pengiriman."), 0);
		kirim->do_kirim=false;
		*/
	}else{
		// Panji.
		panji=INTRANSFER_FLAG;
		
		// Mendapatkan posisi berkas.
		long int penunjuk_berkas=0
			+((long)(kelompok_kirim-1)*(MAX_CHUNK_ID-1)*CHUNK_MESSAGE_SIZE)
			+((long)identifikasi-1)*(long)CHUNK_MESSAGE_SIZE;
		
		/*
		// Panjang baca.
		int panjang_akhir=(int)((0
			+ (double)penunjuk_berkas
			- ((double)kirim->ukuran_berkas+(double)CHUNK_MESSAGE_SIZE)
			)*(double)-1);
		
		if( panjang_akhir>=0 && panjang_akhir<CHUNK_MESSAGE_SIZE){
			DEBUG3(_("Menggunakan panjang akhir (%1$i bita)."), panjang_akhir);
			panjang_baca=(size_t)panjang_akhir;
		}else{
			DEBUG3(_("Membaca sejumlah panjang pesan pecahan (%1$li bita)."), (long)CHUNK_MESSAGE_SIZE);
			panjang_baca=(size_t)CHUNK_MESSAGE_SIZE;
		}
		*/
		
		// Bila mendekati dan pecahan transfer terakhir,
		// maka penunjuk merupakan
		// panjang berkas dikurangi CHUNK_MESSAGE_SIZE.
		int panjang_geser_akhir=0;
		size_t panjang_baca=CHUNK_MESSAGE_SIZE;
		if (
			kirim->ukuran_kirim + (double)CHUNK_MESSAGE_SIZE
				>= kirim->ukuran_berkas
			&& kirim->ukuran_berkas - kirim->ukuran_kirim
				<= (double)CHUNK_MESSAGE_SIZE
		){
			// Pesan.
			DEBUG3(_("Merupakan pecahan akhir."), 0);
			
			// Jumlah geser.
			panjang_geser_akhir=(int)0
				+((double)CHUNK_MESSAGE_SIZE
				-(kirim->ukuran_berkas-kirim->ukuran_kirim));
			
			// panjang_geser_akhir=(int)((0
				// + (double)penunjuk_berkas
				// - ((double)kirim->ukuran_berkas+(double)CHUNK_MESSAGE_SIZE)
				// )*(double)-1);
			
			// Mengubah penunjuk berkas.
			penunjuk_berkas=kirim->ukuran_berkas-CHUNK_MESSAGE_SIZE;
			
			// Panjang baca.
			panjang_baca=CHUNK_MESSAGE_SIZE;
		}else{
			
			// Jumlah geser.
			panjang_geser_akhir=0;
			
			// Panjang baca.
			panjang_baca=CHUNK_MESSAGE_SIZE;
		};
		
		
		// Menggeser penunjuk berkas.
		status=fseek(pberkas, penunjuk_berkas, SEEK_SET);
		
		// Memeriksa posisi.
		if(status<0){
			FAIL(
				_("Gagal memindah posisi penunjuk berkas '%1$s': %2$s (%3$i)."),
				nama_berkas, strerror(errno), errno
			);
			exit(EXIT_FAILURE_IO);
		}else{
			long posisi_sekarang=ftell(pberkas);
			if(posisi_sekarang<0){
				FAIL(
					_("Gagal mendapatkan posisi penunjuk berkas '%1$s': %2$s (%3$i)."),
					nama_berkas, strerror(errno), errno
				);
				exit(EXIT_FAILURE_IO);
			}else if(posisi_sekarang!=penunjuk_berkas){
				FAIL(
					_("Posisi penunjuk berkas '%1$s' (%2$li) tidak sesuai. Diharapkan: %2$li."),
					nama_berkas, posisi_sekarang, errno
				);
				exit(EXIT_FAILURE_IO);
			}else{
				// Berhasil.
				if(kelompok_kirim>1){
					DEBUG3(
						_("Mulai membaca di bita %1$i sebesar %2$lu bita untuk pesan %3$i kelompok %4$i."),
						penunjuk_berkas, (long unsigned int) panjang_baca, identifikasi, kelompok_kirim
					);
				}else{
					DEBUG3(
						_("Mulai membaca di bita %1$i sebesar %2$lu bita untuk pesan %3$i."),
						penunjuk_berkas, (long unsigned int) panjang_baca, identifikasi
					);
				};
			};
		};
		
		// Baca berkas untuk pesan.
		panjang_pesan=fread(pesan, 1, CHUNK_MESSAGE_SIZE, pberkas);
		
		// Menambah bita terkirim.
		if(identifikasi_awal ||
			identifikasi!=identifikasi_sebelumnya){
			// Menambah.
			identifikasi_awal=false;
			// kirim->ukuran_kirim+=(double)panjang_pesan;
			identifikasi_sebelumnya=identifikasi;
			kirim->identifikasi_sebelumnya=identifikasi;
		};
		
		// Menggeser.
		if(
			panjang_geser_akhir>0
			&& panjang_geser_akhir <= CHUNK_MESSAGE_SIZE
		){
			// Membuat sementara.
			char pesan_tmp[CHUNK_MESSAGE_SIZE+1];
			memset(pesan_tmp, 0, CHUNK_MESSAGE_SIZE+1);
			
			// Mengeser.
			panjang_pesan=CHUNK_MESSAGE_SIZE-panjang_geser_akhir;
			memmove(pesan_tmp+0, pesan+panjang_geser_akhir, panjang_pesan);
			
			// Mengembalikan.
			memset(pesan, 0, CHUNK_MESSAGE_SIZE);
			memcpy(pesan, pesan_tmp, panjang_pesan);
			
			// Membersihkan.
			memset(pesan_tmp, 0, CHUNK_MESSAGE_SIZE+1);
			
			// Pesan.
			DEBUG3(
				_("Menggeser pesan sebanyak %1$i bita."),
				panjang_geser_akhir
			);
			DEBUG3(
				_("Mulai dibaca di bita %1$i sebesar %2$lu bita"),
				penunjuk_berkas+panjang_geser_akhir, panjang_pesan
			);
		};
		
		// Bila telah selesai.
		if (!panjang_pesan){
			DEBUG3(_("Panjang pesan akhir adalah %1$i bita"), panjang_pesan);
			if(feof(pberkas)!=0){
				// Selesai kirim.
				char penyangga_feof[ukuberkas_panjang];
				INFO(
					_("Berkas '%1$s' dengan ukuran %2$s (%3$.0lf bita) telah selesai dikirim."),
					nama_berkas, readable_fs(kirim->ukuran_berkas, penyangga_feof), kirim->ukuran_berkas
					);
				memset(penyangga_feof, 0, ukuberkas_panjang);
				clearerr(pberkas);
			}else if(ferror(pberkas)!=0){
				// Kesalahan.
				FAIL(_("Gagal membaca berkas '%1$s': %2$s (%3$i)."), nama_berkas, strerror(errno), errno);
				clearerr(pberkas);
				exit(EXIT_FAILURE_IO);
			}else{
				// Kesalahan.
				WARN(_("Kesalahan berkas yang tidak diketahui: %1$s (%2$i)."), strerror(errno), errno);
			};
			
			
			// Pesan.
			DEBUG1(_("Mengirim panji '%1$s'."),_("Henti"));
			panji=STOP_FLAG;
			
			// Mengirim panji STOP
			// dan keluar dari perulangan.
			DEBUG1(_("Menghentikan pengulangan pengiriman."), 0);
			kirim->do_kirim=false;
		};
	};
	
	// Bangun pesan.
	memcpy(pecahan, buat_pesan(
		pecahan,
		identifikasi,
		&paritas,
		pesan
	), MAX_CHUNK_SIZE);
	
	// Bangun pengepala.
	DEBUG2(_("Panji %1$s."), arti_panji(panji));
	memcpy(pecahan, buat_pengepala(
		pecahan,
		identifikasi,
		panji,
		paritas,
		status_gerbang,
		status_peladen
	), MAX_CHUNK_SIZE);
	
	// Pesan.
	if(kelompok_kirim>1){
		DEBUG1(
			_("Mengirim pesan %1$i kelompok %2$i."),
			identifikasi, kelompok_kirim);
	}else{
		DEBUG1(
			_("Mengirim pesan %1$i."),
			identifikasi);
	};
	
	// ============= Enkripsi  =======
	int panjang_pecahan;
	// Enkripsi.
	if(!aturan.rawtransfer){
	
		// Pesan mentah.
		DEBUG5(_("Pesan mentah dikirim"), pecahan, 0, MAX_CHUNK_SIZE);
		
		// Penyandian.
		unsigned char pesan_ency[MAX_CHUNK_SIZE+1];
		memcpy(pesan_ency, pecahan, MAX_CHUNK_SIZE);
		unsigned char tujuan_ency[ENCRYPTED_CONTAINER_SIZE+1];
		panjang_pecahan=rsa_encrypt(
			pesan_ency,
			MAX_CHUNK_SIZE+1,
			default_rsa_pubkey(),
			tujuan_ency,
			RSA_PKCS1_OAEP_PADDING
		);
		
		// Bersihkan.
		memset(pesan_ency, 0, MAX_CHUNK_SIZE);
		
		// Pesan mentah.
		DEBUG5(_("Pesan mentah dikirim tersandikan"), tujuan_ency, 0, MAX_CHUNK_SIZE);
		
		// Salin.
		memcpy(pecahan, tujuan_ency, panjang_pecahan);
	}else{
		panjang_pecahan=MAX_CHUNK_SIZE;
	};
	// ============ /Enkripsi  =======
	
	// Kirim.
	int panjang_diterima;
	char *kirim_data=kirimdata(
		pecahan,
		panjang_pecahan,
		hostname,
		portno,
		infoalamat,
		&panjang_diterima
	);
	
	// Bila kosong.
	if(kirim_data == NULL){
		// Pesan kesalahan.
		FAIL(_("Kegagalan %1$s."), _("Soket"));
		
		// Keluar.
		exit(EXIT_FAILURE_SOCKET);
	};
	
	memcpy(pecahan, kirim_data, ENCRYPTED_CONTAINER_SIZE);
	
	// Pesan mentah.
	DEBUG4(_("Panjang pesan mentah diterima: %1$i"), panjang_pecahan);
	DEBUG5(_("Pesan mentah diterima"), pecahan, 0, panjang_pecahan);
	
	// ============= Dekripsi  =======
	
	if(!aturan.rawtransfer){
		// Pemecah sandi.
		unsigned char pesan_deco[ENCRYPTED_CONTAINER_SIZE+1];
		memcpy(pesan_deco, pecahan, ENCRYPTED_CONTAINER_SIZE);
		unsigned char tujuan_deco[MAX_CHUNK_SIZE+1];
		panjang_pecahan=rsa_decrypt(
			pesan_deco,
			panjang_diterima,
			default_rsa_privatekey(),
			tujuan_deco,
			RSA_PKCS1_OAEP_PADDING
		);
		
		// Periksa.
		// print_unsigned_array(tujuan_deco, 100);
		
		// Bersihkan.
		memset(pesan_deco, 0, MAX_CHUNK_SIZE);
		
		// Pesan mentah.
		DEBUG4(_("Panjang pesan mentah diterima terpecahkan: %1$i"), panjang_pecahan);
		DEBUG5(_("Pesan mentah diterima terpecahkan"), tujuan_deco, 0, panjang_pecahan);
		
		// Salin.
		memcpy(pecahan, tujuan_deco, MAX_CHUNK_SIZE);
		
	};
	
	// Mendapatkan pesan.
	memset(pesan, 0, CHUNK_MESSAGE_SIZE);
	memcpy(pesan, ambil_pesan(pecahan), CHUNK_MESSAGE_SIZE);
	// ============ /Dekripsi  =======
	
	// Periksa.
	// print_unsigned_array(tujuan_deco, 100);
	
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
	
	// Mendapatkan pesan.
	memcpy(pesan, ambil_pesan(pecahan), CHUNK_MESSAGE_SIZE);
	
	// Mendapatkan informasi peladen.
	char* r_berkas_id;
	char* berkas_ukuran_str;
	char* berkas_diterima_str;
	char* unix_time_str;
	
	// Alikasi memori.
	r_berkas_id=malloc(sizeof(r_berkas_id)* (CHUNK_MESSAGE_SIZE/2));
	berkas_ukuran_str=malloc(sizeof(berkas_ukuran_str)* (CHUNK_MESSAGE_SIZE/2));
	berkas_diterima_str=malloc(sizeof(berkas_diterima_str)* (CHUNK_MESSAGE_SIZE/2));
	unix_time_str=malloc(sizeof(unix_time_str)* (CHUNK_MESSAGE_SIZE/2));
	
	// Ambil informasi.
	status=ambil_pesan_peladen(
		pesan,
		CHUNK_MESSAGE_SIZE,
		&r_berkas_id,
		&berkas_ukuran_str,
		&berkas_diterima_str,
		&unix_time_str
	);
	
	// Ubah nilai.
	double r_berkas_ukuran=0;
	double r_berkas_diterima=0;
	double r_unixtime=0;
	
	if(status>0){
		// Tidak menerima.
		DEBUG1(_("Tidak memahami Peladen di pecahan %1$i (Status %2$i)."), r_identifikasi, status);
	}else{
		// Ubah nilai.
		r_berkas_ukuran=strtod(berkas_ukuran_str, NULL);
		r_berkas_diterima=strtod(berkas_diterima_str, NULL);
		r_unixtime=strtod(unix_time_str, NULL);
	};
	
	// Balasan
	DEBUG2(_("Balasan: Versi: %1$i."), r_versi);
	DEBUG2(_("Balasan: Identifikasi: %1$i."), r_identifikasi);
	DEBUG2(_("Balasan: Panji: %1$s."), arti_panji(r_panji));
	DEBUG2(_("Balasan: Paritas: %1$i."), r_paritas);
	DEBUG2(_("Balasan: Status Gerbang: %1$s."), arti_status(r_status_gerbang));
	DEBUG2(_("Balasan: Status Peladen: %1$s."), arti_status(r_status_peladen));
	DEBUG2(_("Balasan: Identifikasi berkas: %1$s."), r_berkas_id);
	DEBUG2(_("Balasan: Ukuran berkas: %1$.0f."), r_berkas_ukuran);
	DEBUG2(_("Balasan: Berkas diterima: %1$.0f."), r_berkas_diterima);
	DEBUG2(_("Balasan: Waktu Peladen: %1$.06f."), r_unixtime);
	
	
	// Menyimpan.
	if(r_berkas_diterima<=0){
		DEBUG2(_("Berkas terterima berukuran %1$.0f bita. %2$s"), r_berkas_diterima, berkas_diterima_str);
	}else{
		kirim->ukuran_kirim=r_berkas_diterima;
	};
	
	// Bila berkas yang diterima
	// telah sama atau lebih besar dari ukuran.
	if(r_berkas_diterima>=kirim->ukuran_berkas){
		// Pesan.
		if(
			(r_berkas_diterima-kirim->ukuran_berkas) > (double)CHUNK_MESSAGE_SIZE * (aturan.parallel)
		){
			WARN(_("Peladen telah menerima berkas melebihi %1$.0f bita."), r_berkas_diterima-kirim->ukuran_berkas);
		}else if(r_berkas_diterima>kirim->ukuran_berkas){
			DEBUG1(_("Peladen telah menerima berkas melebihi %1$.0f bita."), r_berkas_diterima-kirim->ukuran_berkas);
		}else{
			DEBUG1(_("Peladen telah menerima keseluruhan berkas."), 0);
		};
		
		// Ubah nilai.
		kirim->ukuran_kirim=r_berkas_diterima;
		kirim->paksa_panji=STOP_FLAG;
		
		// Keluar.
		return identifikasi;
	};
	
	
	
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
			// Mengunlang pengiriman
			// informasi berkas.
			int tunggu=ulang_tunggu;
			NOTICE(_("Meminta pengiriman ulang informasi berkas."), 0);
			NOTICE(_("Menunggu %1$i detik untuk mengirim ulang."), tunggu);
			sleep(tunggu);
			
			// Mengatur ulang ukuran berkas terkirim.
			kirim->ukuran_kirim=0;
		}else if(r_panji==INTRANSFER_FLAG){
			// Meminta pengiriman ulang
			// berkas berdasarkan identifikasi.
			NOTICE(_("Meminta pengiriman ulang pecahan identifikasi '%1$i'."), r_identifikasi);
			
			// Mengatur ulang ukuran berkas terkirim
			// sesuai dengan ukuran peladen.
			kirim->ukuran_kirim=r_berkas_diterima;
		};
		
		// Mengirim ulang.
		NOTICE(_("Percobaan ke-%1$i. Mengulangi pengiriman pecahan %2$i."),kirim->coba, r_identifikasi);
		identifikasi=r_identifikasi;
		
		// Menambah
		// percobaan pengiriman.
		kirim->coba++;
	}else{
		
		/*
		// Bila ukuran terterima Peladen
		// lebih besar dari pada terketahui Klien.
		if(r_berkas_diterima> kirim->ukuran_kirim){
			// Pesan.
			DEBUG2(
				_("Peladen telah menerima berkas melebihi %1$.0f bita dari yang terkirim."),
				r_berkas_diterima-(kirim->ukuran_berkas)
			);
			
			// Ubah isi.
			kirim->ukuran_kirim=r_berkas_diterima;
		};
		*/
		
		// Berhasil.
		DEBUG2(_("Berhasil mengirim ke Gerbang dan Peladen."), 0);
		
		// Menambahkan.
		identifikasi++;
		DEBUG2(_("Menambah identifikasi menjadi %1$i."), identifikasi);
		
		// Bila belum selesai.
		if(kirim->ukuran_kirim<=kirim->ukuran_berkas){
			
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
				DEBUG1(_("Menunggu %1$i detik untuk melanjutkan."), urut_tunggu);
				DEBUG1(_("Kelompok pecahan selanjutnya adalah '%1$i'."), kelompok_kirim);
				
				// Tunggu.
				sleep(urut_tunggu);
				
				// Mengulangi identifikasi.
				identifikasi=0;
			};
		}else{
			// DEBUG1(_("Menghentikan pengulangan pengiriman."), 0);
			// kirim->do_kirim=false;
		};
		kirim->coba=1;
	};
	
	/*
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
	*/
	
	// Memastikan nilai kelompok benar.
	kirim->kelompok_kirim=kelompok_kirim;
	
	// Mengembalikan nilai identifikasi.
	return identifikasi;
			// kirim->identifikasi=identifikasi;
	
	// Mengembalikan data.
	// kirim->berkas=berkas;
	
	// if(identifikasi>3)
		// kirim->do_kirim=false;
}