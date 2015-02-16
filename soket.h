/* 
 * `soket.h`
 * Fungsi komunikasi soket.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */

#ifndef _KANCIL_SOKET_
#define _KANCIL_SOKET_

// Fungsi.
bool sendall(int s, char *buf, int *len);
int getaddrfamily(const char *addr);
bool is_ipaddrs(char *str);
char *kirimdata(
	char* pesan,
	int panjang_pesan,
	char* hostname,
	char* portno,
	struct INFOALAMAT *alamat,
	int *panjang_diterima
);

// Variabel global.
char *PENYANGGA; 
char *POTONGAN;
char PESAN[MAX_CHUNK_SIZE];
char *SOCK_HOST_TX;
int  SOCK_PORT_TX;

#endif /* _KANCIL_PESAN_ */
