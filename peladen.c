/*
 * `peladen.c`
 * Sebagai peladen dari kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */

#include "peladen.h"

int main(/*int argc, char *argv[]*/){
	// Mengugaskan penangan sinyal.
	signal(SIGINT, signal_callback_handler);
	
	// Lokalisasi.
	setlocale(LC_ALL,"");
	bindtextdomain("kancil", "./locale");
	textdomain("kancil");
	
	// Aturan umum.
	aturan.show_error=true;
	aturan.show_error=true;
	aturan.show_warning=true;
	aturan.show_notice=true;
	aturan.show_info=true;
	aturan.show_debug1=true;
	aturan.show_debug2=true;
	aturan.show_debug3=true;
	aturan.show_debug4=true;
	aturan.tempdir="tmp";
	
	// Informasi Kancil.
	info_kancil();
	
	// Berbagi memori.
	int berbagi_ukuran = 1024 * 1024 * 256; //256 mb
	int shm_berkas = -1; 
	int berbagi_panji = MAP_SHARED;
	#ifdef SHM
		// Buka berbagi memori.
		shm_berkas = shm_open("/BERKASFL-PELADEN.memory", 
		  O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

		// Ubah ukuran berbagi memori.
		ftruncate(shm_berkas, berbagi_ukuran);
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
	
	BERKAS *berkas_mmap;
	berkas_mmap = mmap(NULL, berbagi_ukuran, 
		PROT_WRITE | PROT_READ, berbagi_panji, 
		shm_berkas, 0 );
	
	// Nilai awal.
	// Berkas.
	memset(berkas_mmap->identifikasi, 0, 64);
	memset(berkas_mmap->nama, 0, 64);
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
		INFO(
			_("Menerima klien: %1$s:%2$i."),
			inet_ntop(cli_addr.sin_addr),
			(int) ntohs(cli_addr.sin_port)
			);
		
		if (newsockfd < 0){
			FAIL(_("Kegagalan '%1$s': %2$s (%3$i)."), "accept", strerror(errno), errno);
			exit(EXIT_FAILURE_SOCKET);
		};
		
		if(connection>=max_connection){
			for (i = 1; i < max_connection; ++i) {
				
				DEBUG1(_("Menunggu proses %1$i (%2$i)."), i, pids[i]);
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
			close(sockfd);
			anak_sambungan(newsockfd, berkas_mmap);
			
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
	
	return 0;
}



/*
 * Menutup.
 */
void stop_listening(int sock){
	shm_unlink("BERKAS");
	printf("Berhenti mendengar...\n");
	shutdown(sock, 2);
	exit(0);
}

/*
 * signal_callback_handler()
 * Menangangi sinyal balasan.
 */
void signal_callback_handler(int signum){
	printf("\nCaught signal %i.\n",signum);
	
	// Bila modus DEVEL.
	#if defined (COMPILE_MODE_DEVEL) && defined (EXECINFO_COMPATIBLE)
		void *array[10];
		size_t size;
		
		// Menampilkan semua void* semua masukan di tumpukan.
		size = backtrace(array, 10);
		
		// Menampilkan semua bingkai ke stderr.
		backtrace_symbols_fd(array, size, STDERR_FILENO);
	#endif
	
	// Menutup.
	stop_listening(sockid);
	
	// Mematikan.
	exit(signum);
}

/*
 * Informasi kancil.
 */
void info_kancil(){
	char* BUILT_VERSION;
	BUILT_VERSION=malloc(sizeof(BUILT_VERSION)*12);
	snprintf(BUILT_VERSION, sizeof(BUILT_VERSION)*12,
		"%1$i.%2$i.%3$i.%4$i-%5$s",
		__VERSION_MAJOR,
		__VERSION_MINOR,
		__VERSION_PATCH,
		__BUILT_NUMBER,
		COMPILE_MODE
	);
	
	time_t BUILT_TIME;
	char BUILT_TIME_STR[50];
	struct tm *lcltime;
	BUILT_TIME=__BUILT_TIME;
	lcltime = localtime ( &BUILT_TIME );
	strftime(BUILT_TIME_STR, sizeof(BUILT_TIME_STR), "%c", lcltime);
	
	// Awal tampil.
	printf("%1$s (%2$s).\n", PROGNAME, BUILT_VERSION);
	printf(_("Dibangun pada %1$s. Protokol versi %2$i."), BUILT_TIME_STR, PROTOCOL_VERSION );
	printf("\n");
	
	free(BUILT_VERSION);
}
