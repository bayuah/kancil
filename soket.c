/* 
 * `soket.c`
 * Fungsi komunikasi soket.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */

#include "kancil.h"
#include "faedah.h"

#define _POSIX_C_SOURCE 200112L 

// Variabel global apakah sudah dimuat.
// #ifndef _KANCIL_PESAN_
	// char *PENYANGGA; 
	// char *POTONGAN;
	// char PESAN[MAX_CHUNK_SIZE];
	// char *hostname;
	// int  SOCK_PORT_TX;
// #endif /* _KANCIL_PESAN_ */

/*
 * getaddrfamily()
 * Menentukan keluarga alamat IP
 * @url: tdistler.com/?p=785
 * @penulis: Tom Distler
 * @HakCipta: (C) 2011 Tom Distler 
 * @param: addr (char*) Alamat inang.
 * @hasil: (int)
 *    AF_INET, AF_INET6,
 *    atau -1 bila galat
 */
int getaddrfamily(const char *addr){
	struct addrinfo hint, *info =0;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_UNSPEC;
	
	// Beri marka baris
	// di bawah untuk mematikan
	// pencarian DNS.
	hint.ai_flags = AI_NUMERICHOST;
	
	int ret = getaddrinfo(addr, 0, &hint, &info);
	if (ret)
		return -1;
	int result = info->ai_family;
	freeaddrinfo(info);
	return result;
}

/*
 * sendall()
 * Mengirim pesan terpecah-pecah.
 * @url: http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
 * @penulis: Brian "Beej Jorgensen" Hall <beej@beej.us>.
 * @HakCipta: (C) 2012 Brian "Beej Jorgensen" Hall
 * @hasil: (bool) `true` bila sukses
 */
bool sendall(int s, char *buf, int *len){
	int selesai = 0;        // Bita terkirim.
	int kurang = *len; // Bita tersisa.
	int n;

	while(selesai < *len) {
		n = send(s, buf+selesai, kurang, 0);
		if (n == -1) { break; }
		selesai += n;
		kurang -= n;
	};
	
	// Nilai hasil.
	*len = selesai;

	return n==-1?false:true;
}

/*
 * kirimdata()
 * Melakukan pengiriman data.
 */
