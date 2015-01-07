/* 
 * `rsa.c`
 * Penangan algoritma RSA untuk kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */

#include "kancil.h"
#include "faedah.h"

#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
// #include <stdio.h>

/*
 * rsa_encrypt()
 * @url: http://hayageek.com/?p=1099
 * @penulis-asli: Ravishanker Kusuma (http://hayageek.com/)
 */
int rsa_encrypt(
	unsigned char *data,
	int data_len,
	unsigned char *key,
	unsigned char *target,
	int bantalan
	){
	
	// Inisiasi
	RSA *rsa= NULL;
	BIO *bio;
	bio = BIO_new_mem_buf(key, -1);
	if (bio==NULL){
		// Gagal
		return 1;
	}else{
		rsa=PEM_read_bio_RSA_PUBKEY(bio, &rsa, NULL, NULL);
	};
	
	// Enkripsi RSA.
	int hasil = RSA_public_encrypt(
		data_len,
		data,
		target,
		rsa,
		bantalan);
	
	// Kosongkan.
	BIO_set_close(bio, BIO_NOCLOSE); 
	BIO_free(bio, BIO_NOCLOSE); 
	
	// Hasil.
	return hasil;
}
/*
 * rsa_decrypt()
 * @url: http://hayageek.com/?p=1099
 * @penulis-asli: Ravishanker Kusuma (http://hayageek.com/)
 */
int rsa_decrypt(
	unsigned char *data,
	int data_len,
	unsigned char *key,
	unsigned char *target,
	int bantalan
	){
	
	// Inisiasi
	RSA *rsa= NULL;
	BIO *bio;
	bio = BIO_new_mem_buf(key, -1);
	if (bio==NULL){
		// Gagal
		return 1;
	}else{
		rsa=PEM_read_bio_RSAPrivateKey(bio, &rsa, NULL, NULL);
	};
	
	// Enkripsi RSA.
	int hasil = RSA_public_encrypt(
		data_len,
		data,
		target,
		rsa,
		bantalan);
	
	// Kosongkan.
	BIO_set_close(bio, BIO_NOCLOSE); 
	BIO_free(bio, BIO_NOCLOSE); 
	
	// Hasil.
	return hasil;
}

/*
 * Kunci privat asali RSA.
 */
