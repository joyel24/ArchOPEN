/*
*   kernel/fs/vfs_fops.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
* Part of this code is from Rockbox project
* Copyright (C) 2002 by Bj�n Stenberg
*
*/

#include <lib/string.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/list.h>

#include <fs/stdfs.h>
#include <fs/fat.h>
#include <fs/vfs.h>
#include <fs/vfs_node.h>

extern struct vfs_mountPoint * root_mountPoint;

int vfs_strToFlag(char * str)
{
    
    
    int read=0,write=0,append=0,plus=0;
    int res=0;
    while(*str)
    {
        if(*str=='r') read=1;
        if(*str=='w') write=1;
        if(*str=='a') append=1;
        if(*str=='+') plus=1;
        str++;
    }
  
    if(plus)
    {
        if(write)
        {
            res = O_RDWR | O_TRUNC;
        }
        else if(read)
        {
            res = O_RDWR;  
        }
        else if(append)
        {
            res = O_RDWR | O_APPEND;
        }                    
    }
    else
    {
        if(read && write)
        {
            res = O_RDWR | O_CREAT;
        }
        else if(read && append)
        {
            res = O_RDWR | O_APPEND | O_CREAT;
        }
        else if(read)
        {
            res = O_RDONLY;
        }
        else if(write)
        {
            res = O_WRONLY | O_CREAT;   
        }
        else if(append)
        {
            res = O_APPEND | O_WRONLY | O_CREAT;
        }
    }
    
    return res;       
}

int open(char * name,int flags)
{
    MED_RET_T ret_val;
    struct vfs_pathname path;
    struct vfs_node * myRoot_node;

    FILE * fd;

    if(!root_mountPoint)
        return -MED_ENOENT;
    
    path.length = strlen(name);
    if(path.length>0)
        path.str=name;
    else
        return -MED_EINVAL;

    if(path.str[0] != '/')
    {
        /* relative path */
        if(threadCurrent->path)
        {
            myRoot_node=threadCurrent->path;
        }
        else
        {
            /* current thread path == NULL => using root node */ 
            myRoot_node=root_mountPoint->root_node;
        }
    }
    else
    {
        /* absolute path */ 
        myRoot_node=root_mountPoint->root_node;
    }

    ret_val=vfs_nodeLookup(&path,myRoot_node,&fd,&path);

    if(ret_val!=MED_OK && ret_val!=-MED_ENOENT)
    {
        printk("Err(%d): during 'vfs_nodeLookup' call\n",-ret_val);
        return -MED_EINVAL;
    }

    if(path.length<=0)
    {
        /* ok found it !! */
        if(fd->opened)
        {
            /* already opened */
            printk("File %s alreday opened\n",name);
            return -MED_EBUSY;
        }

        if(fd->type == VFS_TYPE_DIR)
        {
            /* it is a dir => can't open dir with fopen !! */
            printk("Trying to open a folder with fopen !!\n");
            printk("Folder name: '%s'\n",name);
            return -MED_EISDIR;
        }
    }
    else
    {
        /* not found, should we create it? */
        if(FILE_CREATE(flags) && FILE_WRITE(flags))
        {
            struct  vfs_pathname path2;
            struct  vfs_node * new_node;
            /* are we in the needed folder ?*/
            VFS_PRINT("Need to create file\n");
            vfs_pathnameSplit(&path,&path,&path2);
            if(path2.length != 0)
            {
                printk("File not existing, and can't create beacuse folder also absent (%s)\n",name);
                return -MED_ENOMEM;
            }
            new_node = (struct  vfs_node *)kmalloc(sizeof(struct  vfs_node));
            if(!new_node)
            {
                printk("Error can't create new node, not enough mem\n");
                return -MED_ENOMEM;
            }

            memset(new_node,0x0,sizeof(struct  vfs_node));
            ret_val = vfs_nodeInitChild(fd->mount_point,fd,&path,new_node,VFS_TYPE_FILE);
            if(ret_val!=MED_OK)
            {
                printk("Error in initChild (err=%d)\n",-ret_val);
                kfree(new_node);
                return ret_val;
            }

            ret_val=fat_createFile(path.str,new_node,fd);
            if(ret_val!=MED_OK)
            {
                printk("Error in fat_create_file (err=%d)\n",-ret_val);
                ret_val=vfs_rmNodeFromTree(new_node);
                if(ret_val!=MED_OK)
                    printk("Error in rm node from tree (err=%d)\n",-ret_val);
                return ret_val;
            }
            fd=new_node;
        }
        else
        {
            printk("File '%s' not found\n",name);
            return -MED_ENOENT;
        }
    }

    
    ret_val=fat_fileOpen(fd);

    if(ret_val != MED_OK)
    {
        printk("Error in fat_open: %d\n",-ret_val);
        return ret_val;
    }

    /* ok file is opened */
    fd->opened=1;
    fd->position = 0;
    fd->dirty=0;    
    fd->flags = flags;

    /* setup struct */
    if(FILE_TRUNC(flags))
    {
        ret_val=fat_fileTruncate(fd,0);
        if(ret_val != MED_OK)
        {
            printk("error truncating file\n");
            fat_fileClose(fd);
            fd->opened=0;
            return ret_val;
        }
    }

    if (FILE_APPEND(flags) && FILE_WRITE(flags))
    {
        ret_val = lseek((int)fd,0,SEEK_END);
        if(ret_val<0) /* lseek returns the current position, and sometinh <0 on error*/
        {
            printk("error seeking at end for file (flags APPEND)\n");
            fat_fileClose(fd);
            fd->opened=0;
            return ret_val;
        }
    }

    vfs_nodeRef(fd);
    
    LIST_ADD_TAIL_NAMED(fd->mount_point->opened_node,fd,prev_open,next_open);
    thread_listAdd(THREAD_PTR_2_LIST(fd),FILE_RESSOURCE,THREAD_NO_FORCE);
    return (int)fd;
}

