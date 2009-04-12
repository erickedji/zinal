/*
 * ZINAL (Zinal Is Not A Library) - A collection of a few programs
 * written by a dwarf standing on the shoulders of giants.
 *
 * Copyleft (C) 2007 Eric KEDJI <eric_kedji@yahoo.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include "open_file.h"
#include "../pretty_io/pretty_io.h"

#define FILE_NAME_MAX 50
#define XSTR(s) STR(s)
#define STR(s) #s

/*
 * Ce flag est mis a 1 a chaque fois que l'opération est avortée par
 * l'utilisateur. Cela permet au programmeur de savoir qui (le système
 * ou l'utilisateur) est responsable de la valeur NULL renvoyée. Il
 * reste a zero quand le probleme vient du systeme.
 */
int OF_abandon;


/*
 * La fonction quelnom() demande de la façon la plus neutre possible 
 * un nom de fichier à l'utilisateur et renvoie le nom entré
 */
char *
quel_nom(void)
{
    static char nom_fichier[FILE_NAME_MAX + 1];

    invite("Nom de fichier:");
    scanf("%"XSTR(FILE_NAME_MAX)"[^\n]",nom_fichier);
    (void) getchar();

    return nom_fichier;
}

/*
 * La fonction fichier_existe() teste l'existence d'un fichier sur le
 * disque et renvoie une valeur non nulle le cas écheant
 */
int
fichier_existe(char * nom_fichier)
{
    FILE * fichier = fopen(nom_fichier,"r");

    if(fichier)
    {
        fclose (fichier);
        return 1;
    } else {
        return 0;
    }
}

/*
 * La fonction open_r() tente d'ouvrir un fichier en mode lecture et renvoie
 * la valeur de retour de fopen(), NULL si l'utilisateur abandonne
 */
FILE *
open_r(char * nom_fichier, int plus)
{
    int choix;
    OF_abandon = 0;

    if(fichier_existe(nom_fichier))
    {
        return fopen(nom_fichier, (plus) ? "r+" : "r");
    } else {
        erreur("Le fichier %s n\'existe pas. ",nom_fichier);
        info("Vous pouvez:\n\t 1- Reessayer avec un autre nom"
                "\n\t 2- Abandonner\n");
        invite("Votre choix:");
        scanf("%d",&choix);
        vider_stdin();

        if(choix == 1)
        {
            return open_r(quel_nom(), plus);
        } else {
            OF_abandon = 1;
            return NULL;
        }
    }
}

/*
 * La fonction open_w() tente d'ouvrir un fichier en mode écriture et renvoie
 * la valeur de retour de fopen(), NULL si l'utilisateur abandonne
 */
FILE *
open_w(char * nom_fichier, int plus)
{
    int choix;
    OF_abandon = 0;

    if(!fichier_existe(nom_fichier))
    {
        return fopen(nom_fichier, (plus) ? "w+" : "w");
    } else {//le fichier existe
        erreur("Le fichier %s existe deja. ",nom_fichier);
        info("Vous pouvez:\n\t 1- Reessayer avec un autre nom\n\t "
                "2- Ecraser le fichier\n\t 3- Ajouter les donnees a "
                "la fin du fichier\n\t 4- Abandonner\n");
        invite("Votre choix:");
        scanf("%d",&choix);
        vider_stdin();

        switch(choix)
        {
            case 1:
                return open_w(quel_nom(), plus);
            case 2:
                return fopen(nom_fichier, (plus) ? "w+" : "w");
            case 3:
                return fopen(nom_fichier, (plus) ? "a+" : "a");
            case 4:
            default:
                OF_abandon = 1;
                return NULL;
        }
    }
}

/*
 * La fonction open_a() tente d'ouvrir un fichier en mode ajout et renvoie
 * la valeur de retour de fopen(), NULL si l'utilisateur abandonne
 */
FILE *
open_a(char * nom_fichier, int plus)
{
    int choix;
    OF_abandon = 0;

    if(fichier_existe(nom_fichier))
        return fopen(nom_fichier, (plus) ? "a+" : "a");
    else
    {
        erreur("Le fichier %s n\'existe pas. ",nom_fichier);
        info("Vous pouvez:\n\t 1- Reessayer avec un autre nom\n\t "
                "2- Creer le fichier\n\t 3- Abandonner\n");
        invite("Votre choix:");
        scanf("%d",&choix);
        vider_stdin();

        switch(choix)
        {
            case 1:
                return open_a(quel_nom(), plus);
            case 2:
                return fopen(nom_fichier, (plus) ? "w+" : "w");
            case 3:
            default:
                OF_abandon = 1;
                return NULL;
        }
    }
}

