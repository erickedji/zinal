/*
                                TP de CALCUL NUMERIQUE
    GROUPE 3
    KEDJI Komlan Akpédjé, BENKHAYAT Aouatif
    ENSIAS, 08 Janvier 2007
    Programme de calcul de l'interpolé de Lagrange d'une fonction connue en n points de la droite réelle
    Les valeurs de la fonction peuvent être entrées en live au clavier ou lues a partir d'un fichier
    Il est possible de sauvegarder les valeurs lues à partir du clavier dans un fichier
    Les fichiers utilisés sont ainsi formatés:
%lf:%lf\n
    Exemple:
2:4
3.124:14.2
13.5:145.1

    Note: Habituellement, quand on pose à l'utilisateur une question de réponses oui/non, on lui demande de répondre par o/n. C'est justifié dans un programme où on manipule du texte: les doigts de l'utilisateur sont proches de o/n. Dans ce programme, l'utilisateur entre en grande partie des chiffres. Il a donc probablement les doigts sur le pavé numérique. D'où le codage de oui/non par 1/0.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../pretty_io/pretty_io.h"
#include "../menu_generique/menug.h"
#include "../open_file/open_file.h"

#define MALLOC(type,n) (type * ) malloc(n * sizeof(type))
#define FUNCTION_POINTS_COLOR GREEN
#define DATA_POINTS_COLOR RED
#define BGI_DIR "C:\\TC\\BGI"
#define NB_POINTS 100 // nombre de points utilisés pour le tracer fin de la courbe

void menu(void);

void du_clavier(void); // Gère la lecture des données à partir du clavier
void du_fichier(void); // Gère la lecture des données à partir d'un fichier

void saisie_donnees(void);
void ouvrir_fichier(void);

int verifier(void); // Vérifie si les xi sont deux a deux distincts

void evaluer(void); // Demande des valeurs x et affiche les valeurs y
                    // correspondantes en utilistant l'interpolé de Lagrange
void show_data(void); // visualisations de données

double p(double xtest); // Interpolé de la fonction inconnue par la méthode de Lagrange
double l(double xtest, int indice); // Polynôme de Lagrange de rang 'indice'

double p_neville(double xtest); // p version neville
double neville_rec(int debut, int fin, double xtest); // helper pour p_neville

int n;          // Nombre de points
double *x,*y;   // Tableaux pour stocker les valeurs x[i] et les y[i] correspondants
FILE * donnees; // Le fichier où les données sont stockées

extern int OF_abandon; // le flag positionné par open_file en cas d'abandon


int main(void)
{
    titre("INTERPOLATION PAR LA METHODE DE LAGRANGE");
    menu();

    //On ne devrait jamais arriver ici ...
    //A César ce qui est à César!

    return EXIT_SUCCESS;
}

void du_clavier(void)
{
    int i,choix;

    info("On travaille sur un intervalle (x[1],x[n])");
    invite("Entrez n:\t");
    scanf("%d",&n);
    vider_stdin();

    x = MALLOC(double,n);
    y = MALLOC(double,n);

    if((x == NULL) || (y == NULL))
    {
        erreur("Impossible d\'allouer la mémoire pour x et y (dans du_clavier())");
        perror("Description système de l\'erreur");
        menu();
    }

    saisie_donnees();

    while(verifier() == 0)
    {
        erreur("Les valeurs des xi ne sont pas deux a deux distinctes.");
        info("Vous pouvez :\n\t1 - Recommencer la saisie\n\t2 - Abandonner");
        invite("Votre choix :");
        scanf("%d",&choix);
        vider_stdin();

        if(choix == 1)
        {
            saisie_donnees();
        } else {
            menu();
            return; // plus rien à faire ici
        }
    }

    // Suggérer à l'utilisateur d'enrégistrer les données
    info("Un instant, avant de continuer il est *recommandé* de sauvegarder vos données dans un fichier.");
    invite("Sauvegarder? (0 = non, 1 = oui):");
    scanf("%d", &choix);
    vider_stdin();

    if(choix == 1)
    {
        info("Sage décision!");

        donnees = open_w(quel_nom(), 0);

        if(!donnees)
        {
            if(!OF_abandon) // pas d'abandon, c'est donc une erreur système
            {
                erreur("Erreur fatale, impossible de créer le fichier: données non sauvegardées");
                perror("Description système de l\'erreur");
            }
        } else {
            for(i = 0; i < n; i ++)
            {
                fprintf(donnees, "%lf\t%lf\n", x[i], y[i]);
            }

            fclose(donnees);
            info("[Données sauvagardées]");
            pause();
        }
    } // fin de sauvegarde des données.

    menu();
}

void du_fichier(void)
{
    int i,choix;

    info("Lecture des valeurs x[i] et y[i] à partir d'un fichier:");
    info("Le fichier doit contenir un point par ligne comme suit:\n");
    info("\tx[i]\ty[i]\n\t\tx[i+1]\ty[i+1]\n\t\t...");

    ouvrir_fichier();

    x = MALLOC(double,n);
    y = MALLOC(double,n);

    if((x == NULL) || (y == NULL))
    {
        erreur("Impossible d\'allouer la mémoire pour x et y (dans du_fichier())");
        perror("Description système de l\'erreur");
        menu();
        return;
    }

    for(i = 0; i < n; i ++)
    {
        fscanf(donnees,"%lf\t%lf\n",x+i,y+i);
    }
    fclose(donnees);

    if(verifier() == 0)
    {
        erreur("Les valeurs des xi ne sont pas deux a deux distinctes.");
        info("Vous pouvez :\n\t1 - Recommencer\n\t2 - Abandonner");
        invite("Votre choix :");
        scanf("%d",&choix);
        vider_stdin();

        if(choix == 1)
        {
            du_fichier();
            return; //plus rien à faire ici
        } else { // il abandonne
            menu();
            return;
        }
    }

    menu();
}

void
evaluer(void)
{
    int evaluations_restantes; 
    /* C'est fastidieux de devoir répondre oui à la question 'Continuer?'
     * après chaque x. Cette variable permet à l'utilisateur de spécifier
     * le nombre d'évaluations à faire pour éviter de répéter la question
     */
    double xtest;

    evaluations_restantes = 1;
    while(evaluations_restantes){
        evaluations_restantes --; //Soustraire la présente évaluation
        invite("Entrez la valeur de x:");
        scanf("%lf",&xtest);

        info("La valeur de la fonction en ");
        printf("%lf est:\t%lf",xtest,p_neville(xtest));

        if(!evaluations_restantes)
        {
            invite("Une autre évaluation?\n\t"
                    "(1 = oui, 0 = non, k = k évaluations consécutives):");
            scanf("%d",&evaluations_restantes);
        }
    }

    menu();
}

