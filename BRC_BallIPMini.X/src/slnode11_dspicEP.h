/* Serial Loop device dependent header */

/* for dsPIC33EP
 *
 * Only support UART1
 */

#include <p33Exxxx.h>
#include "uart.h"

#define ENABLE_FORWARD
//#define USE_BULK

// #define DEBUG_SLTI LATBbits.LATB7

/* Device dependent definitions ================== */

#define EnableUARTInt(a)  {  EnableIntU1RX;  EnableIntU1TX; }
#define DisableUARTInt(a) { DisableIntU1TX; DisableIntU1RX; }
 