MED_RET_T fsync(int fdesc)
{
    
    FILE * fd = (FILE *)fdesc;
    CHK_FD(fd)
    return vfs_fileSync(fd);
}

MED_RET_T vfs_fileSync(struct vfs_node * fd)
{            
    MED_RET_T ret_val=MED_OK;    
    if(FILE_WRITE(fd->flags))
    {
        /* flush sector cache */
        if ( fd->dirty )
        {
            ret_val = fat_fileFlushCache(fd);
            if (ret_val != MED_OK)
                return ret_val;
        }

        /* truncate? */
        if (FILE_TRUNC(fd->flags))
        {
            VFS_PRINT("FCLOSE: We need to truncate file\n");
            ret_val=fat_fileTruncate(fd,fat_getFileSize(fd));
            if (ret_val != MED_OK)
                return ret_val;
        }
        /* tie up all loose ends */
        ret_val = fat_fileSync(fd);
        if (ret_val != MED_OK)
               return ret_val;
    }
    return ret_val;
}

MED_RET_T close(int fdesc)
{
    FILE * fd = (FILE *)fdesc;
    CHK_FD(fd)
    if(thread_listRm(THREAD_PTR_2_LIST(fd),FILE_RESSOURCE,THREAD_NO_FORCE)==MED_OK)
        return vfs_fileClose(fd);
    return -MED_ERROR;
}
   
MED_RET_T vfs_fileClose(struct vfs_node * fd)
{
    
    MED_RET_T ret_val=MED_OK;
    
    vfs_fileSync(fd);
    ret_val=fat_fileClose(fd);
    if(ret_val!=MED_OK)
        return ret_val;

    fd->opened = 0;
    LIST_DELETE_NAMED(fd->mount_point->opened_node,fd,prev_open,next_open);
    vfs_nodeUnRef(fd);
    return MED_OK;
}

MED_RET_T truncate(int fdesc, unsigned int size)
{
    
    FILE * fd = (FILE *)fdesc;
    CHK_FD(fd);
    if(FILE_WRITE(fd->flags))
        return fat_fileTruncate(fd,size);
    return MED_OK;
}

int write(int fdesc, const void* buf, unsigned int count)
{
    FILE * fd = (FILE *)fdesc;
    
    CHK_FD(fd);
    if (!FILE_WRITE(fd->flags))
        return -MED_EINVAL;
    return fat_fileWrite(fd, (void *)buf, count);
}

int read(int fdesc, const void* buf, unsigned int count)
{
    FILE * fd = (FILE *)fdesc;
    CHK_FD(fd);
    return fat_fileRead(fd, (void *)buf, count);
}

int lseek(int fdesc, unsigned int offset, int whence)
{
    FILE * fd = (FILE *)fdesc;
    int pos;

    MED_RET_T ret_val=MED_OK;
    CHK_FD(fd);
#warning when we ll add real EOF
#warning we should manage it in lseek => eof=0 when seek to beg, eof=1 when seek to end
    switch ( whence )
    {
        case SEEK_SET:
            pos = offset;
            break;

        case SEEK_CUR:
            pos = fd->position + offset;
            break;

        case SEEK_END:
            pos = fat_getFileSize(fd) + offset;
            break;

        default:
        {
            printk("[lseek] Bad whence:%d\n",whence);
            return -MED_EINVAL;
        }
    }

    if (pos == fd->position) // optimize the case where we just want current file position
        goto done;
    
    if ((pos < 0) || (pos > fat_getFileSize(fd))) {
        printk("[lseek] Bad pos:%d\n",pos);
        return -MED_EINVAL;
    }

    ret_val=fat_fileSeek(fd,pos);
    if(ret_val<0)
    {
        VFS_PRINT("[lseek] error from fat_fileSeek: %d\n",-ret_val);
        return ret_val;
    }

    fd->position=pos;

done:
    return pos;
}

int ftell(int fdesc)
{
    FILE * fd = (FILE *)fdesc;
    CHK_FD(fd);
    return fd->position;
}


int filesize(int fdesc)
{
    FILE * fd = (FILE *)fdesc;
    CHK_FD(fd);
    return fat_getFileSize(fd);
}

