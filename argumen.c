/*
 * `argumen.c`
 * Pengurai argumen dari kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "kancil.h"
#include "return.h"
#include "faedah.h"
#include "struktur.h"

// Lokalisasi.
#ifndef _LOCALE_H
	#include <libintl.h>
	#include <locale.h>
	#define _(String) gettext (String) // Makro.
	
	// Pengaturan kompilasi.
	#pragma GCC diagnostic ignored "-Wformat"
#endif

// Fungsi.
void urai_argumen(int argc, char *argv[]);
void info_bangun();
void info_versi();
void info_lisensi();
void info_tanya();
void bantuan_param_standar();
void bantuan();
bool is_klien();
bool is_gerbang();
bool is_peladen();

// Memeriksa jenis program.
bool is_klien(){
	return !strcmp(infokancil.progcode, "KANCIL_KLIEN");
}
bool is_gerbang(){
	return !strcmp(infokancil.progcode, "KANCIL_GERBANG");
}
bool is_peladen(){
	return !strcmp(infokancil.progcode, "KANCIL_PELADEN");
}

/*
 * urai_argumen()
 * Mengurai argumen.
 */
void urai_argumen(int argc, char *argv[]){
	// Bila kosong.
	int status;
	
	if(!argc){
		bantuan();
	};
	
	while (1) {
		// int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		static struct option long_options[] = {
			{"host",    required_argument, 0, 'H'},
			{"config",  required_argument, 0, 'c'},
			{"logfile", required_argument, 0, 'l'},
			{"debug",   required_argument, 0, 'd'},
			{"debug1",  no_argument,       0, '1'},
			{"debug2",  no_argument,       0, '2'},
			{"debug3",  no_argument,       0, '3'},
			{"debug4",  no_argument,       0, '4'},
			{"debug5",  no_argument,       0, '5'},
			{"show-error",   no_argument,  0, 'E'},
			{"show-warning", no_argument,  0, 'W'},
			{"show-notice",  no_argument,  0, 'N'},
			{"show-info",    no_argument,  0, 'I'},
			{"verbose", no_argument,       0, 'v'},
			{"quiet",   no_argument,       0, 'q'},
			{"tries",   no_argument,       0, 't'},
			{"parallel", no_argument,      0, 'p'},
			{"version", no_argument,       0, 'V'},
			{"help",    no_argument,       0, 'h'},
			{0,         0,                 0,  0 }
		};
		
		// Mendapatkan argumen.
		int c = getopt_long(argc, argv, "12345c:d:H:hl:RrvVq?",
				 long_options, &option_index);
		if (c == -1)
			break;
		
		switch (c) {
		case '1':
			aturan.show_debug1=true;
			DEBUG1(_("Kekutu tingkat %1$i aktif."), 1);
			break;
		case '2':
			aturan.show_debug2=true;
			DEBUG2(_("Kekutu tingkat %1$i aktif."), 2);
			break;
		case '3':
			aturan.show_debug3=true;
			DEBUG2(_("Kekutu tingkat %1$i aktif."), 3);
			break;
		case '4':
			aturan.show_debug4=true;
			DEBUG4(_("Kekutu tingkat %1$i aktif."), 4);
			break;
		case '5':
			// Debug5 merupakan pemecah karakter,
			// sehingga menggunakan debug lain untuk menampilkan pesan.
			aturan.show_debug5=true;
			status=aturan.show_debug4;
			aturan.show_debug4=true;
			DEBUG4(_("Kekutu tingkat %1$i aktif."), 5);
			aturan.show_debug4=status;
			break;
		case 'E':
			aturan.show_error=true;
			DEBUG1(_("Menampilkan kesalahan."), 0);
			break;
		case 'W':
			aturan.show_warning=true;
			DEBUG1(_("Menampilkan kesalahan."), 0);
			break;
		case 'N':
			aturan.show_notice=true;
			DEBUG1(_("Menampilkan maklumat."), 0);
			break;
		case 'I':
			aturan.show_info=true;
			DEBUG1(_("Menampilkan informasi."), 0);
			break;
		case 'v':
			aturan.show_debug1=true;
			DEBUG1(_("Menampilkan kekutu."), 0);
			break;
		case 'q':
			aturan.show_error=false;
			aturan.show_warning=false;
			aturan.show_notice=false;
			aturan.show_info=false;
			aturan.show_debug1=false;
			aturan.show_debug2=false;
			aturan.show_debug3=false;
			aturan.show_debug4=false;
			aturan.show_debug5=false;
			break;
		case 'd':
			INFO(_("Kekutu tingkat '%1$s'."), optarg);
			switch(atoi(optarg)){
				case 5:
					aturan.show_debug5=true;
				case 4:
					aturan.show_debug4=true;
				case 3:
					aturan.show_debug3=true;
				case 2:
					aturan.show_debug2=true;
				case 1:
					aturan.show_debug1=true;
				break;
			}
			break;
		case 'H':
			// Menambah inang tujuan.
			if(!is_nonascii(optarg) && strlen(optarg)){
				DEBUG1(_("Menambah inang %1$s dari argumen."), optarg);
				strcpy(aturan.hostname[aturan.hostname_c], optarg);
				aturan.hostname_c++;
			};
			break;
		case 'c':
			if(!is_nonascii(optarg) && strlen(optarg)){
				DEBUG1(_("Berkas konfigurasi %1$s."), optarg);
				strcpy(aturan.config, optarg);
			};
			break;
		case 'p':
			DEBUG1(_("Paralel sebanyak %s sambungan."), optarg);
			aturan.parallel=atoi(optarg);
			break;
		case 'R':
			DEBUG1(_("Transfer mentah."), 0);
			aturan.rawtransfer=true;
			break;
		case 'r':
			DEBUG1(_("Transfer terenkripsi."), 0);
			aturan.rawtransfer=false;
			break;
		case 'h':
			bantuan_param_standar();
			bantuan();
			exit(EXIT_SUCCESS);
			break;
		case '?':
			bantuan_param_standar();
			bantuan();
			exit(EXIT_SUCCESS);
			break;
		case 'V':
			info_bangun();
			info_lisensi();
			exit(EXIT_SUCCESS);
			break;
		default:
			FAIL(_("Karakter tidak dikenal ASCII-%i"), c);
		}
	}
	
	if (optind < argc) {
		char arg_sisa[argc][INFOALAMAT_MAX_STR+1];
		memset(arg_sisa, 0, INFOALAMAT_MAX_STR+1);
		int i=0;
		
		// Memindah 
		while (optind < argc)
			strncpy(arg_sisa[i++], argv[optind++], INFOALAMAT_MAX_STR);
		
		// Mendapatkan argumen tanpa kunci.
		if(i>0 && is_klien()){
			// Klien minimal adalah
			// BERKAS dikirim.
			strcpy(aturan.inputfile, arg_sisa[0]);
			if(i>1
				&&!is_nonascii(arg_sisa[1])
				&& strlen(arg_sisa[1])
			){
				DEBUG3(_("Menambah inang %1$s dari argumen."), arg_sisa[1]);
				strcpy(aturan.hostname[aturan.hostname_c], arg_sisa[1]);
				aturan.hostname_c++;
			};
			
		}else if(i>0 && is_gerbang()){
			// Gerbang minimal adalah
			// PORTA dan INANG Peladen.
			// atau takada.
			strcpy(aturan.listening, arg_sisa[0]);
			if(i>1
				&&!is_nonascii(arg_sisa[1])
				&& strlen(arg_sisa[1])
			){
				DEBUG3(_("Menambah inang %1$s dari argumen."), arg_sisa[1]);
				strcpy(aturan.hostname[aturan.hostname_c], arg_sisa[1]);
				aturan.hostname_c++;
			};
			
		}else if(is_peladen()){
			// Peladen minimal adalah
			// takada.
			strcpy(aturan.listening, arg_sisa[0]);
			
		}else{
			// Lainnya.
			bantuan_param_standar();
			exit(EXIT_FAILURE_ARGS);
		}
		
	}else{
		if(!is_peladen() && !is_gerbang()){
			// Peladen dan Gerbang dapat
			// tanpa argumen minimal.
			bantuan_param_standar();
			exit(EXIT_FAILURE_ARGS);
		};
	}
}

