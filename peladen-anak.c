/*
 * `peladen-anak.c`
 * Sebagai anak peladen dari Kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */

#include "peladen.h"
/*
 * anak_tulis()
 * Proses yang menangani penulisan berkas.
 */
void anak_tulis(struct BERKAS *berkas){
	// Berkas.
	double dipotong=0;
	
	// Perilaku.
	int status;
	
	// Memasang status sibuk.
	berkas->sedang_sibuk=true;
	
	// Bila diterima lebih besar dari ukuran.
	if(berkas->diterima>berkas->ukuran){
		dipotong=berkas->diterima-berkas->ukuran;
		DEBUG1(
			_("Berkas diterima (%1$.0f bita) lebih besar dari ukuran (%2$.0f bita)."),
			berkas->diterima, berkas->ukuran
			);
		DEBUG2(_("Memotong berkas sebesar %1$.0f bita."), dipotong);
		
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
	
	// Pesan.
	DEBUG1(_("Menjalankan penulisan berkas '%1$s'."), berkas->nama);
	
	// Mulai melakukan penulisan.
	// Membangun lajur.
	char *berkas_lajur;
	berkas_lajur=malloc(0
		+strlen(aturan.tempdir)
		+strlen(berkas->nama)
		+sizeof(berkas_lajur)
		+3
		);
	strcpy(berkas_lajur,aturan.tempdir);
	// strcat(berkas_lajur,(char*)DIR_SEPARATOR);
	// strcpy(berkas_lajur,berkas->nama);
	
	TEST(berkas_lajur);
	sleep(60);
	// Membuka berkas.
	
	
	// Pesan.
	DEBUG1(_("Selesai menulis berkas '%1$s'."), berkas->nama);
	
	// Melepas status sibuk.
	berkas->sedang_sibuk=false;
	
	// Membersihkan.
	// free(berkas_lajur);
}

/*
 * anak_sambungan()
 * Proses yang menangani sambungan dari klien.
 */
void anak_sambungan (int sock, struct BERKAS *berkas){
	int n, status;
	pid_t pid;
	// pid_t result_waitpid;
	int diterima = 0;
	int ukuberkas_panjang=12;
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
	
	// Perilaku.
	int detik_tunggu_penulisan=10;
	int cek_paritas;
	bool do_ulang=false;
	
	// Mulai.
	do{
		// Memeriksa apakah sedang sibuk.
		if(!berkas->sedang_sibuk){
			// Tidak sibuk.
			
			// Menerima pesan.
			while(diterima < MAX_CHUNK_SIZE){
				n = recv(sock, penyangga+diterima, diterima-MAX_CHUNK_SIZE, 0);
				if (n < 0){
					FAIL(_("GALAT! Gagal membaca soket"), 0);
					exit(EXIT_FAILURE);
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
			cek_paritas=check_parity(pesan, 0, CHUNK_MESSAGE_SIZE);
			if(cek_paritas!=paritas){
				status_peladen=0;
				DEBUG1(_("Paritas TIDAK SAMA."), 0);
				DEBUG1(_("Paritas Klien adalah %1$i tetapi ditemukan %2$i."), paritas, cek_paritas);
			}else{
				status_peladen=1;
				DEBUG2(_("Paritas sama (%1$i)"), paritas);
				
				// Memeriksa apakah panji adalah Mulai.
				if(panji==START_FLAG){
					// Mendapatkan informasi berkas.
					char* berkas_id;
					char* berkas_nama;
					char* berkas_ukuran;
					
					berkas_id=malloc(sizeof(berkas_id)* (CHUNK_MESSAGE_SIZE/2));
					berkas_nama=malloc(sizeof(berkas_nama)* (CHUNK_MESSAGE_SIZE/2));
					berkas_ukuran=malloc(sizeof(berkas_ukuran)* (CHUNK_MESSAGE_SIZE/2));
					
					// memset(&berkas_id, 0, (CHUNK_MESSAGE_SIZE/2));
					// memset(&berkas_nama, 0, (CHUNK_MESSAGE_SIZE/2));
					// memset(&berkas_ukuran, 0, (CHUNK_MESSAGE_SIZE/2));
					
					// Ambil informasi.
					status=ambil_pesan_start(
						pesan,
						CHUNK_MESSAGE_SIZE,
						&berkas_id,
						&berkas_nama,
						&berkas_ukuran
					);
					
					// Apakah terjadi kesalahan.
					if(status>0){
						// Tidak menerima.
						DEBUG1(_("Tidak menerima pecahan %1$i (Status %2$i)."), identifikasi, status);
						status_peladen=0;
					}else{
						// Memasukkan informasi berkas.
						strncpy(berkas->identifikasi, berkas_id, 64);
						strncpy(berkas->nama,berkas_nama, 64);
						berkas->ukuran=strtod(berkas_ukuran, NULL);
						berkas->ofset=0;
						berkas->diterima=0;
						
						// Pesan.
						char *ukuberkas;
						ukuberkas=malloc(sizeof(char*)*ukuberkas_panjang);
						ukuberkas=readable_fs(berkas->ukuran, ukuberkas);
						INFO(
							_("Menerima berkas '%1$s': %2$s (%3$.0lf bita)."),
							berkas->nama,
							ukuberkas, berkas->ukuran
							);
						free(ukuberkas);
					}
					
					free(berkas_id);
					free(berkas_nama);
					free(berkas_ukuran);
				}else if(panji==INTRANSFER_FLAG){
					
					// Memeriksa apakah berkas telah diketahui.
					if(!strlen(berkas->nama) || berkas->ukuran <=0){
						// Kesalahan.
						WARN(_("Nama dan ukuran berkas tidak diketahui."), 0);
						NOTICE(_("Meminta Klien mengulang pengiriman."), 0);
						
						// Meminta mengulang ke identifikasi 0.
						identifikasi=0;
						panji=START_FLAG;
						status_peladen=0;
						
					}else if(!strlen(berkas->nama)){
						// Kesalahan.
						WARN(_("Nama berkas tidak diketahui."), 0);
						NOTICE(_("Meminta Klien mengulang pengiriman."), 0);
						
						// Meminta mengulang ke identifikasi 0.
						identifikasi=0;
						panji=START_FLAG;
						status_peladen=0;
						
					}else if(berkas->ukuran <=0){
						// Kesalahan.
						WARN(_("Ukuran berkas tidak diketahui."), 0);
						NOTICE(_("Meminta Klien mengulang pengiriman."), 0);
						
						// Meminta mengulang ke identifikasi 0.
						identifikasi=0;
						
					}else{
						// Tidak ada masalah.
						
						// Menambahkan yang terkirim.
						berkas->diterima=0
							+((double)(berkas->diterima))
							+((double)diterima)
							-((double)CHUNK_HEADER_SIZE);
						
						// Mendapat informasi.
						double br_diterima=(berkas->diterima)+berkas->ofset;
						double br_ukuran=berkas->ukuran;
						float persen_selesai=(float)br_diterima/(float)br_ukuran*100;
						
						// Mempersiapkan tampilan ukuran.
						char *ukuberkas_diterima;
						ukuberkas_diterima=malloc(sizeof(char*)*ukuberkas_panjang);
						ukuberkas_diterima=readable_fs(br_diterima, ukuberkas_diterima);
						
						char *ukuberkas_ukuran;
						ukuberkas_ukuran=malloc(sizeof(char*)*ukuberkas_panjang);
						ukuberkas_ukuran=readable_fs(br_ukuran, ukuberkas_ukuran);
						
						// Pesan.
						INFO(
							_("Menerima berkas '%1$s' (%2$.2f%%)."),
								berkas->nama, persen_selesai
							);
						DEBUG1(
							_("Menerima %1$s/%2$s (%3$.0lf/%4$.0lf bita)."),
								ukuberkas_diterima, ukuberkas_ukuran,
								br_diterima, br_ukuran
							);
						
						// Membersihkan.
						free(ukuberkas_diterima);
						free(ukuberkas_ukuran);
						
						// printf("Pesan:\n");
						// print_char(pesan, CHUNK_MESSAGE_SIZE);
						// printf("\n");
					};
				// Bila berhenti.
				}else if(panji==STOP_FLAG){
					NOTICE(_("Mendapatkan sinyal akhir kelompok pecahan."), 0);
					// Perkembangan.
					double br_diterima=(berkas->diterima)+berkas->ofset;
					double br_ukuran=berkas->ukuran;
					tampil_info_progres_berkas(
						PROGRES_TERIMA, berkas->nama,
						br_diterima, br_ukuran,
						ukuberkas_panjang
					);
					
					// Memanggil proses juru tulis.
					pid = fork();
					if (pid < 0){
						FAIL(_("Kegagalan proses cabang: %1$s (%2$i)."), strerror(errno), errno);
						exit(EXIT_FAILURE_FORK);
					}else{
						if (pid == 0){
							// Proses anak.
							anak_tulis(berkas);
							exit(0);
						}else{
							// Proses bapak.
							berkas->pid_tulis=pid;
						};
					};
				};
			};
			
			// Jangan diulangi.
			do_ulang=false;
		}else{
			WARN(_("Sambungan klien ditunda."), 0);
			if(berkas->pid_tulis){
				
				// bool tunggupid=false;
				// do{
					if(kill(berkas->pid_tulis, 0) == 0){
						DEBUG1(_("Proses cabang sedang berlangsung."), 0);
						berkas->sedang_sibuk=true;
						
						// Pesan.
						NOTICE(_("Menunggu penulisan berkas '%1$s' selama %2$i detik."),
							berkas->nama, detik_tunggu_penulisan);
						// Menunggu TIGA detik.
						// Periksa kembali
						// apakah masih sibuk.
						sleep(3);
						if(!berkas->sedang_sibuk){
							berkas->sedang_sibuk=false;
							// tunggupid=true;
							// break;
						};
					}else if(errno==ESRCH){
						WARN(_("Kegagalan proses cabang (%1$i): %2$s (%3$i)."), berkas->pid_tulis, strerror(errno), errno);
						berkas->sedang_sibuk=false;
						// tunggupid=false;
					}else{
						WARN(_("Kegagalan proses cabang (%1$i): %2$s (%3$i)."), berkas->pid_tulis, strerror(errno), errno);
						berkas->sedang_sibuk=false;
						// tunggupid=false;
					};
				// }while(tunggupid);
				
				
				/*
				do {
					result_waitpid  = waitpid(berkas->pid_tulis, &status, WUNTRACED | WCONTINUED);
					if (result_waitpid  == -1) {
						// Pesan.
						WARN(_("Kegagalan proses cabang (%1$i): %2$s (%3$i)."), berkas->pid_tulis, strerror(errno), errno);
						
						// Pesan.
						NOTICE(_("Menunggu penulisan berkas '%1$s' selama %2$i detik."),
							berkas->nama, detik_tunggu_penulisan);
						// Menunggu TIGA detik.
						// Bila masih sibuk,
						// maka memberi pesan
						// dengan status_peladen NOL.
						sleep(3);
						berkas->sedang_sibuk=false;
						break;
					}else{
						// Telah selesai.
						berkas->pid_tulis=0;
					};
					
					// Status anak.
					if (WIFEXITED(status)) {
						DEBUG1(_("Proses cabang selesai. Status pengawas: %1$i."), WEXITSTATUS(status));
						berkas->sedang_sibuk=false;
					} else if (WIFSIGNALED(status)) {
						DEBUG1(_("Proses cabang terbunuh sinyal %1$i."), WTERMSIG(status));
						berkas->sedang_sibuk=false;
					} else if (WIFSTOPPED(status)) {
						DEBUG1(_("Proses cabang dihentikan sinyal %1$i."), WSTOPSIG(status));
					} else if (WIFCONTINUED(status)) {
						DEBUG1(_("Proses cabang sedang berlangsung."), 0);
						berkas->sedang_sibuk=true;
					}
				} while (!WIFEXITED(status) && !WIFSIGNALED(status));
				*/
				// Selesai.
			}else{
				// PID tidak ditemukan.
				// berkas->sedang_sibuk=false;
				
				NOTICE(_("Menunggu penulisan berkas '%1$s' selama %2$i detik."),
					berkas->nama, detik_tunggu_penulisan);
				// Menunggu TIGA detik.
				// Bila masih sibuk,
				// maka memberi pesan
				// dengan status_peladen NOL.
				sleep(detik_tunggu_penulisan);
			}
			
			// Bila masih sibuk.
			if(!berkas->sedang_sibuk){
				do_ulang=true;
			}else{
				NOTICE(_("Terlalu lama menunggu penulisan."), 0);
				WARN(_("Sambungan klien ditolak."), 0);
				do_ulang=false;
				identifikasi=0;
				panji=INVALID_FLAG;
				cek_paritas=0;
				status_peladen=0;
			};
		};
	}while(do_ulang);
	
	// printf("\n");
	// print_char_csv(pesan, 0, CHUNK_MESSAGE_SIZE);
	// printf("\n");
	
	// printf("Pesan:\n");
	// print_array(berkas.nama, 20);
	// printf("\n");
	
	// Informasi berkas.
	
	// Menulis balasan.
	// Pesan Kosong.
	penyangga=buat_pesan(
		penyangga, identifikasi, &paritas, "peladen");
	
	// Pengepala.
	penyangga=buat_pengepala(
		penyangga, identifikasi, panji, cek_paritas,
		1, status_peladen);
		
	// Balasan.
	int len=MAX_CHUNK_SIZE;
	if (!sendall(sock, penyangga, &len)){
	// if (!sendall(sock, "peladen", &len)){
		FAIL(_("Kesalahan dalam menulis ke soket: %1$s (%2$i)."),strerror(errno), errno);
		exit(EXIT_FAILURE_SOCKET);
	};
	
	// Membuang isi pesan.
	free(pesan);
	close(sock);
}
