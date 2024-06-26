#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include <ncurses.h>


// var glob
int MAX_X = 10; 
int MAX_Y = 10;
int POSX_BONBON;
int POSY_BONBON;
char ** MAP; // "b" = bonbon, "x" = cell/snake, "*" nothing 

typedef struct cell{
    struct cell *next;
    int x;
    int y;
} cell;

typedef struct snake{
    cell *head;
    cell *tail;
    int size;
} snake;




// déclaration
int majScoreSnake(snake *mySnake, int newPosHeadX, int newPosHeadY); // 1 collision, 0 nothing
void majSnakeMap(snake *mySnake, int newPosHeadX, int newPosHeadY);
void mapInit(int);
void randomBonbon(void);
void dessine(void);
int running(snake *mySnake, int slowness);

// debug
void debugPrintSnake(snake *mySnake);
void debugPrint(const char *toPrint);
void debugMap();



// définitions:
void debugPrint(const char *toPrint)
{
    // Ouvrir un fichier pour écrire les résultats
    FILE *file = fopen("debugHelp.txt", "a");
    if (file == NULL) {
        fprintf(stderr, "Erreur d'ouverture du fichier.\n");
        return;
    } 

    fprintf(file, toPrint);
    
    fprintf(file, "\r\n");

    // Fermer le fichier
    fclose(file);
}


void debugPrintSnake(snake *mySnake)
{
    // Ouvrir un fichier pour écrire les résultats
    FILE *file = fopen("debugSnake.txt", "a");
    if (file == NULL) {
        fprintf(stderr, "Erreur d'ouverture du fichier.\n");
        return;
    } 

    cell *curCell = mySnake->head;
    fprintf(file, "{cell:%p, x:%d, y:%d} --> ", curCell, curCell->x, curCell->y);
    while(curCell){
        fprintf(file, "{cell:%p, x:%d, y:%d} --> ", curCell, curCell->x, curCell->y);
        curCell = curCell->next;
    }
    fprintf(file, "\r\n");

    // Fermer le fichier
    fclose(file);

}


