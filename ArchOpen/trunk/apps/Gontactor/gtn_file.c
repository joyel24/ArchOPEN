//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_file.h                           Author : Captain H@dock
//  Last change : 12/12/2006                    Created : 30/11/2006
//
//  Functions to save and read Gontact structure fields from and to *.gtn files
//------------------------------------------------------------------------------
#include <api.h>

//Constants define
#include "constant.h"

#include "gtn_file.h"
#include "newout.h"
//------------------------------------------------------------------------------
//  gtn_save function
//
//  Save a Gontactor structure in a file
//
//
//  Input args : (2)
//      *gtn        =>  Gontactor struture to save
//      *char       =>  Name of the file to (over)write
//
//  Input arg : (1)
//      int error code  (0=OK)
//------------------------------------------------------------------------------
int gtn_save(Gontact *gtn, char *gtn_filename)
{
    char *group_id=NULL;                    //Group id write buffer
        int str_len=0;

    int gtn_file=-1;                    //Vers le fichier
    gtn_file = open(gtn_filename, O_WRONLY | O_CREAT);     //Creation
    if (gtn_file<=0)                      //if oppening error
    {
        printf("Erreur d'ouverture du fichier gtn.\n");
        return -1;
    }

    write(gtn_file, "#",1);                                                                //write separator
    if((*gtn).str_nom!=NULL)                                                        //avoid write void string
    {
        for(str_len=0;*((*gtn).str_nom+str_len)!='\0';str_len++) {}   //count string length
        write(gtn_file, (*gtn).str_nom,str_len);                                //write the string
    }                                                                                                //Next one

    write(gtn_file, "#",1);
    if((*gtn).str_prenom!=NULL)
    {
        for(str_len=0;*((*gtn).str_prenom+str_len)!='\0';str_len++) {}
        write(gtn_file, (*gtn).str_prenom,str_len);
    }

    write(gtn_file, "#",1);
    if((*gtn).str_naiss!=NULL)
    {
        for(str_len=0;*((*gtn).str_naiss+str_len)!='\0';str_len++) {}
        write(gtn_file, (*gtn).str_naiss,str_len);
    }

    write(gtn_file, "#",1);
    if((*gtn).str_tel_fixe!=NULL)
    {
        for(str_len=0;*((*gtn).str_tel_fixe+str_len)!='\0';str_len++) {}
        write(gtn_file, (*gtn).str_tel_fixe,str_len);
    }

    write(gtn_file, "#",1);
    if((*gtn).str_tel_port!=NULL)
    {
        for(str_len=0;*((*gtn).str_tel_port+str_len)!='\0';str_len++) {}
        write(gtn_file, (*gtn).str_tel_port,str_len);
    }

    write(gtn_file, "#",1);
    if((*gtn).str_adresse!=NULL)
    {
        for(str_len=0;*((*gtn).str_adresse+str_len)!='\0';str_len++) {}
        write(gtn_file, (*gtn).str_adresse,str_len);
    }

    write(gtn_file, "#",1);
    if((*gtn).str_mail!=NULL)
    {
        for(str_len=0;*((*gtn).str_mail+str_len)!='\0';str_len++) {}
        write(gtn_file, (*gtn).str_mail,str_len);
    }

    write(gtn_file, "#",1);
    if((*gtn).pict!=NULL)
    {
        for(str_len=0;*((*gtn).pict+str_len)!='\0';str_len++) {}
        write(gtn_file, (*gtn).pict,str_len);
    }

    write(gtn_file, "#",1);
    if((*gtn).str_crea_date!=NULL)
    {
        for(str_len=0;*((*gtn).str_crea_date+str_len)!='\0';str_len++) {}
        write(gtn_file, (*gtn).str_crea_date,str_len);
    }

    write(gtn_file, "#",1);
    if((*gtn).str_mod_date!=NULL)
    {
        for(str_len=0;*((*gtn).str_mod_date+str_len)!='\0';str_len++) {}
        write(gtn_file, (*gtn).str_mod_date,str_len);
    }


    write(gtn_file, "#",1);
    if((*gtn).group==NULL)
    {
        write(gtn_file, "0|",2);
    }
    else
    {
        group_id = (char *) malloc(5*sizeof(char));     //mem alloc ("|xxx\0")

        int i=-1;
        do{                                             //For each ID
                i++;
                sprintf(group_id,"%d|",*((*gtn).group+i));  //formating text
                for(str_len=0;*(group_id+str_len)!='\0';str_len++) {}
                write(gtn_file, group_id,str_len);                //Write
        }while(*((*gtn).group+i)!=0);                   //End with 0 id

        free(group_id);                                 //mem free
        group_id=NULL;
    }
    write(gtn_file, "#",1);                                //end of file


    close(gtn_file);                        //close file

    return 0;
}


