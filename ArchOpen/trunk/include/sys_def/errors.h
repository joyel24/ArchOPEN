/*
*   include/kernel/errors.h
*
*   MediOS project
*
*/

#ifndef __ERRORS_H
#define __ERRORS_H

typedef int MED_RET_T;

#define MED_OK               0 /* everything is ok */
#define MED_ENOENT           1 /* missing data (file/dir/other) */
#define MED_EIO              2 /* IO error */
#define MED_ERROR            3 /* something is going wrong */
#define MED_ENOMEM           4 /* No more available memory */
#define MED_EEXIST           5 /* already exists */
#define MED_EBUSY            6 /* Object still in use */
#define MED_ENBUSY           7 /* Object not in use */
#define MED_EINVAL           8 /* bad arg error */
#define MED_EBADDATA         9 /* bad data */
#define MED_EMOBJ           10 /* No more obj available */
#define MED_ENOTFOUND       11 /* object not found */
#define MED_ENOTDIR         12 /* Dir does not exist */
#define MED_ECACHE          13 /* Error during cache access */
#define MED_ENOSPACE        14 /* Not enough space on device */
#define MED_ENOTEMPTY       15 /* Not empty */
#define MED_EISDIR          16 /* we should not be working on a dir */
#define MED_EISFILE         17 /* we should not be working on a file */
#define MED_ENOTOPEN        18 /* file/dir not open yet */
#define MED_EOF             19 /* end of file */

#endif
