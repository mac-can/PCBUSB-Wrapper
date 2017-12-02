/*	-- $HeadURL: https://svn.uv-software.com/projects/uv-software/CAN/I386/CAN/trunk/ute/Suite/api_test.h $ --
 *
 *	project   :  CAN - Controller Area Network
 *
 *	purpose   :  CAN Controller Interface, Version 3
 *
 *	copyright :  (C) 2005-2010, UV Software, Friedrichshafen
 *	             (C) 2014, UV Software, Berlin
 *
 *	compiler  :  Apple LLVM Compiler (Version 6.0, GCC 4.2.1)
 *
 *	export    :  (see below)
 *
 *	includes  :  (none)
 *
 *	author(s) :  Uwe Vogt, UV Software
 *
 *	e-mail    :  uwe.vogt@uv-software.de
 *
 *
 *	-----------  description  -------------------------------------------
 *
 *	api_test  : "Master Control Program" header
 *
 */

#ifndef __API_TEST_H
#define __API_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/*  -----------  set-up  ------------------------------------------------
 */

#define _TEST_SETUP		(0)


/*  -----------  options  -----------------------------------------------
 */

#define _TEST_RETURN_VALUES
//#undef _DRV_TEST_AUTOMATED


/*  -----------  defines  -----------------------------------------------
 */

#ifdef _TEST_SETUP			/* (default) */
 #define DUT_VENDOR			"UV Software"
 #define DUT_BOARD			PCAN_USB1
 #define DUT_INVALID		0x00
 /* 2nd CAN interface */
 #define UTE_VENDOR			"UV Software"
 #define UTE_BOARD			PCAN_USB2
 /* issues and deviations */
 #define _ISSUE_TC06_NO_RTR_CHECK
 #define _ISSUE_TC06_28_WORKAROUND
#endif

#define DUT_INTERFACE		DUT_BOARD
#define DUT_BAUDRATE		CANBDR_250
#define DUT_BAUDRATE_SLOW	CANBDR_20
#define DUT_BAUDRATE_FAST	CANBDR_1000

#define UTE_INTERFACE		UTE_BOARD
#define UTE_BAUDRATE		DUT_BAUDRATE
#define UTE_BAUDRATE_SLOW	DUT_BAUDRATE_SLOW
#define UTE_BAUDRATE_FAST	DUT_BAUDRATE_FAST

#define DUT_INVALID_VENDOR	DUT_INVALID
#define DUT_INVALID_BOARD	DUT_INVALID


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __API_TEST_H */

/*	----------------------------------------------------------------------
 *	Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *	Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *	E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
