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

/*
 * << Computer science education cannot make anybody an
 * expert programmer any more than studying brushes
 * and pigment can make somebody an expert painter.>>
 * - Eric Raymond (http://catb.org/~esr/)
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

#define NB_FONCTIONS 17

static char * func_names[NB_FONCTIONS] =
{
    "acos", "asin", "atan", "ceil", "cos", "cosh", "exp", "fabs",
    "abs", "floor", "log", "log10", "sin", "sinh", "sqrt", "tan", "tanh"
};
// Paniquez pas, la suivante est juste un tableau de pointeurs de fonctions retournant un double :-)
static double (* func_pointers[NB_FONCTIONS])() =
{
    acos, asin, atan, ceil, cos, cosh, exp, fabs,
    fabs, floor, log, log10, sin, sinh, sqrt, tan, tanh
};
// C'est exprès que j'ai fais correspondre abs à fabs: on est en analyse numérique et tout est float!

static double full_exp(char * clean_exp, int * offset, double * x, double * y);
static double mult_exp(char * clean_exp, int * offset, double * x, double * y);
static double lazy_mult_exp(char * clean_exp, int * offset, double * x, double * y);
static double unary_exp(char * clean_exp, int * offset, double * x, double * y);
static double basic_unary_exp(char * clean_exp, int * offset, double * x, double * y);
static double primary_exp(char * clean_exp, int * offset, double * x, double * y);
static double get_basic_primary(char * clean_exp, int * offset, double * x, double * y);
static double get_all_powers(char * clean_exp, int * offset, double base, double * x, double * y);
static double get_number(char * clean_exp, int * offset);
static double get_function(char * clean_exp, int * offset, double * x, double * y);

/*
 * eval calcule la valeur de la fonction définie par la chaine
 * expression en (*x, *y).
 * Elle retourne la valeur calculée si tout se passe bien, 0 si
 * il y a une erreur du genre expression vide ou appel de fonction
 * inconne.
 */
double
eval(char * expression, double * x, double * y)
{
    double value;
    char * clean_exp; // expression débarassée des espaces
    int i,j,
        offset_val = 0, // position courante dans la chaine
        *offset; // pointeur sur offset_val, transmise aux autres fonctions

    /*
     * Note: clean_exp ne sert pas seulement à débarasser l'expression 
     * des espaces. Il copie l'expression dans le heap pour que toutes
     * les autres fonctions puissent y accéder sans pour autant en faire
     * un variable globale (on passe à chaque fonction l'adresse de
     * clean_exp et la position à partir de laquelle elle est supposée
     * la lire)
     */

    offset = &offset_val;
    clean_exp = (char *) malloc(1 + strlen(expression));

    if(clean_exp) // allocation réussie
    {
        // copier expression dans clean_exp en éliminant les espaces
        for(i = 0, j = 0;*(expression + i) != '\0'; i ++)
        {
            if(!isspace(*(expression + i)))
            {
                *(clean_exp + j) = *(expression + i);
                j ++;
            }
        }
        *(clean_exp + i) = '\0'; //tout string qui se respecte...

        // calculer la valeur de l'expression
        value = full_exp(clean_exp,offset,x,y);

        // renvoyer la valeur obtenue;
        free(clean_exp);
        return value;
    } else {
        /* Un problème lié à l'allocation. Je retourne quoi?
         * 0,nan,infinity,nil, un message?
         * Le message n'est pourtant pas désirable si la parser
         *  est utilisé par un programme non interactif.
         * 
         * Dilema...
         * <<Fools ignore complexity. Pragmatists suffer it.
         * Some can avoid it. Geniuses remove it.>>
         *      Alan J. Perlis, Epigrams on Programming (Yale University)
         */

        // C'est plutôt rare une telle erreur, profitons en pour raconter notre vie ...
        printf("\n\t!!! Une erreur est survenue lors d\'une allocation mémoire");
        printf("\n\t--- Détails techniques:");
        printf("\n\t\tFonction en cours d'exécution: eval(\"%s\",%lf,%lf)",expression,*x,*y);
        printf("\n\n\tC\'est pas ma faute je vous assure :-). Ce doit être votre système");
        printf("\n\tBon, soyons sérieux: achetez de la RAM ou arrêtez avec ce jeux 3D !");
        perror("\n\tDesciption système de l\'erreur ");

        return 0; //O et moi, c'est une histoire d'amour alors ...
        /* 
         * A propos, saviez vous qu'il y a eu une grande fête à MathLand
         * le jour où on a inventé le zéro? Eh oui, une graaaande fête ...
         * Mais cette fonction a assez duré et il est temps de conclure
         * (c'est pas du Zola ça? :-))
         */
    }
}