//------------------------------------------------------------------------------
//  gtn_read function
//
//  Read a Gontactor structure in a file
//
//
//  Input args : (2)
//      *gtn        =>  Gontactor struture to save
//      *char       =>  Name of the file to (over)write
//
//  Input arg : (1)
//      int error code
//          0   =   OK
//          -2  =   File openning error
//          -3  =   invalid file structure
//          -4  =   invalid group digit
//          -5  =   last group id isn't 0
//------------------------------------------------------------------------------
int gtn_read(Gontact *gtn, char *gtn_filename)
{
    int *field_length=NULL;                 //Length of each GTN struct field
    char *input_buff=NULL;                  //Reading buffer
    char **gtn_ptn=NULL;
    int cur_field=0,cur_char=0;

    int gtn_file=-1, gtn_file_save=-1;                    //File pointer
    gtn_file = open(gtn_filename, O_RDONLY);     //Open
    if (gtn_file<0)                      //if openning fail
    {
        #ifdef LOG_ON
        char *out_buff=NULL;
        out_buff = (char *) malloc(80 * sizeof(char));
        sprintf(out_buff,"Erreur d'ouverture (r) du fichier gtn (%s).\n", gtn_filename);
        newout(out_buff);
        free(out_buff);
        #endif
        return -2;                          //Bad file
    }

    gtn_file_save = gtn_file;                //save start position
    //First pass : Determine string length for each field.

    input_buff = (char *) malloc(2*sizeof(char));   //char readed + '\0'
    field_length = (int *) malloc(11*sizeof(int));
    //field length (for group, nb of group ids)
    //initialize
    for(cur_field=0;cur_field<11;cur_field++)
    {
        *(field_length+cur_field)=0;
    }
    cur_field=0;

    read(gtn_file,input_buff,1);
    if (*input_buff!='#')
    {
        #ifdef LOG_ON
        newout("Premier caractère invalide, fichier corrompu.\n");
        #endif
        return -3;
    }
    do{
        read(gtn_file,input_buff,1);        //read file char
        if (*input_buff=='#')               //if field end
        {
            cur_field++;                    //Next field
        }
        else
        {
            *(field_length+cur_field)+= 1;  //incr field length
        }

    }while(cur_field<10);                   //for each field

    //Group
    do{
        read(gtn_file,input_buff,1);  //read file char
        if (*input_buff=='#')               //if field end
        {
            cur_field++;                    //Next field
        }
        if (*input_buff=='|')               //if field end
        {
            *(field_length+cur_field)+= 1;  //incr field length
        }

    }while(cur_field<11);                   //for each field


    close(gtn_file);                       //close file
    gtn_file=-1;


//DEBUG PRINT FIELD LENGTH
//    for(cur_field=0;cur_field<11;cur_field++)
//    {
//        printf("%d-",*(field_length+cur_field));
//    }
//    printf("\n");


    gtn_file = open(gtn_filename, O_RDONLY);     //Open
    if (gtn_file<0)                      //if openning fail
    {
        #ifdef LOG_ON
        char *out_buff=NULL;
        out_buff = (char *) malloc(80 * sizeof(char));
        sprintf(out_buff,"Erreur d'ouverture (r) du fichier gtn (%s).\n", gtn_filename);
        newout(out_buff);
        free(out_buff);
        #endif
        return -2;                          //Bad file
    }
    gtn_file = gtn_file_save;                //load start position  FIXME : WHY DOES IT NEEDS THAT????

    //Second pass : load string in gtn struct for each field.

    //mem allocation
    //table of strings pointers
    gtn_ptn = (char **) malloc(10*sizeof(char *));


    for(cur_field=0;cur_field<10;cur_field++)       //string field mem alloc
    {
        *(gtn_ptn+cur_field)=(char *) malloc((*(field_length+cur_field)+1)*sizeof(char));
    }

    (*gtn).group = (int *) malloc(*(field_length+10)*sizeof(int));  //group list


    //Loading data
    for(cur_field=0;cur_field<10;cur_field++)           //For each string field
    {
        read(gtn_file,input_buff,1);                                  //jump the #
        for(cur_char=0;cur_char<*(field_length+cur_field);cur_char++)   //for each char
        {
            read(gtn_file,input_buff,1);
            *(*(gtn_ptn+cur_field)+cur_char)=*input_buff;
        }
        *(*(gtn_ptn+cur_field)+cur_char)='\0';          //End char for each string
    }

    //group

    read(gtn_file,input_buff,1);                                      //jump the #
    for(cur_field=0;cur_field<*(field_length+10);cur_field++)
    {
        *((*gtn).group+cur_field)=0;
        do{
            read(gtn_file,input_buff,1);
            if(*input_buff>='0' && *input_buff<='9')        //if it's a digit
            {
                *((*gtn).group+cur_field)*=10;              //update group int
                *((*gtn).group+cur_field)+=*input_buff-'0';
            }
            else
            {
                if(*input_buff!='|')                        //unexpected char
                {
                    #ifdef LOG_ON
                    newout("Digit de groupe invalide, fichier corrompu.\n");
                    #endif
                    free((*gtn).group);         //free mem
                    (*gtn).group=NULL;          //set NULL value
                    return -4;
                }
            }

        }while(*input_buff!='|');
    }

    if(*(field_length+10)==1)
    {
            free((*gtn).group);         //free mem
            (*gtn).group=NULL;          //set NULL value
    }
    else
    {
        if(*((*gtn).group-1+(*(field_length+10)))!=0)         //if last id isn't 0
        {
            #ifdef LOG_ON
            newout("Pas de groupe 0 terminal, fichier corrompu.\n");
            #endif
            free((*gtn).group);         //free mem
            (*gtn).group=NULL;          //set NULL value
            return -5;
        }
    }

    //set pointer of "\0" string to NULL;
    for(cur_field=0;cur_field<10;cur_field++)
    {
        if(*(field_length+cur_field)==0)
        {
            free(*(gtn_ptn+cur_field));         //free mem
            *(gtn_ptn+cur_field)=NULL;          //set NULL value
        }
    }

    //copy pointer to gtn struct
    (*gtn).str_nom=*(gtn_ptn+0);
    (*gtn).str_prenom=*(gtn_ptn+1);
    (*gtn).str_naiss=*(gtn_ptn+2);
    (*gtn).str_tel_fixe=*(gtn_ptn+3);
    (*gtn).str_tel_port=*(gtn_ptn+4);
    (*gtn).str_adresse=*(gtn_ptn+5);
    (*gtn).str_mail=*(gtn_ptn+6);
    (*gtn).pict=*(gtn_ptn+7);
    (*gtn).str_crea_date=*(gtn_ptn+8);
    (*gtn).str_mod_date=*(gtn_ptn+9);

    close(gtn_file);                       //close file


    free(field_length);                     //free mem
    free(gtn_ptn);
    free(input_buff);
    return 0;
}




