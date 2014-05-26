#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#define TAILLE 6

struct pion
{
    char couleur;
    struct pion * suivant;
};
typedef struct pion sPion;
typedef struct pion * pPion;

struct joueur
{
    char couleur;
    int numero;
    int points;
};
typedef struct joueur sJoueur;

struct coordonnees
{
    int x;
    int y;
};
typedef struct coordonnees CD;


//JEU
void poserPion(pPion plateau[][TAILLE], sJoueur, sJoueur, sJoueur);
void deplacerPile(pPion plateau[][TAILLE], sJoueur joueur, sJoueur*, sJoueur*);
CD choixPile(pPion plateau[][TAILLE], sJoueur, sJoueur, sJoueur);

//UTILITAIRE
pPion creerPion(char);
void deplacement(pPion plateau[][TAILLE], CD ini, CD destination, int nombre_pions);
void ajouterPoint(pPion plateau[][TAILLE], sJoueur*, sJoueur*, CD pile);
void supprimerPile(pPion plateau[][TAILLE], CD pile);

//TESTS
int hauteurPile(pPion plateau[][TAILLE], CD pile);
int testDirection(CD ini, CD destination);
int testDistance(pPion plateau[][TAILLE], CD ini, CD destination, int direction);
int testCheminLibre(pPion plateau[][TAILLE], CD ini, CD destination, int direction);
int nombrePiles(pPion plateau[][TAILLE]);

//SAUVEGARDE
void sauvegarderPartie(pPion plateau[][TAILLE], sJoueur, sJoueur, sJoueur);
void chargerPartie(pPion plateau[][TAILLE], sJoueur*, sJoueur*, sJoueur*);

//AFFICHAGE
void affichePlateau(pPion plateau[][TAILLE], sJoueur, sJoueur, sJoueur);
void affichePion(pPion plateau[][TAILLE], int x, int y, int k);
void afficheRegles();
void messageErreur(pPion plateau[][TAILLE], int a, int b, int c, CD pile);
void Pause_effacerMessage();
void Color(int, int);



int main()
{
    int reponse, nb_piles;

    sJoueur joueur1 = {'b',1,0};
    sJoueur joueur2 = {'r',2,0};
    sJoueur actif;//sert à définir le tour de jeu

    pPion plateau[TAILLE][TAILLE]=
    {{NULL,NULL,NULL,NULL,NULL},
     {NULL,NULL,NULL,NULL,NULL},
     {NULL,NULL,NULL,NULL,NULL},
     {NULL,NULL,NULL,NULL,NULL},
     {NULL,NULL,NULL,NULL,NULL}};

    Color(1,7);
    printf("\t\tBienvenue dans Mixtour!\n\t\t");
    Pause_effacerMessage();

    do
    {
        printf("1 - Commencer une nouvelle partie\n2 - Charger une partie\n3 - Lire les regles\n");
        fflush(stdin);
        scanf("%d", &reponse);
        system("cls");

        switch(reponse)
        {
            case 1://                                       - - - - - - - - - - - - - - -NOUVELLE PARTIE
            printf("La partie se joue en 5 points.\nJoueur 1 prend les blancs, Joueur 2 prend les rouges.\n");
            printf("Blanc commence.\n\n");
            actif=joueur1;
            Pause_effacerMessage();
            break;

            case 2://                                       - - - - - - - - - - - - - - -CHARGEMENT PARTIE
            chargerPartie(plateau, &actif, &joueur1, &joueur2);
            break;

            case 3:
            afficheRegles();
            Pause_effacerMessage();
            actif=joueur1;
            break;
        }

    }while(reponse!=1 && reponse!=2 && reponse!=3);


    while(joueur1.points<5 && joueur2.points<5)//           - - - - - - - - - - - - - - -TOURS DE JEU
    {
        affichePlateau(plateau, actif, joueur1, joueur2);

        do
        {
            printf("\nJoueur %d, voulez-vous: \n1 - poser un pion?\n2 - deplacer une pile?\n3 - quitter la partie?\n", actif.numero);
            fflush(stdin);
            scanf("%d", &reponse);

            if(reponse!=1 && reponse!=2  && reponse!=3)
            {
                system("cls");
                affichePlateau(plateau, actif, joueur1, joueur2);
            }
        }while(reponse!=1 && reponse!=2 && reponse!=3);

        switch(reponse)
        {
            case 1://                                       - - - - - - - - - - - - - - -POSER PION
            poserPion(plateau, actif, joueur1, joueur2);
            system("cls");
            break;


            case 2://                                       - - - - - - - - - - - - - - -DEPLACER PILE
            nb_piles=nombrePiles(plateau);

            if(nb_piles<2)//pour éviter un blocage du joueur
            {
                printf("\nIl n'y a pas assez de piles pour faire un quelconque deplacement...");
                printf("\nPosez-donc un pion!\n");
                poserPion(plateau, actif, joueur1, joueur2);
                system("cls");
            }
            else
            {
                deplacerPile(plateau, actif, &joueur1, &joueur2);
                system("cls");
            }
            break;


            case 3://                                       - - - - - - - - - - - - - - -QUITTER JEU
            printf("\nVoulez-vous sauvegarder?\n1 - oui\n2 - non\n");
            fflush(stdin);
            scanf("%d", &reponse);
            if(reponse==1)
            {
                sauvegarderPartie(plateau, actif, joueur1, joueur2);
                printf("Sauvegarde reussie !\n");
                exit(EXIT_SUCCESS);
            }
            else
            {   exit(EXIT_SUCCESS);    }
            break;
        }

        if(actif.numero==joueur1.numero)//                 - - - - - - - - - - - - - - -FIN DU TOUR , ALTERNANCE
        {   actif=joueur2;  }
        else
        {   actif=joueur1;  }
    };


    if(joueur1.points==5)//                                 - - - - - - - - - - - - - - -FIN DU JEU
    {   printf("\n\nFelicitations! Joueur1 remporte la victoire! \n");  }
    else
    {   printf("\n\nFelicitations! Joueur2 remporte la victoire! \n");  }

    return 0;
}



