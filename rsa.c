/* 
 * `rsa.c`
 * Penangan algoritma RSA untuk kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014 - 2015
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

#define CREATE_RSA_FROM_PRIVKEY 1
#define CREATE_RSA_FROM_PUBKEY  2

/*
 * create_rsa()
 * @url: http://hayageek.com/?p=1099
 * @penulis-asli: Ravishanker Kusuma (http://hayageek.com/)
 */

RSA * create_rsa(
	unsigned char * key,
	int flag
){
	int status=0;
	
	// Membangun BIO.
	BIO *keybio;
	DEBUG4("Mulai membangun BIO.", 0);
	keybio = BIO_new_mem_buf(key, -1);
	if (keybio==NULL){
		FAIL(_("Gagal membuat kunci BIO"), 0);
		exit(EXIT_FAILURE_CRYPT);
	}else {
		DEBUG4("Berhasil membangun BIO.", 0);
	};
	
	// Membangun RSA.
	RSA *rsa= NULL;
	DEBUG4("Mulai membangun RSA.", 0);
	if(flag==CREATE_RSA_FROM_PUBKEY){
		DEBUG4(_("Membangun RSA menggunakan kunci publik."), 0);
		rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
	}else if(flag==CREATE_RSA_FROM_PRIVKEY){
		DEBUG4(_("Membangun RSA menggunakan kunci privat."), 0);
		rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
	}else{
		FAIL(_("Tidak mengatahui jenis pembangunan RSA."), 0);
		exit(EXIT_FAILURE_CRYPT);
	};
	
	if(rsa == NULL){
		// Menampilkan kunci
		// yang mungkin bermasalah.
		DEBUG5("Kunci RSA", key, 0, 20);
		
		// Kesalahan.
		unsigned long errr=0;
		ERR_load_crypto_strings();
		errr=ERR_get_error();
		if(errr){
			char *err_buff;
			err_buff=malloc(sizeof(err_buff)*32);
			FAIL("Gagal membangun RSA: %1$s",  ERR_error_string(errr, err_buff));
			free(err_buff);
		}else{
			// Tidak dikenal.
			FAIL(_("Gagal membangun RSA."), 0);
		};
		exit(EXIT_FAILURE_CRYPT);
	}else{
		// Pesan.
		DEBUG4("Berhasil membangun RSA.", 0);
	};
	
	// Menutup.
	DEBUG4("Menutup BIO.", 0);
	BIO_set_close(keybio, BIO_NOCLOSE);
	status=BIO_get_close(keybio);
	if(status){
		DEBUG4("Gagal menutup BIO.", 0);
	}else{
		DEBUG4("Behasil menutup BIO.", 0);
	};
	
	// Bersihkan.
	DEBUG4("Membersihkan BIO.", 0);
	status=BIO_free(keybio);
	if(status){
		DEBUG4("Behasil membersihkan BIO.", 0);
	}else{
		DEBUG4("Gagal membersihkan BIO.", 0);
	};
	
	// Hasil.
	return rsa;
}

/*
 * `rsa_data_size()`
 */
int rsa_data_size(RSA *rsa, int bantalan){
	
	// Ukuran kunci modulus RSA.
	int ukuran_rsa=RSA_size(rsa);
	
	// Mencari ukuran data RSA.
	int ukuran_data=0;
	DEBUG4(_("Memilih bantalan RSA"), 0);
	if(
		bantalan==RSA_PKCS1_PADDING
		|| bantalan==RSA_SSLV23_PADDING
	){
		
		DEBUG4(_("Bantalan RSA jenis %1$s."), "RSA_PKCS1_PADDING");
		ukuran_data=ukuran_rsa-11;
		
	}else if(bantalan==RSA_PKCS1_OAEP_PADDING){
		
		DEBUG4(_("Bantalan RSA jenis %1$s."), "RSA_PKCS1_OAEP_PADDING");
		ukuran_data=ukuran_rsa-42;
		
	}else if(bantalan==RSA_NO_PADDING){
		
		DEBUG4(_("Bantalan RSA jenis %1$s."), "RSA_NO_PADDING");
		ukuran_data=ukuran_rsa;
		
	}else{
		// Tidak dikenal.
		FAIL(_("Tidak mengenali bantalan RSA"), 0);
		exit(EXIT_FAILURE_CRYPT);
	};
	
	// Hasil.
	return ukuran_data;
}

/*
 * rsa_encrypt()
 * @url: http://hayageek.com/?p=1099
 * @penulis-asli: Ravishanker Kusuma (http://hayageek.com/)
 */
