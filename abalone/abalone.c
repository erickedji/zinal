#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Pour la suite:
 * - Validation des coups (pas trop de temps la dessus, la version
 *   graphique risque d'être substantiellement différente)
 * - Gestion des ejections (Aouatif: tester si le marbre à pousser est
 *   au bord de l'abalonier avant de 'pousser')
 * - Le présent code ne gère pas les 'suicides' (est-ce permis dans les
 *   règles officielles ?)
 */

/*
 * Attention aux constructions du préprocesseur dans les if ... else
 * Une construction du préprocesseur peut se développer en plusieurs
 * instruction C rendant les accolades obligatoires.
 * Une bonne raison qui milite en faveur de l'utilisation systématique
 * des accolades
 */

/*
 * Abrréviations utilisées:
 * _t comme dans board_t désigne un type (ici le type de donnée 'board'
 * pos/POS désigne une position sur l'abalonier (un couple (x,y))
 *  (la majuscule est utilisée pour les constructions du préprocesseur)
 * m désigne un marbre (une position de marbre en fait)
 *  m1 par exemple désigne la position du marbre 1
 * Ce qui se trouve dans le tableau board.grid à on 'pos' donnée est
 *  de type player ie VIDE, A ou B
 */

typedef struct{
    int grid[9][9];
} board_t;

board_t board; // la grille utilisée pour le jeu

/*
 * Avec une écriture grid[i][j]:
 * Chaque ligne du tableau ici correspond à une colonne du plateau (i fixé)
 * j évolue dans le sens contraire à son évolution dans un RON 'normal'
 */
board_t standard_board = {{
    {0, 0, 2, 2, 2, 2, 2, 2, 2},
    {0, 0, 2, 2, 2, 2, 2, 2, 2},
    {0, 0, 0, 2, 2, 2, 2, 2, 2},
    {0, 0, 0, 2, 2, 2, 2, 1, 2},
    {0, 0, 0, 2, 2, 2, 1, 1, 1},
    {2, 0, 2, 2, 2, 2, 1, 1, 1},
    {2, 2, 2, 2, 2, 2, 1, 1, 1},
    {2, 2, 2, 2, 2, 2, 2, 1, 1},
    {2, 2, 2, 2, 2, 2, 2, 1, 1}
}};

typedef enum _player{A, B, VIDE} player;
typedef enum _move_t{INLINE, BROADSIDE} move_t;
typedef enum _move_d{EAST, WEST, NE, NW, SE, SW} move_d;

typedef struct _marble_pos
{
    int x,y;
} marble_pos;

/* macros de manipulation de la structure move
 * MARBLE_IS_UNSET rend compte de l'absence de 2eme/3eme marbre
 * dans la structure move et non de l'absence de marbre sur une
 * case de l'abalonier.
 */
#define MARBLE_IS_UNSET(m_pos) (((m_pos).x == -1) && ((m_pos).y == -1))
#define UNSET_MARBLE(m_pos) (m_pos).x = -1; (m_pos).y = -1
/*
 * Macros de manipulation des marbres sur l'abalonier: modifier
 * le marbre à une position donnée, s'informer sur le marbre à
 * une position donnée, la sortie d'un marbre de l'abalonier.
 */
#define SET_POS(m_pos,player_type) board.grid[(m_pos).x][(m_pos).y] = (player_type)
#define M_AT(m_pos) board.grid[(m_pos).x][(m_pos).y]
#define OUT_OF_BOARD(m_pos) (abs((m_pos).x - (m_pos).y) > 4)
/*
 * la macro précédente (OUT_OF_BOARD) peut donner l'impression de
 * faire un calcul inutile puisqu'on peut dès le départ mettre un
 * marbre spécial aux positions hors abalonier et éviter l'appel
 * d'abs et la soustraction (ie un truc du genre M_AT(m_pos) = DEHORS).
 * Elle a cependant l'avantage de ne jamais chercher à lire dans une
 * case mémoire qui, potentiellement, ne nous appartient pas
 */

/*
 * Chaque mouvement est enregistré dans la structure move (par decode_*_mvt)
 * avant d'être éxécuté (par update_board)
 */
struct _abalone_move
{
    marble_pos m1,m2,m3; // identificateurs des marbles à déplacer
    // convention: le déplacement est noté de telle sorte que dans un déplacement
    // inline, ce soit m1 qui soit 'en queue': il bouge et pousse m2 et m3
    move_t type; // type de mouvement (en ligne, latéral)
    move_d dir; // sens du mouvement
} move;

player turn; // le joueur qui a la main.

int nb_marbles_lost[2] = {0}; // marbres perdus (A = 0, B = 1)


void init_game(void);
void play(void);
void get_move_string(char * move_string);
int decode_move_string(char * move_string);
void decode_inline_mvt(char * move_string);
void decode_broadside_mvt(char * move_string);
int validate_move(void);
void update_board(void);
void show_board(void);

move_d get_dir(int i1, int j1, int i2, int j2);
marble_pos next_in_dir(move_d dir, marble_pos ref);

int main(void)
{
    player winner;

    init_game();
    
    while(nb_marbles_lost[A] < 6 && nb_marbles_lost[B] < 6)
    {
        // Faire jouer le joueur qui a la main ...
        play();
        
        show_board();

        // ... faire passer la main à l'autre joueur
        turn = (turn == A) ? B : A;
    }

    winner = (nb_marbles_lost[A] == 6) ? B : A;
    
    printf("\n\n ^^^ FIN DE LA PARTIE, GAGNANT:\t%d ^^^\n\n", winner);
    
    return EXIT_SUCCESS;
}

void init_game(void)
{
    // disposer les marbles dans la variante 'STANDARD'
    board = standard_board;

    // tirer au sort qui commence
    if((rand() % 2) == 0)
        turn = A;
    else
        turn = B;

    show_board();
}

void play(void)
{
    char move_string[10]; // enregistre la notation d'un coup, 10 pour tolérer les trailing spaces
    
    for(;;)
    {
        get_move_string(move_string);
        if(decode_move_string(move_string) == 0)
            continue;   // notation invalide
        if(validate_move())
            break;
    }

    update_board();
}

void get_move_string(char * move_string)
{
    printf("\n\t>>> Joueur %c:\t", (turn == A) ? 'A':'B');
    scanf(" %10s",move_string);
    
    scanf("%*[^\n]");
    (void) getchar(); // enlever le \n restant
}

int decode_move_string(char * str)
{
    int l = strlen(str);
    
#define LETTRE(c) (('A' <= toupper(*(c))) && (toupper(*(c)) <= 'I'))
#define CHIFFRE(c) (('1' <= *(c)) && (*(c) <= '9'))

    if(l == 4 && LETTRE(str) && CHIFFRE(str + 1)
            && LETTRE(str + 2) && CHIFFRE(str + 3))
    {
        move.type = INLINE;
        decode_inline_mvt(str);
        return 1;
    }

    if(l == 6 && LETTRE(str) && CHIFFRE(str + 1) && LETTRE(str + 2)
            && CHIFFRE(str + 3) && LETTRE(str + 4) && CHIFFRE(str + 5))
    {
        move.type = BROADSIDE;
        decode_broadside_mvt(str);
        return 1;
    }

    // Invalid move
    printf("\n\t!!! Coup invalide: mauvaise notation");
    return 0;
}

void decode_inline_mvt(char * move_string)
{
    int i1 = toupper(*move_string) - 'A',
        j1 = *(move_string + 1) - '0' - 1,
        i2 = toupper(*(move_string + 2)) - 'A',
        j2 = *(move_string + 3) - '0' - 1;
    
    marble_pos next_m;
    
    move.m1.x = i1;
    move.m1.y = j1;
    move.dir = get_dir(i1, j1, i2, j2);
    
    // Chercher l'éventuel second marble à bouger
    next_m = next_in_dir(move.dir, move.m1);
    if(M_AT(next_m) == turn)
    {
        move.m2 = next_m;
    } else {
        UNSET_MARBLE(move.m2);
        return;
    }

    // Chercher l'éventuel troisième marble à bouger
    next_m = next_in_dir(move.dir, move.m2);
    if(M_AT(next_m) == turn)
    {
        move.m3 = next_m;
    } else {
        UNSET_MARBLE(move.m3);
    }
    /* incroyable mais vrai:
     * dans le if précédent, l'absence des accolades fait que
     * seul move.m3.x est changé par l'instruction
     * move.m3 = next_m; . move.m3.y est mis à -1
     * J'ai encore de la peine à y croire ...
     * Solved: 
     *  Une construction du préprocesseur peut après expansion
     *  devenir plusieurs instruction C alors autant mettre
     *  systématiquement des accolades dans les if quand on filtre
     *  avec le préprocesseur.
     *  (A propos comment s'en sortent les routines de la bibliothèque
     *  standard qui sont implémentées comme macros ?)
     */
}
    
void decode_broadside_mvt(char * move_string)
{
    int i1 = toupper(*move_string) - 'A',
        j1 = *(move_string + 1) - '0' - 1,
        i2 = toupper(*(move_string + 2)) - 'A',
        j2 = *(move_string + 3) - '0' - 1,
        i3 = toupper(*(move_string + 4)) - 'A',
        j3 = *(move_string + 5) - '0' - 1;
    
    // Définir le premier marbre
    move.m1.x = i1;
    move.m1.y = j1;

    move.dir = get_dir(i1, j1, i3, j3);
    
    // Voir s'il y a deux marbles ou trois
    if(abs(i2 - i1) < 2 && abs(j2 - j1) < 2) // y a que 2 marbres
    {
        move.m2.x = i2;
        move.m2.y = j2;
        UNSET_MARBLE(move.m3);
    } else { // il y en a 3
        move.m2.x = (i1 + i2) / 2;
        move.m2.y = (j1 + j2) / 2;
        move.m3.x = i2;
        move.m3.y = j2;
    }
}

move_d get_dir(int i1, int j1, int i2, int j2)
{
    if(i2 - i1 == 1 && j2 - j1 == 1)
        return NE;
    if(i2 - i1 == -1 && j2 - j1 == -1)
        return SW;
    if(i2 - i1 == 0 && j2 - j1 == 1)
        return NW;
    if(i2 - i1 == 0 && j2 - j1 == -1)
        return SE;
    if(i2 - i1 == 1 && j2 - j1 == 0)
        return EAST;
    if(i2 - i1 == -1 && j2 - j1 == 0)
        return WEST;    
    // to let gcc shut up!
    return WEST;
}

marble_pos next_in_dir(move_d dir, marble_pos ref)
{
    marble_pos result;
    
    switch(dir)
    {
        case EAST:
            result.x = ref.x + 1;
            result.y = ref.y;
            break;
        case WEST:
            result.x = ref.x - 1;
            result.y = ref.y;
            break;
        case NE:
            result.x = ref.x + 1;
            result.y = ref.y + 1;
            break;
        case NW:
            result.x = ref.x;
            result.y = ref.y + 1;
            break;
        case SE:
            result.x = ref.x;
            result.y = ref.y - 1;
            break;
        case SW:
            result.x = ref.x - 1;
            result.y = ref.y - 1;
            break;
    }
    return result;
}
            
int validate_move(void)
{
    // appartennance, rapport de force, ....
    printf("\nMouvement validé.");
    return 1;
}

void update_board(void)
{
    marble_pos next_m;
    player opponent = (turn == A) ? B : A;
    
    if(move.type == BROADSIDE) // cas le plus facile
    {
        // premier marbre
        next_m = next_in_dir(move.dir, move.m1);
        SET_POS(next_m, turn);
        SET_POS(move.m1, VIDE);

        // second marbre
        next_m = next_in_dir(move.dir, move.m2);
        SET_POS(next_m, turn);
        SET_POS(move.m2, VIDE);

        // éventuel troisième marbre
        if(!MARBLE_IS_UNSET(move.m3))
        {
            next_m = next_in_dir(move.dir, move.m3);
            SET_POS(next_m, turn);
            SET_POS(move.m3, VIDE);
        }
    } else { // mouvement INLINE

        // Commencer par vider la position du premier marbre
        SET_POS(move.m1, VIDE);

        // 1 marbre
        if(MARBLE_IS_UNSET(move.m2))
        {
            // on a un seul marbre à bouger, la position devant est forcement vide
            next_m = next_in_dir(move.dir, move.m1);
            SET_POS(next_m, turn);
            return; // necessaire parce que même dans le cas présent m3 est 'unset'
        }

        // 2 marbres
        if(MARBLE_IS_UNSET(move.m3))
        {
            // deux à bouger, commencer par voir ce qu'il y a devant
            
            next_m = next_in_dir(move.dir, move.m2);
            
            /*
             * lors du déplacement de 2 marbres inline, next_m est bien vide mais
             * MARBLE_IS_UNSET(next_m) s'évalue à false. Pourquoi?
             * le cas next_m = opponent ie la condition ne s'évalue jamais à true
             * même le cas 3 marbres présente la même singularité
             *
             * Résolu: Faire du travail relaché finit toujours par vous mordre. Le
             * fonctions du préprocesseur n'étaient pas clairement définies. On ne
             * savait pas si SET_EMPTY_POS mettait un marbre dans move à NULL ou
             * bien vidait une position sur l'abalonier. Les bugs logiques sont
             * *** difficiles *** à détecter, autant être concentré. Deux jours 
             * perdus pour une négligence toute bête.
             */
            if(M_AT(next_m) == VIDE) // personne devant
            {
                SET_POS(next_m, turn);
            } else { // ennemi devant !
                // avancer le 2e marbre (celui qui est devant)
                SET_POS(next_m, turn);
                
                // pousser l'adversaire
                next_m = next_in_dir(move.dir, next_m);
                if(OUT_OF_BOARD(next_m))
                {
                    nb_marbles_lost[opponent] ++;
                } else {
                    SET_POS(next_m, opponent);
                }
            }
        } else { // 3 marbres à bouger;
            
            next_m = next_in_dir(move.dir, move.m3);

            if(M_AT(next_m) == VIDE) //personne devant
            {
                SET_POS(next_m, turn);
            } else { // victime devant (seul ou à deux?)
                // avancer le 3e marbre (celui qui est devant)
                SET_POS(next_m, turn);
                
                next_m = next_in_dir(move.dir, next_m);
                
                // Voyons s'il y avait une seule victime *au bord de l'abalonier*
                if(OUT_OF_BOARD(next_m))
                {
                    nb_marbles_lost[opponent] ++;
                    return;
                }
                
                // voyons maintenant s'il était seul mais pas au bord
                if(M_AT(next_m) == VIDE) //il était seul
                {
                    SET_POS(next_m, opponent);
                } else { // ils sont deux, voir s'il y a ejection du second
                    next_m = next_in_dir(move.dir, next_m);
                    
                    if(OUT_OF_BOARD(next_m))
                    {
                        nb_marbles_lost[opponent] ++;
                    } else {
                        SET_POS(next_m, opponent);
                    }
                } // fin 2 marbres à pousser
            } // fin 1 ou 2 victimes devant les 3 marbres
        } // fin 3 marbres à bouger
    } // fin mvt inline
}

void print_spaces(int n)
{
    int i;
    for(i = 0; i < n; i++)
        printf(" ");
}

void show_board(void)
{
    int i;

    printf("\n\n");
    
    // ligne 1
    putchar('9');
    print_spaces(12);
    for(i = 4; i < 9; i++)
        printf("  %c   ",
                (board.grid[i][8] == 2) ? '_':
                (board.grid[i][8] == A) ? '@':'b');
    printf("\n\n");

    // ligne 2
    putchar('8');
    print_spaces(9);
    for(i = 3; i < 9; i++)
        printf("  %c   ",
                (board.grid[i][7] == 2) ? '_':
                (board.grid[i][7] == A) ? '@':'b');
    printf("\n\n");

    // ligne 3
    putchar('7');
    print_spaces(6);
    for(i = 2; i < 9; i++)
        printf("  %c   ",
                (board.grid[i][6] == 2) ? '_':
                (board.grid[i][6] == A) ? '@':'b');
    printf("\n\n");

    // ligne 4
    putchar('6');
    print_spaces(3);
    for(i = 1; i < 9; i++)
        printf("  %c   ",
                (board.grid[i][5] == 2) ? '_':
                (board.grid[i][5] == A) ? '@':'b');
    printf("\n\n");

    // ligne 5
    putchar('5');
    for(i = 0; i < 9; i++)
        printf("  %c   ",
                (board.grid[i][4] == 2) ? '_':
                (board.grid[i][4] == A) ? '@':'b');
    printf("\n\n");

    // ligne 6
    putchar('4');
    print_spaces(3);
    for(i = 0; i < 8; i++)
        printf("  %c   ",
                (board.grid[i][3] == 2) ? '_':
                (board.grid[i][3] == A) ? '@':'b');
    printf("\n\n");

    // ligne 7
    putchar('3');
    print_spaces(6);
    for(i = 0; i < 7; i++)
        printf("  %c   ",
                (board.grid[i][2] == 2) ? '_':
                (board.grid[i][2] == A) ? '@':'b');
    printf("\n\n");

    // ligne 8
    putchar('2');
    print_spaces(9);
    for(i = 0; i < 6; i++)
        printf("  %c   ",
                (board.grid[i][1] == 2) ? '_':
                (board.grid[i][1] == A) ? '@':'b');
    printf("\n\n");

    // ligne 9
    putchar('1');
    print_spaces(12);
    for(i = 0; i < 5; i++)
        printf("  %c   ",
                (board.grid[i][0] == 2) ? '_':
                (board.grid[i][0] == A) ? '@':'b');
    printf("\n\n");

    print_spaces(15);
    for(i = (int) 'A'; i < (int)'E'; i++)
    {
        putchar((char) i);
        print_spaces(5);
    }
    printf("E  F  G  H  I\n\nA a perdu %d marbres, B en a perdu %d\n",
            nb_marbles_lost[A], nb_marbles_lost[B]);
    /*for(i = 0; i < 9; i ++)
    {
        for(j = i%5; j < (9 - i%5); j++)
            printf("\t%c",
                    (board.grid[i][j] == 2) ? '_':
                    (board.grid[i][j] == A) ? '@':'b');
        printf("\n");
    }*/
}