//JEU

void poserPion(pPion plateau[][TAILLE], sJoueur actif, sJoueur joueur1, sJoueur joueur2)
{
    CD pile;
    int test_hauteur;

    do
    {
        do
        {
            printf("\nOu poser le pion? Entrez les coordonnees (la case doit etre libre) : ");
            pile=choixPile(plateau, actif, joueur1, joueur2);
        }while(pile.x<1 || pile.x>5 || pile.y<1 || pile.y>5);


        test_hauteur=hauteurPile(plateau, pile);//on teste la taille de l'éventuelle pile sur la case choisie
        if(test_hauteur==0)
        {
            pPion pion=creerPion(actif.couleur);
            plateau[pile.y][pile.x]=pion;//le pointeur dans la table 'plateau' pointe vers le pion posé
        }

        else
        {
            printf("\nla case choisie n'est pas libre!\n");
            Pause_effacerMessage();
            affichePlateau(plateau, actif, joueur1, joueur2);
        }
    }while(test_hauteur!=0);
}

void deplacerPile(pPion plateau[][TAILLE], sJoueur actif, sJoueur *joueur1, sJoueur *joueur2)    //type sJoueur* car 'deplacerPile' appelle 'ajouterPoint' qui modifie les structures
{
    CD pile_ini, pile_destination;
    int hauteur, nombre_pions;
    int a, b, c;

    do
    {
        do
        {
            printf("\nQuelle pile souhaitez-vous deplacer?");//                                     - - - - - - - - - - - - -PILE A DEPLACER
            pile_ini=choixPile(plateau, actif, *joueur1, *joueur2);
            messageErreur(plateau,1,1,1, pile_ini);

        }while(pile_ini.x<1 || pile_ini.x>5 || pile_ini.y<1 || pile_ini.y>5 || hauteurPile(plateau, pile_ini)==0);

        hauteur=hauteurPile(plateau, pile_ini);

        if(hauteur==1)//                                                                            - - - - - - - - - - - - -NOMBRE DE PIONS
        {   nombre_pions=1;   }
        else
        {
            do
            {
                printf("\nCombien de pions souhaitez-vous prendre sur cette pile? ");
                fflush(stdin);
                scanf("%d", &nombre_pions);
            }while(hauteur<nombre_pions);//le nombre de pions ne doit pas exceder la hauteur de la pile
        }


        do
        {
            printf("\nA quel endroit souhaitez-vous la deplacer?");//                               - - - - - - - - - - - - -PILE VISEE
            pile_destination=choixPile(plateau, actif, *joueur1, *joueur2);
            messageErreur(plateau,1,1,1, pile_destination);

        }while(pile_destination.x<1 || pile_destination.x>5 || pile_destination.y<1 || pile_destination.y>5 || hauteurPile(plateau, pile_destination)==0);


        a=testDirection(pile_ini, pile_destination);
        b=testDistance(plateau, pile_ini, pile_destination, a);
        c=testCheminLibre(plateau, pile_ini, pile_destination, a);


        if(a!=0 && b!=0 && c!=0)
        {
            deplacement(plateau, pile_ini, pile_destination, nombre_pions);
        }
        else if(a==0 || b==0 || c==0)
        {
            messageErreur(plateau,a,b,c,pile_ini);
            Pause_effacerMessage();
            affichePlateau(plateau, actif, *joueur1, *joueur2);
        }

    }while(a==0 || b==0 || c==0);//les tests doivent etre validés avant de poursuivre


    if(hauteurPile(plateau, pile_destination)>=5)//cas où la pile atteint une hauteur de 5 ou +
    {
        ajouterPoint(plateau, joueur1, joueur2, pile_destination);//parametres de type 'joueur' plutot que '&joueur' car passés en type sJoueur* dans le programme appelant
        supprimerPile(plateau, pile_destination);
    }
}

