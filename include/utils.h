/**
* \file 	utils.h
* \author 	Jorismar Barbosa Meira <jorismar.barbosa@lavid.ufpb.br>
*/

#ifndef JB_TS_UTILS_H
#define JB_TS_UTILS_H

#include <string>
#include <fstream>
#include <iostream>

/**
* \brief This header implement essential functions and types.
*
* \headerfile util.h
*/

//********************************** MACROs **********************************

/**
* \brief	Macro function used for error handling. If the condition is satisfied, the flux of the function where this macro is called is interrupted and a custom message is shown.
* \def		EXIT_IF(cond, msg, ret)
* \param	cond    Condition
* \param	msg     Message to be displayed before leaving.
* \param	ret		Return value.
*/
#define CHECK_ERR(cond, msg, ret) { 					\
	if(cond) {											\
		std::string file(__FILE__);						\
		size_t pos = file.find_last_of("\\/");			\
		std::cout << "[ERROR] " << msg << std::endl;	\
		return ret;										\
	}													\
}

//********************************* TYPEDEFs *********************************

/** Defines a new type to represent a BYTE. */
typedef unsigned char byte_t;

/** Defines a new type to represent a WORD of bytes. */
typedef unsigned short word_t;

/** Defines a new type to represent a DWORD of bytes. */
typedef unsigned int dword_t;

/** Defines a new type to represent a QWORD of bytes. */
typedef unsigned long long qword_t;

//******************************** FUNCTIONs ********************************

/**
* \brief   This method read a file on the computer storage disc.
*
* \param   path        Requested file location.
* \param   buffer    	Buffer to store the bytes.
*
* \return  Returns the number of bytes readed.
*/
int readFile(std::string path, byte_t ** buffer);

#endif // !JB_TS_UTILS_H