void quitter(void)
{
    info("Bye !\n\n");
    if(x)
        free(x);
    if(y)
        free(y);
    exit(EXIT_SUCCESS);
}

void
menu(void)
{
    menug(4,"Entrer des données à partir du clavier", du_clavier,
            "Lire des données à partir d'un fichier", du_fichier,
            "Evaluer la fonction interpolée", evaluer,
//            "Visualiser les données", show_data,
            "Quitter", quitter);
}

void
saisie_donnees(void)
{
    int i;

    for (i = 0; i < n; i++)
    {
        invite("x[%d] =\t", i + 1);
        scanf("%lf", x + i);
        vider_stdin();

        invite("y[%d] =\t", i + 1);
        scanf("%lf", y + i);
        vider_stdin();
    }
}

void
ouvrir_fichier(void)
{
    int choix,nb_champs; // nb_champs est le nombre de champs convertis par fscanf
    double x; // utilisé pour vérifier si les données sont au format attendu

    donnees = open_r(quel_nom(),0);

    if(!donnees)
    {
        if(!OF_abandon) // l'utilisateur n'as pas abandonné, c'est donc une erreur système
        {
            erreur("Une erreur système est survenue lors de l\'ouverture du ficher");
            info("Désolé pour le désagrément");
            info("Description système");
            perror("");
        }

        menu();
        return; // plus rien à faire ici
    }

    // A ce stade, le fichier est correctement ouvert
    // Basic sanity test, profitons-en pour calculer le nombre de points
    n = 0;
    nb_champs = 2;
    while(!feof(donnees) && nb_champs == 2)
    {
        nb_champs = fscanf(donnees,"%lf\t%lf\n", &x, &x);
        n ++;
    }

    if(!feof(donnees)) // on n'est toujours pas à la fin du fichier, il est donc mal formatté
    {
        info("Le fichier ne contient pas des données au format attendu");
        invite("Réessayer? (0 = non, 1 = oui):");
        scanf("%d",&choix);
        vider_stdin();

        if(choix == 0)
            menu();
        else
            ouvrir_fichier();

        return; // plus rien a faire ici, sortir
    }

    // Tout est normal, on a terminé    
    rewind(donnees); //Comme sur le magnétophone! ce sacré Ritchie!
}

/*
 * Vérifier si les xi sont tous distincts avant d'appliquer
 * l'algorithme de lagrange
 */
int
verifier(void)
{
    int i,j;

    for(i = 1; i < n; i++)
        for(j = 0; j < i; j++)
            if(x[i] == x[j])
                return 0;
    /* Entre nous, sais-tu que c'est idiot de tester l'égalité
     * entre flottants à cause des erreurs d'arrondi ?! */
    return 1;
}

double
p(double xtest)
{
    int i;
    double resultat = 0;

    for(i = 0; i < n; i++)
    {
        resultat += y[i] * l(xtest, i);
    }

    return resultat;
}

double
l(double xtest, int indice)
{
    int j;
    double resultat = 1;

    for(j = 0; j < n; j++)
    {
        if (j == indice) continue;
        resultat *= ((double) (xtest - x[j])) / ((double) (x[indice] - x[j]));
        /*
         * le casting en double assure qu'on fait la division *réelle*;
         * la divisionentière renvoie le quotient ... entier!
         */
    }

    return resultat;
}