int rsa_encrypt(
	unsigned char *data,
	int data_len,
	unsigned char *target,
	RSA *rsa,
	int bantalan
	){
	
	// Pesan.
	DEBUG4("Mulai enkripsi RSA.", 0);
	
	// Apakah panjang adalah NOL.
	if(data_len<=0){
		FAIL(_("Panjang data adalah %1$i."), data_len);
		exit(EXIT_FAILURE_CRYPT);
	};
	
	// RSA.
	// RSA *rsa = create_rsa(key, CREATE_RSA_FROM_PUBKEY);
	
	// Mendapatkan ukuran.
	int ukuran_data_rsa=rsa_data_size(rsa, bantalan);
	
	// Pesan.
	DEBUG4(_("Lebar data: %1$i"), data_len);
	DEBUG4(_("Lebar blok: %1$i"), ukuran_data_rsa);
	
	// Enkripsi.
	int awal=0;
	int result=0;
	int tambalan=0;
	int rsa_result=0;
	int ukuran_enkripsi=0;
	int ukuran_data_tersisa=data_len;
	unsigned long errr;
	ERR_load_crypto_strings();
	bool putar=true;
	while(putar){
		errr=0;
		rsa_result=0;
		ukuran_enkripsi=0;
		
		// Memasukkan data
		// tiap ukuran data RSA.
		DEBUG4(_("Menentukan ukuran blok enkripsi."), 0);
		if(ukuran_data_tersisa>ukuran_data_rsa){
			// Pesan.
			DEBUG4(_("Ukuran blok enkripsi (%1$i) lebih dari ukuran blok RSA (%2$i)."), ukuran_data_tersisa, ukuran_data_rsa);
			
			// Bukan akhir.
			ukuran_enkripsi=ukuran_data_rsa;
			ukuran_data_tersisa-=ukuran_data_rsa;
			
		}else{
			
			// Pesan.
			if (ukuran_data_tersisa<ukuran_data_rsa){
				DEBUG4(_("Ukuran blok enkripsi (%1$i) kurang dari ukuran blok RSA (%2$i)."), ukuran_data_tersisa, ukuran_data_rsa);
			}else{
				DEBUG4(_("Ukuran blok enkripsi (%1$i) sama dengan ukuran blok RSA (%2$i)."), ukuran_data_tersisa, ukuran_data_rsa);
			};
			
			// Merupakan akhir.
			ukuran_enkripsi=ukuran_data_tersisa;
			putar=false;
			
		};
		
		// Pesan.
		DEBUG4(_("Panjang data tersisa: %1$i"), ukuran_data_tersisa);
		
		// Bila RSA_NO_PADDING
		// dan ukuran terlalu kecil.
		if(bantalan==RSA_NO_PADDING && ukuran_enkripsi<ukuran_data_rsa){
			tambalan +=ukuran_data_rsa-ukuran_enkripsi;
			DEBUG4(_("Menambah tambalan di akhiran blok enkripsi sebanyak %1$i bita."), (ukuran_data_rsa-ukuran_enkripsi));
			ukuran_enkripsi+=tambalan;
			
			// Menyalin isi.
			unsigned char *data_tambalan;
			data_tambalan=malloc(sizeof(data_tambalan)*(ukuran_data_rsa+1));
			memset(data_tambalan, 0, sizeof(data_tambalan)*(ukuran_data_rsa+1));
			memcpy(data_tambalan, data+awal, ukuran_data_tersisa);
			
			// Mengosongkan isi
			for(int it=ukuran_data_tersisa;it<=ukuran_data_rsa;it++){
				data_tambalan[it]=0;
			};
			
			// Enkripsi.
			DEBUG4(_("Panjang blok enkripsi: %1$i"), ukuran_enkripsi);
			rsa_result = RSA_public_encrypt(ukuran_data_rsa, data_tambalan, target+result, rsa, bantalan);
		}else{
			// Enkripsi.
			DEBUG4(_("Panjang blok enkripsi: %1$i"), ukuran_enkripsi);
			rsa_result = RSA_public_encrypt(ukuran_enkripsi, data+awal, target+result, rsa, bantalan);
		};
		
		// Bila ada kesalahan.
		if(rsa_result<0){
			
			// Kesalahan.
			errr=ERR_get_error();
			char *err_buff;
			err_buff=malloc(sizeof(err_buff)*32);
			FAIL("%1$s",  ERR_error_string(errr, err_buff));
			DEBUG4(_("Dimulai pada bita: %1$i"), awal);
			free(err_buff);
			exit(EXIT_FAILURE_CRYPT);
		}else{
			
			// Tidak ada masalah.
			result +=rsa_result;
			awal +=ukuran_data_rsa;
		};
	};
	
	// Pesan.
	DEBUG4(_("Ukuran terenkripsi: %1$i"), result);
	
	// Hasil.
	return result;
}
/*
 * rsa_decrypt()
 * @url: http://hayageek.com/?p=1099
 * @penulis-asli: Ravishanker Kusuma (http://hayageek.com/)
 */