char *kirimdata(
	char* pesan,
	char* hostname,
	char* portno,
	struct KIRIMBERKAS *kirim
){
	// Pengaturan.
	int     tunggu_detik=10;
	int tunggu_milidetik=0;
	
	// Soket.
	int sockfd;
	// struct hostent *server;
	struct addrinfo set;
	// struct sockaddr_in *serv_sin;
	struct addrinfo *serv_addrinfo_result, *serv_addrinfo;
	char ipstr[INET6_ADDRSTRLEN];
	char *respons;
	
	// Alamat.
	struct DAFTAR_ALAMAT *alamat_tmp;
	
	// Perilaku.
	bool ulangi=false;
	int kali_ulang=1;
	int ulang_maksimal=20;
	int ulang_tunggu=30;
	int status=0;
	int len;
	
	// Hasil;.
	char *rturn;
	
	do{
		// Bila ulangi.
		if(ulangi){
			if(kali_ulang<=ulang_maksimal){
				// Pesan.
				if(kali_ulang==1){
					NOTICE(_("Menunggu %1$i detik untuk mengirim ulang."), ulang_tunggu);
				}else{
					NOTICE(
						_("Menunggu %1$i detik untuk kirim ulang ke %2$i."),
						ulang_tunggu, kali_ulang
						);
				};
				
				// Menunggu.
				sleep(ulang_tunggu);
				
				// Menambah.
				kali_ulang++;
				ulangi=false;
			}else{
				// Pesan.
				FAIL(_("Telah melakukan percobaan sebanyak %1$i kali."), ulang_maksimal);
				exit(EXIT_FAILURE_SOCKET);
			};
		};
		
		// Buat titik soket.
		// DEBUG3(_("Mulai membuka soket."), 0);
		// sockfd = socket(AF_INET, SOCK_STREAM, 0);
		// if (sockfd < 0){
			// FAIL(_("Gagal membuka soket: %2$s (%3$i)."), strerror(errno), errno);
			// exit(EXIT_FAILURE_SOCKET);
		// }else{
			// Pesan.
			// DEBUG3(_("Berhasil membuka soket."), 0);
		// };
		
		// Menemukan alamat.
		// Alokasi memori.
		memset(&set, 0, sizeof(struct addrinfo)+1);
		set.ai_family = AF_UNSPEC; // IPV4 dan IPV6
		set.ai_socktype = SOCK_STREAM; // Aliran. Komunikasi dua arah.
		set.ai_protocol = 0; // Semua porta.
		
		// Mencari alamat.
		DEBUG3(_("Mulai mencari informasi alamat '%1$s'."), hostname);
		
		// Menyimpan posisi.
		alamat_tmp=kirim->alamat;
		
		// Mencoba di tembolok.
		// Mencari dari alamat_tmp.
		serv_addrinfo_result=NULL;
		
		bool lanjut_tembolok=false;
		do{
			DEBUG4(_("Mencoba identifikasi tembolok '%1$i'."), alamat_tmp->identifikasi);
			DEBUG4(_("Ditemukan alamat '%1$s'."), alamat_tmp->nama_inang);
			if(strcmp(hostname, alamat_tmp->nama_inang)==0){
				// Ditemukan.
				DEBUG4(_("Berhasil di identifikasi tembolok '%1$i'."), alamat_tmp->identifikasi);
				DEBUG3(_("Menemukan informasi alamat '%1$s' di tembolok."), hostname);
				
				// Memasukkan.
				serv_addrinfo_result=alamat_tmp->info;
			}else{
				if(alamat_tmp->lanjut!=NULL){
					// Melanjutkan.
					DEBUG4(_("Melanjutkan ke identifikasi selanjutnya."), 0);
					alamat_tmp=alamat_tmp->lanjut;
					lanjut_tembolok=true;
				}else{
					// Berhenti.
					DEBUG4(_("Gagal di identifikasi tembolok '%1$i'."), alamat_tmp->identifikasi);
					DEBUG3(_("Tidak menemukan informasi alamat '%1$s' di tembolok."), hostname);
					lanjut_tembolok=false;
				};
			};
		}while(lanjut_tembolok);
		
		// Bila hasil kosong.
		if(serv_addrinfo_result==NULL){
			
			// Mencari di jaringan.
			DEBUG3(_("Mulai mencari informasi alamat '%1$s' di jaringan."), hostname);
			status = getaddrinfo(hostname, portno, &set, &serv_addrinfo_result);
			if (status != 0) {
			   // fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
				FAIL(_("Gagal mendapatkan alamat '%1$s': %2$s (%3$i)."), hostname, gai_strerror(status), status);
				exit(EXIT_FAILURE_SOCKET);
			}else{
				// Pesan.
				DEBUG3(_("Menemukan informasi alamat '%1$s' di jaringan."), hostname);
			};
			
			// Memasukkan ke tembolok.
			// Memeriksa posisi penunjuk.
			DEBUG3(_("Mulai memasukkan informasi alamat '%1$s' ke tembolok."), hostname);
			
			// Alokasi memori.
			alamat_tmp=malloc(sizeof(struct DAFTAR_ALAMAT));
			alamat_tmp->info=malloc(sizeof(struct addrinfo));
			memset(alamat_tmp->nama_inang, 0, INET6_ADDRSTRLEN+1);
			alamat_tmp->lanjut=NULL;
			
			// Mengisi.
			strncpy(alamat_tmp->nama_inang, hostname, strlen(hostname)+1);
			alamat_tmp->info=serv_addrinfo_result;
			alamat_tmp->identifikasi=(kirim->alamat->identifikasi)+1;
			
			// Memasukkan.
			if(kirim->alamat->identifikasi==0){
				// Pesan.
				DEBUG4(_("Memasukkan informasi alamat '%1$s' ke tembolok muda."), hostname);
				
				// Masukan pertama.
				alamat_tmp->lanjut=NULL;
				kirim->alamat=alamat_tmp;
				
			}else{
				// Pesan.
				DEBUG4(_("Memasukkan informasi alamat '%1$s' ke tembolok tua."), hostname);
				
				// Merambah.
				// while(alamat->lanjut != NULL){
					// alamat=alamat->lanjut;
				// };
				
				// Memasukkan di awal.
				alamat_tmp->lanjut=kirim->alamat;
				kirim->alamat=alamat_tmp;
			};
		};
		
		// Pengaturan.
		// Gunakan ulang.
		int reuse_addr=1;
		
		// Pengaturan.
		// Batas tunggu.
		struct timeval timeout;      
		timeout.tv_sec = tunggu_detik;
		timeout.tv_usec = tunggu_milidetik;
		
		// Mencari alamat.
		// Bila gagal, mencoba alamat selanjutnya.
		for (
			serv_addrinfo = serv_addrinfo_result;
			serv_addrinfo != NULL;
			serv_addrinfo = serv_addrinfo->ai_next
		){
			
			DEBUG3(_("Mulai mencari alamat inang '%1$s'."), hostname);
			// Mendapatkan alamat IP.
			if (serv_addrinfo->ai_family == AF_INET){
				// Pesan.
				DEBUG3(_("Inang '%1$s' adalah %2$s."), hostname, _("IPV4"));
				
				// Mengubah jenis struktur data.
				struct sockaddr_in *ipv = (struct sockaddr_in *)serv_addrinfo->ai_addr;
				
				// Mendapatkan alamat IP.
				inet_ntop(AF_INET, &(ipv->sin_addr), ipstr, sizeof ipstr);
				
			}else if(serv_addrinfo->ai_family == AF_INET6) {
				// Pesan.
				DEBUG3(_("Inang '%1$s' adalah %2$s."), hostname, _("IPV6"));
				
				// Mengubah jenis struktur data.
				struct sockaddr_in6 *ipv = (struct sockaddr_in6 *)serv_addrinfo->ai_addr;
				
				// Mendapatkan alamat IP.
				inet_ntop(AF_INET6, &(ipv->sin6_addr), ipstr, sizeof ipstr);
				
			}else{
				// Kesalahan.
				FAIL(_("Keluarga alamat inang '%1$s' tidak diketahui."), hostname);
				exit(EXIT_FAILURE_SOCKET);
			};
			
			// Menemukan alamat.
			if(ipstr!=NULL){
				DEBUG3(_("Menemukan alamat '%1$s' untuk inang '%2$s'."), ipstr, hostname);
			}else{
				DEBUG3(_("Menemukan alamat inang '%1$s'."), hostname);
			};
			
			DEBUG3(_("Mulai membuka soket."), 0);
			// Membuka soket.
			sockfd = socket(
				serv_addrinfo->ai_family,
				serv_addrinfo->ai_socktype,
				serv_addrinfo->ai_protocol
				);
			
			// Bila gagal.
			// Lanjut.
			if (sockfd == -1){
				// Pesan.
				DEBUG3(
					_("Gagal membuka soket di alamat '%1$s': %2$s (%3$i)."),
					ipstr, strerror(errno), errno
					);
				DEBUG3(_("Melanjutkan mencari alamat inang '%1$s'."), hostname);
				continue;
			};
			
			// Pengaturan.
			// Gunakan alamat kembali.
			DEBUG3(_("Mulai menggunakan ulang alamat."), 0);
			if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
				&reuse_addr, sizeof(int)) < 0 ){
				FAIL(_("Gagal menggunakan ulang alamat: %2$s (%3$i)."), strerror(errno), errno);
				exit(EXIT_FAILURE_SOCKET);
			}else{
				// Pesan.
				DEBUG3(_("Berhasil menggunakan ulang alamat."), 0);
			};
			
			// Pengaturan.
			// Batas tunggu.
			DEBUG3(_("Mulai mengatur batas waktu penerimaan."), 0);
			if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
				(struct timeval *)&timeout, sizeof(struct timeval)) < 0 ){
				FAIL(_("Gagal mengatur batas waktu penerimaan: %1$s (%2$i)."), strerror(errno), errno);
				exit(EXIT_FAILURE_SOCKET);
			}else{
				// Pesan.
				DEBUG3(_("Berhasil mengatur batas waktu penerimaan."), 0);
			};
			
			// Pengaturan.
			// Batas tunggu.
			DEBUG3(_("Mulai mengatur batas waktu pengiriman."), 0);
			if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO,
				(struct timeval *)&timeout, sizeof(struct timeval)) < 0 ){
				FAIL(_("Gagal mengatur batas waktu pengiriman: %1$s (%2$i)."), strerror(errno), errno);
				exit(EXIT_FAILURE_SOCKET);
			}else{
				// Pesan.
				DEBUG3(_("Berhasil mengatur batas waktu pengiriman."), 0);
			};
			
			// Membuka soket.
			DEBUG3(_("Mulai menghubungi '%1$s' (%2$s)."), hostname, ipstr);
			if (connect(sockfd, serv_addrinfo->ai_addr, serv_addrinfo->ai_addrlen) != -1){
				// Berhasil.
				break;
			}else{
				// Pesan.
				DEBUG3(_("Melanjutkan mencari alamat '%1$s'."), hostname);
			};
			
			// Menutup sambungan.
			close(sockfd);
		};
		
		// Bila menemukan kesalahan.
		if (serv_addrinfo == NULL) {
			// Tidak berhasil.
			FAIL(_("Gagal menghubungi inang '%1$s' (%2$s)."), hostname, ipstr);
			exit(EXIT_FAILURE_SOCKET);
		}else{
			DEBUG3(_("Berhasil menghubungi inang '%1$s' (%2$s)."), hostname, ipstr);
		};
		
		// Bersihkan.
		DEBUG3(_("Membersihkan memori %1$s."), _("Info Alamat"));
		freeaddrinfo(serv_addrinfo_result);
		
		// Kirim pesan.
		len=MAX_CHUNK_SIZE;
		DEBUG3(_("Mulai menulis soket ke '%1$s' (%2$s)."), hostname, ipstr);
		if (!sendall(sockfd, pesan, &len)){
			FAIL(_("Kesalahan dalam menulis ke soket: %1$s (%2$i)."),strerror(errno), errno);
			exit(EXIT_FAILURE_SOCKET);
		}else{
			// Pesan.
			DEBUG3(_("Berhasil menulis soket ke '%1$s' (%2$s)."), hostname, ipstr);
		};
		
		// Baca respons.
		respons=malloc(sizeof(respons) * MAX_CHUNK_SIZE);
		memset(respons, 0, MAX_CHUNK_SIZE);
		status = recv(sockfd, respons, MAX_CHUNK_SIZE, MSG_WAITALL);
		if (status < 0){
			if(errno==11){
				WARN(_("Balasan dari '%1$s' terlalu lama."), hostname);
				close(sockfd);
				ulangi=true;
				continue;
			}else{
				FAIL(_("Kesalahan dalam membaca soket: %1$s (%2$i)."),strerror(errno), errno);
				exit(EXIT_FAILURE_SOCKET);
			};
		}else{
			rturn=respons;
			// Pesan.
			DEBUG3(_("Berhasil mendapatkan jawaban dari '%1$s' (%2$s)."), hostname, ipstr);
		};
		
		// Selesai.
		kali_ulang=0;
		close(sockfd);
	}while(ulangi);
	
	// Balasan.
	return rturn;
}