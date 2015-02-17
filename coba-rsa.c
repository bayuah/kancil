/*
 * @url: http://hayageek.com/?p=1099
 * @penulis-asli: Ravishanker Kusuma (http://hayageek.com/)
 */

#include "kancil.h"
#include "faedah.h"
#include "rsa.h"
 
int padding = RSA_PKCS1_PADDING;
 
int main(){
	
	aturan.show_error=true;
	aturan.show_warning=true;
	aturan.show_notice=true;
	aturan.show_info=true;
	aturan.show_debug1=false;
	aturan.show_debug2=false;
	aturan.show_debug3=false;
	aturan.show_debug4=false;
	aturan.show_debug5=true;
	strcpy(aturan.tempdir, "tmp");
	aturan.tries=20;
	aturan.waitretry=30;
	aturan.waitqueue=30;
	
	
	printf("Mulai\n");
	unsigned char *anu;
	anu=malloc(sizeof(anu)*50);
	memset(anu, 0, 50);
	// anu=(unsigned char*)"Sate enak sekali!";
	anu=(unsigned char*)"Sate atau satai (KBBI) (play /ˈsæteɪ/, play /ˈsɑːteɪ/ SAH-tay) adalah makanan yang terbuat dari potongan daging kecil-kecil dan ditusuki dengan tusukan yang biasanya terbuat dari lidi tulang daun kelapa, bambu, dan bahkan jeruji besi kemudian dibakar menggunakan bara arang kayu. Sate kemudian disajikan dengan berbagai macam bumbu yang bergantung pada variasi resep sate.[1] Daging yang dijadikan sate antara lain daging ayam, kambing, domba, sapi, babi, kelinci, kuda, dan lain-lain. Sate diketahui berasal dari Jawa, Indonesia, dan dapat ditemukan di mana saja di Indonesia dan telah dianggap sebagai salah satu masakan nasional Indonesia.[2] Sate juga populer di negara-negara Asia Tenggara lainnya seperti Malaysia, Singapura, Filipina, dan Thailand. Sate juga populer di Belanda yang dipengaruhi masakan Indonesia yang dulu merupakan koloninya. Sate adalah hidangan yang sangat populer di Indonesia; dengan berbagai suku bangsa dan tradisi seni memasak (lihat Masakan Indonesia) telah menghasilkan berbagai jenis sate.";
	int panjang=MAX_CHUNK_SIZE;
	unsigned char *tujuan_ency;
	printf("%s\n", anu);
	tujuan_ency=malloc((sizeof tujuan_ency)*ENCRYPTED_CONTAINER_SIZE);
	panjang=rsa_encrypt(
		anu,
		panjang,
		default_rsa_pubkey(),
		tujuan_ency,
		RSA_NO_PADDING
	);
	DEBUG5(_("Tes"), tujuan_ency, 0, 200);
	unsigned char *tujuan_deco;
	tujuan_deco=malloc((sizeof tujuan_deco)*ENCRYPTED_CONTAINER_SIZE);
	panjang=rsa_decrypt(
		tujuan_ency,
		panjang,
		default_rsa_privatekey(),
		tujuan_deco,
		RSA_NO_PADDING
	);
	// 
	printf("Dekripsi, ukuran: %i\n", panjang);
	// 
	printf("Panjang:%i\n", strlen((char*)tujuan_deco));
	printf("\"%s\"\n", tujuan_deco);
	// printf("\n");
	// printf("\n");
	// print_char_csv(pecahan, 0, pa);
	// printf("\n");
	// print_unsigned_array(tujuan_deco, 30);
	exit(EXIT_TEST);
}
