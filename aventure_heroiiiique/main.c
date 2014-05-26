/**DE MARTINO Alexandre
EPF 1A Groupe B
Promotion 2018**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#define NB_MONST 10
#define MAX 20
#define MAX_CHOIX 4


/**STRUCTURES**/


typedef struct objet
{
    int numero;
    char nom[2*MAX];
    int vie, force, resistance, habilete, chance;
    int conso, equipe;//BOOLEENS 0:NON 1:OUI
    struct objet * suivant;
}sObjet;
typedef sObjet* pObjet;


typedef struct perso
{
    char nom[MAX+1];
    int vie, force, resistance, habilete, chance;

    pObjet inventaire;//objets

}sPerso;



/**PROTOTYPES**/


//INTERACTIONS
sPerso creerPerso();
int testCompetence(int *competence);//BOOL 0:FAIL 1:WIN
int combat(sPerso *perso, int numMonst, int page);//BOOL 0:FAIL 1:WIN
sPerso loadMonster(int numMonst);
void redirection (sPerso *perso, int *nb_Obj, int reussite, int type, int *page, int next[MAX]);
//INVENTAIRE
void inventaire(sPerso *perso, int *nb_Obj);
void equiper (sPerso *perso, int *nb_Obj, pObjet pItem);
void utiliser(sPerso *perso, pObjet pItem);
void jeter (sPerso *perso, int *nb_Obj, pObjet pItem);
void ajouterObjet(sPerso *perso, int numObj);
void supprObjet(sPerso *perso, pObjet pItem);
pObjet loadItem (int numObj);
//UTILITAIRE
int jetD(int nbFaces);
void infosPage(sPerso *perso, int page, int type, int next[MAX], int *numObj, int *numMonst);
void calculMalus(sPerso *perso, int comp, int valeur);
void sauvegarde(sPerso perso, int page);
void chargement(sPerso *perso, int *page, int *nb_Obj);
void autoSave(sPerso perso, int page);
void autoRemove(sPerso perso);
//AFFICHAGE
int afficherPage(int page);
void afficherPerso(sPerso joueur);
void afficherJauge(int valeur, int couleur);//couleur BOOLEN 0:NON 1:OUI
void afficherObjet(sObjet item);
void color(int texte, int fond);
void clearScreen();


int main()
{
    int reponse = 0;
    int page, type;
    int next[MAX_CHOIX + 1];
    int numMonst, numOjb;
    int nb_Obj = 0;
    int reussite = 0;//BOOLEEN 0:NON 1:OUI

    sPerso joueur;

    srand(time(NULL));

    color(1,15);

    //ACCUEIL
    while(reponse != 1 && reponse != 2 && reponse != 3 && reponse != 4)
    {
        printf("\n\t____________________________________________________________");
        printf("\n\t|                                                          |");
        printf("\n\t|                           MENU                           |");
        printf("\n\t|                                                          |");
        printf("\n\t|                                                          |");
        printf("\n\t|                                                          |");
        printf("\n\t|             1  -    Nouvelle partie                      |");
        printf("\n\t|                                                          |");
        printf("\n\t|             2  -    Charger une partie                   |");
        printf("\n\t|                                                          |");
        printf("\n\t|             3  -    Lire les regles                      |");
        printf("\n\t|                                                          |");
        printf("\n\t|             4  -    Quitter le jeu                       |");
        printf("\n\t|                                                          |");
        printf("\n\t|                                                          |");
        printf("\n\t|__________________________________________________________|\n");
        fflush(stdin);
        scanf("%d", &reponse);
        system("cls");

        if(reponse==3)//REGLES DU JEU
        {
            afficherPage(0);
            clearScreen();
            reponse=0;
        }
    }

    if(reponse==1)//NOUVELLE PARTIE
    {
        page = 1;
        afficherPage(page);
        joueur = creerPerso();
        clearScreen();
        page ++ ;

        ajouterObjet(&joueur, 1);
        ajouterObjet(&joueur, 2);
        ajouterObjet(&joueur, 3);
        ajouterObjet(&joueur, 6);
        ajouterObjet(&joueur, 5);
    }


    else if(reponse==2)//CHARGEMENT
        chargement(&joueur, &page, &nb_Obj);

    else//QUITTER
        exit(EXIT_SUCCESS);

    //DEBUT TOUR
    do
    {
        system("cls");

        type = afficherPage(page);

        infosPage(&joueur, page, type, next, &numOjb, &numMonst);

        switch (type)//VOIR LES TYPES DANS LE RAPPORT
        {
            case 2: afficherPerso(joueur);
                reussite=testCompetence(&joueur.chance);
                break;

            case 3: afficherPerso(joueur);
                reussite=testCompetence(&joueur.habilete);
                break;

            case 4: afficherPerso(joueur);
                reussite=testCompetence(&joueur.force);
                break;

            case 5:
                reussite=combat(&joueur, numMonst, page);
                break;

            case 6: //calcul de malus fait dans infosPage
                afficherPerso(joueur);
                break;

            case 7: ajouterObjet(&joueur, numOjb);
                break;

            default ://types 1 et 8
                break;
        }

        if(joueur.vie>0)
            redirection(&joueur, &nb_Obj, reussite, type, &page, next);

    }while (joueur.vie>0);

    return 0;
}




