/* 
 * File:   HardwareProfile.h
 * Author: macload1
 *
 * Created on 3 mars 2013, 19:59
 */

#ifndef __HARDWARE_PROFILE_H
#define	__HARDWARE_PROFILE_H

#ifdef	__cplusplus
extern "C" {
#endif

// PIC24F processor
#define GetSystemClock()		(32000000ul)      // Hz
#define GetInstructionClock()	(GetSystemClock()/2)
#define GetPeripheralClock()	GetInstructionClock()



#ifdef	__cplusplus
}
#endif

#endif /* __HARDWARE_PROFILE_H */