unsigned char *default_rsa_privatekey(){
	return \ 
	"-----BEGIN RSA PRIVATE KEY-----\n" \ 
	"MIIEpAIBAAKCAQEAwK665/EZ5Mt8UkmSDsrCEmnt4FB8LzgunQRhpm4Jk6n92W1T\n" \ 
	"a7MdytFC/EpHKznPEmr2wUxluFaMQ/6Y/islq/ktF5xKvPdCk3X0y+Y/g2z6ie2D\n" \ 
	"kl3rJUNBMhgqcfaaV7i3s5+gwJRaKaG8cyqTKVlFg9dq1i7P8mXx5w//K7P0Thas\n" \ 
	"ot5uEVHgDpluQg30rgm08BEBMyhbZnmzaXo9HWjYn7IuyYXYD5oegc++ubW1ZrTw\n" \ 
	"5pLcKQMPATkhgG8tX57dilVuwdrDFhNXhEaz5ItpGZQXoTLAYC7wT2UGNU5+Vdk3\n" \ 
	"PLULy44PA2QwTL1gB2nSH7wWEBtueZxgq/+J5QIDAQABAoIBAEB9ewNMmB0S2k50\n" \ 
	"X1ViGaiuGbb9WUM4q6lUIRbiN4Rb3iHazJP8hFkwxxnIBp/6HkuMz4ANAPK8zP+p\n" \ 
	"BC315Qr1QFlprz4mB/tw/HkOXG2xmP13KJVhp+V16ClLG1G/dxMrORj+sYAjZzYN\n" \ 
	"8xQcyNqLDZA+Fn0Pd/wqkNeEbCSE5to6h8uHwYXSdEJCII5wHjGgstGgPTehAd+r\n" \ 
	"in1ArJ9Wp6vdSLX8/HDzD9l7e3FTJ0Z2SJE22DDMNMV0KeYgfT9UQPE8lzqaHokN\n" \ 
	"yGW0mrXjgjRhzr766AUFa9wOF75xwEX2bnyIAqRFOR3dL7rqcME6rQCGPW+dSugI\n" \ 
	"m5nhq1UCgYEA5erxHOhA7L8uLDQaw8q1+p5OzYE0aMr1wV04pG2yljdFqx7Y62AQ\n" \ 
	"SbgaperhYA0G8txa/FemjsBDWGGMjCdnzp1qReZD7uK1obA7L1QE6iFzxlYib1ie\n" \ 
	"YjnW5/fGg+UvbJC1FrbjEtThKiIm7MiFNy9HG19t80XGs1TrTVXFzkMCgYEA1opw\n" \ 
	"IE6DcYv6/mMKB5TN/yckoADx63hP/qSBrZq5JYQNaVho48QlLIR7sSnZ/k+E3cLm\n" \ 
	"22naBWd72jlJXYvK3Tbffz475+o9bXRn5sqCsk9Bskl1zIwccyh8dWyRo3CzWc5O\n" \ 
	"YcZdqKo1pgUHvvgwyZQ6xsXekCXbXxptUaFNCLcCgYEAxJJ6Uxy/ZaqO73wc5gkK\n" \ 
	"TZvMBCfTh0T/mLL7Gpovdf8LsaJOE++t/VlKb1DCID0DE+ESaW0HGkWHTqkYYTvP\n" \ 
	"pPcxRVH6AuQSJotmDrDnAsZXFdvO1SQPRivtcaaGM35fbZLLQQBcZF1dpYhNf2Rx\n" \ 
	"gR0KznxopPWjTO/tvdD3rcECgYEAlDBDis0wlbmuwQ//2YS7VUbgUXTVpWQRPT68\n" \ 
	"JAH/aR5xdRaQPOM7GijYl2LlJHggulzpwRy4b4svTWjMeEUa9vn0HpwKHQL7MyN4\n" \ 
	"sT2oUSnVoySCrasJ67FHZoR7hH1QJPQPyBGQl/QZ4ncPBqXAbTxp7Oq39LFxeZtz\n" \ 
	"bzMPVrkCgYAyDgIcBbgUbTrAewriRzW8ppuNHaWIWH796QN582o7D6t/ZpOrdkD5\n" \ 
	"VIb4X8pI+wdSG4t+yZjFNyyAaSEsriCYgn+S639WOXSKL8dChHQKUdOr9pC4a6MM\n" \ 
	"klYa9s5rep0pFOTJXqneFledye/M/Pb1L2D0G/JL2ynbHzcktk39mA==\n" \ 
	"-----END RSA PRIVATE KEY-----";
}
/*
 * Kunci publik asali RSA.
 */
unsigned char *default_rsa_pubkey(){
	return \
	"-----BEGIN PUBLIC KEY-----\n" \ 
	"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAwK665/EZ5Mt8UkmSDsrC\n" \ 
	"Emnt4FB8LzgunQRhpm4Jk6n92W1Ta7MdytFC/EpHKznPEmr2wUxluFaMQ/6Y/isl\n" \ 
	"q/ktF5xKvPdCk3X0y+Y/g2z6ie2Dkl3rJUNBMhgqcfaaV7i3s5+gwJRaKaG8cyqT\n" \ 
	"KVlFg9dq1i7P8mXx5w//K7P0Thasot5uEVHgDpluQg30rgm08BEBMyhbZnmzaXo9\n" \ 
	"HWjYn7IuyYXYD5oegc++ubW1ZrTw5pLcKQMPATkhgG8tX57dilVuwdrDFhNXhEaz\n" \ 
	"5ItpGZQXoTLAYC7wT2UGNU5+Vdk3PLULy44PA2QwTL1gB2nSH7wWEBtueZxgq/+J\n" \ 
	"5QIDAQAB\n" \ 
	"-----END PUBLIC KEY-----\n";
}