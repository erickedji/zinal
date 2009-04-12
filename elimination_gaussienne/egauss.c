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
 * egauss r�soud un syst�me lin�aire de la forme ax = b o�:
 *  - a est une matrice n * n
 *  - b est un vecteur � n composantes
 *  - x ext l'inconnue (vecteur � n composantes)
 * 
 * a, b et c sont des pointeurs sur les tableaux correspondants,
 * n est la dimension de a (et b).
 *
 * La fonction g�re le cas o� la matrice a a �t� allou� dynamiquement
 * (auquel cas l'�criture a[5][6] n'as pas de sens) en faisant elle m�me
 * le calcul pour retrouver un coefficient donn�. On suppose que les
 * coefficients aij ont �t� stock�s comme le compilateur l'aurait fait
 * pour une d�claration du genre: double a[n][n].
 *
 * Les tableaux a, b et c sont toutes *modifi�es* par la fonction egauss
 *
 * Les calculs dont ont conna�t par avance le r�sultat (0 ou 1) n'ont pas
 * �t� faits (en jouant sur les valeurs de d�part des compteurs de boucle)
 */

void egauss(int n, double * a, double * b, double * x)
{
    int i,j,k;      // le pivot courant est a[k][k]
    double pivot,   // valeur du pivot actuel
           tmp;     // sorte de sous-pivot, valeur par laquelle on multiplie
                    // la ligne du pivot avant de la soustraire � la courante


    // Triangularisation de la matrice a
    for(k = 0; k < n; k ++)
    {
        pivot = *(a + (k + 1) * n); // en fait *(a + (k * n) + k)

        // Normaliser le coefficient a[k][k] en divisant la ligne k par a[k][k]
        for(j = k + 1; j < n; j ++)
            *(a + k * n + j)   /= pivot;
        // Faire la m�me op�ration sur b
        *(b + k) /= pivot;

        // Soustraire � chaque ligne i > k, la quantit� a[i][k] * a[k][j]
        for(i = k + 1; i < n; i ++)
        {
            tmp = *(a + i * n + k);

            for(j = k + 1; j < n; j ++)
                *(a + i * n + j) -= tmp  * *(a + k * i + j);
            // Faire la m�me op�ration sur b
            *(b + i) -= tmp * *(b + k);
        }
    } // triangularisation termin�e

    // R�solution du syst�me lin�aire par remont�e
    for(i = n - 1; i > -1; i --)
    {
        *(x + i) = *(b + i);

        for(j = i + 1; j < n; j ++)
            *(x + i) -= *(a + j) * *(x + j);
    }
}

