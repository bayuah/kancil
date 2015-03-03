/*
 * `gerbang.c`
 * Sebagai gerbang dari kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014 - 2015
 * Lisensi: lihat LICENCE.txt
 */

#include "gerbang.h"

int main(int argc, char *argv[]){
	// Menugaskan penangan sinyal.
	signal(SIGINT, signal_callback_handler);
	int status;
	
	// Penyangga STDOUT.
	char stdout_buf[1024];
	setvbuf(stdout, stdout_buf, _IOFBF, sizeof(stdout_buf));
	
	// Lokalisasi.
	setlocale(LC_ALL,"");
	bindtextdomain("kancil", "./locale");
	textdomain("kancil");
	
	// Info kancil.
	infokancil.executable=basename(argv[0]);
	infokancil.progname=PROGNAME;
	infokancil.progcode=PROGCODE;
	infokancil.version_major=__VERSION_MAJOR,
	infokancil.version_minor=__VERSION_MINOR,
	infokancil.version_patch=__VERSION_PATCH,
	infokancil.built_number=__BUILT_NUMBER,
	infokancil.built_time=__BUILT_TIME;
	infokancil.compile_mode=COMPILE_MODE;
	infokancil.compiler_machine=STRINGIZE_VALUE_OF(COMPILER_MACHINE);
	infokancil.compiler_version=STRINGIZE_VALUE_OF(COMPILER_VERSION);
	infokancil.compiler_flags=STRINGIZE_VALUE_OF(COMPILER_FLAGS);
	infokancil.unixtime=current_time(CURRENTTIME_MICROSECONDS);
	
	// Aturan umum.
	aturan.show_error=true;
	aturan.show_warning=true;
	aturan.show_notice=true;
	aturan.show_info=true;
	aturan.show_debug1=false;
	aturan.show_debug2=false;
	aturan.show_debug3=false;
	aturan.show_debug4=false;
	aturan.show_debug5=false;
	aturan.debuglevel=MINI_DEBUG;
	strcpy(aturan.tempdir, "tmp");
	strcpy(aturan.config, "kancil-gerbang.cfg");
	aturan.tries=5;
	aturan.waitretry=5;
	aturan.waitqueue=30;
	aturan.rawtransfer=true;
	strcpy(aturan.listening,"27001");
	strcpy(aturan.defaultport, "27000");
	aturan.rsa_padding=RSA_PKCS1_OAEP_PADDING;
	aturan.gates_c=1;
	aturan.hostname_c=0;
	aturan.gateid=0;
	aturan.timebase=10;
	aturan.timetollerance=10;
	aturan.gateshashing=GATEHASHING_XOR;
	aturan.maxconnection=50;
	
	// Garam.
	unsigned char *uc=(unsigned char*)"kancil dan buaya";
	int puc=strlen((char*)uc);
	memset(aturan.salt, 0, sizeof(aturan.salt[0])*MAX_STR);
	memset(aturan.salt_send, 0, sizeof(aturan.salt_send[0])*MAX_STR);
	memcpy(aturan.salt, uc, puc);
	memcpy(aturan.salt_send, aturan.salt, puc);
	
	// Informasi versi.
	info_versi();
	
	// Urai argumen.
	urai_argumen(argc, argv);
	
	// Konfigurasi.
	baca_konfigurasi();
	
	// Bila inang kosong.
	if(!aturan.hostname_c){
		FAIL(_("Inang kosong."), 0);
		bantuan_param_standar();
		exit(EXIT_FAILURE_ARGS);
	}
	
	// Pengirim.
	// Berbagi memori.
	int berbagi_ukuran = 1024 * 1024 * 256; //256 mb
	int persen_ukuran=0;
	int shm_berkas = -1; 
	int berbagi_panji = MAP_SHARED;
	#ifdef SHM
		// Buka berbagi memori.
		shm_berkas = shm_open(SHM_FILE, 
		  O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

		// Ubah ukuran berbagi memori.
		status=ftruncate(shm_berkas, berbagi_ukuran);
		
		// Bila gagal mengubah ukuran.
		if(status){
			FAIL(
				_("Gagal membuat berkas memori: %1$s (%2$i)."),
				strerror(errno), errno
			);
			exit(EXIT_FAILURE_MEMORY);
		};
	#else
		//shm_berkas = -1;
		shm_berkas = creat("/dev/zero", S_IRUSR | S_IWUSR);
		berbagi_panji |= MAP_ANONYMOUS;
	#endif
	
	// Bila gagal.
	if(shm_berkas == -1) {
		FAIL(
			_("Kegagalan '%1$s': %2$s (%3$i)."),
			"shm_open", strerror(errno), errno
		);
		exit(EXIT_FAILURE_MEMORY);
	};
	
	struct KIRIMBERKAS *kirim_mmap;
	// Persen ukuran.
	persen_ukuran=45;
	kirim_mmap = mmap(NULL,
		(((float)(float)persen_ukuran/(float)100)*berbagi_ukuran)-1024, 
		PROT_WRITE | PROT_READ, berbagi_panji, 
		shm_berkas, 0 );
	
	// Alamat inang.
	struct INFOALAMAT *alamat_mmap;
	// Persen ukuran.
	persen_ukuran=45;
	alamat_mmap = mmap(NULL,
		(((float)(float)persen_ukuran/(float)100)*berbagi_ukuran)-1024, 
		PROT_WRITE | PROT_READ, berbagi_panji, 
		shm_berkas, 0 );
	
	// Mengosongkan isi.
	int mxid=INFOALAMAT_MAX_ID;
	int mxip=INFOALAMAT_MAX_IP;
	int mxst=INFOALAMAT_MAX_STR;
	memset(alamat_mmap->inang, 0,
		sizeof(alamat_mmap->inang[0][0]) * mxid * mxst);
	memset(alamat_mmap->ipcount, 0,
		sizeof(alamat_mmap->ipcount[0]) * mxid);
	memset(alamat_mmap->ai_family, 0,
		sizeof(alamat_mmap->ai_family[0][0]) * mxid * mxip);
	memset(alamat_mmap->ai_socktype, 0,
		sizeof(alamat_mmap->ai_socktype[0][0]) * mxid * mxip);
	memset(alamat_mmap->ai_protocol, 0,
		sizeof(alamat_mmap->ai_protocol[0][0]) * mxid * mxip);
	memset(alamat_mmap->ai_addrlen, 0,
		sizeof(alamat_mmap->ai_addrlen[0][0]) * mxid * mxip);
	memset(alamat_mmap->ai_canonname, 0,
		sizeof(alamat_mmap->ai_canonname[0][0]) * mxid * mxip * mxst);
	memset(
		alamat_mmap->sockaddr_sa_family, 0,
		sizeof(alamat_mmap->sockaddr_sa_family[0][0]) * mxid * mxip);
	memset(
		alamat_mmap->sockaddr_sa_data, 0,
		sizeof(alamat_mmap->sockaddr_sa_data[0][0][0]) * mxid * mxip * 14);
	
	// RSA.
	FILE *irufl;
	int terbaca=0;
	int selesai=0;
	
	// Pubkey.
	RSA *rsapub[aturan.hostname_c];
	unsigned char pubkey[MAX_STR];
	memset(pubkey, 0, MAX_STR);
	
	// Privkey.
	RSA *rsapriv;
	unsigned char privkey[MAX_STR];
	memset(privkey, 0, MAX_STR);
	
	// Buka berkas.
	if(file_exist(aturan.pubkeys[0])){
		// Baca berkas.
		irufl=fopen(aturan.pubkeys[0], "rb");
		
		// Mengosongkan.
		memset(pubkey, 0, MAX_STR);
		
		// Baca pelan
		// setiap 10 bita.
		terbaca=0;
		selesai=0;
		do{
			terbaca=fread(pubkey+selesai, 1, 10, irufl);
			selesai+=terbaca;
		}while(terbaca);
		
		// Pesan.
		DEBUG4(_("Berhasil membaca berkas RSA publik sebesar %1$i bita."), selesai);
		
		// Menutup.
		fclose(irufl);
	}else{
		
		DEBUG4(_("Menggunakan RSA publik standar."), 0);
		
		// Bila tidak ada.
		memcpy(pubkey, default_rsa_pubkey(), MAX_STR);
	};
	
	// Untuk pilih Gerbang.
	DEBUG3(_("Membangun RSA publik untuk memilih Gerbang."), 0);
	RSA *rsapub_pilih_gerbang=create_rsa(pubkey, CREATE_RSA_FROM_PUBKEY);
	DEBUG3(_("Membangun RSA publik untuk memilih Peladen."), 0);
	RSA *rsapub_pilih_peladen=create_rsa(pubkey, CREATE_RSA_FROM_PUBKEY);
	
	// Bila bukan tansfer mentah.
	if(!aturan.rawtransfer){
		// Membuat RSA Pub
		// sebanyak aturan.hostname_c
		for(int iru=0; iru<aturan.hostname_c; iru++){
			
			// Mengosongkan.
			memset(pubkey, 0, MAX_STR);
			
			// Bila terisi.
			if(
				file_exist(aturan.pubkeys[iru])
				&& iru <= aturan.pubkeys_c
			){
				// Baca berkas.
				irufl=fopen(aturan.pubkeys[iru], "rb");
				
				// Baca pelan
				// setiap 10 bita.
				terbaca=0;
				selesai=0;
				do{
					terbaca=fread(pubkey+selesai, 1, 10, irufl);
					selesai+=terbaca;
				}while(terbaca);
				
				// Menutup.
				fclose(irufl);
			}else{
				// Bila tidak ada.
				memcpy(pubkey, default_rsa_pubkey(), MAX_STR);
			};
		
			// Membuat.
			DEBUG3(_("Membangun RSA publik untuk inang %1$i."), iru);
			rsapub[iru] = create_rsa(pubkey, CREATE_RSA_FROM_PUBKEY);
		};
		
		// Privat.
		// Bila terisi.
		if(file_exist(aturan.privkey)){
			// Baca berkas.
			irufl=fopen(aturan.privkey, "rb");
			
			// Baca pelan
			// setiap 10 bita.
			terbaca=0;
			selesai=0;
			do{
				terbaca=fread(privkey+selesai, 1, 10, irufl);
				selesai+=terbaca;
			}while(terbaca);
			
			// Menutup.
			fclose(irufl);
		}else{
			// Bila tidak ada.
			memcpy(privkey, default_rsa_privatekey(), MAX_STR);
		};
		
		// Membuat RSA Priv.
		DEBUG3(_("Membangun RSA privat untuk Klien."), 0);
		rsapriv=create_rsa(privkey , CREATE_RSA_FROM_PRIVKEY);
	};
	
	// Penerima.
	// Soket.
	int sockfd, newsockfd, portno;
	struct sockaddr_in serv_addr, cli_addr;
	
	// Memanggil soket.
	sockid = sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		FAIL(_("Gagal membuka soket: %1$s (%2$i)."), strerror(errno), errno);
		exit(EXIT_FAILURE_SOCKET);
	}
	
	// Membangun struktur soket.
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	portno = atoi(aturan.listening);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	
	// Gunakan kembali alamat.
	int reuse_addr=1;
	if (setsockopt((socklen_t)sockfd, SOL_SOCKET, SO_REUSEADDR,
		&reuse_addr, sizeof(int)) == -1 ){
		FAIL(
			_("Kegagalan '%1$s': %2$s (%3$i)."),
			"setsockopt", strerror(errno), errno
		);
		exit(EXIT_FAILURE_SOCKET);
	}
	
	/* Now bind the host address using bind() call.*/
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
						  sizeof(serv_addr)) < 0){
		FAIL(
			_("Kegagalan '%1$s': %2$s (%3$i)."),
			"bind", strerror(errno), errno
		);
		exit(EXIT_FAILURE_SOCKET);
	}
	
	/* 
	 * Mulai mendengarkan klien.
	 * Proses menuju mode tidur
	 * dan menunggu sambungan masuk.
	 */
	socklen_t clilen;
	int max_connection=aturan.maxconnection;
	int pids[max_connection+1];
	int pid_status;
	int connection;
	int i;
	
	// Mendengarkan.
	connection=0;
	listen(sockfd, max_connection);
	clilen = sizeof(cli_addr);
	INFO(_("Mendengarkan porta %1$i."), portno);
	
	// Inisiasi isi kirim.
	kirim_mmap->identifikasi=0;
	kirim_mmap->identifikasi_sebelumnya=0;
	kirim_mmap->kelompok_kirim=1;
	kirim_mmap->urut_kali=0;
	// kirim_mmap->ukuran_berkas;
	kirim_mmap->ukuran_kirim=0;
	kirim_mmap->do_kirim=true;
	// strncpy(kirim_mmap->berkas, berkas, BERKAS_MAX_STR);
	kirim_mmap->coba=1;
	
	// Bila minus.
	if(aturan.timetollerance<0)
		aturan.timetollerance=0;
	
	// Pilih waktu.
	unsigned char *kunci=(unsigned char*)aturan.salt;
	unsigned char *kunci_peladen=(unsigned char*)aturan.salt_send;
	double waktu_unix;
	bool inang_sama=false;
	char porta_inang[INFOALAMAT_MAX_STR];
	char nama_inang[INFOALAMAT_MAX_STR];
	
	/*
	 * waktu_pilih
	 * Tembolok perhitungan.
	 * Indeks 0: Waktu Unix
	 * Indeks 1: Pilihan peladen.
	 * Indeks >1: Nilai gerbang berdasarkan
	 *            Toleransi>T<Toleransi
	*/
	double waktu_pilih[aturan.timetollerance+4];
	memset(waktu_pilih, 0,
		sizeof(waktu_pilih[0])*(aturan.timetollerance*2)+3);
	
	// Perilaku.
	int tunggu=0;
	int coba=0;
	bool pengawas=false;
	for ever{
		newsockfd = accept(sockfd, 
				(struct sockaddr *) &cli_addr, &clilen);
		
		// Mendapatkan klien.
		// Lebih cepat dari pada getnameinfo();
		INFO(
			_("Menerima klien: %1$s:%2$i."),
			inet_ntoa(cli_addr.sin_addr),
			(int) ntohs(cli_addr.sin_port)
			);
		
		if (newsockfd < 0){
			FAIL(
				_("Kegagalan '%1$s': %2$s (%3$i)."),
				"accept", strerror(errno), errno
			);
			exit(EXIT_FAILURE_SOCKET);
		};
		
		// Pengawas prose anak.
		if(pengawas){
			pengawas=false;
			if(connection>=max_connection){
				for (i = 0; i < max_connection; ++i) {
					
					DEBUG2(_("Menunggu proses %1$i (%2$i)."), i, pids[i]);
					while (-1 == waitpid(
						pids[i], &pid_status, WUNTRACED | WCONTINUED
					)){
						sleep(1);
						killpid(pids[i], SIGKILL);
						WARN(_("Proses %1$i (%2$i) terlalu lama."), i, pids[i]);
					};
					if (!WIFEXITED(pid_status) || WEXITSTATUS(pid_status) != 0){
						WARN(
							_("Proses %1$i (%2$i) tidak mati secara wajar."),
							i, pids[i]
						);
						// exit(EXIT_FAILURE_FORK);
					};
					
					// Menunggu milidetik.
					// usleep(100);
				};
				connection=0;
			};
		};
		
		
		// Memeriksa inang
		// sebelum memecah proses.
		waktu_unix=current_time(CURRENTTIME_SECONDS);
		inang_sama=false;
		int pilih_inang;
		if(aturan.timetollerance<=0){
			// Bila tanpa toleransi.
			
			// Memeriksa tembolok.
			if(waktu_pilih[0]==waktu_unix){
				// Indeks 2
				pilih_inang=waktu_pilih[2];
			}else{
				// Buat kembali.
				pilih_inang=pilih_gerbang(
					aturan.gates_c,
					kunci,
					aturan.timebase,
					waktu_unix,
					rsapub_pilih_gerbang
				);
				// Simpan.
				waktu_pilih[2]=pilih_inang;
			};
			
			// Pilih.
			if(pilih_inang==(int)aturan.gateid){
				inang_sama=true;
			}else{
				inang_sama=false;
			};
		}else{
			// Memeriksa setiap toleransi.
			inang_sama=false;
			int j=2;
			for(
				i=0-aturan.timetollerance;
				i<=(aturan.timetollerance);
				i++
			){
				// Pilih.
				// Memeriksa tembolok.
				if(waktu_pilih[0]==waktu_unix){
					// Indeks >1
					pilih_inang=waktu_pilih[j];
				}else{
					pilih_inang=pilih_gerbang(
						aturan.gates_c,
						kunci,
						aturan.timebase,
						(waktu_unix+i),
						rsapub_pilih_gerbang
					);
					waktu_pilih[j]=pilih_inang;
				};
				
				// Pilih.
				if(pilih_inang==(int)aturan.gateid){
					inang_sama=true;
					break;
				}else{
					inang_sama=false;
				};
				
				// Menaikkan j.
				j++;
			}
		};
		
		// Pilih inang peladen
		// sebelum memecah proses.
		// Memeriksa apakah telah dihitung.
		int pilih_inang_peladen;
		if(waktu_pilih[0]==waktu_unix){
			// Indeks 1;
			pilih_inang_peladen=waktu_pilih[1];
		}else{
			pilih_inang_peladen=pilih_gerbang(
				aturan.hostname_c,
				kunci_peladen,
				aturan.timebase,
				waktu_unix,
				rsapub_pilih_peladen
			);
			waktu_pilih[1]=pilih_inang_peladen;
		};
		
		// Memecah tugas.
		// Mencabangkan proses.
		// 'KANCIL_NOFORK' berguna untuk melakukan pengutuan
		// sebab proses tidak dipecah.
		// Kompilasi Kancil dengan parameter 'nofork=yes'.
		#ifndef KANCIL_NOFORK
			pids[connection]=fork();
		#else
			pids[connection]=0;
		#endif
		
		// Bila terjadi kesalahan.
		if (pids[connection] < 0){
			if(errno==11){
				// Bila kegagalan karena
				// sumber daya hilang.
				close(newsockfd);
				tunggu=5;
				coba=0;
				i=0;
				WARN(_("Sumber daya tidak mencukupi."), 0);
				while (waitpid(0, NULL, WNOHANG)!=-1){
					DEBUG1(
						_("Menunggu proses anak selama %1$i detik."),
						tunggu
					);
					sleep(tunggu);
					
					// BIla terlalu banyak,
					// maka berhenti.
					if(coba++>(max_connection*10)){
						FAIL(
							_("Kegagalan proses cabang: %1$s (%2$i)."),
							strerror(errno), errno
						);
						exit(EXIT_FAILURE_FORK);
					};
				};
			}else{
				// Lainnya.
				FAIL(
					_("Kegagalan proses cabang: %1$s (%2$i)."),
					strerror(errno), errno
				);
				exit(EXIT_FAILURE_FORK);
			};
		};
		
		if (pids[connection] == 0){
			// Proses anak.
			// Bila proses pencabangan.
			#ifndef KANCIL_NOFORK
				close(sockfd);
			#endif
			
			// Memilih.
			if(!inang_sama){
				// Berhenti.
				WARN(_("Menolak Klien %1$s:%2$i di waktu %3$.0f."),
					inet_ntoa(cli_addr.sin_addr),
					(int) ntohs(cli_addr.sin_port),
					waktu_unix
				);
				// Pesan.
				DEBUG1(
					_("ID Gerbang adalah %1$i, sedangkan sekarang adalah %2$i."),
					(int)aturan.gateid, pilih_inang
				);
				
				// Menutup sambungan.
				close(newsockfd);
			}else{
				
				// Memecah nama inang.
				memset(nama_inang, 0, INFOALAMAT_MAX_STR);
				memset(porta_inang, 0, INFOALAMAT_MAX_STR);
				status=sscanf(
					aturan.hostname[pilih_inang_peladen],
					"%[^:]:%s", nama_inang, &porta_inang
					);
				if(status==1 && !strlen(porta_inang)){
					// Bila porta kosong.
					strcpy(porta_inang, aturan.defaultport);
					
				}else if (status > 2|| status <=0){
					// Gagal.
					FAIL(
						_("Gagal mengurai inang %1$s."),
						aturan.hostname[pilih_inang_peladen]
					);
					exit(EXIT_FAILURE_ARGS);
					
				};
				
				// Menyalin.
				strncpy(kirim_mmap->hostname, nama_inang, INFOALAMAT_MAX_STR);
				strncpy(kirim_mmap->portno, porta_inang, INFOALAMAT_MAX_STR);
				
				// Panggil anak.
				anak_gerbang(
					newsockfd,
					kirim_mmap,
					alamat_mmap,
					rsapub[pilih_inang_peladen],
					rsapriv
				);
				
				// Pesan.
				INFO(
_("Selesai menangani Klien %1$s:%2$i untuk Peladen %3$s:%4$i di waktu %5$.0f."),
inet_ntoa(cli_addr.sin_addr),(int) ntohs(cli_addr.sin_port),
nama_inang, atoi(porta_inang),waktu_unix
					);
			};
			
			// Menutup.
			#ifndef KANCIL_NOFORK
				exit(0);
			#endif
		}else{
			close(newsockfd);
			pengawas=true;
		};
		
		// Menambah sambungan.
		connection++;
	};
	
	// Menutup.
	// stop_listening(newsockfd);
	
	// Membersihkan berkas memori.
	free_shm();
	
	return 0;
}