// Bantuan.
void bantuan_param_standar(){
	if(is_klien()){
		printf(_("Gunakan: %1$s BERKAS [INANG[:PORTA]] [--config=BERKAS] [OPSI]."), infokancil.executable);
		printf("\n");
	}else if(is_gerbang()){
		printf(_("Gunakan: %1$s PORTA INANG[:PORTA] [OPSI]."), infokancil.executable);
		printf("\n");
	}else if(is_peladen()){
		printf(_("Gunakan: %1$s [PORTA] [OPSI]."), infokancil.executable);
		printf("\n");
	};
	
}
void bantuan(){
	
}


/*
 * Informasi versi.
 */
void info_versi(){
	char BUILT_VERSION[64];
	snprintf(BUILT_VERSION, 64,
		"%1$i.%2$i.%3$i.%4$i-%5$s",
		infokancil.version_major,
		infokancil.version_minor,
		infokancil.version_patch,
		infokancil.built_number,
		infokancil.compile_mode
	);
	printf("%1$s (%2$s).\n", infokancil.progname, BUILT_VERSION);
	memset(BUILT_VERSION, 0, 64);
}

/*
 * Informasi kancil.
 */
void info_bangun(){
	info_versi();
	
	time_t BUILT_TIME;
	char BUILT_TIME_STR[50];
	struct tm *lcltime;
	BUILT_TIME=infokancil.built_time;
	lcltime = localtime ( &BUILT_TIME );
	strftime(BUILT_TIME_STR, sizeof(BUILT_TIME_STR), "%c", lcltime);
	
	// Awal tampil.
	printf(_("Dibangun pada %1$s. Protokol versi %2$i."), BUILT_TIME_STR, PROTOCOL_VERSION );
	printf("\n");
	
	// Informasi pembangun.
	printf(_("Dibuat di %1$s."), infokancil.compiler_machine);
	printf(" ");
	printf(_("Versi pembangun %1$s."), infokancil.compiler_version);
	printf("\n\n");
	printf(_("Panji pembangun:"));
	printf("\n%1$s\n", infokancil.compiler_flags);
	
}

