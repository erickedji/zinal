#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../pretty_io/pretty_io.h"
#include "../menu_generique/menug.h"

/*
 * La méthode de Gauss-Seidel ne nécessite pas de tableau auxilliaire
 * pour le calcul des X(n). On en a pourtant utilisé parce que la
 * condition d'arrêt a besoin des vecteurs obtenus aux deux dernières
 * itération (ie X(n) et X(n-1).
 */

#define Malloc(taille,type) (type *) malloc(taille * sizeof(type))
#define MISE_A_TERRE if(X) { free(X); X = 0; }   \
    if(X_tmp) { free(X_tmp); X_tmp = 0; }        \
    if(A) { free(A); A = 0; }                    \
    if(b) { free(b); b = 0; }
// les notations du style A__i__j ne sont valides que pour les tableaux
// statiques, les macros suivantes servent juste à simplifier l'écriture
#define A__i__j (*(A + n * i + j))
#define b__i (*(b + i))
#define b__j (*(b + j))
#define X__i (*(X + i))
#define X__j (*(X + j))
#define X_tmp__i (*(X_tmp + i))
#define X_tmp__j (*(X_tmp + j))
#define x__i (*(x + i))
#define x__j (*(x + j))
#define x_tmp__i (*(x_tmp + i))
#define x_tmp__j (*(x_tmp + j))
#define x_k__i (*(x_k + i))
#define x_kplus1__i (*(x_kplus1 + i))
#define x_aux__j (*(x_aux + j))

double *A, *X, *X_tmp, *b, epsilon = 0.1;
int n; // dimension de A
int data_available = 0; // l'utilisateur tente-t-il de lancer l'algorithme sans données?

void menu(void);
void get_data(void);
void set_epsilon(void);
void use_jacobi(void);
void use_gs(void);
void use_gsr(void);
void jacobi(double * x, double * x_tmp);
void gs(double * x, double * x_tmp);
void show_results(int nb_iterations);
int close_enought(double * x_k, double * x_kplus1);

int main(void)
{
    // Au cas où le compilateur ne se prend pas au sérieux ...
    A = X = X_tmp = b = NULL;
    
    titre("RESOLUTION ITERATIVE D\'UN SYSTEME LINEAIRE AX = b D\'ORDRE n");
    info("La valeur de epsilon pour le test d\'arrêt est initialisé à %lf\n", epsilon);
    info("On suppose que les coéfficients A[i][i] sont tous non nuls,");
    info("le test n'est pas fait");
    
    menu();
    return(EXIT_SUCCESS);
}

void menu(void)
{
    menug(6,"Saisie/Modification de n,A et b", get_data,
            "Modification de la valeur de epsilon (test d'arrêt)", set_epsilon,
            "Résolution par la méthode de Jacobi", use_jacobi,
            "Résolution par la méthode de Gauss-Seidel", use_gs,
            "Résolution par la méthode de Gauss-Seidel avec relaxation", use_gsr,
            "Quitter",MG_quitter);
}

/*
 * get data récupère les données A,b et n
 * Il en profite pour allouer la mémoire pour A,b,X et X_tmp.
 */
void get_data(void)
{
    int i,j;
    
    invite("Nombre d'équations (n = dimension de A):");
    scanf("%d",&n);
    vider_stdin();

    MISE_A_TERRE
    
    X       = Malloc(n,double);
    X_tmp   = Malloc(n,double);
    b       = Malloc(n,double);
    A       = Malloc(n*n,double);

    if(!X || !X_tmp || !b || !A)
    {
        erreur("Une erreur système est survenue lors de l\'allocation"
                " de la mémoire pour X,X_tmp,b et A dans get_data(), désolé pour"
                " le désagrément");
        perror("Description système");
        menu();
        return;
    }

    info("Saisie des coefficients de A");
    for(i = 0; i < n; i ++)
        for(j = 0; j < n; j ++)
        {
            invite("A[%d][%d] =", i, j);
            scanf("%lf", &(A__i__j));
            vider_stdin();
        }

    info("Saisie des composantes de b");
    for(i = 0; i < n; i ++)
    {
        invite("b[%d] =", i);
        scanf("%lf", &b__i);
        vider_stdin();
    }

    data_available = 1;
    menu();
}

/*
 * Set epsilon récupère la valeur de epsilon à utiliser pour le test d'arrêt
 */
void set_epsilon(void)
{
    info("On utilise comme test d'arrêt la condition:\n\n\t\t\t"
            "||X(n) - X(n-1)|| <= epsilon * ||X(n)||\n");
    invite("epsilon =");
    scanf("%lf",&epsilon);
    vider_stdin();

    menu();
}

/*
 * Résolution du système par la méthode de Jacobi
 */
