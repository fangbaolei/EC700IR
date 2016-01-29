/*
 * DSPLinkType.h
 *
 *  Created on: 2009-9-4
 *      Author: Administrator
 */

#ifndef DSPLINKTYPE_H_
#define DSPLINKTYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _DSP_
	/*Arm Side*/
	#define COFF_FALSE = 0
	#define COFF_TRUE = 1;

#ifndef _ARM_APP_
#include <asm/semaphore.h>
	typedef struct _tagSEM_Obj{
		int flag;
		wait_queue_head_t	wait;
	}SEM_Obj;
#endif /*_ARM_APP_*/

#else
	/*DSP Side*/
	#include <std.h>
	#include <stdlib.h>

	#ifndef _NO_DSPBIOS_
	#include <lck.h>
	#include <sem.h>
	#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* DSPLINKTYPE_H_ */
