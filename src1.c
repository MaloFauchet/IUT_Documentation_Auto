/**
 * Programme de jeu de sudoku
 * 
 * FAUCHET Malo
 * 0.2
 * 2023-11-20
 * 
 * Ce programme permet de jouer au sudoku en selectionnant une
 * grille au lancement du programme. Le programme s arrete lorsque
 * la grille est pleine.
*/


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <io.h>



const char CELLULE_VIDE =  '.' ;  /** Caractere representant une cellule vide */

int VENIAM = 2;  /** Entier representant le nombre de grilles disponibles */


#define n 5 /** Taille d un bloc de la grille */

#define TAILLE n*n /** Taille de la grille (n*n) */


typedef struct {
    int lorem;        /** commentaire lorem */
    int ipsum;        /** commentaire ipsum */
}
aleatoire; /** Structure contenant deux entiers */

typedef struct {
    int lorem2;        /** commentaire lorem 2 */
    int ipsum2;        /** commentaire ipsum 2 */
}
aleatoire2; /** Structure contenant deux entiers 2 */


int chargerGrille(tGrille grille);
void genererNbAleatoire(int *num, int maximum);
void afficherGrille(tGrille grille);
int nombreChiffre(int nombre);
void afficherEspaces(int nombreEspaces);
void afficherLigneSeparatrice(int nombreEspaces);
void saisir(int *valeur, int minimum, int maximum);
bool possible(tGrille grille, int numLigne, int numColonne, int valeur);
bool grilleEstPleine(tGrille grille);


/**
 * \brief Programme principal
 * \return Code de sortie du programme (0: sortie normale).
 * 
 * \detail Le programme principal permet de jouer une partie de sudoku en selectionnant
 * une grille au lancement du programme. Le programme s arrete lorsque
 * la grille est pleine.
*/
int main() {
    tGrille grille1;
    int numLigne, numColonne, valeur, exit_value;

    int chargerGrille_exit_value = chargerGrille(grille1);
    // si le chargement de la grille s est mal passe :
    if (chargerGrille_exit_value != 0) {
        // si le dossier 'grilles' n est pas trouve
        if (chargerGrille_exit_value == 1) {
            printf("ERREUR. Le dossier \'grilles/\' est introuvable.\n");
        }
        // si il y a eu une erreur lors de l initialisation de la grille
        else if (chargerGrille_exit_value == 2) {
            printf("ERREUR lors du chargement du fichier.\n");
        }
        printf("\tAssurez-vous que le dossier \'grilles/\' soit présent au même endroit que le programme,\n");
        printf("\tet qu'il contienne les fichier \'Grille_.sud\', où \'_\' est un nombre entre de 1 à 10.");
        exit_value = EXIT_FAILURE;
    } 
    else { 
        checkgrille(grille1);
        system("clear");

        while (grilleEstPleine(grille1) == false) {
            afficherGrille(grille1);
            printf("Indices de la case ? \n");
            printf("Numero de ligne : ");
            saisir(&numLigne, 1, TAILLE);
            printf("Numero de colonne : ");
            saisir(&numColonne, 1, TAILLE);

            numColonne -= 1; // Permet de retrouver les bons indices afin d acceder a la bonne case.
            numLigne -= 1;   // Entre 0 et 8 au lieu de 1 et 9.

            if (grille1[numLigne][numColonne] != 0) {
                system("clear");
                printf("IMPOSSIBLE, la case n est pas libre\n\n");
            } else {
                printf("Valeur à inserer ? ");
                saisir(&valeur, 1, TAILLE);
                system("clear");
                if (possible(grille1, numLigne, numColonne, valeur) == true) {
                    grille1[numLigne][numColonne] = valeur;
                }
            }
        }
        afficherGrille(grille1);
        printf("\nGrille pleine, fin de partie\n");
        exit_value = EXIT_SUCCESS;
    }
    
    return exit_value;
}


