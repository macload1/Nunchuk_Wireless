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

//_CONFIG2(FNOSC_PRIPLL & POSCMOD_XT)		// Primary XT OSC with 4x PLL
//_CONFIG1(JTAGEN_OFF & FWDTEN_OFF)		// JTAG off, watchdog timer off

// PIC24F processor
#define GetSystemClock()		(8000000ul)      // Hz
#define GetInstructionClock()	(GetSystemClock()/2)
#define GetPeripheralClock()	GetInstructionClock()



#ifdef	__cplusplus
}
#endif

#endif	/* __HARDWARE_PROFILE_H */

