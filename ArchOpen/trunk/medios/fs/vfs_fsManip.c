/*
*   kernel/fs/vfs_fsManip.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*
*/

#include <lib/string.h>
#include <sys_def/errors.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>

#include <fs/stdfs.h>
#include <fs/vfs.h>
#include <fs/fat.h>

#warning these functions update VFS => any running readdir in the parent will be corrupted
#warning might be a problem when doing recursive rm in a folder

MED_RET_T mkdir(char *name, int mode)
{
#warning MODE not used in mkdir
    DIR *dir;
    struct vfs_pathname path;
    struct vfs_pathname dirName;

    char namecopy[MAX_PATH];
    char namecopy2[MAX_PATH];
    
    struct dirent *entry;
    MED_RET_T ret_val;
    
    path.length = strlen(name);
    if(path.length>0)
        path.str=name;
    else
    {
        printk("Error empty path\n");
        return -MED_EINVAL;
    }

    vfs_pathNameDup(&path,&path,namecopy2);
    
    /* check for an ending '/' */

   if(path.str[path.length-1] == '/')
    {
        printk("We have an ending / discarding it\n");
        path.str[path.length-1]='\0';
        path.length--;
    }

    /* Split the base name and the path */
    vfs_pathnameSplitPath(&path,&path,&dirName);

    /* create a copy of dirName */
    vfs_pathNameDup(&dirName,&dirName,namecopy);

    /*put an end to path*/
    path.str[path.length]='\0';

    if(dirName.length == 0)
    {
        printk("[mkdir] Empty dir name\n");
        return -MED_EINVAL;
    }

    dir = opendir(path.str);

    if(!dir) {
        printk("mkdir: can't open parent dir\n");
        return -MED_ERROR;
    }

    /* Now check if the name already exists */
    while ((entry = readdir(dir))) {
        if ( !strcasecmp(dirName.str, entry->d_name) ) {
            printk("[mkdir] error: folder already exists\n");
            closedir(dir);
            return -MED_EEXIST;
        }
    }

    VFS_PRINT("About to call fat_creatDir\n");

    ret_val = fat_createDir(&dirName, dir);

    closedir(dir);

    return ret_val;
}

MED_RET_T rmdir(char* name,int recurse)
{
    MED_RET_T ret_val;
    DIR* dir;
    struct dirent* entry;
    char fullname[MAX_PATH];
    
    dir = opendir(name);
    if (!dir)
    {
     /* open error */
        return -1;
    }
    
    /* check if the directory is empty */
    while ((entry = readdir(dir)))
    {
        if (strcmp(entry->d_name, ".") &&
            strcmp(entry->d_name, ".."))
        {
#if 1
            if(recurse)
            {
                snprintf(fullname,MAX_PATH,"%s/%s",name[1]==0?"":name,entry->d_name);
                if(entry->type==VFS_TYPE_FILE)
                    /* NOTE: need to rebuild path: name/entry->d_name */
                    rmfile(fullname);
                else
                    rmdir(fullname,1);
            }
            else
#endif
            {
                printk("[rmdir] error, folder not empty\n");
                closedir(dir);
                return -MED_ENOTEMPTY;
            }
        }
    }

    
    
    ret_val = fat_fileRemove(dir);
    if ( ret_val != MED_OK )
    {
        printk("[rmdir] Failed removing dir |%s| (err=%d)\n", name,-ret_val);
    }
    
    closedir(dir);
    
    ret_val = vfs_rmNodeFromTree(dir);
    if ( ret_val != MED_OK )
    {
        printk("[rmdir] Failed removing node (%s) (err=%d)\n", name,-ret_val);
    }
    
    return ret_val;
}


MED_RET_T rmfile(char* name)
{
    MED_RET_T ret_val;
    FILE * file;
    int fd = open(name, O_WRONLY);
    
    if ( fd < 0 )
    {
        printk("[rmfile] file (%s) can't be opened (err=%d)\n", name,-fd);
        return fd;
    }

    file = (FILE*)fd;
    
    ret_val = fat_fileRemove(file);    
    if ( ret_val != MED_OK )
    {
        printk("[rmfile] Failed removing dir |%s| (err=%d)\n", name,-ret_val);
        return ret_val;
    }
   
    fat_setFileSize(file,0);
    
    ret_val = close(fd);
    if ( ret_val != MED_OK )
    {
        printk("[rmfile] error closing |%s| (err=%d)\n", name,-ret_val);
        return ret_val;
    }
    
    ret_val = vfs_rmNodeFromTree(file);
    if ( ret_val != MED_OK )
    {
        printk("[rmfile] Failed removing node (%s) (err=%d)\n", name,-ret_val);
        return ret_val;
    }
    
    return MED_OK;
}