/*
 * full_exp évalue une expression mathématique entière (dans le
 * sens où elle contient tous les opérateurs imaginables) dans
 * une chaine donnée à partir d'une position donnée.
 */
static double
full_exp(char * clean_exp, int * offset, double * x, double * y)
{
    // Commençons par récupérer un mult_exp ...
    double value = mult_exp(clean_exp,offset,x,y);
    char next_char = *(clean_exp + *offset);
    // ... puis voyons voir si une addition/soustraction suit
    switch(next_char)
    {
        case '+':
            (*offset) ++;
            return (value + full_exp(clean_exp,offset,x,y));
        case '-':
            (*offset) ++;
            return (value - full_exp(clean_exp,offset,x,y));
        default: // soit il ne reste rien, soit c'est invalide
            return value;
    }
}

/*
 * mult_exp évalue une expression multiplicative ie un produit
 * d'expressions unaires dans une chaine donnée à partir d'une
 * position donnée.
 */
static double 
mult_exp(char * clean_exp, int * offset, double * x, double * y)
{
    // Commençons par récupérer une expression unaire ...
    double value = unary_exp(clean_exp,offset,x,y);
    char next_char = *(clean_exp + *offset);

    // ... Voyons voir si un lazy_mult suit ...
    if(isalnum(next_char) || (next_char == '('))
        value *= lazy_mult_exp(clean_exp,offset,x,y);

    // ... Voyons voir si le mult_exp continue
    next_char = *(clean_exp + *offset);
    if(next_char == '*')
    {
        (*offset) ++;
        return (value * mult_exp(clean_exp,offset,x,y));
    } else { // fin du mult_exp
        return value;
    }
}

/*
 * lazy_mult_exp évalue une mult_exp 'usuel' dans le sens où
 * le signe '*' est omis dans une chaine donnée à partir d'une
 * position donnée. Ca n'as rien a voir avec 'lazy evaluation'...
 */
static double
lazy_mult_exp(char * clean_exp, int * offset, double * x, double * y)
{
    // Commençons par récupérer une expression primaire ...
    double value = primary_exp(clean_exp,offset,x,y);
    char next_char = *(clean_exp + *offset);

    // ... Voyons voir si le lazy_mult continue
    if(isalnum(next_char) || (next_char == '('))
        return value * lazy_mult_exp(clean_exp,offset,x,y);
    else
        return value;
}

/*
 * unary_exp évalue une expression mathématique unaire (dans le
 * sens où elle est l'application du signe '-' ou '+' à une
 * expression primaire) dans une chaine donnée à partir d'une
 * position donnée.
 */
static double 
unary_exp(char * clean_exp, int * offset, double * x, double * y)
{
    char next_char = *(clean_exp + *offset);

    // Voyons voir de quel signe il s'agit
    switch(next_char)
    {
        case '+':
            (*offset) ++;
            return unary_exp(clean_exp,offset,x,y);
        case '-':
            (*offset) ++;
            return -unary_exp(clean_exp,offset,x,y);
        default:
            return primary_exp(clean_exp,offset,x,y);
    }
}

/*
 * basic_unary_exp évalue une expression unaire basique. Basique dans
 * le sens où le signe s'applique à un basic_primary et non à un 
 * primary) dans une chaine donnée à partir d'une position donnée.
 */
static double 
basic_unary_exp(char * clean_exp, int * offset, double * x, double * y)
{
    char next_char = *(clean_exp + *offset);

    switch(next_char)
    {
        case '+':
            (*offset) ++;
            return basic_unary_exp(clean_exp,offset,x,y);
        case '-':
            (*offset) ++;
            return -basic_unary_exp(clean_exp,offset,x,y);
        default:
            return get_basic_primary(clean_exp,offset,x,y);
    }
}

