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
#include "../pretty_io/pretty_io.h"
#include "../menu_generique/menug.h"
#include "../parser/parser.h"
#include "../open_file/open_file.h"

#define EXP_SIZE 100
#define XSTR(x) STR(x)
#define STR(x) #x
/* Le précédant blablabla du préprocesseur est sans grand intérêt.
 * il est utilisé pour limiter la saisie de f a EXP_SIZE caractères
 * et est requis parce que EXP_SIZE est elle-même une macro
 */

/****** DECLARATION DE VARIABLES ******/
char exp_f[EXP_SIZE + 1]; //expression de la fonction f
double a,b;	//les bornes de l'intervalle
int n;		//nombre de points
double h;       //pas de la discrétisation
double y_zero = 0;   //valeur arbitraire de la fonction y en x0

/* En début de programme (ou après réinitialisation demandée par
 * l'utilisateur. Il est préférable de saisir f,a,b et n en une
 * seule fois sans repasser par le menu.
 * Si 'debut' est non nul, les routines de saisie ne rappelent
 * pas le menu
 */
int debut = 1;

/* La structure utilisée pour garder la trace des résultats en
 * mémoire en vue de la sauvegarde dans un fichier
 */
typedef struct _point{
    double x,y;
} point;
point * resultats = NULL;
FILE * fichier; // le fichier où les résultats seront stockés

/* La bibliothèque open_file positionne ce flag pour signaler
 * que l'utilisateur a choisi d'abandonner lors de la saisie
 * d'un nom de fichier
 */
extern int OF_abandon; 
/****** FIN DECLARATION DE VARIABLES ******/

/****** DECLARATION DE FONCTIONS ******/
void menu(void);
void set_f(void);   // saisie de l'expression de f
void set_abn(void); // saisie de a,b et n
void reset(void);   // effacement des valeurs de a,b,n et f

void my_malloc(void);   // allouer de la mémoire pour les résultats
void sauvegarder(void); // mettre les résultats au chaud dans un fichier ...
/* Interpolation par les méthodes d'Euler, RK1, RK2 et RK3 */
void euler(void);
void rk1(void);
void rk2(void);
void rk3(void);
/****** FIN DECLARATION DE FONCTIONS ******/

double f(double x, double y); // la fonction f, au sens mathématique du terme

int main(void)
{	
    titre("RESOLUTION NUMERIQUE D\'EQUATION DIFFERENTIELLE");

    info("On cherche à résoudre une équation différentielle\n\
            de la forme y\' = f(x,y) sur l\'intervalle [a,b] avec n points");

    info("Saisie des données:\n");
    set_f();
    set_abn();
    debut = 0;

    info("y0 = y(x0)  a été arbitrairement fixé à %d",y_zero);
    info("Choix de la méthode d'interpolation");
    menu();

    info("Bonne journée\n\n");

    return EXIT_SUCCESS;	
}

void set_f(void)
{
    invite("f(x,y) = ("XSTR(EXP_SIZE)" caractères max):");
    scanf(" %"XSTR(EXP_SIZE)"[^\n]",exp_f);
    (void) getchar();

    if(!debut)
        menu();
}

void set_y0(void)
{
    invite("y0 = y(x0) =");
    scanf("%lf",&y_zero);
    vider_stdin();

    menu();
}

void set_abn(void)
{
    invite("a =");
    scanf("%lf",&a);
    vider_stdin();

    invite("b =");
    scanf("%lf",&b);
    vider_stdin();

    invite("n =");
    scanf("%d",&n);
    vider_stdin();

    h = (b - a)/((double) (n - 1));
    if(!debut)
        menu();
}

void reset(void)
{
    debut = 1;

    set_f();
    set_abn();
    debut = 0;

    menu();
}

void my_malloc(void)
{
    if(resultats)
        free(resultats);

    resultats = (point *) malloc(n * sizeof(point));
    if(!resultats)
    {
        erreur("Une erreur système s\'est produite "
                "lors de l\'allocation de la mémoire");
        info("Vous pouvez continuer, mais la sauvegarde "
                "des résultats dans un fichier peut "
                "s'avérer impossible - désolé.");
        pause();
    }
}

void sauvegarder(void)
{
    int i;

    if(!resultats)
    {
        info("Aucun résultat disponible, choisissez d\'abord une méthode d\'interpolation\n");
    } else {
        info("Le fichier contiendra un point sur chaque ligne et chaque point sera sous la forme");
        info("xi\tyi soit par exemple 12\t15.3 pour xi = 12 et yi = 15.3\n");

        //ouverture du fichier texte en mode écriture simple
        fichier = open_w(quel_nom(),0);

        if(!fichier && !OF_abandon) // C'est donc une erreur système
        {
            erreur("Une erreur système s\'est produite "
                    "lors de l\'ouverture du fichier");
            info("Vous pouvez continuer, mais l'écriture "
                    "des résultats dans un fichier peut "
                    "s'avérer impossible - désolé.");
        } else { // pas d'erreur système
            if(fichier)
            {
                for(i = 0; i < n; i++)
                    fprintf(fichier,"%lf\t%lf\n",(resultats + i)->x,(resultats + i)->y);

                info("[Fichier Sauvegardé]");
                fclose(fichier);
            }
        }
    }

    pause();
    menu();
}