/**FONCTIONS**/


//INTERACTIONS

sPerso creerPerso()
{
    char reponse='a';
    sPerso perso;

    printf("\n\nQuel sera le nom de votre heros? (20 caracteres max)\n");
    fflush(stdin);
    scanf("%s", perso.nom);
    printf("\n\n");


    while(reponse!='n' && reponse != 'N')
    {
        printf("Vos statistiques sont tirees aux des, excepte votre resistance :");
        perso.vie=(12 + jetD(6)+jetD(6)); //(14/24)
        perso.force=(3 + jetD(7));              //(4/10)
        perso.resistance = 0;            //améliorable UNIQUEMENT avec objets
        perso.habilete=(6 + jetD(6));           //(7/12)
        perso.chance=(6 + jetD(7));             //(8/13)
        perso.inventaire = NULL ;

        do
        {
            afficherPerso(perso);
            printf("\n\nRelancer les des? (o/n)\n");
            fflush(stdin);
            scanf("%c", &reponse);
            if (reponse !='n' && reponse !='N')
                system("cls");
        }while(reponse !='o' && reponse!='O' && reponse !='n' && reponse !='N');
    };

    return perso;
}

int testCompetence(int *competence)
{
    int resultat=0;
    printf("\n\n\n--> Vous allez devoir jeter deux des pour passer ce test de Competence.");
    printf("\n(appuyez sur Entree pour lancer les des.)");
    fflush(stdin);
    getchar();
    resultat = jetD(6);
    resultat = resultat + jetD(6);

    printf("\nResultat : %d", resultat);

    //en cas de succes, on considere que le livre est trop court pour que l'habilete ou la motivation du joueur
    //tombent a 0 a cause des tests, on ne prend donc pas la peine de verifier.

    if(resultat <= (*competence))
    {
        printf("\n\nBravo! Vous avez passe le test avec brio! Vous pouvez continuer l'aventure!");
        printf("\n(Enfin, votre competence a tout de meme perdu un point.)");
        (*competence)--;
        return 1;
    }

    else
    {
        printf("\n\nHelas, vous echouez a votre test.");
        return 0;
    }


}