/*
 * primary_exp évalue une expression mathématique primaire (dans le
 * sens où c'est un 'atome', une sorte de boîte noire qu'on peut
 * évaluer séparément avant de se demander ce qu'il y a autour) dans
 * une chaine donnée à partir d'une position donnée.
 * Ex: 2 + 3 n'est pas atomique, car dans 2 + 3 * 4 il faut d'abord
 * calculer 3 * 4; 2, cos(x) et (3 + 5) le sont.
 */
static double 
primary_exp(char * clean_exp, int * offset, double * x, double * y)
{
    // Commençons par récupérer une basic_primary
    double value = get_basic_primary(clean_exp,offset,x,y);
    char next_char = *(clean_exp + *offset);

    /*
     * Une expression primaire basique étant ainsi récupérée,
     * voyons voir si une élevation à une puissance suit,
     * auquel cas il faut traiter toutes les puissances
     * qui suivents vu les règles spéciales qui s'appliquent
     * à ce genre d'expression
     */
    if((next_char == '²') || (next_char == '^') || (next_char == '/'))
        value = get_all_powers(clean_exp,offset,value,x,y);

    return value;
}

/*
 * get_basic_primary_exp récupère une expression mathématique
 * primaire basique (dans le sens où elle ne contient pas
 * d'exponentiation, par opposition à un primary_exp) dans
 * une chaine donnée à partir d'une position donnée.
 * Ex:  2, cos(y), (3x² - 2) sont des basic_primary;
 *      ni x^3, ni 3/4 ne le sont
 */
static double
get_basic_primary(char * clean_exp, int * offset, double * x, double * y)
{
    double value;
    char next_char = *(clean_exp + *offset);
    // Tenter de récupérer un nombre ...
    // On gère aussi les nombres du style .35 pour 0.35
    if(isdigit(next_char) || next_char == '.')
    {
        value = get_number(clean_exp,offset);
    } else { // ... expression primaire ne commençant pas par un chiffre
        switch(next_char)
        {
            case 'x':
                (*offset) ++;
                value = *x;
                break;
            case 'y':
                (*offset) ++;
                value = *y;
                break;
            case '(':
                (*offset) ++; // pour la parenthèse ouvrante
                value = full_exp(clean_exp,offset,x,y);
                (*offset) ++; // pour la parenthèse fermante
                break;
            case '\0':
                /*
                 * Il n'y a plus rien dans la chaine.
                 * C'est un appel à full_exp qui a dégénéré jusqu'ici.
                 * Toutes les fonctions dans la chaine d'appels tentent
                 * de récupérer une expression de plus bas niveau, puis
                 * testent le caractère suivant avant d'appeler quoi que
                 * ce soit. Cela veut dire que seul leur premier appel
                 * peut avoir dégénéré jusqu'ici avec next_char = '\0'
                 * L'expression f(x,y) était donc vide.
                 * Quelle valeur acceptable peut on affecter à une fonction
                 * de la forme suivante?
                 * f(x,y) =
                 * Je choisis ce cher zero
                 * Il incombe à l'appelant d'eval() de faire ce 'basic sanity
                 * test' pour voir si la chaine ne contient que des blancs
                 */
                value = 0;
                break;
            default: //c'est un appel de fonction
                value = get_function(clean_exp,offset,x,y);
        }
    }

    return value;
}

/*
 * get_all_powers récupère et évalue toutes les exponentiations
 * suivant un 'atome' donné dans une chaine donnée à partir d'une
 * position donnée.
 * Ex:  Dans 2x^3^5y, on doit traiter juste après 2 tout le lot
 *      x^3^5 pour pouvoir obtenir l'interprêtation
 *      2 * ((x ^ 3) ^ 5) * y
 *      Si on s'amusait à récupérer x puis demander à basic_primary
 *      de nous retourner l'exposant, on obtiendrait
 *      2 * (x ^ (3 ^ 5)) * y, ce qui viole notre règle d'évaluation:
 *      Quand une puissance suit un atome, il faut remplacer le groupe
 *      formé par l'atome, le signe de la puissance et l'exposant (qui
 *      doit être un basic_primary) par le résultat de l'exponentiation
 *      avant de lire le reste de la chaine
 */
