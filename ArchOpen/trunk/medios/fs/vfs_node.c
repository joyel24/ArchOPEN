/*
*   kernel/fs/vfs_node.c
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
#include <kernel/list.h>

#include <fs/fat.h>
#include <fs/vfs.h>
#include <fs/vfs_node.h>

extern struct vfs_mountPoint * root_mountPoint;

void vfs_nodePrintTree(struct vfs_node *node,int level)
{
    struct vfs_node * ptr;
    int nb_children;
    LIST_FOREACH_NAMED(node->children,ptr,nb_children,
                siblings_prev,siblings_next)
    {
        int i;
        for(i=0;i<level;i++)
            printk("  ");
        vfs_nodePrint(ptr);
        if(ptr->type==VFS_TYPE_DIR)
        {
            if(ptr->children)
                vfs_nodePrintTree(ptr,level+1);
        }
    }
}

void vfs_nodePrint(struct vfs_node *node)
{
    struct fat_entry * ptr_fat;
    printk("%s",node->name.str);
    if(node->type==VFS_TYPE_DIR)
    {
        printk("/");
    }

    ptr_fat=(struct fat_entry *)node->custom_data;

    printk("   (%d-%d)\n",node->ref_cnt,ptr_fat->dirEntryNum);
}

MED_RET_T vfs_nodeInitChild(struct vfs_mountPoint * mount_point,
                struct vfs_node * parent,
                struct  vfs_pathname * name,
                struct vfs_node * node,
                vfs_node_type type)
{
    /*allocate space for the name*/
    if(name && name->length>0)
    {
        VFS_PRINT("[vfs_nodeInitChild] name=|%s|len=%d\n",name->str,name->length);
        node->name.str = (char*)kmalloc(name->length+1);
        if(!node->name.str)
            return -MED_ENOMEM;
        memcpy(node->name.str,name->str,name->length+1);
        node->name.length=name->length;
    }
    else
    {
        node->name.str=NULL;
        node->name.length=0;
    }

    node->ref_cnt = 0;
    node->type = type;
    node->mount_point=mount_point;
    vfs_nodeAddChild(parent,node);

    return MED_OK;
}

MED_RET_T vfs_nodeExist(struct vfs_pathname * path,struct vfs_node ** node)
{
    struct  vfs_pathname new_path;
    char path_str[MAX_PATH];
    int ret_val;
    
    if(!node)
        return -MED_EINVAL;
      
    vfs_pathNameDup(path,&new_path,path_str);
          
    ret_val=vfs_nodeLookup(&new_path,root_mountPoint->root_node,node,&new_path);
    if(ret_val!=MED_OK && ret_val!=-MED_ENOENT)
    {
        printk("[vfs_nodeExist] Err(%d): during 'vfs_nodeLookup' call\n",-ret_val);
        return -MED_EINVAL;
    }
    
    if(new_path.length>0)
        *node=NULL;
        
    return MED_OK;
}

MED_RET_T vfs_nodeAddChild(struct vfs_node * parent, struct vfs_node * child)
{
    child->parent=parent;
    if(parent)
    {
        if (strcmp(child->name.str, ".") && strcmp(child->name.str, ".."))
            vfs_nodeRef(parent);
        LIST_ADD_HEAD_NAMED(parent->children,child,siblings_prev,siblings_next);
    }
    return MED_OK;
}

MED_RET_T vfs_nodeLookup(struct  vfs_pathname * path,
                    struct vfs_node * start_node,
                    struct vfs_node ** result_node,
                    struct  vfs_pathname * result_remaining_path)
{
    struct vfs_node * current_node;
    int ret_val;

    if(!start_node) return -MED_ENOENT;

    /* some sanity check */
    if(path->length <= 0)  return -MED_ENOENT;


    /* init of var */
    *result_node = NULL;
    memcpy(result_remaining_path,path,sizeof(*path));
    current_node = start_node;