/**
 * \brief Charge une grille de jeu a partir d un fichier
 * \param grille Grille de jeu a initialiser
 * 
 * \return faux si tout s est bien passe, sinon vrai
 * 
 * \detail La fonction charge une grille de jeu a partir d un fichier
 * dont le nom est saisi au clavier.
*/
int chargerGrille(tGrille grille){
    int exit_value;
    exit_value = 0;
    FILE * f;
    char nomFichier[30];  
    int numFic;

    // test si le dossier ./grilles est present
    if (access("./grilles", 0) == 0) {
        
        // choix du fichier a utiliser afin d initialiser la grille de jeu
        printf("Choisissez un numéro de grille entre 1 et 10 (0 si vous voulez laisser l'aléatoire décider) : ");
        saisir(&numFic, 0, 10);

        // Si l utilisateur a choisi 0, le fichier choisi pour la grille sera aleatoire
        if (numFic == 0)
            genererNbAleatoire(&numFic, 10);

        sprintf(nomFichier, "grilles/Grille%d.sud", numFic);

        // utilisation du fichier
        f = fopen(nomFichier, "rb");
        if (f==NULL){
            printf("\n ERREUR sur le fichier %s\n", nomFichier);
            exit_value = 2;
        } else {
            fread(grille, sizeof(int), TAILLE*TAILLE, f);
        }
        fclose(f);
    } else {
        exit_value = 1;
        printf("Dossier ./grilles non trouvé");
    }

    return exit_value;
}

/**
 * \brief Genere un nombre aleatoire entre 1 et maximum (inclus)
 * \param num la variable dans laquelle stocker le nombre aleatoire
 * \param maximum borne maximale pour la generation du nombre aleatoire
*/
void genererNbAleatoire(int *num, int maximum) {
    srand(time(NULL));
    *num = rand() % maximum+1;
}

/**
 * \brief Affiche la grille de jeu de maniere lisible
 * \param grille Grille de jeu a afficher
*/
void afficherGrille(tGrille grille) {
    int i, j, num_espaces; 

    printf("\n");

    // determine le nombre de chiffres dans le nombre le plus grand de la grille
    num_espaces = nombreChiffre(TAILLE) + 1; 

    // numeros des colonnes
    afficherEspaces(num_espaces);
    for (i = 0; i < TAILLE; i++) {
        // check si i a atteint la fin d une region
        if ((i%n == 0) && (i != 1) && (i != 0)){
            printf("  ");
        }
        printf("%3d", i+1);
    }
    printf("\n");

    //affichage de la premiere ligne de separation
    afficherLigneSeparatrice(num_espaces);


    // corps de la grille + cote gauche
    for (i=0; i < TAILLE; i++) {
        //check si i a atteint la fin d une region
        if ((i%n == 0) && (i != 1) && (i != 0)){
            afficherLigneSeparatrice(num_espaces);
        }

        // numeros des lignes
        printf("%d", i+1);
        afficherEspaces(num_espaces - nombreChiffre(i+1));
        printf("%c", '|');

        for (j=0; j < TAILLE; j++) {
            // check si j a atteint la fin d une region
            if ((j%n == 0) && (j != 1) && (j != 0)){
                printf("%2c",  '|' );
            }

            // affiche '.' au lieu de 0 pour les cellules vides
            if (grille[i][j] == 0) {
                printf("%3c", CELLULE_VIDE);
            } else {
                printf("%3d", grille[i][j]);
            }
        }
        printf("%2c",  '|' );
        printf("\n");
    }
    // derniere ligne
    afficherLigneSeparatrice(num_espaces);
}

/**
 * \brief Calcule le nombre de chiffres d un nombre
 * \param nombre Nombre dont on veut connaitre le nombre de chiffres
 * 
 * \return Nombre de chiffres du nombre
 * 
 * \detail Cette fonction calcule le nombre de chiffres d un nombre en
 * divisant le nombre par 10 jusqu a ce que le nombre soit egal a 0.
*/
int nombreChiffre(int nombre) {
    int nombre_chiffre_tmp = 0;
    while (nombre != 0) {
        nombre /= 10;
        nombre_chiffre_tmp++;
    }
    return nombre_chiffre_tmp;
}

/**
 * \brief Affiche un nombre d espaces
 * \param nombre_espaces Nombre d espaces a afficher
*/
void afficherEspaces(int nombre_espaces) {
    for (int i=0; i < nombre_espaces; i++) {
        printf(" ");
    }
}