// Menampilkan
// pesan lisensi.
void info_lisensi(){
	printf("\n");
	printf(_("HakCipta (C) 2014-2015 Bayu Aditya H."));
	printf("\n");
	printf("Rabu, 26 November 2014, 07:05:50 UTC+7\n");
	printf(_("Lisensi: \"THE COFFEE-WARE\" (Revision 1991-A)"));
	printf("\n\n");
	printf(_("Ditulis oleh %1$s <%2$s>."), "Bayu Aditya H.", "b@yuah.web.id");
	printf("\n");
	info_tanya();
}

void info_tanya(){
	printf(_("Kirim laporan kesalahan dan pertanyaan ke %1$s."), "bug@yuah.web.id");
	printf("\n");
}

int buat_konfigurasi(){
	int i;
	
	FILE *cfg=fopen(aturan.config, "w");
	if(cfg==NULL){
		// Gagal.
		FAIL ( 
			_("Gagal membuka berkas '%1$s': %2$s (%1$i)."),
			aturan.config, strerror(errno), errno
			);
		exit(EXIT_FAILURE_IO);
	};
	
	// Mulai menulis aturan.
	// b2s(aturan.show_error);
	// b2s(aturan.show_warning);
	// b2s(aturan.show_notice);
	// b2s(aturan.show_info);
	// b2s(aturan.show_debug1);
	// b2s(aturan.show_debug2);
	// b2s(aturan.show_debug3);
	// b2s(aturan.show_debug4);
	// b2s(aturan.show_debug5);
	// aturan.tempdir="tmp";
	// aturan.config="kancil-klien.cfg";
	// aturan.tries=20;
	// aturan.waitretry=15;
	// aturan.waitqueue=5;
	// aturan.parallel=1;
	
	// Jumlah yang ditulis adalah jumlah inang
	// dan harus di bawah atau sama dengan MAX_GATE.
	for(i=0;i<=((aturan.hostname_c<MAX_GATE)?aturan.hostname_c:MAX_GATE);i++){
		fprintf(cfg, "HOST=%s\r\n", (aturan.hostname[i])?aturan.hostname[i]:"");
	};
	
	// Menutup.
	if(fclose(cfg)!=0){
		// Gagal.
		FAIL ( 
			_("Gagal menutup berkas '%1$s': %2$s (%1$i)."),
			aturan.config, strerror(errno), errno
			);
		exit(EXIT_FAILURE_IO);
	};
	
	return 0;
}

/*
 * baca_konfigurasi()
 * Membaca konfigurasi berkas.
 * Kunci konfigurasi sah:
 *  HOST, GATEID, DEBUG, DEBUG1, DEBUG2, DEBUG3, DEBUG4, DEBUG5,
 *  SHOW_ERROR, SHOW_WARNING, SHOW_NOTICE, SHOW_INFO, VERBOSE, QUIET,
 *  RAWTRANSFER, TRIES, WAITRETRY, WAITQUEUE, PARALEL,
 *  COMPLETEDIR, TEMPDIR,
 *  LISTENING.
 */