int combat(sPerso *perso, int numMonst, int page)//page pour sauvAuto
{
    int var;
    int somme;
    int degats;
    char fuite='a';

    sPerso monstre = loadMonster(numMonst);

    //AFFICHAGE + FUITE
    afficherPerso(*perso);
    printf("\n\n\n\n\t\t==================| VERSUS |==================\n\n");
    afficherPerso(monstre);

    while(fuite!='o' && fuite!='n')
    {
        printf("\n\nSi vous voulez fuir du combat, c'est maintenant...\nFuir? (o/n) : ");
        fflush(stdin);
        fuite=getchar();
    }


    if (fuite=='o')
    {
        printf("\n\nVous avez fui comme un lache!!");
        return 0;
    }


    else
        autoSave(*perso, page);

    clearScreen();

    //DEBUT COMBAT

    somme = (*perso).habilete + monstre.habilete;

    while(fuite == 'n' && (*perso).vie>0 && monstre.vie>0)
    {
        var = jetD(somme);

        if(var <= (*perso).habilete )
        {
            degats = ((*perso).force +((*perso).force/10) - jetD(4) - monstre.resistance);
            if(degats < 0 )
                degats = 0;

            monstre.vie -= degats;
            printf("Vous frappez votre adversaire et infligez %d pts de degats !!\n\n", degats);
        }

        else
        {
            degats = (monstre.force +(monstre.force/10) - jetD(4) - 2*(*perso).resistance);
            if(degats < 0 )
                degats = 0;
            (*perso).vie -= degats;
            printf("Votre adversaire vous atteint et vous enleve %d points de vie !!\n\n", degats);
        }

        afficherPerso(*perso);
        printf("\n\n\n\n\t\t==================| VERSUS |==================\n\n");
        afficherPerso(monstre);

        if((*perso).vie>0 && monstre.vie>0)
            clearScreen();
    };

     if ( (*perso).vie <= 0)
     {
         printf("\n\n\nHelas, vous avez peri sous les assauts de %s.", monstre.nom);
         printf("\nVous n'etes plus tout a fait operationnel pour la suite de la journee, considerez ceci comme une defaite et recommencez.\n\n");
         return 0 ; //valeur retournee inutile car jeu terminé, a ne pas confondre avec FUITE
     }

     else
     {
         printf("\n\n\nFelicitations! Vous avez vaincu %s !", monstre.nom);
         printf("\n\nMalgre votre affaiblissement, vous continuez l'aventure !\n\n");
         autoRemove(*perso);
         return 1;
     }
}

sPerso loadMonster(int numMonst)
{
    char nomfich[MAX];
    sPerso monster;
    FILE * fichier;

    if (numMonst == 0)//tirage aleatoire
        numMonst = jetD(NB_MONST);

    sprintf(nomfich, "monstre%d.txt", numMonst);
    fichier=fopen(nomfich, "r");

    if(fichier!=NULL)
    {
        fscanf(fichier, "%s", monster.nom);
        fscanf(fichier, "%d %d %d %d %d", &monster.vie, &monster.force, &monster.resistance, &monster.habilete, &monster.chance);
        monster.inventaire = NULL;
    }
    else
        printf("\n\n\nMonstre non trouve !");

    fclose(fichier);

    return monster;
}