/**
 * \brief Affiche une ligne separatrice
 * \param nombre_espaces Nombre d espaces a afficher
 * 
 * \detail Affiche une ligne separatrice dont la taille est adaptative en fonction de la taille de la grille.
 * Taille minimum de la grille : 1
*/
void afficherLigneSeparatrice(int nombre_espaces) {
    afficherEspaces(nombre_espaces);
    for (int i=0; i < n; i++) {
        printf("+----");
        for (int j=0; j < n-1; j++) {
            printf("---");
        }
    }
    printf("+\n");
}


/**
 * \brief Saisie securisee d une valeur
 * \param valeur variable dans laquelle stocker la valeur saisie
 * \param minimum variable contenant la valeur minimum (incluse) à saisir
 * \param maximum variable contenant la valeur maximum (incluse) à saisir
 * 
 * \detail La fonction permet de lire au clavier une valeur. 
 * La saisie se repete tant que la valeur n’est pas valide.
 * La valeur lue doit etre un entier compris entre 'minimum' et 'maximum'.
*/
void saisir(int *valeur, int minimum, int maximum){
    char ch[10];  
    scanf(" %s", ch);
    while (!sscanf(ch, " %d", valeur) || (*valeur < minimum || *valeur > maximum)) {
        printf("Erreur, la valeur doit être un entier compris entre %d et %d inclus.\nVeuillez réessayer : ", minimum, maximum);
        scanf(" %s", ch);
    }
}

/**
 * \brief Verifie si une valeur peut etre inseree dans une case
 * \param grille Grille de jeu
 * \param numLigne Numero de la ligne de la case selectionnee
 * \param numColonne Numero de la colonne de la case selectionnee
 * \param valeur Valeur a inserer dans la case selectionnee
 * 
 * \return true si la valeur peut etre inseree, false sinon
 * 
 * \detail Cette fonction verifie si la valeur peut etre inseree dans la case
 * selectionnee en verifiant si la valeur n est pas deja presente dans
 * la ligne, la colonne ou le bloc de la case selectionnee.
*/
bool possible(tGrille grille, int numLigne, int numColonne, int valeur) {
    int i, j;
    bool possible = true;
    
    // check colonne
    i = 0;
    while (i < TAILLE && possible) {
        if (grille[numLigne][i] == valeur) {
            printf("La valeur %d ne peut pas etre placée dans la ligne %d\ncar elle est déjà présente dans la même ligne à la colonne %d\n", valeur, numLigne+1, i+1);
            possible = false;
        }
        i++;
    }

    // check ligne
    i = 0;
    while (i < TAILLE && possible) {
        if (grille[i][numColonne] == valeur) {
            printf("La valeur %d ne peut pas etre placée dans la colonne %d\ncar elle est déjà présente dans la même colonne à la ligne %d\n", valeur, numColonne+1, i+1);
            possible = false;
        }
        i++;
    }

    // check region
    int startingRow = numLigne - (numLigne%n);
    int startingCol = numColonne - (numColonne%n);

    i = startingRow;
    while (i < startingRow + 3 && possible) {
        j = startingCol;
        while (j < startingCol + 3 && possible) {
            if (grille[i][j] == valeur) {
                printf("La valeur %d ne peut pas etre placée dans cette région\ncar elle est déjà présente dans la même région\n", valeur);
                possible = false;
            }
            j++;
        }
        i++;
    }
    
    return possible;
}

/**
 * \brief Verifie si la grille est pleine
 * \param grille Grille de jeu
 * 
 * \return true si la grille est pleine, false sinon
 * 
 * \detail Cette fonction verifie si la grille est pleine en verifiant si
 * toutes les cases sont remplies.
*/
bool grilleEstPleine(tGrille grille) {
    int i, j;
    bool est_possible = true;

    i = 0;
    while (i < TAILLE && est_possible == true) {
        j = 0;
        while (j < TAILLE && est_possible == true) {
            if (grille[i][j] == 0) {
                est_possible = false;
            }
            j++;
        }
        i++;
    }
    
    return est_possible;
}
