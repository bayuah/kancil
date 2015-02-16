/*
 * `peladen.c`
 * Sebagai peladen dari kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */

#include "peladen.h"

int main(int argc, char *argv[]){
	// Menugaskan penangan sinyal.
	signal(SIGINT, signal_callback_handler);
	int status;
	
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
	aturan.show_error=true;
	aturan.show_warning=true;
	aturan.show_notice=true;
	aturan.show_info=true;
	aturan.show_debug1=true;
	aturan.show_debug2=true;
	aturan.show_debug3=false;
	aturan.show_debug4=false;
	aturan.show_debug5=false;
	strcpy(aturan.completedir, "complete");
	strcpy(aturan.tempdir, "tmp");
	aturan.rawtransfer=true;
	
	// Urai argumen.
	urai_argumen(argc, argv);
	
	// Informasi versi.
	info_versi();
	
	// Berbagi memori.
	int berbagi_ukuran = 1024 * 1024 * 256; //256 mb
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
	
	struct TERIMABERKAS *berkas_mmap;
	berkas_mmap = mmap(NULL, berbagi_ukuran, 
		PROT_WRITE | PROT_READ, berbagi_panji, 
		shm_berkas, 0 );
	
	// Nilai awal.
	// Berkas.
	memset(berkas_mmap->identifikasi, 0, BERKAS_MAX_STR);
	memset(berkas_mmap->nama, 0, BERKAS_MAX_STR);
	memset(berkas_mmap->data_pesan, 0, sizeof(berkas_mmap->data_pesan[0][0])*MAX_CHUNK_ID*(CHUNK_MESSAGE_SIZE+1));
	memset(berkas_mmap->data_terima, 0, sizeof(berkas_mmap->data_terima[0])*MAX_CHUNK_ID);
	berkas_mmap->ofset=0;
	berkas_mmap->ukuran=0;
	berkas_mmap->diterima=0;
	berkas_mmap->terima_berkas=false;
	berkas_mmap->sedang_sibuk=false;
	
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
	portno = 5001;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	
	// Gunakan kembali alamat.
	int reuse_addr=1;
	if (setsockopt((socklen_t)sockfd, SOL_SOCKET, SO_REUSEADDR,
		&reuse_addr, sizeof(int)) == -1 ){
		FAIL(_("Kegagalan '%1$s': %2$s (%3$i)."), "setsockopt", strerror(errno), errno);
		exit(EXIT_FAILURE_SOCKET);
	}
	
	/* Now bind the host address using bind() call.*/
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
						  sizeof(serv_addr)) < 0){
		FAIL(_("Kegagalan '%1$s': %2$s (%3$i)."), "bind", strerror(errno), errno);
		exit(EXIT_FAILURE_SOCKET);
	}
	
	/* 
	 * Mulai mendengarkan klien.
	 * Proses menuju mode tidur
	 * dan menunggu sambungan masuk.
	 */
	socklen_t clilen;
	int max_connection=5000;
	int pids[max_connection+1];
	int pid_status;
	int connection;
	int i;
	
	// char cli_name[INET6_ADDRSTRLEN];
	// char port_name[6]; 
	
	connection=0;
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	INFO(_("Mendengarkan porta %1$i."), portno);
	while (1){
		newsockfd = accept(sockfd, 
				(struct sockaddr *) &cli_addr, &clilen);
		connection++;
		
		// Mendapatkan klien.
		// Lebih cepat dari pada getnameinfo();
		INFO(
			_("Menerima klien: %1$s:%2$i."),
			inet_ntoa(cli_addr.sin_addr),
			(int) ntohs(cli_addr.sin_port)
			);
		// char clntName[INET6_ADDRSTRLEN];
		// char portName[6]; 
		
		// DEBUG3(_("Mendapatkan sambungan."));
		// DEBUG3(_("Mencari alamat klien."));
		// if (getnameinfo((struct sockaddr *) &cli_addr, clilen, clntName,sizeof(clntName),portName,sizeof(portName),0) == 0) {
			// INFO(_("Menerima klien: %1$s:%2$i."), clntName, portName);
		// } else {
			// WARN(_("Alamat klien tidak dikenal."), 0);
		// };
		
		if (newsockfd < 0){
			FAIL(_("Kegagalan '%1$s': %2$s (%3$i)."), "accept", strerror(errno), errno);
			exit(EXIT_FAILURE_SOCKET);
		};
		
		if(connection>=max_connection){
			for (i = 1; i < max_connection; ++i) {
				
				DEBUG2(_("Menunggu proses %1$i (%2$i)."), i, pids[i]);
				while (-1 == waitpid(pids[i], &pid_status, WNOHANG)){
					sleep(1);
					killpid(pids[i], SIGKILL);
					WARN(_("Proses %1$i (%2$i) terlalu lama."), i, pids[i]);
				};
				if (!WIFEXITED(pid_status) || WEXITSTATUS(pid_status) != 0) {
					FAIL(_("Gagal mematikan proses %1$i (%2$i)."), i, pids[i]);
					exit(EXIT_FAILURE_FORK);
				};
				
				// Menunggu milidetik.
				// usleep(100);
			};
			connection=0;
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
			FAIL(_("Kegagalan proses cabang: %1$s (%2$i)."), strerror(errno), errno);
			exit(EXIT_FAILURE_FORK);
		}else{
		};
		
		if (pids[connection] == 0){
			// Proses anak.
			#ifndef KANCIL_NOFORK
				close(sockfd);
			#endif
			
			// Panggil anak.
			anak_sambungan(newsockfd, berkas_mmap);
			
			// Pesan.
			INFO(
				_("Selesai menangani klien: %1$s:%2$i."),
				inet_ntoa(cli_addr.sin_addr),
				(int) ntohs(cli_addr.sin_port)
				);
		
			// Menutup.
			#ifndef KANCIL_NOFORK
				exit(0);
			#endif
		}else{
			close(newsockfd);
		};
	};
	
	// Menutup.
	// stop_listening(newsockfd);
	
	exit(EXIT_SUCCESS);
}

/*
 * Menutup.
 */
void stop_listening(int sock){
	shm_unlink("TERIMABERKAS");
	printf("Berhenti mendengar...\n");
	shutdown(sock, 2);
}

/*
 * signal_callback_handler()
 * Menangangi sinyal balasan.
 */
void signal_callback_handler(int signum){
	printf("\r\n");
	NOTICE(_("Menangkap sinyal %1$s (%2$i)."), kancil_signal_code(signum), signum);
	
	// Menutup.
	stop_listening(sockid);
	
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
	#else
		DEBUG4(_("Tidak terdapat berkas SHM."), 0);
	#endif
}