/*
 * Calcul du polynôme par la méthode de neuville. Elle est fonctionnelle,
 * mais le programme utilise la méthode de lagrange directe.
 */

double
p_neville(double xtest)
{
    return neville_rec(0, n - 1, xtest);
}

double
neville_rec(int debut, int fin, double xtest)
{
    if(debut == fin)
    {
        return y[debut];
    } else {
        return (xtest - x[fin]) * neville_rec(debut, fin - 1, xtest) / (x[debut] - x[fin]) +
            (x[debut] - xtest) * neville_rec(debut + 1, fin, xtest) / (x[debut] - x[fin]);
    }
}

/*
 * Fonctions pour la représentation graphique: pas encore fonctionnelle
 * 
void
calculer_coords_ecran(double * tabx, double * taby, int nb_pt, int vpw, int vph)
{
    int i,minx,maxx,miny,maxy, // bornes du domaine D et de f(D)
        ax,ay,bx,by; // les coefficients du changement de repère:
                    // (tabx[i] = ax*tabx[i] + bx, taby[i] = ay*taby[i] + by)
    
    // Calculer les bornes
    minx = maxx = x[0];
    miny = maxy = y[0];
    for(i = 1; i < nb_pt; i++)
    {
        if(tabx[i] < minx)
            minx = tabx[i];

        if(taby[i] < miny)
            miny = taby[i];

        if(tabx[i] > maxx)
            maxx = tabx[i];

        if(taby[i] > maxy)
            maxy = taby[i];
    }

    // Calculer les coéfficients du changement de repère

    ax = vpw / (double) (maxx - minx);
    ay = vph / (double) (maxy - miny);
    bx = -(vpw * minx) / (double) (maxx - minx);
    by = -(vph * miny) / (double) (maxy - miny);

    // calculer les coordonnées écran
    for(i = 0; i < nb_pt; i ++)
    {
        tabx[i] = ax * tabx[i] + bx;
        taby[i] = ay * taby[i] + by;
    }
}
void
show_data(void)
{
    int i;
    double h = (x[0] + x[n - 1]) / 2.0; // pas de visualisation de la fonction interpolée
    int graphdriver = DETECT, graphmode, errorcode; 
    int vpw, vph, marge; // largeur et hauteur du view_port, marge
    // utilisés pour le stockage des coordonnées écran
    double *raw_data_x, *raw_data_y, *fine_data_x, *fine_data_y; 

    // initialisation du système graphique
    initgraph(&graphdriver, &graphmode, BGI_DIR);

    // lecture du résultat d'initialisation 
    errorcode = graphresult();

    if (errorcode != grOk)  // erreur détectée 
    {
        erreur("Erreur graphique: %s\n", grapherrormsg(errorcode));
        menu();
        return; // plus rien à faire ici
    }

    // Dessiner les axes si possible

    // Allouer de la mémoire pour les données brutes et celles
    // fines (venant de l'interpolation)
    raw_data_x  = MALLOC(double,n);
    raw_data_y  = MALLOC(double,n);
    fine_data_x = MALLOC(double,NB_POINTS);
    fine_data_y = MALLOC(double,NB_POINTS);

    if((raw_data_x == NULL) || (raw_data_y == NULL)
        || (fine_data_x == NULL) || (fine_data_y == NULL))
    {
        closegraph;
        erreur("Erreur d'allocation mémoire (dans show_data())");
        perror("Description système de l\'erreur");
        menu();
        return; // plus rien à faire ici
    }
    
    // remplir les tableaux:
    memcpy(raw_data_x,x, n * sizeof(double));
    memcpy(raw_data_y,y, n * sizeof(double));
    for(i = 0; i < NB_POINTS; i ++)
    {
        fine_data_x[i] = x[0] + i * h;
        fine_data_y[i] = p(fine_data_x[i]);
    }
    
    calculer_coords_ecran(raw_data_x, raw_data_y, n, vpw, vph);
    calculer_coords_ecran(fine_data_x, fine_data_y, NB_POINTS, vpw, vph);

    marge = getmaxx() / 16;
    vpw = getmaxx() - 2 * marge;
    vph = getmaxy() - 2 * marge;
    
    // modifer le viewport: c'est notre nouvelle fenêtre
    setviewport(
            marge,
            marge,
            marge + vpw,
            marge + vph,
            1);

            
    // Dessiner les points (xi,yi) (données brutes)
    for(i = 0; i < n; i++)
        putpixel(raw_data_x[i], raw_data_y[i], DATA_POINTS_COLOR);

    // Dessiner la fonction interpolée
    for(i = 0; i < NB_POINTS; i++)
        putpixel(fine_data_x[i], fine_data_y[i], FUNCTION_POINTS_COLOR);

    // Sortie du mode graphique
    settextjustify(CENTER_TEXT,CENTER_TEXT);
    outtextxy(vpw/2, vph/2, "Appuyez sur une touche pour quitter le mode graphique...");
    (void) getchar();
    closegraph();

    menu();
}
*/