int rsa_decrypt(
	unsigned char *data,
	int data_len,
	unsigned char *target,
	RSA *rsa,
	int bantalan
	){
	
	// Pesan.
	DEBUG4("Mulai dekripsi RSA.", 0);
	
	// Apakah panjang adalah NOL.
	if(data_len<=0){
		FAIL(_("Panjang data adalah %1$i."), data_len);
		exit(EXIT_FAILURE_CRYPT);
	};
	
	// RSA.
	// RSA *rsa = create_rsa(key, CREATE_RSA_FROM_PRIVKEY);
	
	// Mendapatkan ukuran.
	int ukuran_rsa=RSA_size(rsa);
	
	// Pesan.
	DEBUG4(_("Lebar data: %1$i"), data_len);
	DEBUG4(_("Lebar blok: %1$i"), ukuran_rsa);
	
	// print_unsigned_array_nonsymbol(data+0, ukuran_rsa);
	
	// Dekripsi.
	int awal=0;
	int result=0;
	int rsa_result=0;
	int ukuran_dekripsi=0;
	int ukuran_rsa_tersisa=data_len;
	unsigned long errr;
	ERR_load_crypto_strings();
	bool putar=true;
	while(putar){
		errr=0;
		rsa_result=0;
		
		// Memasukkan data
		// tiap ukuran data RSA.
		DEBUG4(_("Menentukan ukuran blok dekripsi."), 0);
		if(ukuran_rsa_tersisa>ukuran_rsa){
			
			// Pesan.
			DEBUG4(_("Ukuran blok dekripsi (%1$i) lebih dari ukuran blok RSA (%2$i)."), ukuran_rsa_tersisa, ukuran_rsa);
			
			// Bukan akhir.
			ukuran_dekripsi=ukuran_rsa;
			ukuran_rsa_tersisa-=ukuran_rsa;
			
		}else{
			
			// Pesan.
			if (ukuran_rsa_tersisa<ukuran_rsa){
				DEBUG4(_("Ukuran blok dekripsi (%1$i) kurang dari ukuran blok RSA (%2$i)."), ukuran_rsa_tersisa, ukuran_rsa);
			}else{
				DEBUG4(_("Ukuran blok dekripsi (%1$i) sama dengan ukuran blok RSA (%2$i)."), ukuran_rsa_tersisa, ukuran_rsa);
			};
			
			
			// Merupakan akhir.
			ukuran_dekripsi=ukuran_rsa_tersisa;
			putar=false;
		};
		
		// Dekripsi.
		DEBUG4(_("Panjang data tersisa: %1$i"), ukuran_rsa_tersisa);
		DEBUG4(_("Panjang blok dekripsi: %1$i"), ukuran_dekripsi);
		rsa_result = RSA_private_decrypt(ukuran_dekripsi, data+awal, target+result, rsa, bantalan);
		
		// Apakah terjadi kesalahan.
		if(rsa_result<0){
			
			// Kesalahan.
			errr=ERR_get_error();
			char *err_buff;
			err_buff=malloc(sizeof(err_buff)*32);
			FAIL("%1$s",  ERR_error_string(errr, err_buff));
			DEBUG4(_("Dimulai pada bita: %1$i"), awal);
			free(err_buff);
			exit(EXIT_FAILURE_CRYPT);
		}else{
			
			// Tidak ada kesalahan.
			result+=rsa_result;
			awal+=ukuran_rsa;
		};
	};
	
	// Pesan.
	DEBUG4(_("Ukuran terdekripsi: %1$i"), result);
	
	// Hasil.
	return result;
}

/*
 * Kunci privat asali RSA.
 */
unsigned char *default_rsa_privatekey(){
	unsigned char *rturn= (unsigned char *)"" \
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
	return rturn;
}
/*
 * Kunci publik asali RSA.
 */
unsigned char *default_rsa_pubkey(){
	unsigned char *rturn= (unsigned char *)"" \
	"-----BEGIN PUBLIC KEY-----\n" \
	"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAwK665/EZ5Mt8UkmSDsrC\n" \
	"Emnt4FB8LzgunQRhpm4Jk6n92W1Ta7MdytFC/EpHKznPEmr2wUxluFaMQ/6Y/isl\n" \
	"q/ktF5xKvPdCk3X0y+Y/g2z6ie2Dkl3rJUNBMhgqcfaaV7i3s5+gwJRaKaG8cyqT\n" \
	"KVlFg9dq1i7P8mXx5w//K7P0Thasot5uEVHgDpluQg30rgm08BEBMyhbZnmzaXo9\n" \
	"HWjYn7IuyYXYD5oegc++ubW1ZrTw5pLcKQMPATkhgG8tX57dilVuwdrDFhNXhEaz\n" \
	"5ItpGZQXoTLAYC7wT2UGNU5+Vdk3PLULy44PA2QwTL1gB2nSH7wWEBtueZxgq/+J\n" \
	"5QIDAQAB\n" \
	"-----END PUBLIC KEY-----";
	return rturn;
}