//------------------------------------------------------------------------------
//  group_read function
//
//  Read group name in a file
//  TODO
//	a progress bar for loading gtn file
//
//  Input args : (2)
//      *gtn        =>  Groups name structure to save
//      *char       =>  Name of the file to read
//
//  Input arg : (1)
//      int error code
//          0   =   OK
//          -2  =   File openning error
//          -3  =   invalid file structure
//          -4  =   endless file structure (or more than 128 groups)
//------------------------------------------------------------------------------
int group_read(Groups *group, char *group_filename)
{
	int group_file=-1; 				                 //File pointer
 	char *read_buff=NULL, *char_buff=NULL;  	//Temp reading buff
	
	int file_readed=0,cur_name=0;
	char *name_len=NULL;
	int cur_char=0;


	read_buff = (char*) malloc(32*sizeof(char));
	char_buff = (char*) malloc(2*sizeof(char));
	
	name_len = (char*) malloc(128*sizeof(char));		//Max 128 groups

	group_file = open(group_filename, O_RDONLY);     //Open File
	if (group_file<0)                      //if openning fail
	{
		#ifdef LOG_ON
		char *out_buff=NULL;
		out_buff = (char *) malloc(80 * sizeof(char));
		sprintf(out_buff,"Erreur d'ouverture (r) du fichier des groupes (%s).\n",group_filename);
		newout(out_buff);
		free(out_buff);
		#endif
		return -2;                          //Bad file
	}

	read(group_file,char_buff,1);

	if(*char_buff!='#')                        //Bad first char
	{
		#ifdef LOG_ON
		newout("Premier caractère invalide, fichier groupe corrompu.\n");
		#endif
		return -3;
	}


	//init struct values

	//First pass : evaluate groups number and strings lengths
	//read loop
	//init value
	cur_name=0;
	file_readed=1;
	
	*(name_len)=1;					//init first len
	do{
		read(group_file,char_buff,1);
		if(*char_buff=='#')
		{
			if(file_readed==0)			//Name end (simple '#')
			{
				cur_name++;			//next name
				*(name_len+cur_name)=1;	//init next len ('\0' size added)
				file_readed=1;
				if(cur_name>=128)
				{
					printf("Limite des groupes possible atteinte.\nFichier probablement endommagé.\n");
					return -4;
				}
			}
			else
			{
				file_readed=2;			//file end (double '#')
			}
		}
		else						//Normal char
		{
			*(name_len+cur_name)+= 1;
			file_readed=0;
		}
	}while(file_readed<2);
	
	
	//save number in struct
	(*group).nb=cur_name;		//last one is empty so there is cur_name names [0;cur_name-1]	

//DEBUG PRINT NAMES LENGTH
//	printf("%d Groupes.\n", (*group).nb);
//	for(cur_name=0;cur_name<(*group).nb;cur_name++)
//	{
//		printf("%d-",*(name_len+cur_name));
//	}
//	printf("\n");

	lseek(group_file,0,SEEK_SET);					//reload start position


	//Second pass : load string in gtn struct for each field.
	
	//mem alloc
	(*group).name = (char**) malloc(((*group).nb)*(sizeof(char *)));
	
	
	//read strings
	for(cur_name=0;cur_name<(*group).nb;cur_name++)
	{
		//Jump the '#'
		read(group_file,char_buff,1);
		//mem alloc
		*((*group).name+cur_name)= (char*) malloc((*(name_len+cur_name))*sizeof(char));
		for(cur_char=0;cur_char<*(name_len+cur_name)-1;cur_char++)
		{
			read(group_file,char_buff,1);
			*(*((*group).name+cur_name)+cur_char)=*char_buff;
		}
		*(*((*group).name+cur_name)+*(name_len+cur_name)-1)='\0';
		
	}
	
	
//DEBUG PRINT NAMES
//	printf("%d groupes.\n", (*group).nb);
//	for(cur_name=0;cur_name<(*group).nb;cur_name++)
//	{
//		printf("%d : %s.\n",cur_name,*((*group).name+cur_name));
//	}
	
	

	free(read_buff);	//free mem
	free(char_buff);
	free(name_len);
	close(group_file);

	return 0;
}



