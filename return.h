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
	#define EXIT_FAILURE_ARGS 11
#endif
#ifndef EXIT_FAILURE_IO
	#define EXIT_FAILURE_IO 12
#endif
#ifndef EXIT_FAILURE_MEMORY
	#define EXIT_FAILURE_MEMORY 13
#endif
#ifndef EXIT_FAILURE_SOCKET
	#define EXIT_FAILURE_SOCKET 20
#endif
#ifndef EXIT_FAILURE_MESSAGES
	#define EXIT_FAILURE_MESSAGES 30
#endif
#ifndef EXIT_FAILURE_FORK
	#define EXIT_FAILURE_FORK 40
#endif
#ifndef EXIT_FAILURE_CHUNK
	#define EXIT_FAILURE_CHUNK 50
#endif

#endif /* _KANCIL_RETURN_H_ */
