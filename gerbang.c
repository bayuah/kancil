/*
 * `gerbang.c`
 * Sebagai gerbang dari kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */

#include "gerbang.h"

int main(int argc, char *argv[]){
	// Menugaskan penangan sinyal.
	signal(SIGINT, signal_callback_handler);
	int status;
	
	// Lokalisasi.
	setlocale(LC_ALL,"");
	bindtextdomain("kancil", "./locale");
	textdomain("kancil");
	
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
	aturan.tempdir="tmp";
	aturan.tries=5;
	aturan.waitretry=1;
	aturan.maxqueue=15000;
	aturan.waitqueue=30;
	aturan.rsa_datasize=204;
	aturan.nowaitqueue=true;
	
	// Informasi Kancil.
	info_kancil();
	
	// Mendapatkan argumen.
	if (argc < 4) {
		printf(_("Gunakan %s <porta> <inang> <porta>.\n"), argv[0]);
		exit(EXIT_FAILURE_ARGS);
	};
	
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
	portno = atoi(argv[1]);
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
	
	connection=0;
	listen(sockfd, 5);
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
	strncpy(kirim_mmap->hostname, argv[2], KIRIMBERKAS_MAX_STR);
	strncpy(kirim_mmap->portno, argv[3], KIRIMBERKAS_MAX_STR);
	// strncpy(kirim_mmap->berkas, berkas, KIRIMBERKAS_MAX_STR);
	kirim_mmap->coba=1;
	
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
			// Bila proses pencabangan.
			#ifndef KANCIL_NOFORK
				close(sockfd);
			#endif
			
			// Panggil anak.
			anak_gerbang(
				newsockfd,
				kirim_mmap,
				alamat_mmap
			);
			
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
	
	// Membersihkan berkas memori.
	free_shm();
	
	return 0;
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