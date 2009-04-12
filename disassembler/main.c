#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../pretty_io/pretty_io.h"
#include "dis.h"

/*
 * Idée d'UI:
 * Donner un prompt pour qu'on puisse entrer le bytestream ligne par ligne
 * et terminer la saisie par une ligne vide
 * Donner aussi la possibilité de lire le bytestream à partir d'un fichier
 */
int main(void)
{
    char * code,result[64];
    unsigned int * memptr, * memstart;
    int i;

    titre("DESASSEMBLEUR MC 68000");

    info("Ce désassembleur est une interface interactive");
    info("pour la fonction \'dis\' qui réalise le désassemblage");
    info("Fonction \'dis\': Copyright (c) 1991 by James Patchell");
    info("Interface : Eric KEDJI <eric_kedji@yahoo.fr>\n");
    
    /*
     * Le standard C garantit qu'un unsigned int sera stocké sur 16
     * bits au mois, ce qui correspond à un nombre hexadécimal de
     * de 4 chiffres. On s'en tient à ce minimum pour des raisons
     * de portabilité
     */
    memstart  = (unsigned int *) malloc(256 * sizeof(unsigned int));
    code    = (char *) malloc(1024);

    /*
     * Cette portion du code fait que le programme
     * s'interrompt avec le message:
     * Erreur malloc: Invalid or incomplete multibyte or wide character
     * une enigme?
    if((memptr == NULL) || (code == NULL))
    {
        perror("Erreur malloc");
        exit(EXIT_FAILURE);
    }*/

    memptr = memstart;

    invite("Nombre d\'instructions:");
    scanf("%d",&i);
    vider_stdin();
    
    info("Saisie de la suite de bits à désassembler");
    info("(sans espaces, en hexa, 1024 caractères max)");
    invite("Code:");
    scanf("%1024[^\n]",code);
    (void) getchar();

    for(;i--;)
    {
        sscanf(code,"%4X",memptr);
        memptr ++;
        code += 4;
    }
    
    info("Code obtenu:\n\n",result);
    memptr = memstart;
    for(i = 5; i > 0; i --)
    {
        memptr = (unsigned int *) dis((void *) memptr,result);
        info("%s",result);
    }

    printf("\n\n");
    info("Bye!\n\n");
    return(EXIT_SUCCESS);
}

