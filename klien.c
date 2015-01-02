/*
 * `klien.c`
 * Sebagai klien dari kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */

#include "klien.h"

int main(int argc, char *argv[]){
	// Mengugaskan penangan sinyal.
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
	
	// Berbagi memori.
	int berbagi_ukuran = 1024 * 1024 * 256; //256 mb
	int shm_berkas = -1; 
	int berbagi_panji = MAP_SHARED;
	#ifdef SHM
		// Buka berbagi memori.
		shm_berkas = shm_open("/BERKASFL-KLIEN.memory", 
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
	
	// Berkas.
	struct KIRIMBERKAS *kirim_mmap;
	kirim_mmap = mmap(NULL, berbagi_ukuran, 
		PROT_WRITE | PROT_READ, berbagi_panji, 
		shm_berkas, 0 );
	
	// Alamat inang.
	// struct DAFTAR_ALAMAT *alamat_mmap;
	// alamat_mmap = mmap(NULL, berbagi_ukuran, 
		// PROT_WRITE | PROT_READ, berbagi_panji, 
		// shm_berkas, 0 );
	
	// Inisiasi isi.
	// memset(kirim_mmap->alamat->nama_inang, 0, INET6_ADDRSTRLEN+1);
	kirim_mmap->alamat=malloc(sizeof (struct DAFTAR_ALAMAT));
	kirim_mmap->alamat->awal=kirim_mmap->alamat;
	kirim_mmap->alamat->identifikasi=0;
	kirim_mmap->alamat->info=NULL;
	kirim_mmap->alamat->lanjut=NULL;
	
	// Aturan umum.
	aturan.show_error=true;
	aturan.show_warning=true;
	aturan.show_notice=true;
	aturan.show_info=true;
	aturan.show_debug1=true;
	aturan.show_debug2=true;
	aturan.show_debug3=true;
	aturan.show_debug4=true;
	aturan.tempdir="tmp";
	aturan.tries=20;
	aturan.waitretry=30;
	aturan.maxqueue=15000;
	aturan.waitqueue=30;
	aturan.nowaitqueue=true;
	
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
			kirim_mmap->hostname=argv[1];
			kirim_mmap->portno=argv[2];
			kirim_mmap->berkas=berkas;
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
						if (WIFEXITED(status)) {
							if(status){
								DEBUG1(_("Proses cabang selesai. Status pengawas: %1$i."), WEXITSTATUS(status));
								// kirim_mmap->do_kirim=false;
								exit(status);
							}else{
								DEBUG1(_("Proses cabang selesai."), 0);
							};
						} else if (WIFSIGNALED(status)) {
							if(status){
								DEBUG1(_("Proses cabang terbunuh sinyal %1$i."), WTERMSIG(status));
								// kirim_mmap->do_kirim=false;
								exit(status);
							}else{
								DEBUG1(_("Proses cabang terbunuh."), 0);
							};
						} else if (WIFSTOPPED(status)) {
							if(status){
								DEBUG1(_("Proses cabang dihentikan sinyal %1$i."), WSTOPSIG(status));
								// kirim_mmap->do_kirim=false;
								exit(status);
							}else{
								DEBUG1(_("Proses cabang terhenti."), 0);
							};
						} else if (WIFCONTINUED(status)) {
							DEBUG1(_("Proses cabang sedang berlangsung."), 0);
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
	
	// Mematikan.
	exit(signum);
}