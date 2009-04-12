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
#include "open_file.h"
#include "../pretty_io/pretty_io.h"

int OF_abandon;
/**
 * Ce flag est mis a 1 a chaque fois que l'operation est avortee par
 * l'utilisateur. Cela permet au programmeur de savoir qui (le systeme
 * ou l'utilisateur) est responsable de la valeur NULL renvoyee. Il
 * reste a zero quand le probleme vient du systeme
 */

int main(void)
{
    FILE * fichier;

    titre("Test de la bibliotheque d'ouverture de fichier");

    info("Ouvrons un fichier en mode lecture ...");
    fichier = open_r(quel_nom(),0);
    if(OF_abandon)
        erreur("Vous avez abandonné !");
    info("Valeur renvoyée:\t%p", fichier);
    if(fichier)
        fclose(fichier);

    info("Ouvrons un fichier en mode écriture ...");
    fichier = open_w(quel_nom(),0);
    if(OF_abandon)
        erreur("Vous avez abandonné !");
    info("Valeur renvoyée:\t%p", fichier);
    if(fichier)
        fclose(fichier);

    info("Ouvrons un fichier en mode ajout ...");
    fichier = open_a(quel_nom(),0);
    if(OF_abandon)
        erreur("Vous avez abandonné !");
    info("Valeur renvoyée:\t%p", fichier);
    if(fichier)
        fclose(fichier);


    info("Ouvrons un fichier en mode lecture plus ...");
    fichier = open_r(quel_nom(),1);
    if(OF_abandon)
        erreur("Vous avez abandonné !");
    info("Valeur renvoyée:\t%p", fichier);
    if(fichier)
        fclose(fichier);

    info("Ouvrons un fichier en mode écriture plus ...");
    fichier = open_w(quel_nom(),1);
    if(OF_abandon)
        erreur("Vous avez abandonné !");
    info("Valeur renvoyée:\t%p", fichier);
    if(fichier)
        fclose(fichier);

    info("Ouvrons un fichier en mode ajout plus ...");
    fichier = open_a(quel_nom(),1);
    if(OF_abandon)
        erreur("Vous avez abandonné !");
    info("Valeur renvoyée:\t%p", fichier);
    if(fichier)
        fclose(fichier);

    info("\nBonne journee !\n\n");
    return(EXIT_SUCCESS);
}