static double
get_all_powers(char * clean_exp, int * offset, double base, double * x, double * y)
{
    double value = base; // juste pour avoir une notation consistante
    char next_char = *(clean_exp + *offset);

    for(next_char = *(clean_exp + *offset);
        ((next_char == '²') || (next_char == '^') || (next_char == '/'));
        next_char = *(clean_exp + *offset))
    {
        (*offset) ++; // pour le '²', le '/' ou le '^' qui vient d'être lu
        switch(next_char)
        {
            case '^':
                value = pow(value,basic_unary_exp(clean_exp,offset,x,y));
                break;
            case '²':
                value *= value;
                break;
            case '/':
                // le casting est nécessaire pour assurer une division réelle
                value = ((float) value) / basic_unary_exp(clean_exp,offset,x,y);
        }
    } // on a fini de rammaser les 'puissances' consécutives

    return value;
}

/*
 * get_number récupère un flottant double précision dans
 * une chaine donnée à partir d'une position donnée.
 */
static double 
get_number(char * clean_exp, int * offset)
{
    double value;
    int nb_chars_lus; //nombre de caracteres lus pas sscanf

    sscanf(clean_exp + *offset,"%lf%n",&value,&nb_chars_lus);

    (*offset) += nb_chars_lus;

    return value;
}

/*
 * get_function récupère et évalue un appel de fonction dans
 * une chaine donnée à partir d'une position donnée.
 * Le paramètre de la fonction est le primary_exp qui suit
 * immédaitement le nom de la fonction.
 */
static double 
get_function(char * clean_exp, int * offset, double * x, double * y)
{
    char * current_position = clean_exp + *offset; // position courante dans la chaine clean_exp
    char function_name[6];  /* t'inquiète, 6 est suffisant: les fonctions de la
                               bibliothèque standard ont des noms de chinois! */
    int i = 0; /* nécessaire pour pouvoir ajouter '\0' après la copie,
                  il me faut connaitre la longueur de la chaine */

    /* Récupérer le nom de la fonction
     * isalnum teste la présence d'un caractère alphanumérique
     * On gère les noms de fonction contenant des nombres à cause
     * de la fonction log10.
     * Cela ne veut pas pour autant dire qu'un nom de fonction peut
     * débuter par un chiffre: l'appelant s'en est assuré.
     */
    while(isalnum(*current_position))
    {
        function_name[i] = tolower(*current_position);
        current_position ++;
        (*offset) ++;
        i ++;
    }

    function_name[i] = '\0'; // Tout bon string qui se respecte ...

    for(i = 0; i < NB_FONCTIONS; i++)
    {
        if(!strcmp( func_names[i],
                    function_name)) // fonction trouvée
        {
            return func_pointers[i](unary_exp(clean_exp,offset,x,y));
        }
    }

    /* Une dernière chose à régler:
     * Et si l'utilisateur entre une fonction inconnue?
     *
     * Un programme interactif devrait demander à l'utilisateur
     * comment calculer une telle fonction (et éventuelle la
     * calculer avec eval si la fonction ne fait intervenir que
     * des primitives connues).
     *
     * Le message n'est pourtant pas désirable si la parser est
     * utilisé par un programme non interactif.
     *
     * Quelle valeur renvoyer alors ?
     * Un 0 serait approprié pour une expression du genre
     *          2x + fonction_inconnue(y) + 
     * un 1 pour une expression du genre
     *          2xy*fonction_inconnue(3x) - 1
     *
     * Mais dans les deux cas à quoi bon renvoyer une valeur qui
     * est fausse? Pourquoi ne pas signaler en clair qu'il y a
     * erreur et sortir? 
     * 
     * Dilema...
     * <<Fools ignore complexity. Pragmatists suffer it. Some can avoid it. Geniuses remove it.>>
     *      Alan J. Perlis, Epigrams on Programming (Yale University)
     */
    // En attendant une solution, renvoyons ce cher zéro
    return 0;
}