/*
 * signal_callback_handler()
 * Menangangi sinyal balasan.
 */
void signal_callback_handler(int signum){
	printf("\r\n");
	NOTICE(
		_("Menangkap sinyal %1$s (%2$i)."),
		kancil_signal_code(signum),
		signum
	);
	
	// Membersihkan berkas memori.
	free_shm();
	
	// Bila modus DEVEL.
	#if defined (COMPILE_MODE_DEVEL) && defined (EXECINFO_COMPATIBLE)
		NOTICE(_("Pelacakan mundur:"), 0);
		
		void *array[10];
		size_t size;
		
		// Menampilkan semua void* semua masukan di tumpukan.
		size = backtrace(array, 10);
		
		// Menampilkan semua bingkai ke stderr.
		backtrace_symbols_fd(array, size, STDERR_FILENO);
	#endif
	
	// Mematikan.
	exit(signum);
}

/*
 * free_shm().
 * Membersihkan berkas memori.
 */
void free_shm(){
	#if defined(SHM_FILE) && defined (SHM)
		int status;
		
		// Pesan.
		DEBUG3(_("Membersihkan berkas memori '%1$s'."), SHM_FILE);
		
		// Membersihkan.
		status=shm_unlink(SHM_FILE);
		
		// Status.
		if(status && errno!=2){
			FAIL(
				_("Gagal membersihkan berkas memori: %1$s (%2$i)."),
				strerror(errno), errno
			);
			exit(EXIT_FAILURE_MEMORY);
		};
	#endif
}