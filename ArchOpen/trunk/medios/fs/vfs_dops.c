/*
*   kernel/fs/vfs_dops.c
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
* Copyright (C) 2002 by Bj�rn Stenberg
*
*/
#include <lib/string.h>
#include <sys_def/errors.h>

#include <kernel/kernel.h>
#include <kernel/list.h>

#include <fs/stdfs.h>
#include <fs/fat.h>
#include <fs/vfs_node.h>
#include <fs/vfs.h>

extern struct vfs_mountPoint * root_mountPoint;

DIR * opendir(char * pathname)
{
    MED_RET_T ret_val;
    struct vfs_pathname path;
    DIR * fd;
    
    if(!root_mountPoint)
        return NULL;
        
    path.length = strlen(pathname);
    if(path.length>0)
        path.str=pathname;
    else
        return NULL; //-MED_EINVAL

    if(path.str[0] != '/')
    {
        printk("'%s' not an absolute path\n",pathname);
        printk("only root path (strating with '/') are supported\n");
        return NULL;//-MED_EINVAL;
    }

    ret_val=vfs_nodeLookup(&path,root_mountPoint->root_node,&fd,&path);

    if(ret_val!=MED_OK && ret_val!=-MED_ENOENT)
    {
        printk("Err(%d): during 'vfs_nodeLookup' call\n",-ret_val);
        return NULL;
    }

    if(path.length<=0)
    {
        /* ok found it !! */
        if(fd->opened)
        {
            /* already opened */
            printk("Dir %s alreday opened\n",pathname);
            return NULL;//-MED_EBUSY;
        }

        if(fd->type == VFS_TYPE_FILE)
        {
            /* it is a file => can't open file with opendir !! */
            printk("Trying to open a file with opendir !!\n");
            printk("File name: '%s'\n",pathname);
            return NULL;
        }

    }
    else
    {
        printk("File '%s' not found\n",pathname);
        return NULL;//-MED_ENOENT;
    }

    ret_val=fat_fileOpen(fd);

    if(ret_val != MED_OK)
    {
        printk("Error in fat_open: %d\n",-ret_val);
        return NULL;
    }

    /* check if we need to load content */
    if(!fd->dir_loaded)
    {
        printk("[VFS] need to load folder\n");
        ret_val = fat_loadDir(fd);
        if(ret_val!=MED_OK)
        {
            printk("error loading folder: err=%d\n",-ret_val);
            return NULL;
        }
    }

    /* ok file is opened */
    fd->curNode = NULL;
    fd->opened = 1;
    vfs_nodeRef(fd);

    LIST_ADD_TAIL_NAMED(fd->mount_point->opened_node,fd,prev_open,next_open);

    return fd;
}

MED_RET_T closedir(DIR * fd)
{
    MED_RET_T ret_val;
    CHK_FD(fd);

    ret_val = fat_fileClose(fd);

    if(ret_val!=MED_OK)
        return ret_val;

    fd->opened = 0;
    LIST_DELETE_NAMED(fd->mount_point->opened_node,fd,prev_open,next_open);
    vfs_nodeUnRef(fd);
    
    return MED_OK;
}

struct dirent * readdir(DIR * fd)
{
    struct dirent * entry;

    if(!fd->opened || !fd->children)
        return NULL;

    entry= &fd->theent;

    if(!fd->curNode)
    {
        entry->d_name=fd->children->name.str;
        entry->type=fd->children->type;
#warning need a statfs function here
        entry->attribute=fat_attribute(fd->children);
        entry->size=fat_getFileSize(fd->children);
        fd->curNode=fd->children->siblings_next;
    }
    else
    {
        if(fd->curNode  != fd->children)
        {
            entry->d_name=fd->curNode->name.str;
            entry->type=fd->curNode->type;
            entry->attribute=fat_attribute(fd->curNode);
            entry->size=fat_getFileSize(fd->curNode);
            fd->curNode=fd->curNode->siblings_next;
        }
        else
            return NULL;
    }
    return entry;
}


