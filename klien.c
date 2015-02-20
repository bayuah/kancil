/*
 * `klien.c`
 * Sebagai klien dari kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014 - 2015
 * Lisensi: lihat LICENCE.txt
 */

#include "klien.h"

int main(int argc, char *argv[]){
	// Menugaskan penangan sinyal.
	signal(SIGINT, signal_callback_handler);
	int ukuberkas_panjang=12;
	int status;
	
	// Penyangga STDOUT.
	char stdout_buf[1024];
	setvbuf(stdout, stdout_buf, _IOFBF, sizeof(stdout_buf));
	
	// Berkas.
	char *berkas;
	
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
	aturan.shifteof=false;
	strcpy(aturan.tempdir, "tmp");
	strcpy(aturan.config, "kancil-klien.cfg");
	aturan.tries=20;
	aturan.waitretry=15;
	aturan.waitqueue=5;
	aturan.parallel=1;
	aturan.debuglevel=MINI_DEBUG;
	strcpy(aturan.defaultport, "5001");
	aturan.rawtransfer=true;
	aturan.hostname_c=0;
	memset(aturan.hostname, 0, sizeof(aturan.hostname[0][0])*MAX_GATE*INFOALAMAT_MAX_STR);
	aturan.rsa_padding=RSA_PKCS1_OAEP_PADDING;
	aturan.gates_c=0;
	aturan.timebase=10;
	
	// Informasi versi.
	info_versi();
	
	// Urai argumen.
	urai_argumen(argc, argv);
	
	// Konfigurasi.
	baca_konfigurasi();
	
	// Bila inang kosong.
	if(!aturan.hostname_c){
		FAIL(_("Inang kosong."), 0);
		exit(EXIT_FAILURE_ARGS);
	};
	
	// Bila jumlah gerbang kosong.
	if(!aturan.gates_c){
		DEBUG1(_("Jumlah Gerbang kosong. Menggunakan jumlah inang."),0 );
		aturan.gates_c=aturan.hostname_c;
	}else if(aturan.gates_c>aturan.hostname_c){
		DEBUG1(_("Jumlah Gerbang melebihi jumlah inang. Menggunakan jumlah inang."),0 );
		aturan.gates_c=aturan.hostname_c;
	};
	
	// Berbagi memori.
	int berbagi_ukuran = 1024 * 1024 * 128; //128 MiB
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
			FAIL(_("Gagal membuat berkas memori: %1$s (%2$i)."), strerror(errno), errno);
			exit(EXIT_FAILURE_MEMORY);
		};
	#else
		//shm_berkas = -1;
		shm_berkas = creat("/dev/zero", S_IRUSR | S_IWUSR);
		berbagi_panji |= MAP_ANONYMOUS;
	#endif
	
	// Bila gagal.
	if(shm_berkas == -1) {
		FAIL(_("Kegagalan '%1$s': %2$s (%3$i)."), "shm_open", strerror(errno), errno);
		exit(EXIT_FAILURE_MEMORY);
	};
	
	// Berkas.
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
	
	// Inisiasi isi.
	int mxid=INFOALAMAT_MAX_ID;
	int mxip=INFOALAMAT_MAX_IP;
	int mxst=INFOALAMAT_MAX_STR;
	memset(alamat_mmap->inang, 0, sizeof(alamat_mmap->inang[0][0]) * mxid * mxst);
	memset(alamat_mmap->ipcount, 0, sizeof(alamat_mmap->ipcount[0]) * mxid);
	memset(alamat_mmap->ai_family, 0, sizeof(alamat_mmap->ai_family[0][0]) * mxid * mxip);
	memset(alamat_mmap->ai_socktype, 0, sizeof(alamat_mmap->ai_socktype[0][0]) * mxid * mxip);
	memset(alamat_mmap->ai_protocol, 0, sizeof(alamat_mmap->ai_protocol[0][0]) * mxid * mxip);
	memset(alamat_mmap->ai_addrlen, 0, sizeof(alamat_mmap->ai_addrlen[0][0]) * mxid * mxip);
	memset(alamat_mmap->ai_canonname, 0, sizeof(alamat_mmap->ai_canonname[0][0]) * mxid * mxip * mxst);
	memset(alamat_mmap->sockaddr_sa_family, 0, sizeof(alamat_mmap->sockaddr_sa_family[0][0]) * mxid * mxip);
	memset(alamat_mmap->sockaddr_sa_data, 0, sizeof(alamat_mmap->sockaddr_sa_data[0][0][0]) * mxid * mxip * 14);
	
	// Berkas.
	berkas = aturan.inputfile;
	
	// Buang STDOUT.
	fflush(stdout);
	
	// Bila berkas tidak ada.
	if (!file_exist(berkas)){
		FAIL ( 
			_("Berkas '%1$s' tidak ditemukan."),
			berkas);
		exit(EXIT_FAILURE_IO);
	}else{
		
		// Bila ada.
		kirim_mmap->ukuran_berkas=fsize(berkas);
		char penyangga_fschar[ukuberkas_panjang];
		memset(penyangga_fschar, 0, ukuberkas_panjang);
		INFO(
			_("Berkas '%1$s' ditemukan dengan ukuran %2$s (%3$.0lf bita)."),
			berkas, readable_fs(kirim_mmap->ukuran_berkas, penyangga_fschar), kirim_mmap->ukuran_berkas
			);
		
		// Jumlah sambungan.
		int sambungan_maksimal=aturan.parallel;
		int sambungan_maksimal_sekarang=sambungan_maksimal;
		FILE *pberkas[sambungan_maksimal+1];
		
		// Membuka berkas
		// sebanyak maksimal sambungan.
		for(int ibo=0; ibo<=sambungan_maksimal;ibo++){
			pberkas[ibo]=fopen(berkas, "rb");
			if(pberkas[ibo]==NULL){
				// Gagal.
				FAIL ( 
					_("Gagal membuka berkas '%1$s': %2$s (%1$i)."),
					berkas, strerror(errno), errno
					);
				exit(EXIT_FAILURE_IO);
			};
		};
		
		// RSA.
		// Pubkey.
		RSA *rsapub[aturan.gates_c];
		unsigned char pubkey[MAX_STR];
		memset(pubkey, 0, MAX_STR);
		memcpy(pubkey, default_rsa_pubkey(), MAX_STR);
		
		// Privkey
		RSA *rsapriv;
		unsigned char privkey[MAX_STR];
		memset(privkey, 0, MAX_STR);
		memcpy(privkey, default_rsa_privatekey(), MAX_STR);
		
		// Untuk pilih Gerbang.
		DEBUG3(_("Membangun RSA publik untuk memilih Gerbang."), 0);
		RSA *rsapub_pilih_gerbang=create_rsa(pubkey, CREATE_RSA_FROM_PUBKEY);
		
		// Bila bukan tansfer mentah.
		if(!aturan.rawtransfer){
			// Membuat RSA Pub
			// sebanyak aturan.gates_c
			for(int iru=0; iru<aturan.gates_c; iru++){
				DEBUG3(_("Membangun RSA publik untuk inang %1$i."), iru);
				rsapub[iru] = create_rsa(pubkey, CREATE_RSA_FROM_PUBKEY);
			};
			
			// Membuat RSA Priv.
			DEBUG3(_("Membangun RSA privat untuk Klien."), 0);
			rsapriv=create_rsa(privkey, CREATE_RSA_FROM_PRIVKEY);
		};
		
		// Inisiasi isi.
		kirim_mmap->identifikasi=0;
		kirim_mmap->identifikasi_sebelumnya=0;
		kirim_mmap->kelompok_kirim=1;
		kirim_mmap->urut_kali=0;
		// kirim_mmap->ukuran_berkas;
		kirim_mmap->ukuran_kirim=0;
		kirim_mmap->ukuran_kirim_sebelumnya=0;
		kirim_mmap->do_kirim=true;
		strncpy(kirim_mmap->berkas_lajur, berkas, BERKAS_MAX_STR);
		strncpy(kirim_mmap->berkas, basename(berkas), BERKAS_MAX_STR);
		memset(kirim_mmap->data_terkirim, 0, sizeof(kirim_mmap->data_terkirim[0])*MAX_CHUNK_ID);
		kirim_mmap->waktu_terkirim=current_time(CURRENTTIME_MICROSECONDS);
		kirim_mmap->paksa_panji=UNDEFINED_FLAG;
		kirim_mmap->kecepatan=0;
		kirim_mmap->coba=1;
		
		// Identifikasi berkas.
		char berkas_identifikasi[BERKAS_MAX_STR];
		char berkas_nama[BERKAS_MAX_STR];
		int waktu=current_time(CURRENTTIME_SECONDS);
		strncpy(berkas_nama, basename(berkas), BERKAS_MAX_STR);
		int panjang_nama=strlen(berkas_nama);
		
		// Membangun Identifikasi berkas.
		snprintf(
			berkas_identifikasi,
			BERKAS_MAX_STR,
			"%1$.3s%2$.0d%3$s",
			berkas_nama,
			waktu,
			berkas_nama+(panjang_nama-3)
		);
		
		// Memasukkan Identifikasi berkas.
		strncpy(kirim_mmap->berkas_identifikasi, berkas_identifikasi, BERKAS_MAX_STR);
		
		// Mendaptakan jumlah pecahan.
		
		// Mulai.
		// int pid_status;
		pid_t pids[sambungan_maksimal+1], pid_anak;
		memset(pids, 0, sizeof(pids[0])*sambungan_maksimal+1);
		
		// Pecahan.
		int sambungan=0;
		unsigned int identifikasi=0;
		
		// Perilaku.
		bool pengawas=true;
		bool pertama=true;
		int coba=0;
		
		// Aturan dasar.
		int kec_rendah_minimal=100*1024; // Bita
		int sambungan_maksimal_kec_rendah=3; // Bita
		
		// Perulangan.
		while(kirim_mmap->do_kirim){
			
			// Pengawas.
			// Mengawasi dan menugasi pengiriman.
			if(pengawas){
				
				// Bila pertama.
				if(pertama){
					pertama=false;
					
					// Ubah sambungan maksimal
					// bila lebih besar dari ukuran.
					if((double)(sambungan_maksimal*CHUNK_MESSAGE_SIZE)>(double)kirim_mmap->ukuran_berkas){
						// Pesan
						DEBUG3(
							_("Sambungan maksimal (%1$i) melebihi ukuran berkas (%2$.0f)."),
							sambungan_maksimal, kirim_mmap->ukuran_berkas
							);
						
						// Ubah.
						sambungan_maksimal=(int)((double)kirim_mmap->ukuran_berkas/(double)CHUNK_MESSAGE_SIZE)*25/100;
						
						// Pesan.
						DEBUG3(
							_("Sambungan maksimal diubah ke %1$i."),
							sambungan_maksimal
							);
					};
					
					// Ubah sambungan maksimal
					// bila lebih besar MAX_CHUNK_ID.
					if((sambungan_maksimal)>MAX_CHUNK_ID){
						// Pesan
						DEBUG3(
							_("Sambungan maksimal (%1$i) melebihi jumlah pecahan (%2$i)."),
							sambungan_maksimal, MAX_CHUNK_ID
							);
						
						// Ubah.
						sambungan_maksimal=MAX_CHUNK_ID-5;
						
						// Pesan.
						DEBUG3(
							_("Sambungan maksimal diubah ke %1$i."),
							sambungan_maksimal
							);
					};
					
					// Bila terlalu kecil.
					if(sambungan_maksimal<=0)
						sambungan_maksimal=1;
				};
				
				// Bila identifikasi adalah lebih dari MAX_CHUNK_ID,
				// maka kembali ke NOL.
				if(kirim_mmap->identifikasi>MAX_CHUNK_ID){
					DEBUG1(_("Telah melebihi maksimal identifikasi %1$i."), MAX_CHUNK_ID);
					DEBUG1(_("Nilai identifikasi adalah %1$i."), kirim_mmap->identifikasi);
					kirim_mmap->identifikasi=0;
				};
				
				// Bila adalah identifikasi NOL,
				// maka hanya SATU sambungan.
				if(kirim_mmap->identifikasi==0){
					DEBUG3(_("Pecahan identifikasi awal."),0);
					sambungan_maksimal_sekarang=1;
					
				}else if(
					kirim_mmap->identifikasi<= (unsigned int) sambungan_maksimal
					|| kirim_mmap->identifikasi < 10
					){
					// Bila adalah identifikasi NOL,
					// maka hanya SATU sambungan.
					DEBUG3(_("Pecahan awal."),0);
					sambungan_maksimal_sekarang=1;
					
				}else if(kirim_mmap->identifikasi==MAX_CHUNK_ID){
					// Bila telah mencapai
					// batas identifikasi.
					DEBUG3(_("Pecahan identifikasi telah sama dengan jumlah maksimal pecahan (%1$i)."), MAX_CHUNK_ID);
					sambungan_maksimal_sekarang=1;
					
				}else if(kirim_mmap->ukuran_kirim>kirim_mmap->ukuran_berkas){
					// Bila melebihi ukuran berkas.
					DEBUG3(
						_("Berkas terkirim (%1$.0f bita) telah melebihi ukuran berkas (%2$.0f bita)."),
						kirim_mmap->ukuran_kirim, kirim_mmap->ukuran_berkas
						);
					sambungan_maksimal_sekarang=1;
					
				}else if(
					(kirim_mmap->ukuran_kirim+(sambungan_maksimal*CHUNK_MESSAGE_SIZE*10))
					> kirim_mmap->ukuran_berkas
				){
					// Bila medekati ukuran berkas.
					DEBUG3(
						_("Berkas terkirim (%1$.0f bita) hampir mendekati ukuran berkas (%2$.0f bita)."),
						kirim_mmap->ukuran_kirim, kirim_mmap->ukuran_berkas
						);
					sambungan_maksimal_sekarang=1;
					
				}else if(
					kirim_mmap->ukuran_berkas-kirim_mmap->ukuran_kirim
					<=  kec_rendah_minimal
					&& sambungan_maksimal > sambungan_maksimal_kec_rendah
				){
					// Bila ukuran tersisa kurang dari
					// kecepatan rendah minimal
					// maka jumlah maksimal paralel adalah
					// sambunan maksimal kecepatan rendah.
					DEBUG3(_("Menggunakan kecepatan rendah dengan %1$i sambungan dalam %2$.0f bita tersisa."),
						sambungan_maksimal_kec_rendah, (kirim_mmap->ukuran_berkas-kirim_mmap->ukuran_kirim)
						);
					sambungan_maksimal_sekarang=sambungan_maksimal_kec_rendah;
					
				}else{
					// Maksimalkan.
					DEBUG3(_("Menggunakan %1$i sambungan."), sambungan_maksimal);
					sambungan_maksimal_sekarang=sambungan_maksimal;
				};
				
				// Pesan.
				DEBUG3(_("Jumlah sambungan adalah %1$i."), sambungan_maksimal_sekarang);
				
				// Berhasil.
				DEBUG4(_("Menghitung kecepatan."), 0);
				
				// Kecepatan.
				// Kecepatan merupakan perbedaan ukuran sekarang
				// dibandingkan dengan perbedaan waktu.
				double kecepatan_sebelumnya=kirim_mmap->kecepatan;
				double beda_ukuran=(kirim_mmap->ukuran_kirim)-(kirim_mmap->ukuran_kirim_sebelumnya);
				double beda_waktu= (current_time(CURRENTTIME_MICROSECONDS) - kirim_mmap->waktu_terkirim );
				double kecepatan = beda_ukuran / beda_waktu;
				
				// Bila kurang dari NOL.
				if (kecepatan<0)
					kecepatan=0;
				
				// Kecepataan rerata.
				// Kecepataan rerata menggunakan
				// rumus Tapis Rerata Kecepatan Pesat Tertimbang
				// (Exponentially Weighted Moving Average Filter).
				// Lihat: http://lorien.ncl.ac.uk/ming/filter/filewma.htm
				// HakCipta (c) 2009 M.T. Tham
				// Berkas dimodifikasi: Jumat, 21 Agustus 2009, 06:22:02 WIB
				// Berkas diakses: Sabtu, 07 Februari 2015, 00:40:40 WIB
				double konstanta=0.9;
				double kecepatan_rerata=konstanta* kecepatan + (1 - konstanta) * kecepatan_sebelumnya;
				
				// Bila kurang dari NOL.
				if (kecepatan_rerata<0)
					kecepatan_rerata=0;
				
				// Menyimpan.
				kirim_mmap->kecepatan=kecepatan_rerata;
				
				// Pesan.
				DEBUG4(_("Berhasil menghitung kecepatan."), 0);
				
				// Mendapat informasi.
				double br_dikirim=kirim_mmap->ukuran_kirim;
				
				// Mempersiapkan tampilan ukuran.
				char ukuberkas_dikirim[ukuberkas_panjang];
				strcpy(ukuberkas_dikirim, readable_fs(br_dikirim, ukuberkas_dikirim));
				
				char ukukecepatan[ukuberkas_panjang];
				if(kecepatan_rerata<1){
					snprintf(ukukecepatan, ukuberkas_panjang, "%1$.04f B", kecepatan);
				}else{
					strcpy(ukukecepatan, readable_fs(kecepatan_rerata, ukukecepatan));
				};
				
				// Tampilan.
				// Bila tidak aktif,
				// maka menggunakan progress.
				if(
					aturan.show_debug1
					|| aturan.show_debug2
					|| aturan.show_debug3
					|| aturan.show_debug4
					|| aturan.show_debug5
				){
					INFO(
						_("Berhasil mengirim %1$s (%2$.0lf bita) (%3$s/s)."),
						ukuberkas_dikirim, br_dikirim, ukukecepatan
						);
				}else{
					PROGRESS(
						_("Berhasil mengirim %1$s (%2$.0lf bita) (%3$s/s)."),
						ukuberkas_dikirim, br_dikirim, ukukecepatan
						);
				};
				
				// Bersihkan.
				DEBUG4(_("Membersihkan penyangga kecepatan."), 0);
				memset(ukuberkas_dikirim, 0, ukuberkas_panjang);
				memset(ukukecepatan, 0, ukuberkas_panjang);
				
				// Menyimpan waktu sekarang.
				DEBUG4(_("Menyimpan waktu sekarang."), 0);
				kirim_mmap->waktu_terkirim=current_time(CURRENTTIME_MICROSECONDS);
				DEBUG4(_("Selesai menyimpan waktu sekarang."), 0);
				
				// Menyimpan ukuran sekarang.
				DEBUG4(_("Menyimpan ukuran sekarang."), 0);
				kirim_mmap->ukuran_kirim_sebelumnya=kirim_mmap->ukuran_kirim;
				DEBUG4(_("Selesai menyimpan ukuran sekarang."), 0);
				
				// Menambah identifikasi.
				// kirim_mmap->identifikasi++;
				// identifikasi=kirim_mmap->identifikasi;
				
				// Matikan pengawas.
				pengawas=false;
				
			};
			
			// Memecah tugas.
			// Mencabangkan proses.
			// 'KANCIL_NOFORK' berguna untuk melakukan pengutuan
			// sebab proses tidak dipecah.
			// Kompilasi Kancil dengan parameter 'nofork=yes'.
			#ifndef KANCIL_NOFORK
				pids[sambungan]=fork();
			#else
				pids[sambungan]=0;
			#endif
			
			// Bila terjadi kesalahan.
			if (pids[sambungan]< 0){
				FAIL(_("Kegagalan proses cabang: %1$s (%2$i)."), strerror(errno), errno);
				exit(EXIT_FAILURE_FORK);
			}else{
			};
			
			if (pids[sambungan] == 0){
				// Proses anak.
				
				// Pesan.
				DEBUG1(_("Jumlah inang adalah %1$i."), aturan.gates_c);
				
				// Pilih gerbang.
				unsigned char *kunci=(unsigned char*)" Sate atau satai";
				double waktu_unix=current_time(CURRENTTIME_SECONDS);
				int pilih_inang=pilih_gerbang(
					aturan.gates_c,
					kunci,
					aturan.timebase,
					waktu_unix,
					rsapub_pilih_gerbang
				);
				
				// Bila kosong.
				if(!strlen(aturan.hostname[pilih_inang])){
					FAIL(_("Inang ke-%1$i kosong."), pilih_inang);
					exit(EXIT_FAILURE_ARGS);
				};
				
				// Memecah nama inang.
				char porta_inang[BERKAS_MAX_STR];
				char nama_inang[BERKAS_MAX_STR];
				status=sscanf(aturan.hostname[pilih_inang], "%[^:]:%s", nama_inang, &porta_inang);
				if(status==1 || (status == 2 && !strlen(porta_inang))){
					// Bila porta kosong.
					DEBUG3(_("Porta inang kosong. Menggunakan nilai asali: %1$s."), aturan.defaultport);
					strcpy(porta_inang, aturan.defaultport);
					
				}else if (status > 2|| status <=0){
					// Gagal.
					FAIL(_("Gagal mengurai inang %1$s."), aturan.hostname[pilih_inang]);
					exit(EXIT_FAILURE_ARGS);
					
				};
				
				// Pesan.
				DEBUG1(
					_("Memilih inang nomor %1$i (%2$s:%3$s) di waktu %4$.0f."),
					pilih_inang, nama_inang, porta_inang, waktu_unix
				);
				
				// Menyalin.
				strncpy(kirim_mmap->hostname, nama_inang, BERKAS_MAX_STR);
				strncpy(kirim_mmap->portno, porta_inang, BERKAS_MAX_STR);
				
				// Panggil anak.
				identifikasi=anak_kirim(
					identifikasi,
					pberkas[sambungan],
					rsapub[pilih_inang],
					rsapriv,
					kirim_mmap,
					alamat_mmap,
					ukuberkas_panjang
				);
				
				// Memasukkan nilai identifikasi.
				// DEBUG2(_("Menyimpan identifikasi %1$i."), identifikasi);
				kirim_mmap->identifikasi=identifikasi;
				// DEBUG2(_("Identifikasi %1$i tersimpan."), kirim_mmap->identifikasi);
				
				// Menutup.
				#ifndef KANCIL_NOFORK
					exit(0);
				#endif
			}else{
				// Proses Bapak.
				if((sambungan+1)>=(sambungan_maksimal_sekarang)){
					for (int i = 0; i < sambungan_maksimal_sekarang; ++i) {
						// Bapak tunggu anak.
						do{
							pid_anak  = waitpid(pids[i], &status, WUNTRACED | WCONTINUED);
							if (pid_anak  == -1) {
								// Pesan.
								FAIL(_("Kegagalan proses cabang (PID%1$i): %2$s (%3$i)."), pids[i], strerror(errno), errno);
								exit(EXIT_FAILURE_FORK);
							};
							
							// Status anak.
							if(WCOREDUMP(status)){
								
								// Terjadi kesalahan.
								FAIL(
									_("Kesalahan memori di proses cabang (PID%1$i): %2$s (%3$i)."),
									pids[i], kancil_signal_code(EXIT_FAILURE_MEMORY),
									EXIT_FAILURE_MEMORY
									);
								
								// Berhenti.
								exit(EXIT_FAILURE_MEMORY);
							}else if (WIFSTOPPED(status)) {
								WARN(
									_("Proses cabang (PID%1$i) dihentikan sinyal %2$s (%3$i)."),
									pids[i], kancil_signal_code(WIFSTOPPED(status)),
									WIFSTOPPED(status)
									);
								
								// Berhenti.
								// kirim_mmap->do_kirim=false;
								exit(WIFSTOPPED(status));
							}else if(WIFSIGNALED(status)) {
								
								// Anak terbunuh sinyal.
								WARN(
									_("Proses cabang (PID%1$i) terbunuh sinyal %1$s (%2$i)."),
									pids[i], kancil_signal_code(WIFSIGNALED(status)),
									status
								);
								
								// Mengulangi.
								NOTICE(_("Mengulangi proses cabang."), 0);
								break;
								
								// kirim_mmap->do_kirim=false;
								// exit(WIFSIGNALED(status));
							}else if(WEXITSTATUS(status)) {
								
								// Bila ditolak oleh inang.
								if (
									WEXITSTATUS(status)==EXIT_FAILURE_SOCKET
									&& coba< aturan.tries
								){
									DEBUG1(
										_("Kegagalan soket di proses cabang (PID%1$i)."),
										pids[i]
									);
									coba++;
								}else{
									// Terjadi kesalahan.
									FAIL(
										_("Proses cabang (PID%1$i) berhenti dengan status: %2$s (%3$i)."),
										pids[i], kancil_signal_code(WEXITSTATUS(status)),
										WEXITSTATUS(status)
										);
									// Berhenti.
									// kirim_mmap->do_kirim=false;
									exit(WEXITSTATUS(status));
								};
							}else if(WIFEXITED(status)) {
								
								// Berhenti dengan normal.
								DEBUG3(_("Proses cabang (PID%1$i) selesai."), pids[i]);
								
								// Tidak ada kesalahan.
								coba=0;
							}else if(WIFCONTINUED(status)) {
								
								// MAsih berjalan.
								DEBUG3(_("Proses cabang sedang berlangsung."), 0);
							}
						} while (!WIFEXITED(status) && !WIFSIGNALED(status));
						
						// Menambah identifikasi.
						// identifikasi++;
						// DEBUG1(_("Menyimpan identifikasi %1$i."), identifikasi);
						// kirim_mmap->identifikasi=identifikasi;
						// DEBUG1(_("Identifikasi %1$i tersimpan."), kirim_mmap->identifikasi);
						identifikasi=kirim_mmap->identifikasi;
						
						// Menunggu milidetik.
						// usleep(100);
					};
					
					// Atur ulang sambungan.
					sambungan=0;
					
					// Aktifkan pengawas.
					pengawas=true;
					
				}else{
					// Tambah.
					sambungan++;
					
					// Identifikasi.
					identifikasi++;
					
				};
			};
			
			#ifdef KANCIL_NOFORK
				sambungan=0;
				identifikasi=kirim_mmap->identifikasi;
				identifikasi++;
			#endif
			
		};
		// Perkembangan.
		tampil_info_progres_berkas(
			PROGRES_KIRIM, berkas,
			kirim_mmap->ukuran_kirim, kirim_mmap->ukuran_berkas,
			ukuberkas_panjang
		);
		
		// Menutup.
		for(int ibc=0; ibc<=sambungan_maksimal;ibc++){
			if(fclose(pberkas[ibc])!=0){
				// Gagal.
				FAIL ( 
					_("Gagal menutup berkas '%1$s': %2$s (%1$i)."),
					berkas, strerror(errno), errno
					);
				exit(EXIT_FAILURE_IO);
			};
		};
	};
	
	// Membersihkan berkas memori.
	free_shm();
	
	exit(EXIT_SUCCESS);
}

/*
 * signal_callback_handler()
 * Menangangi sinyal balasan.
 */
void signal_callback_handler(int signum){
	printf("\r\n");
	NOTICE(_("Menangkap sinyal %1$s (%2$i)."), kancil_signal_code(signum), signum);
	
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
			FAIL(_("Gagal membersihkan berkas memori: %1$s (%2$i)."), strerror(errno), errno);
			exit(EXIT_FAILURE_MEMORY);
		};
	#endif
}