void menu(void)
{
    menug(10,"Méthode d\'Euler", euler,
            "Runge-Kutta implicite à 1 point", rk1,
            "Runge-Kutta explicite à 2 points", rk2,
            "Runge-Kutta explicite à 3 points\n", rk3,
            "Redéfinir f(x,y)", set_f,
            "Redéfinir a,b et n", set_abn,
            "Redéfinir y0", set_y0,
            "Réinitialiser (Redéfinir a,b,n et f(x,y))\n", reset,
            "Sauvegarder les résultats précédents dans un fichier", sauvegarder,
            "Quitter", MG_quitter);
}

void euler(void)
{
    int i;
    double y_precedent = y_zero,y_actuel,x_actuel;

    info("RESULTAT (méthode d\'Euler)\n\n");

    my_malloc();

    printf("\tx0 =\t%lf\t\ty0 = y(x0) =\t%lf\n",a,y_zero);
    if(resultats) // sauvegarder les résultats en vue d'un enregistrment dans un fichier
    {
        resultats->x = a;
        resultats->y = y_zero;
    }

    for(i=1;i<n;i++)
    {
        x_actuel = a+h*i;

        y_actuel = y_precedent + h*f(a+i*h,y_precedent);
        printf("\tx%d =\t%lf\t\ty%d = y(x%d) =\t%lf\n",i,x_actuel,i,i,y_actuel);
        y_precedent = y_actuel;

        if(resultats)
        {
            (resultats + i)->x = x_actuel;
            (resultats + i)->y = y_actuel;
        }
    }

    pause();
    menu();
}

void rk1(void)
{
    int i;
    double alpha,yk_1,y_precedent = y_zero,y_actuel,x_actuel;

    info("RESULTAT (méthode de Runge-Kutta 1)\n\n");

    my_malloc();

    printf("\tx0 =\t%lf\t\ty0 = y(x0) =\t%lf\n",a,y_zero);
    if(resultats) // sauvegarder les résultats en vue d'un enregistrment dans un fichier
    {
        resultats->x = a;
        resultats->y = y_zero;
    }

    for(i=1;i<n;i++)
    {
        x_actuel = a+h*i;

        yk_1 = y_precedent + (h/(2*alpha))*f(x_actuel,y_precedent);
        y_actuel = y_precedent + h*(1 - alpha)*f(x_actuel,y_precedent) + alpha*f(x_actuel + h/(2*alpha),yk_1);
        printf("\tx%d =\t%lf\t\ty%d = y(x%d) =\t%lf\n",i,x_actuel,i,i,y_actuel);
        y_precedent = y_actuel;

        if(resultats)
        {
            (resultats + i)->x = x_actuel;
            (resultats + i)->y = y_actuel;
        }
    }

    pause();
    menu();
}

void rk2(void)
{
    int i;
    double yk_1,yk_2,y_precedent = y_zero,y_actuel,x_actuel;

    info("RESULTAT (méthode de Runge-Kutta 2)\n\n");

    my_malloc();

    printf("\tx0 =\t%lf\t\ty0 = y(x0) =\t%lf\n",a,y_zero);
    if(resultats) // sauvegarder les résultats en vue d'un enregistrment dans un fichier
    {
        resultats->x = a;
        resultats->y = y_zero;
    }

    for(i=1;i<n;i++)
    {
        x_actuel = a+h*i;

        yk_1 = y_precedent + (h/3)*f(x_actuel, y_precedent);
        yk_2 = y_precedent + ((2*h)/3)*f(x_actuel + (h/3), yk_1);

        y_actuel = y_precedent + (h/4)*(f(x_actuel,y_precedent) + 3*f(x_actuel + ((2*h)/3),yk_2));
        printf("\tx%d =\t%lf\t\ty%d = y(x%d) =\t%lf\n",i,x_actuel,i,i,y_actuel);
        y_precedent = y_actuel;

        if(resultats)
        {
            (resultats + i)->x = x_actuel;
            (resultats + i)->y = y_actuel;
        }
    }

    pause();
    menu();
}

void rk3(void)
{
    int i;
    double yk_1,yk_2,yk_3,y_precedent = y_zero,y_actuel,x_actuel;

    info("RESULTAT (méthode de Runge-Kutta 3)\n\n");

    my_malloc();

    printf("\tx0 =\t%lf\t\ty0 = y(x0) =\t%lf\n",a,y_zero);
    if(resultats) // sauvegarder les résultats en vue d'un enregistrment dans un fichier
    {
        resultats->x = a;
        resultats->y = y_zero;
    }

    for(i=1;i<n;i++)
    {
        x_actuel = a+h*i;

        yk_1 = y_precedent + (h/2)*f(x_actuel, y_precedent);
        yk_2 = y_precedent + (h/2)*f(x_actuel + (h/2), yk_1);
        yk_3 = y_precedent + (h/2)*f(x_actuel + (h/2), yk_2);

        y_actuel = y_precedent + (h/6)*(f(x_actuel,y_precedent) + 2*f(x_actuel + (h/2),yk_1) + 2*f(x_actuel + (h/2), yk_2) + f(x_actuel + h, yk_3));

        printf("\tx%d =\t%lf\t\ty%d = y(x%d) =\t%lf\n",i,x_actuel,i,i,y_actuel);
        y_precedent = y_actuel;

        if(resultats)
        {
            (resultats + i)->x = x_actuel;
            (resultats + i)->y = y_actuel;
        }
    }

    pause();
    menu();
}

double f(double x, double y)
{
    return eval(exp_f,&x,&y);
}