int baca_konfigurasi(){
	if(
		!file_exist(aturan.config)
		|| !fsize(aturan.config)
	){
		buat_konfigurasi();
	};
	
	int status;
	
	// Membaca konfigurasi.
	FILE *cfg=fopen(aturan.config, "rb");
	if(cfg==NULL){
		// Gagal.
		FAIL ( 
			_("Gagal membuka berkas '%1$s': %2$s (%1$i)."),
			aturan.config, strerror(errno), errno
			);
		exit(EXIT_FAILURE_IO);
	};
	
	// Membaca inang.
	char kunci[INFOALAMAT_MAX_STR];
	memset(kunci, 0, INFOALAMAT_MAX_STR);
	char nilai[INFOALAMAT_MAX_STR];
	
	// Ke posisi awal.
	fseek(cfg, 0, SEEK_SET);
	
	// Mengambil isi.
	while(fscanf(cfg, "%[^=]=%[^\r]\r\n", kunci, nilai) != EOF){
		if(!strcmp(kunci, "HOST")){
			if(!is_nonascii(nilai) && strlen(nilai)){
				DEBUG3(_("Menambah inang %1$s dari berkas."), nilai);
				strcpy(aturan.hostname[aturan.hostname_c], nilai);
				aturan.hostname_c++;
			};
		}else if(!strcmp(kunci, "GATEID")){
			aturan.gateid=atoi(nilai);
		}else if(!strcmp(kunci, "DEBUG")){
			switch(atoi(nilai)){
				case 5:
					aturan.show_debug5=true;
				case 4:
					aturan.show_debug4=true;
				case 3:
					aturan.show_debug3=true;
				case 2:
					aturan.show_debug2=true;
				case 1:
					aturan.show_debug1=true;
				break;
			}
		}else if(!strcmp(kunci, "DEBUG1")){
			aturan.show_debug1=!strlen(nilai)?true:(s2b(nilai));
			if(aturan.show_debug1)
				DEBUG1(_("Kekutu tingkat %1$i aktif."), 1);
		}else if(!strcmp(kunci, "DEBUG2")){
			aturan.show_debug2=!strlen(nilai)?true:(s2b(nilai));
			if(aturan.show_debug2)
				DEBUG2(_("Kekutu tingkat %1$i aktif."), 2);
		}else if(!strcmp(kunci, "DEBUG3")){
			aturan.show_debug3=!strlen(nilai)?true:(s2b(nilai));
			if(aturan.show_debug3)
				DEBUG3(_("Kekutu tingkat %1$i aktif."), 3);
		}else if(!strcmp(kunci, "DEBUG4")){
			aturan.show_debug4=!strlen(nilai)?true:(s2b(nilai));
			if(aturan.show_debug4)
				DEBUG4(_("Kekutu tingkat %1$i aktif."), 4);
		}else if(!strcmp(kunci, "DEBUG5")){
			aturan.show_debug5=!strlen(nilai)?true:(s2b(nilai));
			if(aturan.show_debug5){
				// Debug5 merupakan pemecah karakter,
				// sehingga menggunakan debug lain untuk menampilkan pesan.
				aturan.show_debug5=true;
				status=aturan.show_debug4;
				aturan.show_debug4=true;
				DEBUG4(_("Kekutu tingkat %1$i aktif."), 5);
				aturan.show_debug4=status;
			};
		}else if(!strcmp(kunci, "SHOW_ERROR")){
			aturan.show_error=!strlen(nilai)?true:atoi(nilai);
		}else if(!strcmp(kunci, "SHOW_WARNING")){
			aturan.show_error=!strlen(nilai)?true:atoi(nilai);
		}else if(!strcmp(kunci, "SHOW_NOTICE")){
			aturan.show_notice=!strlen(nilai)?true:atoi(nilai);
		}else if(!strcmp(kunci, "SHOW_INFO")){
			aturan.show_info=!strlen(nilai)?true:atoi(nilai);
		}else if(!strcmp(kunci, "VERBOSE")){
			aturan.show_debug1=!strlen(nilai)?true:atoi(nilai);
		}else if(!strcmp(kunci, "QUIET")){
			aturan.show_error=false;
			aturan.show_warning=false;
			aturan.show_notice=false;
			aturan.show_info=false;
			aturan.show_debug1=false;
			aturan.show_debug2=false;
			aturan.show_debug3=false;
			aturan.show_debug4=false;
			aturan.show_debug5=false;
		}else if(!strcmp(kunci, "RAWTRANSFER")){
			aturan.rawtransfer=!strlen(nilai)?true:atoi(nilai);
		}else if(!strcmp(kunci, "TRIES")){
			aturan.tries=atoi(nilai);
		}else if(!strcmp(kunci, "WAITRETRY")){
			aturan.waitretry=atoi(nilai);
		}else if(!strcmp(kunci, "WAITQUEUE")){
			aturan.waitqueue=atoi(nilai);
		}else if(!strcmp(kunci, "PARALLEL")){
			aturan.parallel=atoi(nilai);
		}else if(!strcmp(kunci, "COMPLETEDIR")){
			strcpy(aturan.completedir, nilai);
		}else if(!strcmp(kunci, "TEMPDIR")){
			strcpy(aturan.tempdir, nilai);
		}else if(!strcmp(kunci, "LISTENING")){
			strcpy(aturan.listening, nilai);
		};
		
		memset(nilai, 0, INFOALAMAT_MAX_STR);
	}
	
	// Menutup.
	if(fclose(cfg)!=0){
		// Gagal.
		FAIL ( 
			_("Gagal menutup berkas '%1$s': %2$s (%1$i)."),
			aturan.config, strerror(errno), errno
			);
		exit(EXIT_FAILURE_IO);
	};
	return 0;
}