CD choixPile(pPion plateau[][TAILLE], sJoueur actif, sJoueur joueur1, sJoueur joueur2)
{
    CD pile;

    printf("\nx = ");
    fflush(stdin);
    scanf("%d", &pile.x);

    printf("y = ");
    fflush(stdin);
    scanf("%d", &pile.y);

        if(pile.x<1 || pile.x>5 || pile.y<1 || pile.y>5)//la pile doit etre sur le plateau
        {
            printf("\nLa pile choisie est incorrecte.\n");
            Pause_effacerMessage();
            affichePlateau(plateau, actif, joueur1, joueur2);
        }

    return pile;
}


//UTILITAIRE

pPion creerPion(char couleur)
{
    pPion pion;
    pion=(pPion)malloc(sizeof(sPion));
    pion->couleur=couleur;
    pion->suivant=NULL;

    return pion;
}

void deplacement(pPion plateau[][TAILLE], CD pile_ini, CD pile_destination, int nombre_pions)
{
    int h, i;
    h=hauteurPile(plateau, pile_ini);

    pPion compteur1=NULL;
    pPion compteur2=NULL;
    compteur1=plateau[pile_ini.y][pile_ini.x];
    compteur2=plateau[pile_destination.y][pile_destination.x];//on initialise un pointeur pour chaque pile qui entre en jeu

    if(h!=nombre_pions)//si on ne prend pas la pile complète, le pointeur va la pacourir
    {
        for(i=0; i<(h-nombre_pions)-1; i++)
        {   compteur1=compteur1->suivant;   }//'compteur1' parcourt la liste jusqu'au pion juste avant le pion le plus bas de ceux que l'on souhaite déplacer
    }


    while(compteur2->suivant!=NULL)//en revanche on va directement faire pointer 'compteur2' vers le dernier élément de la pile visée
    {   compteur2=compteur2->suivant;   }


    if(h==nombre_pions)
    {
        (compteur2->suivant)=compteur1;         //les pions sont ajoutés a la pile visée
        plateau[pile_ini.y][pile_ini.x]=NULL;   //on raccourcit la pile de départ
    }
    else
    {
        (compteur2->suivant)=(compteur1->suivant);
        (compteur1->suivant)=NULL;
    }
}

void ajouterPoint(pPion plateau[][TAILLE], sJoueur *joueur1, sJoueur *joueur2, CD pile)
{
        pPion compteur;
        compteur=plateau[pile.y][pile.x];//on initialise une cellule Pion vide

        while(compteur->suivant!=NULL)
        {   compteur=compteur->suivant;   }//on la fait pointer vers le dernier pion de la pile

        if(compteur->couleur==(*joueur1).couleur)//selon la couleur du dernier pion, 1 point est attribué au joueur
        {
            (*joueur1).points++;
            printf("\nJoueur1 gagne un point!\n");
        }

        else
        {
            (*joueur2).points++;
            printf("\nJoueur1 gagne un point!\n");
        }
}

void supprimerPile(pPion plateau[][TAILLE], CD pile)
{
    int X=pile.x, Y=pile.y;
    pPion temp;

    while(plateau[Y][X]->suivant!=NULL)
    {
        temp=plateau[Y][X];
        plateau[Y][X]=plateau[Y][X]->suivant;
        free(temp);//on enleve tous les pions 1 par 1
    }
    free(plateau[Y][X]);//on enlève le dernier pion
    plateau[Y][X]=NULL;//on réinitialise la pile
}


