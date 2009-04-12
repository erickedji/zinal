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
#include <stdarg.h>
#include "menug.h"
#include "../pretty_io/pretty_io.h"

/*
 * La fonction menug affiche une liste d'option, recueille le choix de l'utilisateur
 * et appele la fonction correspondante.
 *
 * Les paramètres description et fonction sont en nombre variable et sont dans cet
 * ordre:
 *      description1,fonction1,description2,fonction2, ... descriptionn,fonctionn.
 *
 * @param   {int} nb_options Le nombre d'options (indispensable pour les macros de
 *          <stdarg.h>
 * @param   {char *} description_i La description de l'option numero i
 * @param   {void (*)()} fonction_i La fonction correspondant à l'option i
 */
void menug(int nb_options, ...)
{
    int choix,i;
    va_list argp;
    typedef void (* func_pointer)(); // pointeur de fonction de type void
    func_pointer fonction_choisie;
    char * description;

    va_start(argp,nb_options);

    printf("\n\n"); //separaton visuelle entre le menu et les sorties precedentes

    printf("\t\t\t>>>---------- MENU ----------<<<\n");

    for(i = 0; i < nb_options; i++)
    {
        description = va_arg(argp,char *);
        printf("\n\t%d - %s",i+1,description);
        (void) va_arg(argp, func_pointer);
    }
    va_end(argp);

    printf("\n"); //separaton visuelle entre le menu et l'invite de choix

    invite("Choix:");
    scanf("%d",&choix);
    vider_stdin();

    while(choix < 1 || choix >nb_options)
    {
        erreur("Choix incorrect. "
                "Veuillez entrer un nombre compris entre 1 et %d\n",nb_options);
        invite("Choix:");
        scanf("%d",&choix);
        vider_stdin();
    }

    va_start(argp,nb_options);
    
    //avancer jusqu'a la fonction qui nous interesse
    for(i = 1; i < choix; i++)
    {
        (void) va_arg(argp, char *);
        (void) va_arg(argp, func_pointer);
    }
    
    //se débarraser de la description de l'option choisie
    (void) va_arg(argp, char *);
    
    //récuperer la fonction à éxécuter
    fonction_choisie = va_arg(argp, func_pointer);

    //éxécuter la fonction
    fonction_choisie();
}

void MG_quitter(void)
{
    info("Bye!\n\n");
    exit(EXIT_SUCCESS);
}
