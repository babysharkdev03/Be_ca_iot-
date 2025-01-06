// This file has been prepared for Doxygen automatic documentation generation.
/*! \file ********************************************************************
*
* Atmel Corporation
*
* - File              : pid.h
* - Compiler          : IAR EWAAVR 4.11A
*
* - Support mail      : avr@atmel.com
*
* - Supported devices : -
*
* - AppNote           : AVR221 - PID controller
*
* - Description       : General PID implementation for AVR
*
* $Revision: 1.2 $
* $Author: jtyssoe $, Atmel Corp.
* $Date: Thursday, November 10, 2005 10:10:48 UTC $
*****************************************************************************/
#ifndef PID_H
#define PID_H

/*! \brief P, I and D parameter values
 *
 * The K_P, K_I and K_D values (P, I and D gains)
 * need to be modified to adapt to the application at hand
 */
//! \xrefitem todo "Todo" "Todo list"
#define K_P     4
//! \xrefitem todo "Todo" "Todo list"
#define K_I     1
//! \xrefitem todo "Todo" "Todo list"
#define K_D     1

#define SCALING_FACTOR  128

// Maximum value of error
#define MAXERROR        1023
// Maximum value of integers
#define MAX_INT         255


/*! \brief Sampling Time Interval
 *
 * Specify the desired PID sample time interval
 * With a 8-bit counter (255 cylces to overflow), the time interval value is calculated as follows:
 * TIME_INTERVAL = ( desired interval [sec] ) * ( frequency [Hz] ) / 255
 */
//! \xrefitem todo "Todo" "Todo list"
#define TIME_INTERVAL   157


/*! \brief PID Status
 *
 * Setpoints and data used by the PID control algorithm
 */
struct PID_DATA{
  //! Last process value, used to find derivative of process value.
  int LAST_PROCESS_VALUE;
  //! Summation of errors, used for integrate calculations
  int SUM_ERROR;
  //! The Proportional tuning constant, given in x100
  int P_FACTOR;
  //! The Integral tuning constant, given in x100
  int I_FACTOR;
  //! The Derivative tuning constant, given in x100
  int D_FACTOR;
  //! Maximum allowed error, avoid overflow (= MAX_INT / PFactor)
  int MAX_ERROR;
  //! Maximum allowed sumerror, avoid overflow (= MAX_INT / IFactor)
  int MAX_SUM_ERROR;
};

/*! \brief Maximum error values
 *
 * Needed to avoid sign/overflow problems
 */

// Boolean values
#define FALSE           0
#define TRUE            1

void Init_PID(int p_factor, int i_factor, int d_factor, struct PID_DATA *pid);
int PID(int setPoint, int processValue, struct PID_DATA *pid_st);

#endif