//TESTS

int hauteurPile(pPion plateau[][TAILLE], CD pile)
{
    int hauteur=0;
    pPion compteur=NULL;//on initialise un pointeur pPion
    compteur=plateau[pile.y][pile.x];

    while(compteur!=NULL)
    {
        hauteur++;
        compteur=(compteur->suivant);//'compteur' parcourt la pile et compte le nombre de pions
    }

    return hauteur;
}

int testDirection(CD pile_ini, CD pile_destination)//retourne la nature du deplacement
{
    int dx, dy;
    dx=pile_destination.x-pile_ini.x;
    dy=pile_destination.y-pile_ini.y;

    if(dy==0)//cas d'un déplacement horizontal
    {return 1;}

    else if(dx==0)//cas d'un déplacement vertical
    {return 2;}

    else if(dx==dy || dx==-dy)//cas d'un déplacement en diagonale
    {return 3;}

    else
    {return 0;}
}

int testDistance(pPion plateau[][TAILLE], CD pile_ini, CD pile_destination, int direction)//teste la distance de deplacement en fct de la pile visée
{
    int hauteur_pile_visee, dx, dy, a, b;
    hauteur_pile_visee=hauteurPile(plateau, pile_destination);

    dx=pile_destination.x-pile_ini.x;
    dy=pile_destination.y-pile_ini.y;

    a=abs(dx);
    b=abs(dy);//a et b sont les valeurs absolues de dx et dy

    switch(direction)
    {
        case 1:
        if(hauteur_pile_visee==a)
        {   return 1;   }
        else
        {   return 0;   }
        break;

        case 2:
        if(hauteur_pile_visee==b)
        {   return 1;   }
        else
        {   return 0;   }
        break;

        case 3:
        if(hauteur_pile_visee==(a+b)/2)
        {   return 1;   }
        else
        {   return 0;   }
        break;

        default:
        return 1;
        break;
    }

}

int testCheminLibre(pPion plateau[][TAILLE], CD pile_ini, CD pile_destination, int direction)
{
    int i=1, m, n, dx, dy, abs_dx, abs_dy;

    pPion compteur=NULL;
    dx=pile_destination.x-pile_ini.x;
    dy=pile_destination.y-pile_ini.y;

    abs_dx = abs(dx);  //abs_dx est la valeur absolue de dx
    abs_dy = abs(dy);//abs_dy est la valeur absolue de dy


    switch (direction)
    {
        case 1:        //deplacement horizontal, on part du pion le plus a gauche (visé ou initial)...
            m = dx>0 ? pile_ini.x : pile_destination.x;
            while(i<abs_dx && compteur==NULL)
            {
                compteur=plateau[pile_ini.y][m+i];//...et on fait le trajet vers la droite (case par case)
                if(compteur!=NULL)
                    {return 0;}//renvoie 0 si 'compteur' rencontre un pion sur le chemin
                i++;
            }
            return 1;//renvoie 1 si la boucle while a pu se finir sans probleme (pas de probleme dans le cas des pions voisins!)
        break;



        case 2:        //meme système avec les ordonnées (donc les lignes du tableau, trajet vers le bas)
            m = dy>0 ? pile_ini.y : pile_destination.y;
            while(i<abs_dy && compteur==NULL)
            {
                compteur=plateau[m+i][pile_ini.x];
                if(compteur!=NULL)
                    {return 0;}
                i++;
            }
            return 1;
        break;



        case 3:
            if(dx==dy)//déplacement selon la diagonale (1,1)->(5,5)
            {
                m = dx>0 ? pile_ini.x : pile_destination.x;//selon le sens du déplacement, on prend les coordonées de la pile initiale OU de la pile visée..
                n = m==pile_ini.x ? pile_ini.y : pile_destination.y;
                while(i<abs_dx && compteur==NULL)
                {
                    compteur=plateau[n+i][m+i];            //..et on fait le trajet vers la droite
                    if(compteur!=NULL)
                        {return 0;}
                    i++;
                }
                return 1;
            }

            else if(dx==(-dy))//déplacement selon la diagonale (1,5)->(5,1)
            {
                m = dx>0 ? pile_ini.x : pile_destination.x;
                n = m==pile_ini.x ? pile_ini.y : pile_destination.y;
                while(i<abs_dx && compteur==NULL)
                {
                    compteur=plateau[n-i][m+i];
                    if(compteur!=NULL)
                        {return 0;}
                    i++;
                }
                return 1;
            }
        break;


        default:
        return 2;
        break;

    }
    return 0;
}

