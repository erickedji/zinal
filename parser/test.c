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
#include "parser.h"
#include "../pretty_io/pretty_io.h"
#include "../menu_generique/menug.h"

#define XSTR(s) STR(s)
#define STR(s) #s
#define EXP_MAX_SIZE 100

void menu(void);
void getf(void);
void evalf(void);

char expression[EXP_MAX_SIZE];


int main(void)
{
    titre("TEST DU PARSER D\'EXPRESSIONS MATHEMATIQUES");

    *expression = '\0';
    menu();

    return EXIT_SUCCESS;
}

void menu(void)
{
    menug(3,"(Re)définir f(x,y)", getf,
    "Evaluer f(x,y) en un point", evalf,
    "Quitter", MG_quitter);
}

void getf(void)
{
    info("["XSTR(EXP_MAX_SIZE)" caractères max, appuyez sur ENTRER pour utiliser l'expression précédente]");
    invite("f(x,y) =");
    scanf("%"XSTR(EXP_MAX_SIZE)"[^\n]",expression);
    (void) getchar();
    menu();
}

void evalf(void)
{
    double x,y;

    if(*expression == '\0') // aucune expression n'as été entrée
    {
        erreur("Désolé, f(x,y) n\'as pas été défini. Commencez par là ...");
    } else {
        info("Evaluation de f(x,y) en un point (x,y)");

        invite("x =");
        scanf("%lf", &x);
        vider_stdin();

        invite("y =");
        scanf("%lf", &y);
        vider_stdin();

        info("La valeur de la fonction en (%lf,%lf) est\t%lf",x,y,eval(expression,&x,&y));
    }

    menu();
}