void debugPrintMap(void)
{

    FILE *file = fopen("debugMap.txt", "a");
    if (file == NULL) {
        fprintf(stderr, "Erreur d'ouverture du fichier.\n");
        return;
    } 

    for(int i=0; i<MAX_X; i++)
    {
        for(int j=0; j<MAX_Y ;j++)
            fprintf(file, "%c", MAP[i][j]);
        fprintf(file, "\r\n");
    }
    fprintf(file, "\r\n\r\n\r\n");
    fclose(file);

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////   FIN DEBUG   ////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




void dessine(void)
{
    // Affichage de la carte:
    for (int i = 1; i < MAX_X-1; i++) {
        for(int j = 1; j < MAX_Y-1; j++){
            if(MAP[i][j] == 'x')
                mvprintw(j, i, "x");

        }
    }

    // bonbon:
    mvprintw(POSY_BONBON, POSX_BONBON, "b");
    
    // box pour les bords
    mvhline(0, 0, '-', MAX_X);
    mvhline(MAX_X-1, 0, '-', MAX_X);
    mvvline(0, MAX_X-1, '|', MAX_X);
    mvvline(0, 0, '|', MAX_X);
}

void randomBonbon(void) {
    int **parcoursXY = (int**)malloc(sizeof(int*)*MAX_X*MAX_Y);
    for(int p = 0; p<MAX_X*MAX_Y; p++)
        parcoursXY[p] = (int*)malloc(sizeof(int)*2); // X puis Y

    int curSizeParcours = 0;
    for(int i = 1; i<MAX_X-1; i++)
        for(int j=1; j<MAX_Y-1; j++)
            if(MAP[i][j] == '*')
            {  
                
                parcoursXY[curSizeParcours][0] = i;
                parcoursXY[curSizeParcours][1] = j;
                curSizeParcours++;
            }
                

    int value = rand() % curSizeParcours; // Générer un entier entre 0 et curSizeParcours-1
    int *aleatoire = parcoursXY[value]; 
    POSX_BONBON = aleatoire[0]; 
    POSY_BONBON = aleatoire[1]; 

    /*
    char str1[100], str2[100];
    sprintf(str1, "%d ", POSX_BONBON);
    sprintf(str2, "%d\r\n", POSY_BONBON);
    strcat(str1, str2);
    debugPrint(str1);
    */
    
    MAP[POSX_BONBON][POSY_BONBON] = 'b';


    /* free parcours */
    for(int p = 0; p<MAX_X*MAX_Y; p++)
        free(parcoursXY[p]);
    free(parcoursXY);

}


void mapInit(int MAPSize)
{
    // init map
    MAX_X = MAX_Y = MAPSize;
    POSX_BONBON = MAX_X/2;
    POSY_BONBON = MAX_Y/2;
    MAP = (char**)malloc((MAX_X) * sizeof(char*)); // -2 pour les walls
    for (int i = 0; i < (MAX_X); i++) {
        MAP[i] = (char*)malloc((MAX_Y)*sizeof(char));
        for(int j = 0; j<MAX_Y; j++)
            MAP[i][j] = '*'; // rien
    }

    MAP[POSX_BONBON][POSY_BONBON] = 'b';
}



int majScoreSnake(snake *mySnake, int newPosHeadX, int newPosHeadY){
    if(MAP[newPosHeadX][newPosHeadY] == 'x') { // collision
        return 1;
    }

    if(mySnake->size == MAX_X*MAX_Y -1)
        return 2; // victoire taille max atteint !


    if(newPosHeadX == POSX_BONBON && newPosHeadY == POSY_BONBON){ // ajout d'une cell
        cell *newCell = (cell*)malloc(sizeof(cell));
        newCell->next = NULL;
        newCell->x = 0;
        newCell->x = 0;

        mySnake->tail->next = newCell;
        
        mySnake->tail = newCell;
        mySnake->size++;

        randomBonbon(); // on ajoute un bonbon
    }

    
        return 0;
}

void majSnakeMap(snake *mySnake, int newPosHeadX, int newPosHeadY){

    int newX = newPosHeadX;
    int newY = newPosHeadY;
    int lastX = mySnake->head->x;
    int lastY = mySnake->head->y;
    
    cell *currentCell = mySnake->head;
    while(currentCell)
    {
        MAP[newX][newY] = 'x'; // nouvelle position est donc occupée
        
        lastX = currentCell->x;
        lastY = currentCell->y;

        MAP[lastX][lastY] = '*'; // l'ancienne position est libre

        currentCell->x = newX;
        currentCell->y = newY;

        newX = lastX; // les coordonnées du suivant sont les anciennes coordonnées du courant
        newY = lastY;

        currentCell = currentCell->next;
    }

}









//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////   DEBUT RUNNING   ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int running(snake *mySnake, int slowness)
{
    
    // ncurses:
    initscr();            // Initialise l'écran
    raw();               // Désactive le buffering des entrées
    keypad(stdscr, TRUE); // Permet l'utilisation des touches spéciales
    noecho();             // Désactive l'affichage des entrées utilisateur
    curs_set(0);          // Cache le curseur
    resizeterm(MAX_X, MAX_Y);
    
	
    int ch, saveCh, result;

    saveCh = 261; // dir de base = droite
    ch = 410; // err de base

    while(1)
    {
       

        int totalWaitMs = slowness;
        int intervalMs = totalWaitMs/100 + 1;
        int elapsedWaitMs = 0;

        nodelay(stdscr, TRUE); // getch() ne bloque pas

        while (elapsedWaitMs < totalWaitMs && ch != 'q') {
            ch = getch();
            usleep(intervalMs*1000);
            if(ch==ERR || ch==410){ // touche pas pressée

            ch = saveCh;
            // printf("ERR(savech:%d ch:%d) ", saveCh, ch);
            } 
            else{
            // printf("OK(savech:%d ch:%d) ", saveCh, ch);
            saveCh = ch;
            }
            elapsedWaitMs += intervalMs;
        }

        if(ch == 'q')
        {
            result = 3;
            break;
        }

        /*
        debugPrintSnake(mySnake);
        debugPrintMap();
        */


        int x = mySnake->head->x;
        int y = mySnake->head->y;
        
        

        switch(ch) {
            case KEY_UP:
                y = (y > 1) ? y-1 : MAX_Y-2;
                break;
            case KEY_DOWN:
                y = (y < MAX_Y-2) ? y+1 : 1;
                break;
            case KEY_LEFT:
                x = (x > 1) ? x-1 : MAX_X-2;
                break;
            case KEY_RIGHT:
                x = (x < MAX_X-2) ? x+1 : 1;    
                break;
        }


        // maj variable
        result = majScoreSnake(mySnake, x, y); // if bobon eaten = score ++
        if(result) // result = 1 = collision or result = 2 = win
            break;
        majSnakeMap(mySnake, x, y);

        // reste aff
        clear();

        // affichage
        dessine();

        
        

        refresh();
    }




    endwin(); // Restaure les paramètres du terminal

    return result;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////   DEBUT MAIN   ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




int main() {
    int quit = 0;

    while(1){
        
        printf("-----MENU-----\r\n");
        printf("1. Easy\r\n");
        printf("2. Hard\r\n");
        printf("3. Quit\r\n");
        int choice;
        printf("Enter your choice > ");
        scanf("%d", &choice);
        printf("Your choice : %d\r\n", choice);

        int MAPSize, slowness;
        switch(choice)
        {
            case 1:
                MAPSize = 20;
                slowness = 300;
                break;
            case 2:
                MAPSize = 10;
                slowness = 100;
                break;
            case 3:
                quit = 1;
                break;
        }
        if(choice == 3)
            break;

        // map init
        mapInit(MAPSize);


        // snake init
        cell *firstCell = (cell*)malloc(sizeof(cell));
        firstCell->next = NULL;
        firstCell->x = 1;
        firstCell->y = 1;
        snake *mySnake = (snake*)malloc(sizeof(snake));
        mySnake->head = firstCell;
        mySnake->tail = firstCell;
        mySnake->size = 1;

        MAP[firstCell->x][firstCell->y] = 'x';
        


        int result = running(mySnake, slowness);
        if(result == 1)
            printf("You lost! You reached the size of: %d cells! GG!\r\n", mySnake->size);
        else if(result == 2)
            printf("You won! You reached the maximum possible size for this game: %d. GG!\r\n", mySnake->size);



        /* free map */
        for (int i = 0; i < MAX_X; i++) {
            free(MAP[i]);
        }
        free(MAP);

        /* free cells */
        firstCell = mySnake->head;
        cell *nextCell = mySnake->head->next;
        while(nextCell){
            free(firstCell);
            firstCell = nextCell;
            nextCell = nextCell->next;
        }
        free(firstCell);

        /* free snake */
        free(mySnake);

    }
    if(quit)
        printf("You left :( Come back later!\r\n");


    return 0;
}