int nombrePiles(pPion plateau[][TAILLE])
{
    int i, j, nb_piles;
    pPion compteur;

    for(i=1; i<TAILLE; i++)
    {
        for(j=1; j<TAILLE; j++)
        {
            compteur=plateau[i][j];
            if(compteur!=NULL)
            {   nb_piles++;   }
        }
    }
    return nb_piles;
}


//SAUVEGARDE

void sauvegarderPartie(pPion plateau[][TAILLE], sJoueur actif, sJoueur joueur1, sJoueur joueur2)
{
    int i, j;
    char nomfich[21];
    FILE * save;
    pPion compteur;

    printf("\nNommez le fichier de sauvegarde : \n(ATTENTION : ecrire '.txt' a la fin du nom, 1 a 20 caracteres)\n\n");
    fflush(stdin);
    scanf("%20s", nomfich);
    printf("sauvegarde en cours...\n\n");

    save=fopen(nomfich, "w");

    switch (actif.numero)//on écrit le tour du joueur
    {
        case 1:
        fprintf(save, "x");
        break;

        case 2:
        fprintf(save, "y");
        break;
    }

    fprintf(save, "%d%d", joueur1.points, joueur2.points);


    for(i=1; i<TAILLE; i++)
    {
        for(j=1; j<TAILLE; j++)
        {
            compteur=plateau[i][j];

            if(plateau[i][j]==NULL)
            {
                fprintf(save, "N");//case NULL (Non, ou Niet, peu importe)
            }
            else
            {
                while(compteur!=NULL)
                {
                    fprintf(save, "%c", compteur->couleur);//on écrit ensuite les couleurs des pions
                    compteur=compteur->suivant;
                }

                fprintf(save, "F");//FIN de pile
            }

        }
    }

    fclose(save);
}

void chargerPartie(pPion plateau[][TAILLE], sJoueur *actif, sJoueur *joueur1, sJoueur *joueur2)
{
    int i, j;
    char lettre, nomfich[20+1];
    FILE * load;
    pPion finListe=NULL;

    do
    {
        printf("Entrez le nom du fichier de sauvegarde :\n(ATTENTION : ecrire '.txt' a la fin du nom, 1 a 20 caracteres)\n\n");
        fflush(stdin);
        scanf("%20s", nomfich);
        printf("\nchargement en cours...\n\n");

        load=fopen(nomfich, "r");

        if(load==NULL)
        {
            printf("Fichier introuvable!\n\n");
            Pause_effacerMessage();
        }
    }while(load==NULL);


    lettre=fgetc(load);

    switch(lettre)//on récupère le tour et la couleur
    {
        case 'x':
        (*actif).numero=1;
        (*actif).couleur='b';
        break;

        case 'y':
        (*actif).numero=2;
        (*actif).couleur='r';
        break;
    }

    (*joueur1).points=(fgetc(load)-48);//on récupère les points
    (*joueur2).points=(fgetc(load)-48);//-48 a cause du changement en décimal


    for(i=1; i<TAILLE; i++)//on "parcourt" le plateau de jeu en entier
    {
        for(j=1; j<TAILLE; j++)
        {
            lettre=fgetc(load);

            while(lettre!='N' && lettre!='F')//1er pion
            {
                plateau[i][j]=creerPion(lettre);
                finListe=plateau[i][j];

                lettre=fgetc(load);

                while(lettre!='F')//pions suivants
                {
                    finListe->suivant=creerPion(lettre);
                    finListe=finListe->suivant;
                    lettre=fgetc(load);
                }
            }
        }
    }

    printf("chargement reussi!\n\n");
    Pause_effacerMessage();
}


//AFFICHAGE

