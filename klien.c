/*
 * `klien.c`
 * Sebagai klien dari kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */

#include "klien.h"

int main(int argc, char *argv[]){
	// Menugaskan penangan sinyal.
	signal(SIGINT, signal_callback_handler);
	int ukuberkas_panjang=12;
	int status;
	
	char *berkas;
	
	// Lokalisasi.
	setlocale(LC_ALL,"");
	bindtextdomain("kancil", "./locale");
	textdomain("kancil");
	
	// Informasi Kancil.
	info_kancil();
	
	// Mendapatkan argumen.
	if (argc < 4) {
		printf(_("Gunakan %s <inang> <porta> <berkas>.\n"), argv[0]);
		exit(EXIT_FAILURE_ARGS);
	};
	
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
	aturan.tries=20;
	aturan.waitretry=30;
	aturan.maxqueue=15000;
	aturan.waitqueue=30;
	aturan.rsa_datasize=204;
	aturan.nowaitqueue=true;
	
	// Berbagi memori.
	int berbagi_ukuran = 1024 * 1024 * 128; //128 mb
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
	berkas = argv[3];
	
	// Bila berkas tidak ada.
	if (!file_exist(berkas)){
		FAIL ( 
			_("Berkas '%1$s' tidak ditemukan."),
			berkas);
		exit(EXIT_FAILURE_IO);
	}else{
		
		// Bila ada.
		kirim_mmap->ukuran_berkas=fsize(berkas);
		char *penyangga_fschar;
		penyangga_fschar=malloc(sizeof(penyangga_fschar)*ukuberkas_panjang);
		INFO(
			_("Berkas '%1$s' ditemukan dengan ukuran %2$s (%3$.0lf bita)."),
			berkas, readable_fs(kirim_mmap->ukuran_berkas, penyangga_fschar), kirim_mmap->ukuran_berkas
			);
		free(penyangga_fschar);
		
		// Membaca berkas.
		FILE *pberkas=fopen(berkas, "rb");
		
		// Bila gagal.
		if(pberkas == NULL){
			FAIL ( 
				_("Gagal membuka berkas '%1$s': %2$s (%1$i)."),
				berkas, strerror(errno), errno
				);
			exit(EXIT_FAILURE_IO);
		}else{
			
			// Inisiasi isi.
			kirim_mmap->identifikasi=0;
			kirim_mmap->identifikasi_sebelumnya=0;
			kirim_mmap->kelompok_kirim=1;
			kirim_mmap->urut_kali=0;
			// kirim_mmap->ukuran_berkas;
			kirim_mmap->ukuran_kirim=0;
			kirim_mmap->do_kirim=true;
			strncpy(kirim_mmap->hostname, argv[1], KIRIMBERKAS_MAX_STR);
			strncpy(kirim_mmap->portno, argv[2], KIRIMBERKAS_MAX_STR);
			strncpy(kirim_mmap->berkas, berkas, KIRIMBERKAS_MAX_STR);
			kirim_mmap->coba=1;
			
			// Mulai.
			pid_t pid, pid_anak;
			while(kirim_mmap->do_kirim){
				
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
				
				// Bila terjadi kesalahan.
				if (pid< 0){
					FAIL(_("Kegagalan proses cabang: %1$s (%2$i)."), strerror(errno), errno);
					exit(EXIT_FAILURE_FORK);
				}else{
				};
				
				if (pid == 0){
					
					// Panggil anak.
					anak_kirim(
						pberkas,
						kirim_mmap,
						alamat_mmap,
						ukuberkas_panjang
					);
					
					// Menutup.
					#ifndef KANCIL_NOFORK
						exit(0);
					#endif
				}else{
				// Bapak tunggu anak.
					do{
						pid_anak  = waitpid(pid, &status, WUNTRACED | WCONTINUED);
						if (pid_anak  == -1) {
							// Pesan.
							FAIL(_("Kegagalan proses cabang: %1$s (%2$i)."), strerror(errno), errno);
							exit(EXIT_FAILURE_FORK);
						};
						
						// Status anak.
						if(WCOREDUMP(status)){
							
							// Terjadi kesalahan.
							FAIL(
								_("Kesalahan memori di proses cabang: %1$s (%2$i)."),
								kancil_signal_code(EXIT_FAILURE_MEMORY),
								EXIT_FAILURE_MEMORY
								);
							
							// Berhenti.
							exit(EXIT_FAILURE_MEMORY);
						}else if (WIFSTOPPED(status)) {
							WARN(
								_("Proses cabang dihentikan sinyal %1$s (%2$i)."),
								kancil_signal_code(WIFSTOPPED(status)),
								WIFSTOPPED(status)
								);
							
							// Berhenti.
							// kirim_mmap->do_kirim=false;
							exit(WIFSTOPPED(status));
						}else if(WIFSIGNALED(status)) {
							
							// Anak terbunuh sinyal.
							WARN(
								_("Proses cabang terbunuh sinyal %1$s (%2$i)."),
								kancil_signal_code(WIFSIGNALED(status)),
								status
							);
							
							// Mengulangi.
							NOTICE(_("Mengulangi proses cabang."), 0);
							break;
							
							// kirim_mmap->do_kirim=false;
							// exit(WIFSIGNALED(status));
						}else if(WEXITSTATUS(status)) {
							
							// Terjadi kesalahan.
							FAIL(
								_("Proses cabang berhenti dengan status: %1$s (%2$i)."),
								kancil_signal_code(WEXITSTATUS(status)),
								WEXITSTATUS(status)
								);
							
							// Berhenti.
							// kirim_mmap->do_kirim=false;
							exit(WEXITSTATUS(status));
						}else if(WIFEXITED(status)) {
							
							// Berhenti dengan normal.
							DEBUG3(_("Proses cabang selesai."), 0);
						}else if(WIFCONTINUED(status)) {
							
							// MAsih berjalan.
							DEBUG3(_("Proses cabang sedang berlangsung."), 0);
						}
					} while (!WIFEXITED(status) && !WIFSIGNALED(status));
				};
			};
			// Perkembangan.
			tampil_info_progres_berkas(
				PROGRES_KIRIM, berkas,
				kirim_mmap->ukuran_kirim, kirim_mmap->ukuran_berkas,
				ukuberkas_panjang
			);
			
			// Menutup.
			fclose (pberkas);
		};
	};
	
	// Membersihkan berkas memori.
	free_shm();
	
	exit(EXIT_SUCCESS);
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