void use_jacobi(void)
{
    int i = 0;

    if(!data_available)
    {
        erreur("Désolé, aucune donnée disponible, commencez par là");
        pause();
        menu();
        return;
    }

    // choisir comme valeur de départ de X le vecteur nul
    memset(X_tmp, 0, n * sizeof(double));
    memset(X, 0, n * sizeof(double));

    do
    {
        jacobi(X,X_tmp);
        i ++;
    } while(close_enought(X_tmp, X) == 0);

    show_results(i);
    menu();
}

/*
 * Une itération par la méthode de jacobi
 */
void jacobi(double * x, double * x_tmp)
{
    int i,j;

    // sauvegarder le vecteur de l'itération précédente
    memcpy(x_tmp, x, n * sizeof(double));

    memset(x, 0, n * sizeof(double));

    for(i = 0; i < n; i ++)
    {
        for(j = 0; j < n; j ++)
        {
            if(j == i)
                continue;
            
            x__i += A__i__j * x_tmp__j;
        }
        
        // (* (A + (n + 1) * i)) == A[i][i]
        x__i = (double) (b__i - x__i) / (* (A + (n + 1) * i));
    }
}

/*
 * Résolution du système par la méthode de Gauss-Seidel
 */
void use_gs(void)
{
    int i = 0;
    
    if(!data_available)
    {
        erreur("Désolé, aucune donnée disponible, commencez par là");
        pause();
        menu();
        return;
    }

    // choisir comme valeur de départ de X le vecteur nul
    memset(X_tmp, 0, n * sizeof(double));
    memset(X, 0, n * sizeof(double));

    do
    {
        gs(X,X_tmp);
        i ++;
    } while(close_enought(X_tmp, X) == 0);
    
    show_results(i);
    menu();
}

/*
 * Une itération par la méthode de Gauss-Seidel
 */
void gs(double * x, double * x_tmp)
{
    int i,j;

    // sauvegarder le vecteur de l'itération précédente
    memcpy(x_tmp, x, n * sizeof(double));

    for(i = 0; i < n; i ++)
    {
        x__i = 0;
        
        for(j = i + 1; j < n; j ++)
            x__i += A__i__j * x__j;
        
        for(j = 0; j < i; j ++)
            x__i += A__i__j * x__j;

        // (* (A + (n + 1) * i)) == A[i][i]
        x__i = (double) (b__i - x__i) / (* (A + (n + 1) * i));
    }
}

/*
 * Résolution par la méthode de relaxation couplée avec celle de Gauss-Seidel
 */
void use_gsr(void)
{
    double omega, *x_aux; // x_aux contient le x(k) calculés pas gs
    int i = 0,j;
    
    if(!data_available)
    {
        erreur("Désolé, aucune donnée disponible, commencez par là");
        pause();
        menu();
        return;
    }

    x_aux = Malloc(n,double);
    
    if(x_aux == NULL)
    {
        erreur("Une erreur système est survenue lors de l\'allocation"
                " de la mémoire pour x_aux dans use_gsr(), désolé pour"
                " le désagrément");
        perror("Description système");
        menu();
        return;
    }
    
    // choisir comme valeur de départ de X le vecteur nul
    memset(X_tmp, 0, n * sizeof(double));
    memset(X, 0, n * sizeof(double));
    memset(x_aux, 0, n * sizeof(double));

    do
    {
        invite("Valeur du poids omega (0 < omega < 2):");
        scanf("%lf",&omega);
        vider_stdin();
    } while(omega <= 0 || omega >= 2);

    do
    {
        for(j = 0; j < n; j ++)
            X__j = (1 - omega) * X_tmp__j + omega * x_aux__j;
        
        gs(x_aux, X_tmp);
        i ++;
    } while(close_enought(X_tmp, X) == 0);

    show_results(i);
    menu();
}

/*
 * Affichage du résultat
 */
void show_results(int nb_iterations)
{
    int i;
    
    info("Algorithme arrêté après %d itération%c\n",
            nb_iterations, (nb_iterations > 1) ? 's':' ');
    for(i = 0; i < n; i++)
        info("\tX[%d] =\t%lf", i, X__i);
}

/*
 * Determiner si la valeur de X obtenue est suffisament bonne
 * pour arrêter l'algorithme:
 * Il s'agit de comparer ||X(k+1) - X(k)|| (g comme gauche) à
 * epsilon * ||X(k)|| (d comme droite);
 */
int close_enought(double * x_k, double * x_kplus1)
{
    double g = 0, d = 0, tmp;
    int i;

    for(i = 0; i < n; i ++)
    {
        tmp = x_kplus1__i - x_k__i;
        g += tmp * tmp;
        d += x_k__i * x_k__i;
    }

    g = sqrt(g);
    d = sqrt(d);

    if(g < (epsilon * d))
        return 1;
    else
        return 0;
}


