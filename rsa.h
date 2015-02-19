/* 
 * `rsa.h`
 * Penangan algoritma RSA untuk kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014 - 2015
 * Lisensi: lihat LICENCE.txt
 */
#ifndef _KANCIL_RSA_H_
#define _KANCIL_RSA_H_

// Menampilan kesalahan.
#include <openssl/err.h>

// Makro.
#ifndef RSA_PKCS1_PADDING
	#define RSA_PKCS1_PADDING 1
#endif
#ifndef RSA_PKCS1_OAEP_PADDING
	#define RSA_PKCS1_OAEP_PADDING 4
#endif
#ifndef RSA_SSLV23_PADDING
	#define RSA_SSLV23_PADDING 2
#endif
#ifndef RSA_NO_PADDING
	#define RSA_NO_PADDING 3
#endif

// Fungsi.
int rsa_encrypt(
	unsigned char *data,
	int data_len,
	unsigned char *key,
	unsigned char *target, 
	int bantalan
	);
int rsa_decrypt(
	unsigned char *data,
	int data_len,
	unsigned char *key,
	unsigned char *target,
	int bantalan
	);
unsigned char *default_rsa_privatekey();
unsigned char *default_rsa_pubkey();

#endif /* _KANCIL_RSA_H_ */