void redirection (sPerso *perso, int * nb_Obj, int reussite, int type, int *page, int next[MAX_CHOIX+1])
{
    int rep = 0;
    int i = 0;
    char rep2 = 'a';

    printf("\n\n===================================\n\n");

    if (type == 2 || type == 3 || type == 4 || type == 5)//REDIRECTION AUTO
    {
        (*page) = next[reussite];

        while (rep != 1)
        {
            rep = -1;

            while(rep != 1 && rep != 2 && rep != 3 && rep != 4 && rep != 5)
            {
                printf("Vous allez etre rediriges vers la page %d", *page);
                printf("\n\nQue souhaitez vous faire?");
                printf("\n\n1 - Continuer l'aventure\n2 - Acceder a l'inventaire\n3 - Sauvegarder");
                printf("\n4 - Sauvegarder et quitter\n5 - Quitter\n\n");
                fflush(stdin);
                scanf("%d", &rep);

                if(rep != 1 && rep != 2 && rep != 3 && rep != 4 && rep != 5)
                {
                    printf("\nRepondez correctement...");
                    clearScreen();
                }
            };

            switch (rep)
            {
                case 1: break;

                case 2: inventaire(perso, nb_Obj);
                    break;

                case 3: sauvegarde(*perso, *page);
                    break;

                case 4: sauvegarde(*perso, *page);
                    exit(EXIT_SUCCESS);
                    break;

                case 5: exit(EXIT_SUCCESS);
                    break;

                default: printf("\nEntrez un numero valide...");
                    clearScreen();
                    printf("\n\n===================================\n\n");
                    break;
            }

            system("cls");
        };
    }

    else // CHOIX NORMAL : 4 PAGES POSSIBLES MAX !
    {
        while (rep == 0 || rep==1 || rep==2 || (rep != next[1] && rep != next[2] && rep != next[3] && rep != next[4]))
        {
            printf("Entrez la page ou vous souhaitez aller :");
            printf("\n(page 0 pour sauvegarder , page 1 pour l'inventaire, page 2 pour quitter)");
            printf("\n\npages possibles : ");

            i = 0;

            while (i < next[0])
            {
                printf("%d ", next[i+1]);
                i++;
            }

            printf("\n\n");
            fflush(stdin);
            scanf("%d", &rep);//si entrée non numerique, rep = 0..

            if (rep == 0)
                sauvegarde(*perso, *page);

            else if (rep == 1)
                inventaire(perso, nb_Obj);

            else if (rep == 2)
            {

                rep2 = 'a';
                while (rep2 != 'o' && rep2 != 'n')
                {
                    printf("\n\n ==== QUITTER ====");
                    printf("\n\nEtes-vous sur ? (o/n) : ");
                    fflush(stdin);
                    scanf("%c", &rep2);
                    if (rep2 == 'o')
                        exit (EXIT_SUCCESS);
                }
            }

            else if (rep != next[1] && rep != next[2] && rep != next[3] && rep != next[4])//entrée foireuse
                printf("\nChoisissez une page valide ...\n");

            if (rep == 0 || rep == 1 || rep == 2)
            {
                clearScreen();
                afficherPage(*page);
                printf("\n\n===================================\n\n");
            }
        };

        (*page) = rep;
    }
}



//INVENTAIRE

void inventaire(sPerso *perso, int *nb_Obj)
{
    int rep = 1;
    int num ;

    pObjet temp;
    temp = (*perso).inventaire;

    while(rep != 4)
    {
        system("cls");

        temp = (*perso).inventaire;

        printf("================================================================================\n");
        printf("                                  INVENTAIRE                                    \n");
        printf("================================================================================");

        afficherPerso(*perso);

        if (temp != NULL)
        {
            printf("\n\nVoici la liste des objets que vous transportez :");
            printf("\nLes objets equipes sont surlignes en rouge. Vous avez droit a 6 objets equipes au maximum) :");
            while (temp != NULL)
                {
                    afficherObjet(*temp);
                    temp = temp -> suivant;
                }
            printf("\n\nNombre d'objets equipes : %d", *nb_Obj);
        }
        else
            printf("\n\nVotre inventaire est vide!");

        do
        {
            printf("\n\n\t____________________________________________________________");
            printf("\n\t|                                                          |");
            printf("\n\t|   1 -  Equiper/enlever un objet                          |");
            printf("\n\t|                                                          |");
            printf("\n\t|   2 -  Utiliser un objet                                 |");
            printf("\n\t|                                                          |");
            printf("\n\t|   3 -  Jeter un objet                                    |");
            printf("\n\t|                                                          |");
            printf("\n\t|   4 -  Sortir                                            |");
            printf("\n\t|__________________________________________________________|\n");
            fflush(stdin);
            scanf("%d", &rep);
        }while(rep != 1 && rep != 2 && rep != 3 && rep != 4);

        temp = (*perso).inventaire;

        if(rep != 4 && temp != NULL)//inventaire non vide + action
        {
            printf("\nEntrez le numero de l'objet choisi : ");
            fflush(stdin);
            scanf("%d", &num);

            while(temp != NULL && temp -> numero != num)//on cherche le bon objet
            {
                temp = temp -> suivant;
            }

            if (temp != NULL)//objet trouvé
            {
                printf("\nNom objet : %s", temp -> nom);

                if(rep == 1)
                    equiper(perso, nb_Obj, temp);

                else if (rep == 2)
                    utiliser(perso, temp);

                else if (rep == 3)
                    jeter(perso, nb_Obj, temp);
            }

            else
                printf("\nObjet non trouve !");
        }

        else if (rep != 4 && temp == NULL)
            printf("\nVotre inventaire est vide, pas d'action possible !");

        else if (rep != 4)//ne doit pas afficher en cas de sortie
            printf("\nPROBLEME INVENTAIRE");

        if (rep != 4)
            clearScreen();
    };
}