MED_RET_T mvEntry(char* path,char* newpath,struct vfs_node * openedfile, vfs_node_type type)
{
    MED_RET_T ret_val;
    struct vfs_pathname VFS_newPath;
    struct vfs_pathname VFS_newName;
    char newpath1[MAX_PATH];
    char newpath2[MAX_PATH];
    DIR * parent_dir;
    
    VFS_newPath.str = newpath;
    VFS_newPath.length = strlen(newpath);
    
    /* making sure not to change original string */
    vfs_pathNameDup(&VFS_newPath,&VFS_newPath,newpath1);
    
    /* Split the base name and the path */
    vfs_pathnameSplitPath(&VFS_newPath,&VFS_newPath,&VFS_newName);

    /* create a copy of dirName */
    vfs_pathNameDup(&VFS_newName,&VFS_newName,newpath2);

    /*put an end to path*/
    VFS_newPath.str[VFS_newPath.length]='\0';
        
    parent_dir = opendir(VFS_newPath.str);
    if(!parent_dir)
    {
        printk("[mv%s] Error opening parent folder |%s|\n",type==VFS_TYPE_FILE?"file":"dir",
            VFS_newPath.str);
        return -MED_ERROR;
    }
    
    if(parent_dir->mount_point != openedfile->mount_point)
    {
        printk("[mv%s] error: src and dest not on the same drive => not supported atm",
            type==VFS_TYPE_FILE?"file":"dir");
        return -MED_ERROR;
    }
    
    ret_val = fat_mvFileDir(openedfile, parent_dir, &VFS_newName);
    if ( ret_val != MED_OK )
    {
        printk("[mv%s] Failed renaming file: %d\n",type==VFS_TYPE_FILE?"file":"dir", -ret_val);
        return ret_val;
    }

    ret_val = closedir(parent_dir);
    if (ret_val != MED_OK)
    {
        printk("[mv%s] Failed closing parent folder: %d\n",type==VFS_TYPE_FILE?"file":"dir", -ret_val);
        return ret_val;
    }

    return ret_val;
}

MED_RET_T cpEntry(char* path,char* newpath,struct vfs_node * fd)
{
    int fd_dest;
    char * buffer;
    int nbBytes,size;
    
    fd_dest=open(newpath,O_CREAT|O_RDWR);
    
    if(fd_dest<0)
    {
        printk("[cpEntry] error creating new file: %s\n",-fd_dest);
        return fd_dest;   
    }
    
    buffer=(char*)kmalloc(512);
    
    if(!buffer)
    {
        printk("[cpEntry] error malloc buffer\n");
        return -MED_ENOMEM;
    }
    
    size=filesize((int)fd);
    
    printk("Need to copy %d bytes\n");
    
    while((nbBytes=read((int)fd,buffer,512))>0)
    {
        size-=nbBytes;
        if(write(fd_dest,buffer,nbBytes)!=nbBytes)
        {
            printk("Wrote less than expected\n");
            close(fd_dest);
            rmfile(newpath);
            kfree(buffer);
            return -MED_EIO;
        }
    }
    
    kfree(buffer);
    close(fd_dest);
    
    if(size>0)
    {
        printk("Org file read ended while size >0: %d\n",size);
        return -MED_EIO;
    }
    
    return MED_OK;    
}

