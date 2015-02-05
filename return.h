/* 
 * `return.h`
 * Pengepala standar untuk hasil keluaran kancil.
 * Penulis: Bayu Aditya H. <b@yuah.web.id>
 * HakCipta: 2014
 * Lisensi: lihat LICENCE.txt
 */

#ifndef _KANCIL_RETURN_H_
#define _KANCIL_RETURN_H_

// Nilai keluaran.
#ifndef EXIT_SUCCESS
	#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
	#define EXIT_FAILURE 1
#endif
#ifndef EXIT_FAILURE_ARGS
	#define EXIT_FAILURE_ARGS 40
#endif
#ifndef EXIT_FAILURE_VARS
	#define EXIT_FAILURE_VARS 41
#endif
#ifndef EXIT_FAILURE_ENV
	#define EXIT_FAILURE_ENV 42
#endif
#ifndef EXIT_FAILURE_IO
	#define EXIT_FAILURE_IO 43
#endif
#ifndef EXIT_FAILURE_MEMORY
	#define EXIT_FAILURE_MEMORY 44
#endif
#ifndef EXIT_FAILURE_SOCKET
	#define EXIT_FAILURE_SOCKET 45
#endif
#ifndef EXIT_FAILURE_CHUNK
	#define EXIT_FAILURE_CHUNK 46
#endif
#ifndef EXIT_FAILURE_MESSAGES
	#define EXIT_FAILURE_MESSAGES 47
#endif
#ifndef EXIT_FAILURE_FORK
	#define EXIT_FAILURE_FORK 48
#endif
#ifndef EXIT_FAILURE_CRYPT
	#define EXIT_FAILURE_CRYPT 49
#endif
#ifndef EXIT_TEST
	#define EXIT_TEST 222
#endif

#endif /* _KANCIL_RETURN_H_ */