void equiper (sPerso *perso, int *nb_Obj, pObjet pItem)
{
    if (pItem -> equipe == 0 && (*nb_Obj) < 6 && pItem -> conso == 0)
    {
        pItem -> equipe=1;
        (*nb_Obj) ++ ;
        (*perso).vie += pItem -> vie;
        (*perso).force += pItem -> force;
        (*perso).resistance += pItem -> resistance;
        (*perso).habilete += pItem -> habilete;
        (*perso).chance += pItem -> chance;
    }

    else if (pItem -> equipe==0 && (*nb_Obj) >= 6 && pItem -> conso == 0)
    {
        printf("\n\nVous avez deja 6 objets equipes ! Enlevez en un d'abord.");
        clearScreen();
    }

    else if (pItem -> conso ==1)
    {
        printf("\nObjet consommable, pas a equiper !");
        clearScreen();
    }

    else
    {
        pItem -> equipe = 0 ;
        (*nb_Obj)-- ;
        (*perso).vie -= pItem -> vie;
        (*perso).force -= pItem -> force;
        (*perso).resistance -= pItem -> resistance;
        (*perso).habilete -= pItem -> habilete;
        (*perso).chance -= pItem -> chance;
    }
}

void utiliser(sPerso *perso, pObjet pItem)
{
    if (pItem -> conso == 1)//CONSO UNIQUEMENT POUR SE HEAL
    {
        (*perso).vie += pItem -> vie;
        printf("\nObjet consomme avec succes!");
        supprObjet(perso, pItem);
    }

    else
        printf("\nObjet non consommable !");
}

void jeter (sPerso *perso, int *nb_Obj, pObjet pItem)
{
    char rep = 'a' ;
    while (rep!='o' && rep != 'n')
    {
        if (pItem -> equipe == 1)
            printf("\n\nCet objet est equipe, etes-vous sur de vouloir le supprimer? (o/n) : ");

        else
            printf("\n\nEtes-vous sur de vouloir supprimmer cet objet? (o/n) : ");

        fflush(stdin);
        scanf("%c", &rep);
    }

    if (rep == 'o' && pItem -> equipe != 1)
        supprObjet(perso, pItem);

    else if (rep == 'o' && pItem -> equipe == 1)
    {
        equiper(perso, nb_Obj, pItem);
        supprObjet(perso, pItem);
    }
}

void ajouterObjet(sPerso *perso, int numObj)
{
    pObjet temp2;

    temp2 = (*perso).inventaire;

    //INSERTION AU DEBUT
    if(temp2 != NULL)//inventaire non vide
    {
        (*perso).inventaire = loadItem(numObj);
        (*perso).inventaire -> suivant = temp2;
    }
    else
        (*perso).inventaire = loadItem(numObj);

    printf("\n\n===================================");
    printf("\n\nFelicitations, vous avez gagne un objet !");
    afficherObjet(*(*perso).inventaire);
}

void supprObjet(sPerso *perso, pObjet pItem)
{
    pObjet temp2 = (*perso).inventaire;

    if (temp2 == pItem && temp2 -> suivant == NULL)//premier et unique objet
        (*perso).inventaire = NULL ;

    else if (temp2 == pItem && temp2 -> suivant != NULL)//premier objet
        (*perso).inventaire = temp2 -> suivant ;

    else //2nd objet au minimum
    {
        while (temp2 -> suivant != pItem)//on est deja sur que l'objet est trouvé, pItem pointe dessus.
            {
                temp2 = temp2 -> suivant;
            }

        temp2 -> suivant = pItem -> suivant;
    }

    free(pItem);
}

