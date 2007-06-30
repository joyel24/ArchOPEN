/* 
*   include/kernel/uart.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/


#ifndef __VERSION_H
#define __VERSION_H

//#include <kernel/version_svn.h>

#define BUILD_RELEASE

#if defined(BUILD_RELEASE) || !defined(VERSION_NUMBER)
#undef VERSION_NUMBER
#define VERSION_NUMBER    "2.1"
#endif

#endif