MED_RET_T cpMvFile(char* path, char* newpath,int type)
{
    int fd;
    MED_RET_T ret_val,ret_val2;
    /* verify new path does not already exist or isn't a folder */
    fd = open(newpath, O_RDONLY);
    if ( fd >= 0 || fd == -MED_EISDIR)
    {
        close(fd);
        printk("[mvFile] error in params : newpath %s %s\n",newpath,fd == -MED_EISDIR?"is a dir":"already exists");
        return -MED_EINVAL;
    }
    
    fd = open(path, O_RDONLY);
    if ( fd < 0 )
    {
        printk("[mvFile] error in params : curpath %s does not exists or is not a file\n",path);
        return -fd;
    }
    
    if(type==1) /* MV */
    {
        ret_val = mvEntry(path, newpath, (FILE*)fd,VFS_TYPE_FILE);
        if(ret_val == MED_OK)
        {
            ((FILE*)fd)->ref_cnt = 0 ;
            ret_val2 = vfs_rmNodeFromTree((FILE*)fd);
            if ( ret_val2 != MED_OK )
            {
                printk("[mvfile] error removing node |%s| (err=%d)\n", path,-ret_val2);
                close(fd);
                return ret_val2;
            }
        }
    }
    else /* CP */
    {
        ret_val = cpEntry(path, newpath, (FILE*)fd);
    }
    close(fd);
    return ret_val;
}

MED_RET_T mvfile(char* path, char* newpath)
{
    return cpMvFile(path,newpath,1);   
}

MED_RET_T cpfile(char* path,char* newpath)
{
    return cpMvFile(path,newpath,0);
}

MED_RET_T mvdir(char* path,char* newpath)
{
    DIR * dir;
    int fd;
    MED_RET_T ret_val,ret_val2;
    
    /* verify new path does not already exist */
    dir = opendir(newpath);
    if ( dir != NULL )
    {
        closedir(dir);
        printk("[mvDir] error in params : newpath %s already exists\n",newpath);
        return -MED_EINVAL;
    }
    
    fd = open(newpath, O_RDONLY);
    if(fd >= 0)
    {
        close(fd);
        printk("[mvDir] error in params : newpath %s is a file\n",newpath);
        return -MED_EINVAL;
    }
    
    dir = opendir(path);
    if ( dir == NULL )
    {
        printk("[mvDir] error in params : curpath %s does not exists or is a file\n",path);
        return -MED_EINVAL;
    }
    
    ret_val = mvEntry(path, newpath, dir,VFS_TYPE_DIR);
    
    ret_val2 = closedir(dir);
    if ( ret_val2 != MED_OK )
    {
        printk("[mvDir] error closing dir |%s| (err=%d)\n", path,-ret_val2);
        return ret_val2;
    }
    
    if(ret_val == MED_OK)
    {
        dir->ref_cnt = 0 ;
        ret_val2 = vfs_rmNodeFromTree(dir);
        if ( ret_val2 != MED_OK )
        {
            printk("[mvdir] error removing node |%s| (err=%d)\n", path,-ret_val2);
            return ret_val2;
        }
    }
    
    return ret_val; 
    
}

MED_RET_T fs_cd(char * name)
{
    struct vfs_node * ptrNode;
    
    MED_RET_T ret_val;
    struct vfs_pathname path;
    struct vfs_node * myRoot_node;

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

    ret_val=vfs_nodeLookup(&path,myRoot_node,&ptrNode,&path);
    
    if((ret_val!=MED_OK && ret_val!=-MED_ENOENT) || path.length>0)
    {
        return -MED_ENOENT;   
    }
    
    if(ptrNode->type != VFS_TYPE_DIR)
        return -MED_ENOTDIR;
    
    threadCurrent->path=ptrNode;
    
    return MED_OK;
}

MED_RET_T fs_pwd(char * name)
{
    char * ptr;
    char pwd_str[MAX_PATH];
    struct vfs_node * ptrNode;
    
    if(threadCurrent->path==NULL ||threadCurrent->path==root_mountPoint->root_node)
    {
        name[0]='/';
        name[1]='\0';
    }
    else
    {
        pwd_str[MAX_PATH-1]='\0';
        ptr=&pwd_str[MAX_PATH-1];
        for(ptrNode=threadCurrent->path;ptrNode!=root_mountPoint->root_node;ptrNode=ptrNode->parent)
        {
            ptr-=ptrNode->name.length;
            memcpy(ptr,ptrNode->name.str,ptrNode->name.length);
            ptr--;
            *ptr='/'; 
        }
        /* root / has been added auto by *ptr='/' => 
        we can stop loop without dealing with root node*/
        strcpy(name,ptr);
    }
    return MED_OK;
}