pObjet loadItem(int numObj)
{
    char nomfich[MAX+1];
    pObjet item;
    FILE * fichier;

    sprintf(nomfich, "objet%d.txt", numObj);

    fichier=fopen(nomfich, "r");

    if(fichier!=NULL)
    {
        item = (pObjet)malloc(sizeof(sObjet));

        fscanf(fichier, "%s", item -> nom);
        fscanf(fichier, "%d %d %d %d %d", &item->vie, &item->force, &item->resistance, &item->habilete, &item->chance);
        fscanf(fichier, "%d", &item -> conso);
        (item -> numero) = numObj;
        (item -> equipe) = 0;//objet non équipé au départ
        (item -> suivant) = NULL;
    }
    else
        printf("ERREUR LOADITEM");

    return item;
}



//UTILITAIRE

int jetD(int nbFaces)
{
    return (1+ rand()%nbFaces);
}

void infosPage(sPerso *perso, int page, int type, int next[MAX_CHOIX], int *numObj, int *numMonst)
{
    int comp, val;
    int i = 0;
    char c = 'a';
    char nomfich[MAX];
    FILE *fichier;

    while (i < MAX_CHOIX)//on réinitialise pour etre sur
    {                    //de ne pas avoir de probleme
        next[i] = 0;
        i++ ;
    }
    sprintf(nomfich, "page%d.txt", page);

    fichier=fopen(nomfich, "r");

    if(fichier!=NULL)
    {
        while(c != '/' && c != EOF)
        {         c = fgetc(fichier);      }
            //ON VA JUSQU'AU SLASH
    }
    else
        printf("PAGE NON TROUVEE");


    //ACTIONS SPECIFIQUES
    if (type == 2 || type == 3 || type == 4)//tests de competences
        fscanf(fichier, "%d %d", &next[0], &next[1]);//0: pageFAIL  1: pageWIN

    else if (type == 5)//combat
        fscanf (fichier, "%d %d %d", &next[0], &next[1], &(*numMonst));//0: pageFUITE 1: pageWIN

    else if(type == 6)//bonus ou malus
    {
        fscanf(fichier, "%d %d", &comp, &val);//competence / valeur
        calculMalus(perso, comp, val);
    }

    else if (type == 7)//obtention d'item
        fscanf(fichier, "%d", &(*numObj));

    else if (type != 1)
        printf("\nPROBLEME ACTIONS SPECIFIQUES");


    //ACTIONS COMMUNES
    if (type == 1 || type == 6 || type == 7)
    {
        i = 0;
        fscanf(fichier, "%d", &next[0]);//nombre de pages
        while (i < next[0])         //numeros de page un par un
        {
            fscanf(fichier, "%d", &next[i+1]);
            i ++ ;
        }
        while(i < MAX_CHOIX)
        {
            next[i+1] = -1 ;
            i++ ;
        }
    }

    fclose(fichier);
}

void calculMalus(sPerso *perso, int comp, int valeur)
{
    printf("\n\ncomp : %d\nvaleur : %d", comp, valeur);

    switch (comp)
    {
        case 1: (*perso).vie = (*perso).vie + valeur;
            break;

        case 2: (*perso).force = (*perso).force + valeur;
            break;

        case 3:(*perso).resistance = (*perso).resistance + valeur;
            break;

        case 4:(*perso).habilete = (*perso).habilete + valeur;
            break;

        case 5:(*perso).chance = (*perso).chance + valeur;
            break;

        default : printf("\nPROBLEME CALCUL MALUS");
            break;
    }
}

