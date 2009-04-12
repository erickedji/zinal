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
// C'est expr�s que j'ai fais correspondre abs � fabs: on est en analyse num�rique et tout est float!

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
 * eval calcule la valeur de la fonction d�finie par la chaine
 * expression en (*x, *y).
 * Elle retourne la valeur calcul�e si tout se passe bien, 0 si
 * il y a une erreur du genre expression vide ou appel de fonction
 * inconne.
 */
double
eval(char * expression, double * x, double * y)
{
    double value;
    char * clean_exp; // expression d�barass�e des espaces
    int i,j,
        offset_val = 0, // position courante dans la chaine
        *offset; // pointeur sur offset_val, transmise aux autres fonctions

    /*
     * Note: clean_exp ne sert pas seulement � d�barasser l'expression 
     * des espaces. Il copie l'expression dans le heap pour que toutes
     * les autres fonctions puissent y acc�der sans pour autant en faire
     * un variable globale (on passe � chaque fonction l'adresse de
     * clean_exp et la position � partir de laquelle elle est suppos�e
     * la lire)
     */

    offset = &offset_val;
    clean_exp = (char *) malloc(1 + strlen(expression));

    if(clean_exp) // allocation r�ussie
    {
        // copier expression dans clean_exp en �liminant les espaces
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
        /* Un probl�me li� � l'allocation. Je retourne quoi?
         * 0,nan,infinity,nil, un message?
         * Le message n'est pourtant pas d�sirable si la parser
         *  est utilis� par un programme non interactif.
         * 
         * Dilema...
         * <<Fools ignore complexity. Pragmatists suffer it.
         * Some can avoid it. Geniuses remove it.>>
         *      Alan J. Perlis, Epigrams on Programming (Yale University)
         */

        // C'est plut�t rare une telle erreur, profitons en pour raconter notre vie ...
        printf("\n\t!!! Une erreur est survenue lors d\'une allocation m�moire");
        printf("\n\t--- D�tails techniques:");
        printf("\n\t\tFonction en cours d'ex�cution: eval(\"%s\",%lf,%lf)",expression,*x,*y);
        printf("\n\n\tC\'est pas ma faute je vous assure :-). Ce doit �tre votre syst�me");
        printf("\n\tBon, soyons s�rieux: achetez de la RAM ou arr�tez avec ce jeux 3D !");
        perror("\n\tDesciption syst�me de l\'erreur ");

        return 0; //O et moi, c'est une histoire d'amour alors ...
        /* 
         * A propos, saviez vous qu'il y a eu une grande f�te � MathLand
         * le jour o� on a invent� le z�ro? Eh oui, une graaaande f�te ...
         * Mais cette fonction a assez dur� et il est temps de conclure
         * (c'est pas du Zola �a? :-))
         */
    }
}

/*
 * full_exp �value une expression math�matique enti�re (dans le
 * sens o� elle contient tous les op�rateurs imaginables) dans
 * une chaine donn�e � partir d'une position donn�e.
 */
