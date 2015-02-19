/*
 * `argumen.c`
 * Pengurai argumen dari kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014 - 2015
 * Lisensi: lihat LICENCE.txt
 */

#define _GNU_SOURCE
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "kancil.h"
#include "return.h"
#include "faedah.h"
#include "rsa.h"
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
bool info_sudah_tampil;

// Memeriksa jenis program.
bool is_klien(){
	return !strcasecmp(infokancil.progcode, "KANCIL_KLIEN");
}
bool is_gerbang(){
	return !strcasecmp(infokancil.progcode, "KANCIL_GERBANG");
}
bool is_peladen(){
	return !strcasecmp(infokancil.progcode, "KANCIL_PELADEN");
}
bool is_kanciltest(){
	return !strcasecmp(infokancil.progcode, "KANCIL_TEST");
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
			{"host",         required_argument, 0, 'H'},
			{"config",       required_argument, 0, 'c'},
			{"logfile",      required_argument, 0, 'l'},
			{"listening",    required_argument, 0, 'L'},
			{"debug",        required_argument, 0, 'd'},
			{"debug1",       no_argument,       0, '1'},
			{"debug2",       no_argument,       0, '2'},
			{"debug3",       no_argument,       0, '3'},
			{"debug4",       no_argument,       0, '4'},
			{"debug5",       no_argument,       0, '5'},
			{"debuglevel",   required_argument, 0, 'D'},
			{"show-error",   no_argument,       0, 'E'},
			{"show-warning", no_argument,       0, 'W'},
			{"show-notice",  no_argument,       0, 'N'},
			{"show-info",    no_argument,       0, 'I'},
			{"verbose",      no_argument,       0, 'v'},
			{"quiet",        no_argument,       0, 'q'},
			{"gateid",       required_argument, 0, 'G'},
			{"gatesnum",     required_argument, 0, 'g'},
			{"timebase",     required_argument, 0, 'B'},
			{"tries",        no_argument,       0, 't'},
			{"parallel",     no_argument,       0, 'p'},
			{"shifteof",     no_argument,       0, 'S'},
			{"rsapadding",   no_argument,       0, 'P'},
			{"version",      no_argument,       0, 'V'},
			{"help",         no_argument,       0, 'h'},
			{0,              0,                 0,  0 }
		};
		
		// Mendapatkan argumen.
		int c = getopt_long(argc, argv, "12345B:c:d:D:g:G:H:hl:L:p:P:RrSvVq?",
				 long_options, &option_index);
		if (c == -1)
			break;
		
		// Informasi.
		if(
			!info_sudah_tampil
			&& c!='q'
		)info_versi();
		
		switch (c) {
		case '1':
			aturan.show_debug1=true;
			DEBUG1(_("Argumen: Kekutu tingkat %1$i aktif."), 1);
			break;
		case '2':
			aturan.show_debug2=true;
			DEBUG2(_("Argumen: Kekutu tingkat %1$i aktif."), 2);
			break;
		case '3':
			aturan.show_debug3=true;
			DEBUG2(_("Argumen: Kekutu tingkat %1$i aktif."), 3);
			break;
		case '4':
			aturan.show_debug4=true;
			DEBUG4(_("Argumen: Kekutu tingkat %1$i aktif."), 4);
			break;
		case '5':
			// Debug5 merupakan pemecah karakter,
			// sehingga menggunakan debug lain untuk menampilkan pesan.
			aturan.show_debug5=true;
			status=aturan.show_debug4;
			aturan.show_debug4=true;
			DEBUG4(_("Argumen: Kekutu tingkat %1$i aktif."), 5);
			aturan.show_debug4=status;
			break;
		case 'c':
			if(!is_nonascii(optarg) && strlen(optarg)){
				DEBUG1(_("Berkas konfigurasi %1$s."), optarg);
				strcpy(aturan.config, optarg);
			};
			break;
		case 'E':
			aturan.show_error=true;
			DEBUG1(_("Argumen: Menampilkan kesalahan."), 0);
			break;
		case 'W':
			aturan.show_warning=true;
			DEBUG1(_("Argumen: Menampilkan kesalahan."), 0);
			break;
		case 'N':
			aturan.show_notice=true;
			DEBUG1(_("Argumen: Menampilkan maklumat."), 0);
			break;
		case 'I':
			aturan.show_info=true;
			DEBUG1(_("Argumen: Menampilkan informasi."), 0);
			break;
		case 'v':
			aturan.show_debug1=true;
			DEBUG1(_("Argumen: Menampilkan kekutu."), 0);
			break;
		case 'd':
			status=atoi(optarg);
			if(status>=1&&status<=5){
				INFO(_("Kekutu tingkat %1$i."), status);
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
			};
			break;
		case 'D':
			status=aturan.debuglevel;
			if(!strcasecmp("MINI_DEBUG",optarg)){
				status=MINI_DEBUG;
				DEBUG1(_("Argumen: Tampilan kekutu %s."), "MINI_DEBUG");
			}else if(!strcasecmp("MEDIUM_DEBUG",optarg)){
				status=MEDIUM_DEBUG;
				DEBUG1(_("Argumen: Tampilan kekutu %s."), "MEDIUM_DEBUG");
			}else if(!strcasecmp("FULL_DEBUG",optarg)){
				status=FULL_DEBUG;
				DEBUG1(_("Argumen: Tampilan kekutu %s."), "FULL_DEBUG");
			}else{
				switch(atoi(optarg)){
					case MINI_DEBUG:
						status=MINI_DEBUG;
						DEBUG1(
							_("Argumen: Tampilan kekutu %s."),
							"MINI_DEBUG"
						);
					break;
					case MEDIUM_DEBUG:
						status=MEDIUM_DEBUG;
						DEBUG1(
							_("Argumen: Tampilan kekutu %s."),
							"MEDIUM_DEBUG"
						);
					break;
					case FULL_DEBUG:
						status=FULL_DEBUG;
						DEBUG1(
							_("Argumen: Tampilan kekutu %s."),
							"FULL_DEBUG"
						);
					break;
				}
			};
			// Menyimpan.
			aturan.debuglevel=status;
			break;
		case 'q':
			aturan.quiet=true;
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
		case 'H':
			// Menambah inang tujuan.
			if(!is_nonascii(optarg) && strlen(optarg)){
				DEBUG1(_("Argumen: Menambah inang %1$s."), optarg);
				strcpy(aturan.hostname[aturan.hostname_c], optarg);
				aturan.hostname_c++;
			};
			break;
		case 'B':
			// Menambah inang tujuan.
			status=atoi(optarg);
			if(status){
				DEBUG1(
					_("Argumen: Basis waktu pemilihan gerbang adalah %1$i detik."),
					status
				);
				aturan.timebase=status;
			};
			break;
		case 'g':
			status=atoi(optarg);
			if(status){
				DEBUG1(_("Argumen: Jumlah gerbang adalah %1$i."), status);
				aturan.gates_c=status;
			};
			break;
		case 'G':
			if(!is_nonascii(optarg) && strlen(optarg)){
				status=atoi(optarg);
				DEBUG1(_("Argumen: ID gerbang adalah %1$i."), status);
				aturan.gateid=status;
			};
			break;
		case 'L':
			status=atoi(optarg);
			if(status){
				DEBUG1(_("Argumen: Mendengarkan di porta %1$i."), status);
				strcpy(aturan.listening, optarg);
			};
			break;
		case 'p':
			DEBUG1(_("Paralel sebanyak %s sambungan."), optarg);
			aturan.parallel=atoi(optarg);
			break;
		case 'P':
			status=aturan.rsa_padding;
			if(!strcasecmp("RSA_PKCS1_PADDING",optarg)){
				status=RSA_PKCS1_PADDING;
				DEBUG1(
					_("Argumen: RSA menggunakan bantalan %s."),
					"RSA_PKCS1_PADDING"
				);
			}else if(!strcasecmp("RSA_SSLV23_PADDING",optarg)){
				status=RSA_SSLV23_PADDING;
				DEBUG1(
					_("Argumen: RSA menggunakan bantalan %s."),
					"RSA_SSLV23_PADDING"
				);
			}else if(!strcasecmp("RSA_PKCS1_OAEP_PADDING",optarg)){
				status=RSA_PKCS1_OAEP_PADDING;
				DEBUG1(
					_("Argumen: RSA menggunakan bantalan %s."),
					"RSA_PKCS1_OAEP_PADDING"
				);
			}else if(!strcasecmp("RSA_NO_PADDING",optarg)){
				status=RSA_NO_PADDING;
				DEBUG1(
					_("Argumen: RSA menggunakan bantalan %s."),
					"RSA_NO_PADDING"
				);
			}else{
				switch(atoi(optarg)){
					case RSA_PKCS1_PADDING:
						status=RSA_PKCS1_PADDING;
						DEBUG1(
							_("Argumen: RSA menggunakan bantalan %s."),
							"RSA_PKCS1_PADDING"
						);
					break;
					case RSA_SSLV23_PADDING:
						status=RSA_SSLV23_PADDING;
						DEBUG1(
							_("Argumen: RSA menggunakan bantalan %s."),
							"RSA_SSLV23_PADDING"
						);
					break;
					case RSA_PKCS1_OAEP_PADDING:
						status=RSA_PKCS1_OAEP_PADDING;
						DEBUG1(
							_("Argumen: RSA menggunakan bantalan %s."),
							"RSA_PKCS1_OAEP_PADDING"
						);
					break;
					case RSA_NO_PADDING:
						status=RSA_NO_PADDING;
						DEBUG1(
							_("Argumen: RSA menggunakan bantalan %s."),
							"RSA_NO_PADDING"
						);
					break;
				}
			};
			// Menyimpan.
			aturan.parallel=status;
			break;
		case 'R':
			DEBUG1(_("Argumen: Transfer mentah."), 0);
			aturan.rawtransfer=true;
			break;
		case 'r':
			DEBUG1(_("Argumen: Transfer terenkripsi."), 0);
			aturan.rawtransfer=false;
			break;
		case 'S':
			DEBUG1(_("Argumen: Menggeser pembacaan akhir berkas."), 0);
			aturan.shifteof=true;
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
			FAIL(_("Argumen: Karakter tidak dikenal ASCII-%i"), c);
		}
	}
	
	if (optind < argc) {
		char arg_sisa[argc][BERKAS_MAX_STR+1];
		memset(arg_sisa, 0, BERKAS_MAX_STR+1);
		int i=0;
		
		// Memindah 
		while (optind < argc)
			strcpy(arg_sisa[i++], argv[optind++]);
		
		// Mendapatkan argumen tanpa kunci.
		if(i>0 && is_klien()){
			// Klien minimal adalah
			// BERKAS dikirim.
			strcpy(aturan.inputfile, arg_sisa[0]);
			if(i>1
				&&!is_nonascii(arg_sisa[1])
				&& strlen(arg_sisa[1])
			){
				// Informasi.
				if(!info_sudah_tampil)
					info_versi();
				
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
				// Informasi.
				if(!info_sudah_tampil)
					info_versi();
				
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
		if(!is_peladen() && !is_gerbang() && !is_kanciltest()){
			// Peladen dan Gerbang dapat
			// tanpa argumen minimal.
			bantuan_param_standar();
			exit(EXIT_FAILURE_ARGS);
		};
	}
}

// Bantuan.
void bantuan_param_standar(){
	// Tidak tampil.
	if(aturan.quiet)
		return;
	
	if(!info_sudah_tampil)
		info_versi();
	
	if(is_klien()){
		printf(
			_("Gunakan: %1$s BERKAS [INANG[:PORTA]] [--config=BERKAS] [OPSI]."),
			infokancil.executable
		);
		printf("\n");
	}else if(is_gerbang()){
		printf(
			_("Gunakan: %1$s PORTA INANG[:PORTA] [OPSI]."),
			infokancil.executable
		);
		printf("\n");
	}else if(is_peladen()){
		printf(_("Gunakan: %1$s [PORTA] [OPSI]."), infokancil.executable);
		printf("\n");
	}else{
		printf(_("Gunakan: %1$s."), infokancil.executable);
		printf("\n");
	};
	
}
void bantuan(){
	
}

/*
 * Informasi versi.
 */
void info_versi(){
	info_sudah_tampil=true;
	
	// Tidak tampil.
	if(aturan.quiet)
		return;
	
	// Membangun.
	char BUILT_VERSION[64];
	snprintf(BUILT_VERSION, 64,
		"%1$i.%2$i.%3$i.%4$i-%5$s",
		infokancil.version_major,
		infokancil.version_minor,
		infokancil.version_patch,
		infokancil.built_number,
		infokancil.compile_mode
	);
	
	// Tampilan.
	printf("%1$s (%2$s).\n", infokancil.progname, BUILT_VERSION);
	memset(BUILT_VERSION, 0, 64);
}

/*
 * Informasi kancil.
 */
void info_bangun(){
	time_t BUILT_TIME;
	char BUILT_TIME_STR[50];
	struct tm *lcltime;
	BUILT_TIME=infokancil.built_time;
	lcltime = localtime ( &BUILT_TIME );
	strftime(BUILT_TIME_STR, sizeof(BUILT_TIME_STR), "%c", lcltime);
	
	// Awal tampil.
	printf(
		_("Dibangun pada %1$s. Protokol versi %2$i."),
		BUILT_TIME_STR, PROTOCOL_VERSION
	);
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
	printf(_("Lisensi: \"THE COFFEE-WARE\" (Revision 1991)"));
	printf("\n\n");
	printf(
		_("Ditulis oleh %1$s <%2$s>."),
		"Bayu Aditya H.", "b@yuah.web.id"
	);
	printf("\n");
	info_tanya();
}

void info_tanya(){
	printf(
		_("Kirim laporan kesalahan dan pertanyaan ke %1$s."),
		"bug@yuah.web.id"
	);
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
	
	// Bila ada konfigurasi.
	if(!strlen(aturan.config) || is_nonascii(aturan.config)){
		DEBUG1(_("Tidak menentukan berkas konfigurasi"), 0);
		return -1;
	};
	
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
	int i;
	while(fscanf(cfg, "%[^=]=%[^\n]\n", kunci, nilai) != EOF){
		// Berhenti di karakter CR.
		for (i=0; nilai[i] != '\0'; i++) {
			if (nilai[i] == '\r')
				break;
		}nilai[i]='\0';
		
		// Membandingkan.
		if(!strcasecmp(kunci, "HOST")){
			if(!is_nonascii(nilai) && strlen(nilai)){
				DEBUG3(_("Berkas pengaturan: Menambah inang %1$s."), nilai);
				strcpy(aturan.hostname[aturan.hostname_c], nilai);
				aturan.hostname_c++;
			};
		}else if(!strcasecmp(kunci, "GATEID")){
			aturan.gateid=atoi(nilai);
		}else if(!strcasecmp(kunci, "DEBUG")){
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
		}else if(!strcasecmp(kunci, "DEBUG1")){
			aturan.show_debug1=!strlen(nilai)?true:(s2b(nilai));
			if(aturan.show_debug1)
				DEBUG1(_("Berkas pengaturan: Kekutu tingkat %1$i aktif."), 1);
		}else if(!strcasecmp(kunci, "DEBUG2")){
			aturan.show_debug2=!strlen(nilai)?true:(s2b(nilai));
			if(aturan.show_debug2)
				DEBUG2(_("Berkas pengaturan: Kekutu tingkat %1$i aktif."), 2);
		}else if(!strcasecmp(kunci, "DEBUG3")){
			aturan.show_debug3=!strlen(nilai)?true:(s2b(nilai));
			if(aturan.show_debug3)
				DEBUG3(_("Berkas pengaturan: Kekutu tingkat %1$i aktif."), 3);
		}else if(!strcasecmp(kunci, "DEBUG4")){
			aturan.show_debug4=!strlen(nilai)?true:(s2b(nilai));
			if(aturan.show_debug4)
				DEBUG4(_("Berkas pengaturan: Kekutu tingkat %1$i aktif."), 4);
		}else if(!strcasecmp(kunci, "DEBUG5")){
			aturan.show_debug5=!strlen(nilai)?true:(s2b(nilai));
			if(aturan.show_debug5){
				// Debug5 merupakan pemecah karakter,
				// sehingga menggunakan debug lain untuk menampilkan pesan.
				aturan.show_debug5=true;
				status=aturan.show_debug4;
				aturan.show_debug4=true;
				DEBUG4(_("Berkas pengaturan: Kekutu tingkat %1$i aktif."), 5);
				aturan.show_debug4=status;
			};
		}else if(!strcasecmp(kunci, "SHOW_ERROR")){
			aturan.show_error=!strlen(nilai)?true:s2b(nilai);
		}else if(!strcasecmp(kunci, "SHOW_WARNING")){
			aturan.show_error=!strlen(nilai)?true:s2b(nilai);
		}else if(!strcasecmp(kunci, "SHOW_NOTICE")){
			aturan.show_notice=!strlen(nilai)?true:s2b(nilai);
		}else if(!strcasecmp(kunci, "SHOW_INFO")){
			aturan.show_info=!strlen(nilai)?true:s2b(nilai);
		}else if(!strcasecmp(kunci, "VERBOSE")){
			aturan.show_debug1=!strlen(nilai)?true:s2b(nilai);
		}else if(!strcasecmp(kunci, "QUIET")){
			aturan.show_error=false;
			aturan.show_warning=false;
			aturan.show_notice=false;
			aturan.show_info=false;
			aturan.show_debug1=false;
			aturan.show_debug2=false;
			aturan.show_debug3=false;
			aturan.show_debug4=false;
			aturan.show_debug5=false;
		}else if(!strcasecmp(kunci, "RAWTRANSFER")){
			aturan.rawtransfer=!strlen(nilai)?true:s2b(nilai);
		}else if(!strcasecmp(kunci, "GATESNUM")){
			status=atoi(nilai);
			if(status){
				aturan.gates_c=status;
			};
		}else if(!strcasecmp(kunci, "GATEID")){
			if(!is_nonascii(nilai) && strlen(nilai)){
				status=atoi(nilai);
				aturan.gateid=status;
			};
		}else if(!strcasecmp(kunci, "TIMEBASE")){
			status=atoi(nilai);
			if(status){
				aturan.timebase=status;
			};
		}else if(!strcasecmp(kunci, "SHIFTEOF")){
			aturan.show_debug1=!strlen(nilai)?true:s2b(nilai);
		}else if(!strcasecmp(kunci, "TRIES")){
			aturan.tries=atoi(nilai);
		}else if(!strcasecmp(kunci, "WAITRETRY")){
			aturan.waitretry=atoi(nilai);
		}else if(!strcasecmp(kunci, "WAITQUEUE")){
			aturan.waitqueue=atoi(nilai);
		}else if(!strcasecmp(kunci, "PARALLEL")){
			aturan.parallel=atoi(nilai);
		}else if(!strcasecmp(kunci, "COMPLETEDIR")){
			strcpy(aturan.completedir, nilai);
		}else if(!strcasecmp(kunci, "TEMPDIR")){
			strcpy(aturan.tempdir, nilai);
		}else if(!strcasecmp(kunci, "LISTENING")){
			status=atoi(nilai);
			if(status){
				DEBUG1(_("Berkas pengaturan: Mendengarkan di porta %1$i."), status);
				strcpy(aturan.listening, nilai);
			};
		}else if(!strcasecmp(kunci, "DEBUGLEVEL")){
			status=aturan.debuglevel;
			if(!strcasecmp("MINI_DEBUG",nilai)){
				status=MINI_DEBUG;
				DEBUG1(_("Berkas pengaturan: Tampilan kekutu %s."), "MINI_DEBUG");
			}else if(!strcasecmp("MEDIUM_DEBUG",nilai)){
				status=MEDIUM_DEBUG;
				DEBUG1(_("Berkas pengaturan: Tampilan kekutu %s."), "MEDIUM_DEBUG");
			}else if(!strcasecmp("FULL_DEBUG",nilai)){
				status=FULL_DEBUG;
				DEBUG1(_("Berkas pengaturan: Tampilan kekutu %s."), "FULL_DEBUG");
			}else{
				switch(atoi(nilai)){
					case MINI_DEBUG:
						status=MINI_DEBUG;
						DEBUG1(_("Berkas pengaturan: Tampilan kekutu %s."), "MINI_DEBUG");
					break;
					case MEDIUM_DEBUG:
						status=MEDIUM_DEBUG;
						DEBUG1(_("Berkas pengaturan: Tampilan kekutu %s."), "MEDIUM_DEBUG");
					break;
					case FULL_DEBUG:
						status=FULL_DEBUG;
						DEBUG1(_("Berkas pengaturan: Tampilan kekutu %s."), "FULL_DEBUG");
					break;
				}
			};
			// Menyimpan.
			aturan.debuglevel=status;
		}else if(!strcasecmp(kunci, "RSAPADDING")){
			status=aturan.rsa_padding;
			if(!strcasecmp("RSA_PKCS1_PADDING",nilai)){
				status=RSA_PKCS1_PADDING;
				DEBUG1(
					_("Berkas pengaturan: RSA menggunakan bantalan %s."),
					"RSA_PKCS1_PADDING"
				);
			}else if(!strcasecmp("RSA_SSLV23_PADDING",nilai)){
				status=RSA_SSLV23_PADDING;
				DEBUG1(
					_("Berkas pengaturan: RSA menggunakan bantalan %s."),
					"RSA_SSLV23_PADDING"
				);
			}else if(!strcasecmp("RSA_PKCS1_OAEP_PADDING",nilai)){
				status=RSA_PKCS1_OAEP_PADDING;
				DEBUG1(
					_("Berkas pengaturan: RSA menggunakan bantalan %s."),
					"RSA_PKCS1_OAEP_PADDING"
				);
			}else if(!strcasecmp("RSA_NO_PADDING",nilai)){
				status=RSA_NO_PADDING;
				DEBUG1(
					_("Berkas pengaturan: RSA menggunakan bantalan %s."),
					"RSA_NO_PADDING"
				);
			}else{
				switch(atoi(nilai)){
					case RSA_PKCS1_PADDING:
						status=RSA_PKCS1_PADDING;
						DEBUG1(
							_("Berkas pengaturan: RSA menggunakan bantalan %s."),
							"RSA_PKCS1_PADDING"
						);
					break;
					case RSA_SSLV23_PADDING:
						status=RSA_SSLV23_PADDING;
						DEBUG1(
							_("Berkas pengaturan: RSA menggunakan bantalan %s."),
							"RSA_SSLV23_PADDING"
						);
					break;
					case RSA_PKCS1_OAEP_PADDING:
						status=RSA_PKCS1_OAEP_PADDING;
						DEBUG1(
							_("Berkas pengaturan: RSA menggunakan bantalan %s."),
							"RSA_PKCS1_OAEP_PADDING"
						);
					break;
					case RSA_NO_PADDING:
						status=RSA_NO_PADDING;
						DEBUG1(
							_("Berkas pengaturan: RSA menggunakan bantalan %s."),
							"RSA_NO_PADDING"
						);
					break;
				}
			};
			// Menyimpan.
			aturan.parallel=status;
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