void sauvegarde(sPerso perso, int page)
{
    int i = 0;//nombre d'objets dans l'inventaire
    char nomfich[MAX+1], rep;
    FILE * save;
    pObjet compteur=NULL;


    do
    {
        printf("\nVoulez-vous sauvegarder ? (o/n) : ");
        fflush(stdin);
        scanf("%c", &rep);

        if (rep == 'o')
        {
            printf("\nNommez le fichier de sauvegarde : (20 caracteres max) ");
            fflush(stdin);
            scanf("%20s", nomfich);
            printf("\nSauvegarde en cours...");
            strcat(nomfich, ".txt");//on rajoute l'extension

            save=fopen(nomfich, "w");

            if(save!=NULL)
            {
                fprintf(save, "%s\n", perso.nom);//STATS
                fprintf(save, "%d %d %d %d %d\n", perso.vie, perso.force, perso.resistance, perso.habilete, perso.chance);
                fprintf(save, "%d\n", page);

                compteur = perso.inventaire ;//INVENTAIRE
                if (compteur != NULL)
                {
                    while(compteur != NULL)
                    {
                        i ++ ;
                        compteur = compteur -> suivant;
                    };

                    fprintf(save, "%d\n", i);//on écrit le nombre d'objets dans l'inventaire

                    compteur = perso.inventaire ;//puis tous les objets 1 par 1
                    while(compteur != NULL)
                    {
                        fprintf(save, "%d %d\n", compteur->numero, compteur->equipe );
                        compteur = (compteur->suivant);
                    };
                }
            }

            else
                printf("PROBLEME SAUVEGARDE");

            fclose(save);
            printf("\n\nSauvegarde reussie !");
        }

        else if (rep == 'n')
            break;

    }while (rep != 'o' && rep != 'n');
}

void chargement (sPerso *perso, int *page, int *nb_Obj)
{
    int i = 0, j = 0;
    int num, equip;
    char nomfich[MAX];
    FILE * load;

    (*nb_Obj) = 0;

    do
    {
        printf("Entrez le nom du fichier de sauvegarde : (20 caraceres max)\n\n");
        scanf("%20s", nomfich);
        strcat(nomfich, ".txt");

        load=fopen(nomfich, "r");

        if(load==NULL)
        {
            printf("Fichier introuvable!\n\n");
        }
    }while(load==NULL);

    printf("\nchargement en cours...\n\n");

    fscanf(load, "%s", (*perso).nom);
    fscanf(load, "%d %d %d %d %d", &(*perso).vie, &(*perso).force, &(*perso).resistance, &(*perso).habilete, &(*perso).chance);
    fscanf(load, "%d", &(*page));
    fscanf(load, "%d", &i);//nombre d'objets

    for(j=0; j<i; j++)
    {
        fscanf(load, "%d %d", &num, &equip);
        ajouterObjet(perso, num);

        if(equip == 1)
            equiper(perso, nb_Obj, (*perso).inventaire);
    }

    fclose(load);

    afficherPerso(*perso);
    printf("\n\nVous etes actuellement a la page %d.", *page);

    clearScreen();
}

void autoSave(sPerso perso, int page)
{
    int i = 0;
    char nomfich[MAX+1];
    FILE * aSave;
    pObjet compteur=NULL;

    sprintf(nomfich, "%s.txt", perso.nom);

    aSave = fopen(nomfich, "w");

    if(aSave!=NULL)
    {
        fprintf(aSave, "%s\n", perso.nom);//STATS
        fprintf(aSave, "%d %d %d %d %d\n", perso.vie, perso.force, perso.resistance, perso.habilete, perso.chance);
        fprintf(aSave, "%d\n", page);

        compteur = perso.inventaire ;//INVENTAIRE
        if (compteur != NULL)
        {
            while(compteur != NULL)
            {
                i ++ ;
                compteur = compteur -> suivant;
            };

            fprintf(aSave, "%d\n", i);//on écrit le nombre d'objets dans l'inventaire

            compteur = perso.inventaire ;//puis tous les objets 1 par 1
            while(compteur != NULL)
            {
                fprintf(aSave, "%d %d\n", compteur->numero, compteur->equipe );
                compteur = (compteur->suivant);
            };
        }

        printf("\nSauvegarde automatique reussie.");

    }
    else
        printf("PROBLEME SAUVEGARDE AUTO");

    fclose(aSave);
}