static double
full_exp(char * clean_exp, int * offset, double * x, double * y)
{
    // Commen�ons par r�cup�rer un mult_exp ...
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
 * mult_exp �value une expression multiplicative ie un produit
 * d'expressions unaires dans une chaine donn�e � partir d'une
 * position donn�e.
 */
static double 
mult_exp(char * clean_exp, int * offset, double * x, double * y)
{
    // Commen�ons par r�cup�rer une expression unaire ...
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
 * lazy_mult_exp �value une mult_exp 'usuel' dans le sens o�
 * le signe '*' est omis dans une chaine donn�e � partir d'une
 * position donn�e. Ca n'as rien a voir avec 'lazy evaluation'...
 */
static double
lazy_mult_exp(char * clean_exp, int * offset, double * x, double * y)
{
    // Commen�ons par r�cup�rer une expression primaire ...
    double value = primary_exp(clean_exp,offset,x,y);
    char next_char = *(clean_exp + *offset);

    // ... Voyons voir si le lazy_mult continue
    if(isalnum(next_char) || (next_char == '('))
        return value * lazy_mult_exp(clean_exp,offset,x,y);
    else
        return value;
}

/*
 * unary_exp �value une expression math�matique unaire (dans le
 * sens o� elle est l'application du signe '-' ou '+' � une
 * expression primaire) dans une chaine donn�e � partir d'une
 * position donn�e.
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
 * basic_unary_exp �value une expression unaire basique. Basique dans
 * le sens o� le signe s'applique � un basic_primary et non � un 
 * primary) dans une chaine donn�e � partir d'une position donn�e.
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
 * primary_exp �value une expression math�matique primaire (dans le
 * sens o� c'est un 'atome', une sorte de bo�te noire qu'on peut
 * �valuer s�par�ment avant de se demander ce qu'il y a autour) dans
 * une chaine donn�e � partir d'une position donn�e.
 * Ex: 2 + 3 n'est pas atomique, car dans 2 + 3 * 4 il faut d'abord
 * calculer 3 * 4; 2, cos(x) et (3 + 5) le sont.
 */
static double 
primary_exp(char * clean_exp, int * offset, double * x, double * y)
{
    // Commen�ons par r�cup�rer une basic_primary
    double value = get_basic_primary(clean_exp,offset,x,y);
    char next_char = *(clean_exp + *offset);

    /*
     * Une expression primaire basique �tant ainsi r�cup�r�e,
     * voyons voir si une �levation � une puissance suit,
     * auquel cas il faut traiter toutes les puissances
     * qui suivents vu les r�gles sp�ciales qui s'appliquent
     * � ce genre d'expression
     */
    if((next_char == '�') || (next_char == '^') || (next_char == '/'))
        value = get_all_powers(clean_exp,offset,value,x,y);

    return value;
}

/*
 * get_basic_primary_exp r�cup�re une expression math�matique
 * primaire basique (dans le sens o� elle ne contient pas
 * d'exponentiation, par opposition � un primary_exp) dans
 * une chaine donn�e � partir d'une position donn�e.
 * Ex:  2, cos(y), (3x� - 2) sont des basic_primary;
 *      ni x^3, ni 3/4 ne le sont
 */
static double
get_basic_primary(char * clean_exp, int * offset, double * x, double * y)
{
    double value;
    char next_char = *(clean_exp + *offset);
    // Tenter de r�cup�rer un nombre ...
    // On g�re aussi les nombres du style .35 pour 0.35
    if(isdigit(next_char) || next_char == '.')
    {
        value = get_number(clean_exp,offset);
    } else { // ... expression primaire ne commen�ant pas par un chiffre
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
                (*offset) ++; // pour la parenth�se ouvrante
                value = full_exp(clean_exp,offset,x,y);
                (*offset) ++; // pour la parenth�se fermante
                break;
            case '\0':
                /*
                 * Il n'y a plus rien dans la chaine.
                 * C'est un appel � full_exp qui a d�g�n�r� jusqu'ici.
                 * Toutes les fonctions dans la chaine d'appels tentent
                 * de r�cup�rer une expression de plus bas niveau, puis
                 * testent le caract�re suivant avant d'appeler quoi que
                 * ce soit. Cela veut dire que seul leur premier appel
                 * peut avoir d�g�n�r� jusqu'ici avec next_char = '\0'
                 * L'expression f(x,y) �tait donc vide.
                 * Quelle valeur acceptable peut on affecter � une fonction
                 * de la forme suivante?
                 * f(x,y) =
                 * Je choisis ce cher zero
                 * Il incombe � l'appelant d'eval() de faire ce 'basic sanity
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
 * get_all_powers r�cup�re et �value toutes les exponentiations
 * suivant un 'atome' donn� dans une chaine donn�e � partir d'une
 * position donn�e.
 * Ex:  Dans 2x^3^5y, on doit traiter juste apr�s 2 tout le lot
 *      x^3^5 pour pouvoir obtenir l'interpr�tation
 *      2 * ((x ^ 3) ^ 5) * y
 *      Si on s'amusait � r�cup�rer x puis demander � basic_primary
 *      de nous retourner l'exposant, on obtiendrait
 *      2 * (x ^ (3 ^ 5)) * y, ce qui viole notre r�gle d'�valuation:
 *      Quand une puissance suit un atome, il faut remplacer le groupe
 *      form� par l'atome, le signe de la puissance et l'exposant (qui
 *      doit �tre un basic_primary) par le r�sultat de l'exponentiation
 *      avant de lire le reste de la chaine
 */
static double
get_all_powers(char * clean_exp, int * offset, double base, double * x, double * y)
{
    double value = base; // juste pour avoir une notation consistante
    char next_char = *(clean_exp + *offset);

    for(next_char = *(clean_exp + *offset);
        ((next_char == '�') || (next_char == '^') || (next_char == '/'));
        next_char = *(clean_exp + *offset))
    {
        (*offset) ++; // pour le '�', le '/' ou le '^' qui vient d'�tre lu
        switch(next_char)
        {
            case '^':
                value = pow(value,basic_unary_exp(clean_exp,offset,x,y));
                break;
            case '�':
                value *= value;
                break;
            case '/':
                // le casting est n�cessaire pour assurer une division r�elle
                value = ((float) value) / basic_unary_exp(clean_exp,offset,x,y);
        }
    } // on a fini de rammaser les 'puissances' cons�cutives

    return value;
}

/*
 * get_number r�cup�re un flottant double pr�cision dans
 * une chaine donn�e � partir d'une position donn�e.
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
 * get_function r�cup�re et �value un appel de fonction dans
 * une chaine donn�e � partir d'une position donn�e.
 * Le param�tre de la fonction est le primary_exp qui suit
 * imm�daitement le nom de la fonction.
 */
static double 
get_function(char * clean_exp, int * offset, double * x, double * y)
{
    char * current_position = clean_exp + *offset; // position courante dans la chaine clean_exp
    char function_name[6];  /* t'inqui�te, 6 est suffisant: les fonctions de la
                               biblioth�que standard ont des noms de chinois! */
    int i = 0; /* n�cessaire pour pouvoir ajouter '\0' apr�s la copie,
                  il me faut connaitre la longueur de la chaine */

    /* R�cup�rer le nom de la fonction
     * isalnum teste la pr�sence d'un caract�re alphanum�rique
     * On g�re les noms de fonction contenant des nombres � cause
     * de la fonction log10.
     * Cela ne veut pas pour autant dire qu'un nom de fonction peut
     * d�buter par un chiffre: l'appelant s'en est assur�.
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
                    function_name)) // fonction trouv�e
        {
            return func_pointers[i](unary_exp(clean_exp,offset,x,y));
        }
    }

    /* Une derni�re chose � r�gler:
     * Et si l'utilisateur entre une fonction inconnue?
     *
     * Un programme interactif devrait demander � l'utilisateur
     * comment calculer une telle fonction (et �ventuelle la
     * calculer avec eval si la fonction ne fait intervenir que
     * des primitives connues).
     *
     * Le message n'est pourtant pas d�sirable si la parser est
     * utilis� par un programme non interactif.
     *
     * Quelle valeur renvoyer alors ?
     * Un 0 serait appropri� pour une expression du genre
     *          2x + fonction_inconnue(y) + 
     * un 1 pour une expression du genre
     *          2xy*fonction_inconnue(3x) - 1
     *
     * Mais dans les deux cas � quoi bon renvoyer une valeur qui
     * est fausse? Pourquoi ne pas signaler en clair qu'il y a
     * erreur et sortir? 
     * 
     * Dilema...
     * <<Fools ignore complexity. Pragmatists suffer it. Some can avoid it. Geniuses remove it.>>
     *      Alan J. Perlis, Epigrams on Programming (Yale University)
     */
    // En attendant une solution, renvoyons ce cher z�ro
    return 0;
}
