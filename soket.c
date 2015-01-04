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
	struct INFOALAMAT *alamat
){
	// Pengaturan.
	int     tunggu_detik=5;
	int tunggu_milidetik=0;
	
	// Pengaturan.
	// Batas tunggu.
	struct timeval waktu_tunggu;      
	waktu_tunggu.tv_sec = tunggu_detik;
	waktu_tunggu.tv_usec = tunggu_milidetik;
	
	// Soket.
	int sockfd;
	// struct hostent *server;
	struct addrinfo set;
	// struct sockaddr_in *serv_sin;
	struct addrinfo
		*serv_addrinfo_result,
		*serv_addrinfo;
	char ipstr[INET6_ADDRSTRLEN];
	char *respons;
	
	// Alamat.
	// struct DAFTAR_ALAMAT *alamat_tmp;
	
	// Perilaku.
	bool ulangi=false;
	int kali_ulang=1;
	int ulang_maksimal=aturan.tries;
	int ulang_tunggu=aturan.waitretry;;
	int status=0;
	int len;
	int i, j;
	
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
				FAIL(_("Telah melakukan percobaan sebanyak %1$i kali. Berhenti"), ulang_maksimal);
				return NULL;
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
		memset(&set, 0, sizeof(struct addrinfo));
		set.ai_family = AF_UNSPEC; // IPV4 dan IPV6
		set.ai_socktype = SOCK_STREAM; // Aliran. Komunikasi dua arah.
		set.ai_protocol = 0; // Semua porta.
		
		// Mencari alamat.
		DEBUG3(_("Mulai mencari informasi alamat '%1$s'."), hostname);
		
		// Mencoba di tembolok.
		serv_addrinfo_result=NULL;
		for(i=0; i<INFOALAMAT_MAX_ID; i++){
			DEBUG4(_("Mencoba tembolok identifikasi '%1$i'."), i);
			if(!strlen(alamat->inang[i])){
				// Tidak ditemukan.
				DEBUG4(_("Alamat tidak ditemukan."), 0);
				
				// Lanjut.
				continue;
			}else{
				// Ditemukan alamat.
				DEBUG4(_("Ditemukan alamat '%1$s'."), alamat->inang[i]);
				
				// Membandingkan.
				if(strcmp(hostname, alamat->inang[i])!=0){
					// Tidak sesuai.
					DEBUG4(_("Alamat tidak sesuai."), 0);
					
					// Lanjut.
					continue;
				}else{
					// Ditemukan.
					DEBUG4(_("Alamat sesuai."), 0);
					
					// Membangun struktur.
					struct addrinfo *tmp_addrinfo_utm;
					struct addrinfo *tmp_addrinfo;
					for(j=0;j<alamat->ipcount[i];j++){
						DEBUG4(_("Memasukkan %1$s dari tembolok identifikasi '%2$i'."), _("Informasi Jaringan"), i);
						tmp_addrinfo=malloc(sizeof(struct addrinfo)+1);
						DEBUG4(_("Memasukkan %1$s (%2$i) dari tembolok identifikasi '%3$i'."), _("Keluarga Alamat"), alamat->ai_family[i][j], i);
						tmp_addrinfo->ai_family=alamat->ai_family[i][j];
						DEBUG4(_("Memasukkan %1$s (%2$i) dari tembolok identifikasi '%3$i'."), _("Jenis Soket"), alamat->ai_socktype[i][j], i);
						tmp_addrinfo->ai_socktype=alamat->ai_socktype[i][j];
						DEBUG4(_("Memasukkan %1$s (%2$i) dari tembolok identifikasi '%3$i'."), _("Jenis Protokolt"), alamat->ai_protocol[i][j], i);
						tmp_addrinfo->ai_protocol=alamat->ai_protocol[i][j];
						DEBUG4(_("Memasukkan %1$s (%2$i) dari tembolok identifikasi '%3$i'."), _("Panjang Alamat"), alamat->ai_addrlen[i][j], i);
						tmp_addrinfo->ai_addrlen=alamat->ai_addrlen[i][j];
						DEBUG4(_("Memasukkan %1$s dari tembolok identifikasi '%2$i'."), _("Nama Alias"), i);
						if(strlen(alamat->ai_canonname[i][j])){
							memcpy(tmp_addrinfo->ai_canonname, alamat->ai_canonname[i][j], INFOALAMAT_MAX_STR);
						};
						
						// Sockaddr.
						DEBUG4(_("Memasukkan %1$s dari tembolok identifikasi '%2$i'."), _("Informasi Alamat"), i);
						struct sockaddr *tmp_sockaddr;
						tmp_sockaddr=malloc(sizeof(struct sockaddr)+1);
						DEBUG4(_("Memasukkan %1$s (%2$i) dari tembolok identifikasi '%3$i'."), _("Keluarga Alamat"), alamat->sockaddr_sa_family[i][j], i);
						tmp_sockaddr->sa_family=alamat->sockaddr_sa_family[i][j];
						DEBUG4(_("Memasukkan %1$s dari tembolok identifikasi '%2$i'."), _("Data Alamat"), i);
						memcpy(tmp_sockaddr->sa_data, alamat->sockaddr_sa_data[i][j], 14);
						
						// Menyalin Sockaddr.
						DEBUG4(_("Menyalin %1$s dari tembolok identifikasi '%2$i'."), _("Informasi"), i);
						tmp_addrinfo->ai_addr=tmp_sockaddr;
						
						// Bila masih kurang.
						if(j==0){
							// Bila pertama.
							DEBUG4(_("Menyalin ke alamat sementara pertama."), 0);
							
							// Meletakkan di awal.
							tmp_addrinfo_utm=tmp_addrinfo;
							tmp_addrinfo_utm->ai_next=NULL;
						}else{
							// Bila masih berlanjut.
							DEBUG4(_("Menyalin ke alamat sementara lama."), 0);
							
							// Meletakkan di akhir.
							tmp_addrinfo->ai_next=NULL;
							tmp_addrinfo_utm->ai_next=tmp_addrinfo;
						};
						
						// Membersihkan.
						// DEBUG4(_("Membersihkan %1$s."), _("Soket Sementara"));
						// free(tmp_sockaddr);
					};
					// Memindahkan hasil.
					DEBUG4(_("Menyalin hasil tembolok."), 0);
					serv_addrinfo_result=tmp_addrinfo_utm;
					
					// Keluar.
					DEBUG4(_("Berhenti mencari alamat tembolok."), 0);
					break;
				};
			};
		};
		
		// Bila hasil kosong.
		if(serv_addrinfo_result==NULL){
			
			// Mencari di jaringan.
			DEBUG3(_("Mulai mencari informasi alamat inang '%1$s' di jaringan."), hostname);
			status = getaddrinfo(hostname, portno, &set, &serv_addrinfo_result);
			if (status != 0) {
			   // fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
				WARN(_("Gagal mendapatkan alamat '%1$s': %2$s (%3$i)."), hostname, gai_strerror(status), status);
				ulangi=true;
				continue;
			}else{
				// Pesan.
				DEBUG3(_("Menemukan informasi alamat inang '%1$s' di jaringan."), hostname);
			};
			
			// Memasukkan ke tembolok.
			DEBUG3(_("Mulai memasukkan informasi alamat inang '%1$s' ke tembolok."), hostname);
			
			// Perulangan.
			bool ulang=false;
			bool jangan_tanya_alamat=false;
			int coba=1;
			int id=0;
			// for(i=0; i<INFOALAMAT_MAX_ID; i++)
			do{
				// Pesan.
				DEBUG4(_("Mencoba memasukkan ke tembolok identifikasi '%1$i'."), id);
				
				// Bila alamat tidak kosong.
				if(!jangan_tanya_alamat && strlen(alamat->inang[id])){
					coba++;
					
					// Ditemukan alamat.
					DEBUG4(_("Ditemukan alamat '%1$s'."), alamat->inang[id]);
					
					// Memeriksa apakah alamat benar-benar ada
					// dengan memeriksa apakah ada isi.
					if(alamat->ipcount[id]<=0){
						// Masih kosong.
						DEBUG4(_("Jumlah IP adalah nol."), 0);
						DEBUG4(_("Kemungkinan alamat masih kosong."), 0);
						DEBUG4(_("Tetap melanjutkan."), 0);
						
						// Muat ulang.
						coba=0;
						jangan_tanya_alamat=true;
						ulang=true;
					}else if(coba>INFOALAMAT_MAX_ID){
						// Pesan.
						DEBUG4(_("Telah mencoba sebanyak %1$i kali."), coba);
						DEBUG4(_("Tetap melanjutkan."), 0);
						
						// Muat ulang.
						id=0;coba=0;
						jangan_tanya_alamat=true;
						ulang=true;
					}else{
						// Pesan.
						DEBUG4(_("Identifikasi selanjutnya."), 0);
						ulang=true;
						id++;
					};
					continue;
				}else{
					// Pesan.
					DEBUG4(_("Tembolok identifikasi '%1$i' dapat digunakan."), id);
					
					// Memasukkan.
					DEBUG4(_("Memasukkan %1$s ke tembolok identifikasi '%2$i'."), _("Nama Inang"), id);
					strcpy(alamat->inang[id], hostname);
					
					// Multidimensi
					j=0;
					for (
						serv_addrinfo = serv_addrinfo_result;
						serv_addrinfo != NULL;
						serv_addrinfo = serv_addrinfo->ai_next
					){
						// Memasukkan.
						DEBUG4(_("Memasukkan %1$s ke tembolok identifikasi '%2$i'."), _("Informasi Jaringan"), id);
						DEBUG4(_("Memasukkan %1$s (%2$i) ke tembolok identifikasi '%3$i'."), _("Keluarga Alamat"), serv_addrinfo->ai_family, id);
						alamat->ai_family[id][j]=serv_addrinfo->ai_family;
						DEBUG4(_("Memasukkan %1$s (%2$i) ke tembolok identifikasi '%3$i'."), _("Jenis Soket"), serv_addrinfo->ai_socktype, id);
						alamat->ai_socktype[id][j]=serv_addrinfo->ai_socktype;
						DEBUG4(_("Memasukkan %1$s (%2$i) ke tembolok identifikasi '%3$i'."), _("Protokol"), serv_addrinfo->ai_protocol, id);
						alamat->ai_protocol[id][j]=serv_addrinfo->ai_protocol;
						DEBUG4(_("Memasukkan %1$s (%2$i) ke tembolok identifikasi '%3$i'."), _("Panjang Alamat"), serv_addrinfo->ai_addrlen, id);
						alamat->ai_addrlen[id][j]=serv_addrinfo->ai_addrlen;
						DEBUG4(_("Memasukkan %1$s ke tembolok identifikasi '%2$i'."), _("Nama Alias"), id);
						if(serv_addrinfo->ai_canonname!=NULL){
						memcpy(
							alamat->ai_canonname[id][j], serv_addrinfo->ai_canonname,
							INFOALAMAT_MAX_STR
							);
						};//else
							// alamat->ai_canonname[id][j];
							
						DEBUG4(_("Memasukkan %1$s (%2$i) ke tembolok identifikasi '%3$i'."), _("Keluarga Alamat"), serv_addrinfo->ai_addr->sa_family, id);
						alamat->sockaddr_sa_family[id][j]=serv_addrinfo->ai_addr->sa_family;
						DEBUG4(_("Memasukkan %1$s ke tembolok identifikasi '%2$i'."), _("Data Alamat"), id);
						memcpy(alamat->sockaddr_sa_data[id][j], serv_addrinfo->ai_addr->sa_data, 14);
						
						// BIla lebih dari maksimal IP.
						if(j>=INFOALAMAT_MAX_IP){
							// Berhenti.
							break;
						}else{
							// Meningkat.
							j++;
						}
					};
					
					// Maksimal IP.
					alamat->ipcount[id]=j;
					
					// Ukuran memori bersama.
					// munmap(alamat, sizeof *alamat);
					
					// Berhenti.
					jangan_tanya_alamat=true;
					ulang=false;
				};
			}while(ulang);
		};
		
		
		// Pengaturan.
		// Gunakan ulang.
		int reuse_addr=1;
		
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
				FAIL(_("Keluarga alamat inang '%1$s' (%2$i) tidak diketahui."), hostname, serv_addrinfo->ai_family);
				return NULL;
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
				return NULL;
			}else{
				// Pesan.
				DEBUG3(_("Berhasil menggunakan ulang alamat."), 0);
			};
			
			// Pengaturan.
			// Batas tunggu.
			DEBUG3(_("Mulai mengatur batas waktu penerimaan."), 0);
			if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
				(struct timeval *)&waktu_tunggu, sizeof(struct timeval)) < 0 ){
				FAIL(_("Gagal mengatur batas waktu penerimaan: %1$s (%2$i)."), strerror(errno), errno);
				return NULL;
			}else{
				// Pesan.
				DEBUG3(_("Berhasil mengatur batas waktu penerimaan."), 0);
			};
			
			// Pengaturan.
			// Batas tunggu.
			DEBUG3(_("Mulai mengatur batas waktu pengiriman."), 0);
			if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO,
				(struct timeval *)&waktu_tunggu, sizeof(struct timeval)) < 0 ){
				FAIL(_("Gagal mengatur batas waktu pengiriman: %1$s (%2$i)."), strerror(errno), errno);
				return NULL;
			}else{
				// Pesan.
				DEBUG3(_("Berhasil mengatur batas waktu pengiriman."), 0);
			};
			
			// Membuka soket.
			DEBUG3(_("Mulai menghubungi '%1$s' (%2$s)."), hostname, ipstr);
			if (connect(sockfd, serv_addrinfo->ai_addr, serv_addrinfo->ai_addrlen) != -1){
				// Berhasil.
				DEBUG3(_("Berhasil menghubungi alamat '%1$s'."), ipstr);
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
			WARN(_("Gagal menghubungi inang '%1$s' (%2$s)."), hostname, ipstr);
			close(sockfd);
			ulangi=true;
			continue;
		}else{
			DEBUG3(_("Berhasil menghubungi inang '%1$s' (%2$s)."), hostname, ipstr);
		};
		
		// Bersihkan.
		// DEBUG3(_("Membersihkan memori %1$s."), _("Info Alamat"));
		// freeaddrinfo(serv_addrinfo_result);
		
		// Kirim pesan.
		len=MAX_CHUNK_SIZE;
		DEBUG3(_("Mulai menulis soket ke '%1$s' (%2$s)."), hostname, ipstr);
		if (!sendall(sockfd, pesan, &len)){
			FAIL(_("Kesalahan dalam menulis ke soket: %1$s (%2$i)."),strerror(errno), errno);
			return NULL;
		}else{
			// Pesan.
			DEBUG3(_("Berhasil menulis soket ke '%1$s' (%2$s)."), hostname, ipstr);
		};
		
		// Baca respons.
		DEBUG3(_("Menunggu balasan dari '%1$s' (%2$s)."), hostname, ipstr);
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
				return NULL;
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