//------------------------------------------------------------------------------
//  gtn_bdd_read function
//
//  Read whole Gontactor database in a single file
//  TODO
//	a progress bar for loading file (add a gui_status_bar)
//
//  Input args : (2)
//      *Gtn_db 	=>  Database structure to load
//      *char       =>  Name of the file to read
//
//  Input arg : (1)
//      int error code
//          0   =   OK
//          -2  =   File openning error
//          -3  =   Invalid file structure
//          -4  =   Out of memory
//------------------------------------------------------------------------------
int gtn_bdd_read(Gtn_bdd *db, char *db_filename)
{
	int db_file=-1, db_file_save=-1;                 	//File pointer
	char *char_buff;			//read buff
	int read_state=0, cur_field=0;
	
	int buff_len=0, cur_char=0;
	char *line_buff=NULL;
	char cur_gtn;
	int file_offset;
	
	db_file = open(db_filename, O_RDONLY);     //Open File
	if (db_file<0)                      //if openning fail
	{
		#ifdef LOG_ON
		char *out_buff=NULL;
		out_buff = (char *) malloc(80 * sizeof(char));
		sprintf(out_buff,"Erreur d'ouverture (r) du fichier des groupes (%s).\n",db_filename);
		newout(out_buff);
		free(out_buff);
		#endif
		return -2;                          //Bad file
	}

	db_file_save = db_file;
	
	char_buff = (char *) malloc(2*sizeof(char));

	(*db).nb=0;
	
	//First pass : Read file structure
	//Check file structure integrity and determine number of gtn in database

	//read_state info :
	//  0	=>	between two database gontact
	//  1	=>	In a Gontact
	//  -1	=>	File fully read
	
	
	do{
		read(db_file,char_buff,1);
		if(read_state==0)
		{
			if(*char_buff=='#')			//First '#' of a new gontact
			{
				read_state=1;
				(*db).nb+=1;
				cur_field=0;
			}
			if(*char_buff=='$')				//double '$' of a bdd end
			{
				#ifdef LOG_ON
				//~ newout("BD_FILE : Reach db file end.\n");
				#endif
				read_state=-1;
			}
			if(*char_buff!='#' && *char_buff!='$')				//abnormal char
			{
				#ifdef LOG_ON
				newout("BD_FILE_ERROR : No initial #.\n");
				#endif
				return -3;
			}
		}
		else
		{			
			if(read_state==1)
			{
				if(*char_buff=='$')						//End of this gtn
				{
					read_state=0;
				}
				if(*char_buff=='#')					//Internal gontact data separator
				{
					cur_field++;
					if(cur_field>11)
					{
						#ifdef LOG_ON
						newout("BD_FILE_ERROR : Too much internal #.\n");
						#endif
						return -3;
					}						
					
				}
			}
		}
		
	}
	while(read_state!=-1);

	#ifdef LOG_ON
	printf("Nb of gtn in db : %d.\n", (*db).nb);
	#endif

	
	(*db).gtn_list= (Gontact *) malloc(((*db).nb)*sizeof(Gontact));	//Mem for each gtn struct in list
	
	if((*db).gtn_list==NULL)								//If malloc fail
	{
		#ifdef LOG_ON
		newout("BD_FILE_ERROR : Out of memory.\n");
		#endif
		return -4;					//Out of memory
	}

	
	file_offset=0;
	
	for(cur_gtn=0;cur_gtn<((*db).nb);cur_gtn++)
	{
		//~ printf("--Offset = %d.\n", file_offset);
		lseek(db_file,file_offset,SEEK_SET);					//reload start position
	
		buff_len=0;
		do{
			read(db_file,char_buff,1);
			buff_len++;
	
		}		
		while(*char_buff!='$');
		buff_len++;
		
		//~ printf("Line %d is %d long.\n", cur_gtn, buff_len);
	
		line_buff = (char *) malloc(buff_len*sizeof(char *));
		if (line_buff==NULL)
		{
			#ifdef LOG_ON
			newout("BD_FILE_ERROR : Out of memory.\n");
			#endif
			return -4;					//Out of memory
		}
	
		lseek(db_file,file_offset,SEEK_SET);					//reload start position
		for(cur_char=0;cur_char<buff_len-1;cur_char++)	//Load line in buffer
		{
			read(db_file,char_buff,1);
			*(line_buff+cur_char)=*char_buff;
		}
		
		*(line_buff+cur_char-1)='\0';
		//~ printf("%s\n\n", line_buff);

		gtn_init((*db).gtn_list+cur_gtn);
		//send line to gtn structure
		gtn_read_line((*db).gtn_list+cur_gtn, line_buff);

		//~ gtn_print((*db).gtn_list+cur_gtn);
		
		free(line_buff);
		line_buff=NULL;
		//~ printf("Offest : %d+%d=", file_offset, buff_len);
		file_offset+=buff_len-1;
		//~ printf("%d.\n", file_offset, buff_len);
	}

	

	free(char_buff);
	free(line_buff);
	close(db_file);

	
	return 0;
}