    while(1)
    {
        struct  vfs_pathname current_component,remaining_path;
        struct vfs_node * nxt_node;
        int slash_after;

        slash_after = vfs_pathnameSplit(result_remaining_path,&current_component,&remaining_path);
        if(current_component.length == 0) /* we've found the whole path */
        {
            memcpy(result_remaining_path,&remaining_path,sizeof(remaining_path));
            *result_node=current_node;
            return MED_OK;
        }

        /* we still have to search, is current_node a DIR? */
        if(current_node->type != VFS_TYPE_DIR)
        {
            /* no => error */
            return -MED_EBADDATA;
        }

        /* let's try to find the entry in current VFS tree */
        nxt_node = NULL;
        if(vfs_pathnameIsStr(".",&current_component))
        {
            /* only asking for current tree level
            -> nothing more to do than return current node */
            nxt_node = current_node;
        }
        else if(vfs_pathnameIsStr("..",&current_component))
        {
            /* trying to reach upper level */
            if(current_node == root_mountPoint->root_node)
            {
                /* already at root level
                -> nothing more to do than return current node */
                nxt_node = current_node;
            }
            else
            {
                if(current_node->parent != NULL)
                {
                    /* parent exists */
                    nxt_node = current_node->parent;
                }
                else
                {
                    printk("[FATAL ERROR] in vfs_nodeLookup current node has no parent\n");
                    nxt_node = current_node;
                }
            }
        }
        else
        {
            /* std lookup */
            int nb_children;
            struct vfs_node * child;

            if(!current_node->dir_loaded)
            {
                ret_val=fat_loadDir(current_node);
                if(ret_val!=MED_OK)
                {
                    printk("Error loading dir %s: %d\n",current_node->name.str,-ret_val);
                    return ret_val;
                }
            }

            LIST_FOREACH_NAMED(current_node->children,child,nb_children,
                siblings_prev,siblings_next)
            {
                if(vfs_pathnameIsEq(&child->name,&current_component))
                        break; /* found it */

            }
            if(LIST_FOREACH_EARLY_BREAK(current_node->children,child,nb_children))
            {
                /* found it */
                nxt_node = child;
            }
        }

        if(nxt_node == NULL)
        {
            *result_node=current_node;
            return -MED_ENOENT;
        }
        /*else
        {
            //vfs_nodeUnRef(current_node);
        }*/

        /* sanity check on new node */
        if(slash_after && nxt_node->type != VFS_TYPE_DIR)
        {
            /* we have a slash => folder, but type != dir */
            return -MED_ENOTDIR;
        }
        memcpy(result_remaining_path,&remaining_path,sizeof(remaining_path));

        current_node=nxt_node;

    }

    printk("[FATAL ERROR] in vfs_nodeLookup should not go to end of fct\n");
    return MED_ERROR;
}

MED_RET_T vfs_nodeRef(struct vfs_node * node)
{
    if(!(node->ref_cnt >= 0))
    {
        printk("[FATAL ERROR] in vfs_nodeRef cnt is not =>0 : %d\n",node->ref_cnt);
        return -MED_EBADDATA;
    }
    node->ref_cnt++;
    return MED_OK;
}

MED_RET_T vfs_nodeUnRef(struct vfs_node * node)
{
    if(!(node->ref_cnt >= 1))
    {
        printk("[FATAL ERROR] in vfs_nodeUnRef cnt is not =>1 : %d\n",node->ref_cnt);
        return -MED_EBADDATA;
    }
    node->ref_cnt--;
    return MED_OK;
}

MED_RET_T vfs_rmNodeFromTree(struct vfs_node * node)
{
    if(node->ref_cnt>0)
    {
        printk("Error: can't delete node %s there has still %d ref\n",node->name.str,node->ref_cnt);
        return -MED_EINVAL;
    }

    if(node->parent)
    {
        struct vfs_node * parent = node->parent;
        if (strcmp(node->name.str, ".") && strcmp(node->name.str, ".."))
            if(vfs_nodeUnRef(node->parent)!=MED_OK)
                return -MED_EBADDATA;
        /* are we removing current node ?*/
        if(parent->children == node &&  parent->curNode == node->siblings_next)
            parent->curNode = NULL;
        LIST_DELETE_NAMED(parent->children,node,siblings_prev,siblings_next);
    }
    vfs_nodeDestroy(node);
    return MED_OK;
}

MED_RET_T vfs_nodeDestroy(struct vfs_node * node)
{
    if(node)
    {
        kfree(node->name.str);
        if(node->custom_data)
            kfree(node->custom_data);
        kfree(node);
    }
    return MED_OK;
}

MED_RET_T vfs_nodeSetDirty(struct vfs_node * node)
{
    if(node->dirty)
        return MED_OK;
    node->dirty=1;
    LIST_ADD_TAIL_NAMED(node->mount_point->dirty_list,node,prev_dirty,next_dirty);
    return MED_OK;
}

MED_RET_T vfs_nodeClearDirty(struct vfs_node * node)
{
    if(!node->dirty)
        return MED_OK;
    node->dirty=0;
    LIST_DELETE_NAMED(node->mount_point->dirty_list,node,prev_dirty,next_dirty);
    return MED_OK;
}
