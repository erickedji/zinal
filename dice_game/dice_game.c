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
#include <string.h>
#include "../pretty_io/pretty_io.h"
#include "../menu_generique/menug.h"

char player1[21],player2[21];
char * nom_ordi = "[DEEP-BLUE]";
int valeur_visee = 10;
int valeur_actuelle = 0;
int coup_precedent = 0;
enum _joueurs { JOUEUR1, JOUEUR2 } joueur_precedent;

void menu(void);
void two_players(void);
void one_player(void);
void initialiser(void);
int jouer(void);
void afficher(void);
void afficher(void);
int bon_coup(int coup);
void set_target(void);

int main(void)
{
    titre("UN PETIT JEU DE DES");
    info("La valeur ciblée est par défaut 10");

    menu();

    return EXIT_SUCCESS;
}

void menu(void)
{
    menug(4,"Mode 1 joueur (jouer contre le PC)", one_player,
            "Mode 2 joueurs", two_players,
            "Modifier la valeur visée", set_target,
            "Quitter", MG_quitter);
}

void two_players(void)
{
    info("Les noms des joueurs doivent comporter 20 caractères au maximum");

    invite("Nom du joueur 1");
    scanf("%20[^\n]",player1);
    (void) getchar();

    invite("Nom du joueur 2");
    scanf("%20[^\n]",player2);
    (void) getchar();

    initialiser();
}

void one_player(void)
{
    strcpy(player1,nom_ordi);
    info("Moi je m\'appelle ");
    printf("%s, et vous\?", nom_ordi);
    info("Votre nom doit comporter 20 caractères au maximum");

    invite("Votre nom:");
    scanf("%20[^\n]",player2);
    (void) getchar();

    initialiser();
}

void initialiser(void)
{
    int valeur_initiale;
    char qui_commence[21];

    info("Tirons au sort pour décider de qui commence le jeux");

    srand((int) *player2);
    if(rand() > 2)
    {
        info("Le sort a choisi le joueur %s",player1);
        joueur_precedent = JOUEUR2; //puisque le prochain c'est JOUEUR1
        strcpy(qui_commence, player1);
    } else {
        info("Le sort a choisi le joueur %s",player2);
        joueur_precedent = JOUEUR1; //puisque le prochain c'est JOUEUR2
        strcpy(qui_commence, player2);
    }
    
    if(strcmp(qui_commence, nom_ordi)) // c'est l'humain qui commence
    { //il faut un traitement spécial parce 1,2,3,4,5 et 6 sont valables
        valeur_initiale = 0;
        while((valeur_initiale < 1) || (valeur_initiale > 6))
        {
            invite("Choisissez une valeur initiale comprise entre 1 et 6");
            scanf("%d",&valeur_initiale);
            vider_stdin();
        }

        // on n'as pas joué en passant par jouer(), il faut donc modifier
        // les paramètres manuellement
        joueur_precedent = (joueur_precedent == JOUEUR1) ? JOUEUR2 : JOUEUR1;
        valeur_actuelle = coup_precedent = valeur_initiale;

    } else { //c'est le PC qui commence
        valeur_actuelle = jouer();
    }

    afficher();
}

int jouer(void)
{
    int coup = 0;
    char qui_joue[21];
    strcpy(qui_joue, (joueur_precedent == JOUEUR1)? player2:player1);

    info("A %s de jouer", qui_joue);
    if(!strcmp(qui_joue, nom_ordi)) //c'est le pc qui joue
    {
        int coup_trouve = 0;

        for(coup = 1; coup < 7; coup++) // voir si on peut gagner sur le champ
        {
            if((coup == coup_precedent) || (coup == 7 - coup_precedent)) // coup interdit
                continue;

            if(coup + valeur_actuelle == valeur_visee) //on a gagné
            {
                coup_trouve = 1; // on a trouvé un bon coup à jouer
                break;
            }
        }
        
        if(!coup_trouve) // pas possible de gagner sur le champ, empêchons-le de gagner
        {
            for(coup = 1; coup < 7; coup++)
            {
                if((coup == coup_precedent) || (coup == 7 - coup_precedent)) // coup interdit
                    continue;

                if(bon_coup(coup) && (coup + valeur_actuelle < valeur_visee))
                {
                    coup_trouve = 1;
                    break;
                }
            }
        }

        if(!coup_trouve) // c'est triste mais on a perdu
        {
            info("FELICITATIONS - je suis coincé, vous avez gagné!");
            // réinitialiser le jeu
            coup_precedent = valeur_actuelle = 0;
            menu();
            return; // plus rien a faire ici
        } else {
            // mettons l'adversaire au courant de notre coup
            printf("\n\n\t*** %s a joué %d\n",nom_ordi,coup);
        }
    } else { // c'est pas le pc qui joue
        while((coup < 1) || (coup > 6) || (coup == coup_precedent) || (coup == 7 - coup_precedent))
        {
            info("Votre coup doit être compris entre 1 et 6, différent de %d et %d", coup_precedent, 7 - coup_precedent);
            invite("Votre coup:");
            scanf("%d",&coup);
            vider_stdin();
        }
    }
    
    coup_precedent = coup; // mettre a jour le coup précédent ...
    joueur_precedent = (joueur_precedent == JOUEUR1) ? JOUEUR2:JOUEUR1; // ... et le joueur précédent
    return coup;
}

int bon_coup(int coup)
{
    int prochain_coup;

    for(prochain_coup = 1; prochain_coup < 7; prochain_coup++)
    {
        if((prochain_coup == coup) || (prochain_coup == 7 - coup))
            continue;

        if(valeur_visee == coup + prochain_coup + valeur_actuelle)
            return 0; // c'est pas un bon coup car l'adversaire peut gagner au tour suivant
    }

    // si on est toujours là, c'est que l'adversaire ne peut en aucun cas
    // gagner au coup suivant, c'est donc un bon coup
    return 1;
}

void afficher(void)
{
    if(valeur_actuelle == valeur_visee) // jeu terminé
    {
        info("Valeur visée atteinte, gagnant:\t%s",(joueur_precedent == JOUEUR1)? player1:player2);
        valeur_actuelle = coup_precedent = 0; //réinitialiser le jeu
        menu();
    } else if (valeur_actuelle > valeur_visee) {
        info("Valeur visée dépassée, gagnant:\t%s",(joueur_precedent == JOUEUR1)? player2:player1);
        valeur_actuelle = coup_precedent = 0; //réinitialiser le jeu
        menu();
    } else { // le jeu continue
        info("Valeur actuelle = %d",valeur_actuelle);
        info("Distance restante = %d",valeur_visee - valeur_actuelle);
        valeur_actuelle += jouer();
        afficher();
    }
}

void set_target(void)
{
    invite("Nouvelle valeur visée:");
    scanf("%d",&valeur_visee);
    vider_stdin();
    menu();
}

