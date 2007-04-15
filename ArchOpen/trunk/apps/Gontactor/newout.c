//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : newout.c                 		        Author : Captain H@dock
//  Last change : 07/01/2007                    Created : 29/11/2006
//
//  Utilise la sortie active et une sauvegarde texte.
//------------------------------------------------------------------------------

//Constants define
#include "constant.h"

#include <api.h>

#ifdef LOG_ON
#include "newout.h"

//------------------------------------------------------------------------------
//  newout function
//
//  Combine un printf(char*) et un fprintf(char*) afin de garder une trace
//  des op�ations dans le fichier LOG_FILENAME
//  Peut �re annul�par la non d�inition de LOG_ON (pour les builds finales)
//  Syntaxe recommand� :
//
//    #ifdef LOG_ON
//    sprintf(debug_buff,char*,[opt arg]);
//    newout(debug_buff);
//    #endif
//
//  Input arg : (1)
//      char*        =>  chaine de charact�e ��rire
//
//  Input arg : (0)
//
//------------------------------------------------------------------------------

void newout(char *ligne)
{
	int str_size=0;
	printf(ligne);                      //Vers l'�ran

    int log=-1;                    //Vers le fichier
    log = open(LOG_FILENAME, O_WRONLY | O_APPEND);    //Ouverture
    if (log<0)                      //Si erreur d'ouverture
    {
        printf("Erreur d'ouverture du log.\n");
        return;
    }
    //get string size
    for(str_size=0;*(ligne+str_size)!='\0';str_size++)
    {}
	    
    write(log, ligne,str_size);                //�riture

    close(log);                        //Fermeture
    log=-1;

    return;
}


//------------------------------------------------------------------------------
//  startout function
//
//  D�arre le log. A lancer en d�ut de programme
//
//
//  Input args : (0)
//
//  Input arg : (0)
//
//------------------------------------------------------------------------------

void startout(void)
{
    printf("GTN_LOG compiled on ");                   //Vers l'�ran
    printf(__DATE__);                   //Vers l'�ran
    printf(" ");                   //Vers l'�ran
    printf(__TIME__);                   //Vers l'�ran
    printf("\n");                   //Vers l'�ran

	//Create a dir if needed
	mkdir(GTN_PATH,0);
	
	
    int log= -1;                    //Vers le fichier
    log = open(LOG_FILENAME, O_WRONLY | O_CREAT | O_TRUNC);     //ouverture
    if (log<=0)                      //Si erreur d'ouverture
    {
        printf("Erreur d'ouverture du log.\n");
        return;
    }

    write(log, "-----\n",6);           //�riture
    write(log, "GTN_LOG compiled on ",20);            //�riture
    write(log, __DATE__ , 11);            //�riture
    write(log, " ",1);            //�riture
    write(log, __TIME__, 8);            //�riture
    write(log, "\n-----\n",7);         //�riture

    close(log);                        //Fermeture
    log=-1;

    return;
}


//------------------------------------------------------------------------------
//  startout function
//
//  Finis le log. A lancer en fin de programme
//
//
//  Input args : (0)
//
//  Input arg : (0)
//
//------------------------------------------------------------------------------
void endout(void)
{

    int log= -1;                    //Vers le fichier
    log = open(LOG_FILENAME, O_WRONLY | O_APPEND);     //R�uverture
    if (log<0)                      //Si erreur d'ouverture
    {
        printf("Erreur d'ouverture du log.\n");
        return;
    }
    write(log, "-----\n",6);           //�riture
    write(log, "END_LOG\n\n",9);        //�riture

    close(log);                        //Fermeture
    log=-1;
	
    printf("END_LOG\n");                   //Vers l'�ran
    return;
}

#endif
