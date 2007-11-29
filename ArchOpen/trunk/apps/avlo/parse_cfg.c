/*
*   loader/parse_cfg.c
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/kernel.h>

#include <fs/stdfs.h>

#include <lib/string.h>

#include <parse_cfg.h>

/*
#include <string.h>
#include <fs_io.h>
#include <kernel.h>
*/

int file;
int feof_reached=0;

int line_num;
int back = 0; /* can go back by one char */

char item_buff[MAX_TOKEN+1];
char value_buff[MAX_TOKEN+1];

int file_open(char *name)
{
    if ((file = open(name,O_RDONLY))<0)
    {
    	printk("error reading config file %s\n",name);
	   return -1;
    }
    printk("file %s opened\n",name);
    line_num = 1;
    feof_reached=0;
    back = 0;
    return 0;
}

void file_close()
{
	close(file);
}

int next_char(void)
{
    char ch[2];
    int ch2;
    

    if (!back)
    {
        if(read(file,&ch,1)!=1)
        {
	    feof_reached=1;
            return 0;
	}
        //printk("read: %c %x\n",ch[0],ch[0]);
	return (int)ch[0]&0xFF;

    }
    ch2 = back;
    back = 0;
    return ch2;
}

void again(int ch)
{
    back = ch;
}

int nxt_token(char * buff)
{
    char *here;
    int ch;

/* processing empty lines and comments */
    while (1) {
	while ((ch = next_char()), ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
        {
            //printk("processing %c, it is a space, tab or return \n",ch);
	    if (ch == '\n' || ch == '\r')
            {
               // printk("return => inc line num\n");
                line_num++;
            }
        }
	if (feof_reached)
        {
            //printk("eof loop1 => return 0\n");
            return 0;
        }
	if (ch != '#')
        {
            //printk("char %c <> # => break loop 1\n",ch);
            break;
        }
        
	while ((ch = next_char()), (ch != '\n' && ch != '\r'))
        {
            //printk("processing comment %c\n",ch);
	    if (feof_reached)
            {
                //printk("eof loop2 => return 0\n");
                return 0;
            }
        }
        //printk("end of comment => loop again\n");
	line_num++;
    }
    //printk("found an interesting char : %c\n",ch);
/* processing '=' char */    
    if (ch == '=')
    {
        //printk("it's a '=' => return it\n");
    	buff[0]='=';
	buff[1]=0;
    	return 1;
    }
/* processing quoted string */
    if (ch == '"') {
        //printk("start of a quote\n");
	here = buff;
	while (here-buff < MAX_TOKEN) {
	    ch = next_char();
	    if (feof_reached)
            {
                printk("EOF in quoted string");
                return 0;
            }
	    if (ch == '"') {
		*here = 0;
                //printk("found end of quoted string : %s\n",buff);
		return 1;
	    }
	    if (ch == '\n' || ch == '\r' || ch == '\t')
		printk("\\n and \\t are not allowed in quoted strings");
	    *here++ = ch;
	}
	printk("Quoted string is too long");
	return 0; /* not reached */
    }
/* processing normal char buff-> start of string, here-> pointer to current char*/
    here = buff;
    //printk("std process of char : %c\n",ch);
    while (here-buff < MAX_TOKEN) {
        //printk("current char: %c %d\n",ch,ch);
	if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '#' ||
	      ch == '=' || feof_reached) {
		again(ch);
		*here = 0;
                //printk("end of current item: %s\n",buff);
		return 1;
	    }
	*here++ = ch;
	ch = next_char();
    }
    printk("Token is too long: %s\n",buff);
    return 0; /* not reached */
}

int nxt_cfg(char *item,char *value)
{    
    if (!nxt_token(item)) return 0;
    
    //printk("[nxt_cfg]: item=|%s|\n",item);
    
    if (!strcmp(item,"="))
    {
    	printk("Syntax error");
	return 0;
    }
    
    if (!nxt_token(value)) return 0;
    if (strcmp(value,"="))
    {
        printk("Error '=' expected (get: %s - l:%d)\n",value,line_num);
        return 0;
    }
    if (!nxt_token(value))
    {
    	printk("Value expected");
	return 0;
    }
    
    //printk("[nxt_cfg]: value=|%s|\n",value);
    
    if (!strcmp(value,"="))
    {
    	printk("Syntax error after %s",item);
	return 0;
    }
    return 1;
}

void iniCfg(struct config_image * cfg)
{
	cfg->image[0]=0;
	cfg->label[0]=0;
	cfg->append[0]=0;
	cfg->comment[0]=0;
}

void iniCfgG(struct config_gene * cfgG)
{
	cfgG->defBin[0]=0;
//        cfgG->key[0]=0;
        cfgG->repeat=0;
        cfgG->timeOut=0;
        cfgG->bg_img[0]=0;
}

int chkCfg(struct config_image * cfg)
{
	if(cfg->image[0] != 0 && cfg->label[0] !=0)
		return 1;
	else
	{
		printk("invalide config %s %s\n",cfg->image,cfg->label);
		return 0;
	}
}	

int do_parse(struct config_image * cfg,struct config_gene * cfgG)
{
    char *item=item_buff;
    char *value=value_buff;
    int cfg_id=-1;
    int pos;
    
    for(pos=0;pos<MAX_CFG;pos++)
		iniCfg(&cfg[pos]);
    	iniCfgG(cfgG);
	
    while (1) {
	if (!nxt_cfg(item,value))
        {
        	//cfg_id=-1;
        	break;
        }
        
	if(!strcmp(item,"image"))
	{
		if(cfg_id>=0)
		{
			if(chkCfg(&cfg[cfg_id]))
			{
				cfg_id++;
				if(cfg_id>=MAX_CFG)
				{
					printk("too much images defined\n");
					//return cfg_id-1;
				}
			}
			else
				iniCfg(&(cfg[cfg_id]));
		}
		else
		{
			cfg_id++;		
			if(cfg_id>=MAX_CFG)
			{
				printk("too much images defined\n");
				//return cfg_id-1;
			}
		}
		strcpy(cfg[cfg_id].image,value);
	}
	else if(!strcmp(item,"label"))
	{
		if(cfg_id<0)
		{
			printk("label param before image\n");
		}
		else
		{
			strcpy(cfg[cfg_id].label,value);
		}
		
	}
	else if(!strcmp(item,"append"))
	{
		if(cfg_id<0)
		{
			printk("append param before image\n");
		}
		else
		{
			strcpy(cfg[cfg_id].append,value);
		}
	}
	else if(!strcmp(item,"default"))
	{
		strcpy(cfgG->defBin,value);
	}
        /*else if(!strcmp(item,"key"))
	{
		strcpy(cfgG->key,value);
	}*/
        else if(!strcmp(item,"repeat"))
	{
		cfgG->repeat=atoi(value);
	}
        else if(!strcmp(item,"time-out"))
	{
		cfgG->timeOut=atoi(value);
	}
        else if(!strcmp(item,"bg_img"))
	{
		strcpy(cfgG->bg_img,value);
	}
	else
		printk("unknown item type: %s on line %d (cfg_id=%d)\n",item,line_num,cfg_id);
    }
    return cfg_id;    
}



