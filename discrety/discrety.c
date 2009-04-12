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

#include <stdlib.h>
#include <stdio.h>
#include "../pretty_io/pretty_io.h"
#include "../parser/parser.h"
#include "../open_file/open_file.h"
#include "../menu_generique/menug.h"

#define EXP_SIZE 50
#define XSTR(s) STR(s)
#define STR(s) #s

int n;
int reset; // flag non null si on est entrain de redéfinir toutes les données
double a,b;
char f[EXP_SIZE + 1];
int OF_abandon = 0; // flag positionné par open_file en cas d'abandon de l'utilisateur

void menu(void);
void set_f(void);
void set_ab(void);
void set_n(void);
void set_all(void);
void save_data(void);

int main(void)
{
    titre("BIENVENUE DANS DISCRETY");
    info("Les règles sont simples:");
    info(" * Vous définissez une fonction");
    info(" * Vous choisissez un domaine ie un intervalle [a,b]");
    info(" * Vous choisissez le nombre de points de discrétisation");
    info(" * Je place les valeurs xi,f(xi) dans un fichier de votre choix");

    reset = 1;
    menu();

    return EXIT_SUCCESS;
}

void menu(void)
{
    reset = 0;
    menug(6,"(Re)Initialiser (définir a, b et n)", set_all,
            "(Re)Définir f(x)", set_f,
            "(Re)Définir n", set_n,
            "(Re)Définir a et b", set_ab,
            "Enregistrer les résultats", save_data,
            "Quitter", MG_quitter);
}

void set_f(void)
{
    invite("f(x) =");
    scanf("%"XSTR(EXP_SIZE)"[^\n]",f);
    (void) getchar();

    if(!reset)
        menu();
}

void set_n(void)
{
    invite("n =");
    scanf("%d",&n);
    vider_stdin();

    if(!reset)
        menu();
}

void set_ab(void)
{
    invite("a =");
    scanf("%lf", &a);
    vider_stdin();

    invite("b =");
    scanf("%lf", &b);
    vider_stdin();

    if(!reset)
        menu();
}

void set_all(void)
{
    reset = 1;
    set_f();
    set_ab();
    set_n();

    menu();
}

void save_data(void)
{
    FILE * fichier;
    double h = (double) (b - a)/(n - 1); // pas de la subdivision
    int i;
    double x;

    info("Les données seront enrégistreés au format texte pur");
    info("avec un point par ligne, xi et yi séparés par une tabulation");

    fichier = open_w(quel_nom(),0); // ouverture en mode "w"
    if(!fichier)
    {
        if(!OF_abandon) // l'utilisateur n'as pas abandonné, c'est une erreur système
        {
            erreur("Une erreur système a empêché l'ouverture du fichier, désolé.");
            perror("\n\tDétails:");
        }
    } else { // le fichier est ouvert
        for(i = 0; i < n; i++)
       {
            x = a + i * h;
            fprintf(fichier,"%lf\t%lf\n", x, eval(f, &x, &x));
        }
        fclose(fichier);
        info("Sauvegarde réussie");
    }

    menu();
}

