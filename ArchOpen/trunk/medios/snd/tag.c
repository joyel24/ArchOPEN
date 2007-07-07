/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <lib/string.h>
#include <sys_def/colordef.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/thread.h>
#include <kernel/delay.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <driver/buttons.h>
#include <driver/dsp.h>
#include <driver/aic23.h>
#include <fs/stdfs.h>
#include <gfx/graphics.h>

#include <snd/buffer.h>
#include <snd/playlist.h>
#include <snd/codec.h>
#include <snd/output.h>
#include <snd/tag.h>


static void tag_getValue(PLAYLIST_ITEM * item, char * tag, char * output,int length){

    if(!strcasecmp(tag,"title") && item->tag.title!=NULL){
        strncpy(output,item->tag.title,length);
    }else if(!strcasecmp(tag,"artist") && item->tag.artist!=NULL){
        strncpy(output,item->tag.artist,length);
    }else if(!strcasecmp(tag,"album") && item->tag.album!=NULL){
        strncpy(output,item->tag.album,length);
    }else if(!strcasecmp(tag,"genre") && item->tag.genre!=NULL){
        strncpy(output,item->tag.genre,length);
    }else if(!strcasecmp(tag,"track") && item->tag.track!=NULL){
        strncpy(output,item->tag.track,length);
    }else if(!strcasecmp(tag,"date") && item->tag.date!=NULL){
        strncpy(output,item->tag.date,length);
    }else if(!strcasecmp(tag,"filename")){
        strncpy(output,strrchr(item->name,'/')+1,length);
    }else if(!strcasecmp(tag,"bitrate")){
        snprintf(output,length,"%d",item->tag.bitRate/1000);
    }else if(!strcasecmp(tag,"samplerate")){
        snprintf(output,length,"%d",item->tag.sampleRate);
    }

}

static void tag_parseValues(PLAYLIST_ITEM * item, char * format, char * output,int length){
    char * c;
    char * tag;
    char * op;
    bool inTag;
    char save;

    c=format;
    op=output;
    inTag=false;
    tag=NULL;

    while(*c && op-output<length){

        if(*c=='%'){

            if(inTag){

                if(tag==c){ // detect %% -> output %

                    *op++='%';

                }else{
                    // tag end -> get tag value and add is to output
                    *op='\0';
                    
                    save=*c;
                    *c='\0';
                    tag_getValue(item,tag,op,length-(op-output));
                    *c=save;

                    op=output+strlen(output);
                }

                inTag=false;

            }else{
                // store tag start
                tag=c+1;
                inTag=true;
            }
        }else{
            if(!inTag){ // not in tag -> add chars to output
                *op++=*c;
            }
        }

        ++c;
    }

    *op='\0';
}

static void tag_getFunctionResult(char * function,char * * params,int paramCount,char * output,int length){

    if(function==NULL) return;
    
    if(!strcasecmp(function,"if")){
        if(params[0]==NULL || strlen(params[0])==0){
            if (params[2]!=NULL){
                strncpy(output,params[2],length);
            }
        }else{
            if (params[1]!=NULL){
                strncpy(output,params[1],length);
            }
        }
    }
}

static void tag_parseFunctions(char * format, char * output,int length){
    char * localFormat;
    char * c;
    char * op;
    char * start;
    char * tmp;
    bool inFunction;
    int parenthesisLevel;
    char save;
    int i;

    char * function;
    char * params[10];
    int paramCount;

    localFormat=strdup(format);

    c=localFormat;
    op=output;
    start=NULL;
    inFunction=false;
    parenthesisLevel=0;

    paramCount=0;
    function=NULL;
    for(i=0;i<10;++i){
        params[i]=NULL;
    }

    while(*c && op-output<length){

        switch(*c){

            case '$':
                if(inFunction){

                    if(start==c && parenthesisLevel==0){ // detect $$ -> output $
                        *op++='$';
                    }

                }else{
                    // store function name start
                    start=c+1;
                    inFunction=true;
                }
                break;

            case '(':
                ++parenthesisLevel;

                if(parenthesisLevel==1){
                    // get function name
                    *c='\0';
                    function=start;

                    // store params start
                    start=c+1;
                }
                break;

            case '|':
            case ')':
                save=*c;

                if(parenthesisLevel==1){
                    // get new parameter
                    *c='\0';
                    params[paramCount]=start;
                    paramCount=MIN(9,paramCount+1);

                    // store next param start
                    start=c+1;

                    if(save==')'){ // end of function
                        inFunction=false;

                        // recurse to handle nested functions
                        for(i=0;i<10;++i){
                            if(params[i]!=NULL){
                                tmp=params[i];
                                params[i]=malloc(1000);
                                tag_parseFunctions(tmp,params[i],1000);
                            }
                        }

                        // add function result to output
                        *op='\0';
                        tag_getFunctionResult(function,params,paramCount,op,length-(op-output));
                        op=output+strlen(output);

                        // clean up
                        paramCount=0;
                        function=NULL;
                        for(i=0;i<10;++i){
                            if(params[i]!=NULL){
                                kfree(params[i]);
                            }
                            params[i]=NULL;
                        }
                    }
                }
 
                if(save==')'){
                    --parenthesisLevel;
                }
                break;

            default:
                if(!inFunction){ // not in function -> add chars to output
                    *op++=*c;
                }
        }
        ++c;
    }

    *op='\0';

    kfree(localFormat);
}

void tag_parse(PLAYLIST_ITEM * item, char * format, char * output,int length){
    char * temp;

    temp=malloc(length+1);

    tag_parseValues(item,format,temp,length);

    tag_parseFunctions(temp,output,length);

    kfree(temp);
}

void tag_clear(TAG * tag){
    tag->title=NULL;
    tag->artist=NULL;
    tag->album=NULL;
    tag->genre=NULL;
    tag->track=NULL;
    tag->date=NULL;
    tag->length=-1;
    tag->bitRate=-1;
    tag->sampleRate=-1;
    tag->stereo=false;
    tag->sampleCount=-1;
    tag->badFile=false;
}

void tag_free(TAG * tag){
    if(tag->title!=NULL) kfree(tag->title);
    if(tag->artist!=NULL) kfree(tag->artist);
    if(tag->album!=NULL) kfree(tag->album);
    if(tag->genre!=NULL) kfree(tag->genre);
    if(tag->track!=NULL) kfree(tag->track);
    if(tag->date!=NULL) kfree(tag->date);
}