void autoRemove(sPerso perso)
{
    char nomfich[MAX];

    sprintf(nomfich, "%s.txt", perso.nom);

    if (remove(nomfich) == 0)
        printf("\n\nSuppression auto reussie.");

    else
        printf("\n\nPROBLEME REMOVE");
}



//AFFICHAGE

int afficherPage(int page)
{
    int type;
    char c = 'a';
    char nomfich[MAX];
    FILE *fichier;

    printf("===============================================================================\n\n");
    printf("                                      PAGE  %d                                 \n\n", page);
    printf("===============================================================================\n\n");


    sprintf(nomfich, "page%d.txt", page);

    fichier=fopen(nomfich, "r");

    if(fichier!=NULL)
    {
        fscanf(fichier, "%d", &type);

        while(c != '/' && c != EOF)
        {
            c = fgetc(fichier);

            if (c!= '/')
                printf("%c", c);
        }
    }
    else
        printf("PAGE NON TROUVEE");

    fclose(fichier);

    if (type == 8)
        exit(EXIT_FAILURE);

    return type;
}

void afficherPerso(sPerso perso)
{
    printf("\n\nNOM : %s", perso.nom);
    printf("\n\nCaracteristiques :\t");

    if(perso.vie>0)
    {
        printf("Vie\t\t%d", perso.vie);
        afficherJauge(perso.vie, 1);
    }
    else
        printf("Vie\t\t0");

    printf("\n\t\t\tForce\t\t%d",perso.force);
    afficherJauge(perso.force,0);
    printf("\n\t\t\tResistance\t%d",perso.resistance);
    afficherJauge(perso.resistance,0);
    printf("\n\t\t\tHabilete\t%d",perso.habilete);
    afficherJauge(perso.habilete,0);
    printf("\n\t\t\tChance\t\t%d", perso.chance);
    afficherJauge(perso.chance,0);
}

void afficherJauge(int valeur, int couleur)
{
    int i=0;

    if (valeur<10)
        printf(" ");//alignement visuel

    if (couleur==1)//BOOLEN 0:NON 1:OUI
    {
        if(valeur>=25)
            color(0,11);//bleu
        else if(valeur>=15)
            color(0,10);//vert
        else if(valeur>=8)
            color(0,14);//jaune
        else
            color(0,12);//rouge
    }
    else
        color(0,7);//gris

    while(i<valeur)
    {
        printf(" ");
        i++;
    }

    color(1,15);
}

void afficherObjet(sObjet item)
{
    if(item.equipe==1)
        color(15,12);
    else if (item.conso == 1)
        color(2,15);
    else
        color (12,15);
    printf("\n\n%d NOM : %s", item.numero, item.nom);
    color(1,15);
    printf("\n\nSTATS :");

    printf("\tVie\t");

    if (item.conso != 1)
    {
        printf("\t%d", item.vie);
        afficherJauge(item.vie, 0);

        printf("\n\tForce\t\t");
        printf("%d", item.force);
        afficherJauge(item.force,0);

        printf("\n\tResist\t\t");
        printf("%d", item.resistance);
        afficherJauge(item.resistance, 0);

        printf("\n\tHabilete\t");
        printf("%d", item.habilete);
        afficherJauge(item.habilete, 0);

        printf("\n\tChance\t\t");
        printf("%d", item.chance);
        afficherJauge(item.chance, 0);
    }

    else
    {
        printf("+ %d ", item.vie);
        afficherJauge(item.vie, 0);
    }
}

void color(int couleurDuTexte, int couleurDeFond)
{
    HANDLE H=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(H,couleurDeFond*16+couleurDuTexte);
}

void clearScreen()
{
    printf("\n** Appuyer sur Entree pour continuer **");
    fflush(stdin);
    getchar();
    system("cls");
}

