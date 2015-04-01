/*
 * `peladen-anak.c`
 * Sebagai anak peladen dari Kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014 - 2015
 * Lisensi: lihat LICENCE.txt
 */

#include "peladen.h"

/*
 * anak_sambungan()
 * Proses yang menangani sambungan dari klien.
 */
void anak_sambungan (
	int sock,
	struct TERIMABERKAS *berkas,
	RSA *rsapub,
	RSA *rsapriv
){
	int n, status;
	pid_t pid;
	// pid_t result_waitpid;
	int diterima = 0;
	int ukuberkas_panjang=12;
	char penyangga[ENCRYPTED_CONTAINER_SIZE];
	memset(penyangga, 0, ENCRYPTED_CONTAINER_SIZE);
	
	// Pengepala.
	int versi;
	int panji;
	int paritas;
	int status_gerbang;
	int status_peladen;
	unsigned int identifikasi;
	
	// Pesan.
	char pesan[CHUNK_MESSAGE_SIZE];
	memset(pesan, 0, CHUNK_MESSAGE_SIZE);
	
	// Perilaku.
	int detik_tunggu_penulisan=5;
	int cek_paritas;
	bool do_ulang=false;
	
	// Mulai.
	do{
		// Simpan waktu.
		infokancil.unixtime=current_time(CURRENTTIME_MICROSECONDS);
		
		// Memeriksa apakah sedang sibuk.
		if(!berkas->sedang_sibuk){
			// Tidak sibuk.
			
			// Menerima pesan.
			while(diterima < CHUNK_MESSAGE_SIZE){
				n = recv(sock, penyangga+diterima, diterima-CHUNK_MESSAGE_SIZE, 0);
				if (n < 0){
					FAIL(_("Gagal membaca soket"), 0);
					exit(EXIT_FAILURE);
				};
				diterima+=n;
			};
			
			// Pesan mentah.
			DEBUG4(_("Panjang pesan mentah diterima: %1$i."), diterima);
			DEBUG5(_("Pesan mentah diterima"), penyangga, 0, diterima);
			
			// Bila terenkripsi.
			if(!aturan.rawtransfer){
			
				// Memecahkan pesan.
				unsigned char pesan_deco[ENCRYPTED_CONTAINER_SIZE+1];
				memcpy(pesan_deco, penyangga, ENCRYPTED_CONTAINER_SIZE);
				unsigned char tujuan_deco[MAX_CHUNK_SIZE+1];
				diterima=rsa_decrypt(
					pesan_deco,
					diterima,
					tujuan_deco,
					rsapriv,
					aturan.rsa_padding
				);
				
				// Buang.
				memset(pesan_deco, 0, ENCRYPTED_CONTAINER_SIZE);
				
				// Pesan mentah.
				DEBUG4(_("Panjang terpecahkan: %1$i."), diterima);
				DEBUG5(_("Pesan mentah diterima terpecahkan"), tujuan_deco, 0, diterima);
			
				// print_unsigned_array(tujuan_deco, 20);
				// Penugasan.
				memcpy(penyangga, tujuan_deco, MAX_CHUNK_SIZE);
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
					
					// Alikasi memori.
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
						strncpy(berkas->identifikasi, berkas_id, BERKAS_MAX_STR);
						strncpy(berkas->nama,berkas_nama, BERKAS_MAX_STR);
						berkas->ukuran=strtod(berkas_ukuran, NULL);
						berkas->diterima=0;
						
						// Memeriksa ofset.
						// Membangun lajur.
						char pembatas[2];
						pembatas[0]=DIR_SEPARATOR;
						pembatas[1]=0;
						
						// Nama untuk berkas belum selesai.
						int panjang_tmp=strlen(aturan.tempdir)+strlen(berkas_id)+3;
						char berkas_lajur_tmp[panjang_tmp];
						strcpy(berkas_lajur_tmp,aturan.tempdir);
						strcat(berkas_lajur_tmp,pembatas);
						strcat(berkas_lajur_tmp,berkas_id);
						
						// Memeriksa bila berkas sementara ada.
						if(file_exist(berkas_lajur_tmp)){
							// Memeriksa ukuran.
							berkas->ofset=fsize(berkas_lajur_tmp);
							DEBUG1(_("Berkas '%1$s' ditemukan dengan ukuran %2$.0f bita."), berkas_id, berkas->ofset);
						}else{
							berkas->ofset=0;
							DEBUG1(_("Berkas '%1$s' tidak ditemukan."), 0);
						};
						
						// Atur ulang isi.
						memset(berkas->data_pesan, 0, sizeof(berkas->data_pesan[0][0])*MAX_CHUNK_ID*(CHUNK_MESSAGE_SIZE+1));
						memset(berkas->data_terima, 0, sizeof(berkas->data_terima[0])*MAX_CHUNK_ID);
						
						// Pesan.
						char ukuberkas[ukuberkas_panjang];
						strcpy(ukuberkas, readable_fs(berkas->ukuran, ukuberkas));
						INFO(
							_("Menerima berkas '%1$s': %2$s (%3$.0lf bita)."),
							berkas->nama, ukuberkas, berkas->ukuran
							);
						memset(ukuberkas, 0, ukuberkas_panjang);
					}
					
					free(berkas_id);
					free(berkas_nama);
					free(berkas_ukuran);
				}else if(panji==INTRANSFER_FLAG){
					// Sedang dalam transfer data.
					// Kesalahan.
					if(
						!strlen(berkas->nama) || berkas->ukuran <=0
						|| !strlen(berkas->nama)
					){
						
						// Identifikasi berkas.
						memset(
							berkas->identifikasi, 0,
							sizeof(berkas->identifikasi)*BERKAS_MAX_STR);
						
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
						};
					}else if(berkas->ukuran <=0){
						// Kesalahan.
						WARN(_("Ukuran berkas tidak diketahui."), 0);
						NOTICE(_("Meminta Klien mengulang pengiriman."), 0);
						
						// Meminta mengulang ke identifikasi 0.
						identifikasi=0;
						
					}else{
						// Tidak ada masalah.
						
						// Bila telah terkirim.
						if(berkas->data_terima[identifikasi]){
							// Pesan.
							NOTICE(_("Identifikasi %1$i telah diterima."), identifikasi);
							
						}else{
							// Menyimpan ke penyangga.
							memcpy(berkas->data_pesan[identifikasi], pesan, CHUNK_MESSAGE_SIZE);
							berkas->data_terima[identifikasi]=true;
							
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
							char ukuberkas_diterima[ukuberkas_panjang];
							strcpy(ukuberkas_diterima, readable_fs(br_diterima, ukuberkas_diterima));
							
							char ukuberkas_ukuran[ukuberkas_panjang];
							strcpy(ukuberkas_ukuran, readable_fs(br_ukuran, ukuberkas_ukuran));
							
							// Pesan.
							INFO(
								_("Menerima berkas '%1$s' (%2$.2f%%)."),
									berkas->nama, persen_selesai
								);
							DEBUG1(
								_("Identifikasi berkas: '%1$s'"),
									berkas->identifikasi
								);
							DEBUG1(
								_("Diterima %1$s/%2$s (%3$.0lf/%4$.0lf bita)."),
									ukuberkas_diterima, ukuberkas_ukuran,
									br_diterima, br_ukuran
								);
							
							// Membersihkan.
							memset(ukuberkas_diterima, 0, ukuberkas_panjang);
							memset(ukuberkas_ukuran, 0, ukuberkas_panjang);
							
							/*
							// Periksa setiap penyangga apakah masih kosong.
							// Jika masih kosong, maka meminta Klien
							// untuk mengirim bagian tersebut.
							for(unsigned int id=1; id<identifikasi; id++){
								if(!berkas->data_terima[id]){
									// Menunggu 5 detik
									// untuk memeriksa apakah
									// telah dikirim oleh proses lain.
										NOTICE("Meminta Klien mengirim identifikasi '%1$i.'", id);
									sleep(5);
									if(!berkas->data_terima[id]){
										NOTICE("Meminta Klien mengirim identifikasi '%1$i.'", id);
										identifikasi=id;
										status_peladen=0;
										break;
									};
								};
							};
							*/
							// printf("Pesan:\n");
							// print_char(pesan, CHUNK_MESSAGE_SIZE);
							// printf("\n");
						};
					};
				// Bila berhenti atau telah melebihi ukuran.
				}else if(
					   panji==STOP_FLAG
					|| (berkas->diterima)+berkas->ofset > berkas->ukuran
					){
					berkas->sedang_sibuk=true;
					
					// Pesan.
					if(panji==STOP_FLAG){
						NOTICE(_("Mendapatkan sinyal akhir kelompok pecahan."), 0);
					}else{
						NOTICE(
							_("Telah melebihi ukuran berkas sebesar %1$.0f bita."),
							((berkas->diterima)+berkas->ofset)-berkas->ukuran
						);
						
						// Pesan.
						DEBUG1(_("Mengirim panji '%1$s'."),_("Henti"));
						panji=STOP_FLAG;
					};
					
					// Perkembangan.
					double br_diterima=(berkas->diterima)+berkas->ofset;
					double br_ukuran=berkas->ukuran;
					tampil_info_progres_berkas(
						PROGRES_TERIMA, berkas->nama,
						br_diterima, br_ukuran,
						ukuberkas_panjang
					);
					
					// Periksa setiap penyangga apakah masih kosong.
					// Jika masih kosong, maka meminta Klien
					// untuk mengirim bagian tersebut.
					for(unsigned int id=1; id<identifikasi; id++){
						if(!berkas->data_terima[id]){
							NOTICE("Meminta Klien mengirim identifikasi '%1$i.'", id);
							identifikasi=id;
							status_peladen=0;
							break;
						};
					};
					
					// Memanggil proses juru tulis.
					// Memecah tugas.
					// Mencabangkan proses.
					// 'KANCIL_NOFORK' berguna untuk melakukan pengutuan
					// sebab proses tidak dipecah.
					// Kompilasi Kancil dengan parameter 'nofork=yes'.
					#ifndef KANCIL_NOFORK
						pid=fork();
					#else
						pid=0;
					#endif
					
					if (pid < 0){
						FAIL(_("Kegagalan proses cabang: %1$s (%2$i)."), strerror(errno), errno);
						exit(EXIT_FAILURE_FORK);
					}else{
						if (pid == 0){
							// Proses anak.
							anak_tulis(berkas);
							berkas->sedang_sibuk=false;
							
							// Menutup.
							#ifndef KANCIL_NOFORK
								exit(0);
							#endif
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
						NOTICE(_("Menunggu penulisan berkas '%1$s' maksimal selama %2$i detik."),
							berkas->nama, detik_tunggu_penulisan);
						
						// Menunggu maksimal `detik_tunggu_penulisan` detik.
						// Periksa kembali
						// apakah masih sibuk.
						int kali_periksa=0;
						while (berkas->sedang_sibuk){
							// Jeda setiap
							// 100.000 mikrodetik/100 milidetik/0,1 detik
							usleep(100000);
							if (kali_periksa++<=(detik_tunggu_penulisan*10))
								break;
						};
						
						// sleep(detik_tunggu_penulisan);
						// if(!berkas->sedang_sibuk){
							// berkas->sedang_sibuk=false;
							// tunggupid=true;
							// break;
						// };
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
		
		// Simpan waktu.
		infokancil.unixtime=current_time(CURRENTTIME_MICROSECONDS);
		
	}while(do_ulang);
	
	// printf("\n");
	// print_char_csv(pesan, 0, CHUNK_MESSAGE_SIZE);
	// printf("\n");
	
	// printf("Pesan:\n");
	// print_array(berkas.nama, 20);
	// printf("\n");
	
	// Informasi berkas.
	
	// Buat pesan.
	// char* pesan;
	memset(pesan, 0, CHUNK_MESSAGE_SIZE);
	
	// Pesan peladen.
	memcpy(pesan, buat_pesan_peladen(
		pesan,
		CHUNK_MESSAGE_SIZE,
		berkas->identifikasi,
		berkas->ukuran,
		(berkas->diterima)+berkas->ofset
	), CHUNK_MESSAGE_SIZE);
	
	// Menulis balasan.
	// Pesan Kosong.
	memcpy(penyangga, buat_pesan(
		penyangga, identifikasi, &paritas, pesan
	), MAX_CHUNK_SIZE);
	
	// Pengepala.
	memcpy( penyangga, buat_pengepala(
		penyangga, identifikasi, panji, cek_paritas,
		1, status_peladen
	), MAX_CHUNK_SIZE);
	
	
	// Pesan.
	DEBUG5(_("Pesan mentah dikirim"), penyangga, 0, CHUNK_HEADER_SIZE);
	
	// Enkripsi.
	int panjang_pecahan;
	if(!aturan.rawtransfer){
		unsigned char pesan_ency[MAX_CHUNK_SIZE+1];
		memset(pesan_ency, 0, MAX_CHUNK_SIZE+1);
		memcpy(pesan_ency, penyangga, MAX_CHUNK_SIZE);
		
		// Penyandian.
		unsigned char tujuan_ency[ENCRYPTED_CONTAINER_SIZE];
		panjang_pecahan=rsa_encrypt(
			pesan_ency,
			MAX_CHUNK_SIZE,
			tujuan_ency,
			rsapub,
			aturan.rsa_padding
		);
		
		// Pesan mentah.
		DEBUG4(_("Panjang pesan mentah dikirim tersandikan: %1$i."), panjang_pecahan);
		DEBUG5(_("Pesan mentah dikirim tersandikan"), tujuan_ency, 0, panjang_pecahan);
		
		// Salin.
		memcpy(penyangga, tujuan_ency, panjang_pecahan);
		
	}else{
		panjang_pecahan=MAX_CHUNK_SIZE;
	};
	
	// Balasan.
	if (!sendall(sock, penyangga, &panjang_pecahan)){
	// if (!sendall(sock, "peladen", &len)){
		FAIL(_("Kesalahan dalam menulis ke soket: %1$s (%2$i)."),strerror(errno), errno);
		exit(EXIT_FAILURE_SOCKET);
	};
	
	// Menutup.
	close(sock);
}
