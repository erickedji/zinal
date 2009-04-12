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
#include <string.h>
#include <stdarg.h>
#include "pretty_io.h"

#define MAX_CMD 100
#define XSTR(s) STR(s)
#define STR(s) #s

/*
 * Generic_io prend une spécification de format (format_string) et un va_list qui
 * pointe sur les variables à utiliser pour la substitution dans le format_string.
 * Il écrit sur l'entrée standard la chaine résultante précédée de la chaine
 * begin_with et suivie de la chaine end_with
 */
void
generic_io(const char * begin_with, const char * end_with, const char * format_string, va_list argp)
{
    printf("%s",begin_with);

    vprintf(format_string,argp);

    printf("%s",end_with);
}

/*
 * titre prend une chaine de caractères et la met en valeur en l'encadrant avec ':'
 */
void
titre(const char * titre)
{
    int i,longueur = strlen(titre) + 10;

    printf("\n\n\t");
    for(i = 0; i < longueur; i++)
        (void) putchar(':');

    printf("\n\t:::  %s  :::\n\t",titre);

    for(i = 0; i < longueur; i++)
        (void) putchar(':');
    printf("\n\n");
}

/*
 * info prend des informations typiquement passées à printf et affiche le
 * résultat précédé de '--- ' pour signifier que c'est une information
 */
void
info(const char * format, ...)
{
    va_list argp;

    va_start(argp, format);
    generic_io("\n\t--- ","",format,argp);
    va_end(argp);
}


/*
 * saisie prend des informations typiquement passées à printf et affiche le
 * résultat précédé de '>>> ' pour signifier qu'un saisie est attendue.
 */
void
invite(const char * format, ...)
{
    va_list argp;

    va_start(argp, format);
    generic_io("\n\t>>> ","\t", format, argp);
    va_end(argp);
}

/*
 * erreur prend des informations typiquement passées à printf et affiche le
 * résultat précédé de '!!! ' pour signifier qu'une erreur s'est produite.
 */
void erreur(const char * format, ...)
{
    va_list argp;

    va_start(argp, format);
    generic_io("\n\t!!! ","", format, argp);
    va_end(argp);
}

/*
 * get_line affiche un prompt et stocke la chaine entrée par
 * l'utilisateur. La chaine a une longueur de MAX_CMD au max.
 */

void get_line(char *prompt, char *line)
{
    vider_stdin();
    printf("%s ", prompt);
    scanf("%"XSTR(MAX_CMD)"[^\n]", line);
    //vider_stdin();
}

/*
 * vider_stdin se charge de vider le buffer du clavier après un scanf
 * susceptible d'y avoir laissé des traces indésirables
 * (un scanf("%d",&a) par exemple prend "4" dans "4d" et laisse "d")
 */
void vider_stdin(void)
{
    scanf("%*[^\n]");
    (void) getchar();
}

void pause(void)
{
    printf("\n\t*****\tAPPUYEZ SUR UNE TOUCHE POUR CONTINUER\t*****");
    (void) getchar();
}

