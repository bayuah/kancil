/* 
 * `rsa.h`
 * Penangan algoritma RSA untuk kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */
#ifndef _KANCIL_RSA_H_
#define _KANCIL_RSA_H_

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