void affichePlateau(pPion plateau[][TAILLE], sJoueur actif, sJoueur joueur1, sJoueur joueur2)
{
    int i, j, k;
    CD pile;

    for(i=0; i<TAILLE; i++)
    {
        for(k=5; k>0; k--)//k découpe chaque case verticalement en 5
        {
            for(j=0; j<TAILLE; j++)//on affiche couche par couche la ligne au complet
            {
            /*tous les 'else if' correspondent a des cases particulieres, les deux derniers sont généraux.
                i, k, et j sont toujours testés dans cet ordre pour gagner du temps*/

                pile.x=j;
                pile.y=i;
                Color(1,8);

                if(i==0 && k==3 && j!=0)//affiche les abscisses
                {   printf("    %d    ", j);  }

                else if(i!=0 && k==3 && j==0)//affiche les ordonnées
                {   printf("    %d    ", i);  }

                else if(i==0 && k==1 && j>0)//sépare les abscisses des case du haut du plateau
                {   printf(" _______ ");  }

                else if(i==0 && k==4 && j==5)//affiche le nombre de points de J1
                {
                    switch(actif.numero)
                    {
                        case 1:
                        printf("         ");
                        Color(1,7);
                        printf("\t");
                        Color(1,11);
                        printf("J1");
                        Color(1,7);
                        printf(" : %d pts", joueur1.points);
                        break;

                        case 2:
                        printf("         ");
                        Color(1,7);
                        printf("\tJ1 : %d pts", joueur1.points);
                        break;
                    }
                }

                else if(i==0 && k==2 && j==5)//affiche le nombre de points de J2
                {
                    switch(actif.numero)
                    {
                        case 1:
                        printf("         ");
                        Color(1,7);
                        printf("\tJ2 : %d pts", joueur2.points);
                        break;

                        case 2:
                        printf("         ");
                        Color(1,7);
                        printf("\t");
                        Color(1,11);
                        printf("J2");
                        Color(1,7);
                        printf(" : %d pts", joueur2.points);
                        break;
                    }
                }

                else if(i==0 || j==0)//complete les cases autour des abscisses et des ordonnées
                {   printf("         ");  }

                else if(i>0 && j>0)//affiche toutes les cases susceptibles d'avoir des pions
                {
                    if(k==1 && hauteurPile(plateau, pile)==0)//i et j correspondent respectivement à y et x, ils sont donc dans cet ordre
                    {   printf("|_______|");   }               //sépare chaque ligne (pour plus de visibilité)

                    else
                    {
                        printf("|");
                        affichePion(plateau, j, i, k);//affiche le k-ième pion de la pile en [i][j]
                        Color(1,8);
                        printf("|");
                    }
                }
            }
            Color(1,7);
            printf("\n");//retour à la ligne quand j a fini sa boucle
        }
    }
}

void affichePion(pPion plateau[][TAILLE], int x, int y, int k)//affiche les couleurs correspondant à une pile de pions
{
    int a;
    pPion courant;
    CD pile={x,y};

    courant=plateau[y][x];//on declare un pointeur qui va parcourir la liste en entier

    if(hauteurPile(plateau, pile)<k)//si la pile n'a pas la hauteur de k, on affiche une couche vide
    {   printf("       ");  }

    else
    {
        for(a=0; a<k-1; a++)
        {   courant=courant->suivant;   }//courant atteint le k-ième pion de la pile

        printf("__");
        switch(courant->couleur)
        {
            case 'b':
            Color(1,15);//on fait apparaitre le pion selon la couleur du joueur
            printf("___");
            break;

            case 'r':
            Color(1,12);
            printf("___");
            break;
        }
        Color(1,8);
        printf("__");
    }

}

void afficheRegles()
{
    FILE * regles;
    char ligne[60];

    regles = fopen("regles.txt", "r");

    if (regles != NULL)
    {
        while (fgets(ligne, 60, regles) != NULL) // On lit le fichier tant qu'on ne reçoit pas d'erreur (NULL)
        {
            printf("%s", ligne); // On affiche la chaîne qu'on vient de lire
        }

        printf("\n\n");
        fclose(regles);
    }

    else
    {
        printf("\n\nFichier non trouve !\n");
        Pause_effacerMessage();
    }
}

void messageErreur(pPion plateau[][TAILLE], int a, int b, int c, CD pile)
{
    if(a==0)
    {   printf("\nLa direction indiquee n'est pas valide.");   }

    if(b==0)
    {   printf("\nLa distance du deplacement choisi n'est pas correcte!");   }

    if(c==0)
    {   printf("\nUn pion est en travers du chemin!!");   }

    if(hauteurPile(plateau, pile)==0)
    {   printf("\nLa pile choisie n'existe pas.");   }

    printf("\n");
}

void Pause_effacerMessage()
{
    printf("(appuyez sur Entree)");
    fflush(stdin);
    getchar();
    system("cls");
}

void Color(int couleurDuTexte, int couleurDeFond)
{
    HANDLE H=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(H,couleurDeFond*16+couleurDuTexte);
}
