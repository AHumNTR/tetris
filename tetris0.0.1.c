#include<stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "pthread.h"
#include<conio.h>
#include <time.h>
#include <stdlib.h>

struct tetriminoBoundries
{
    int iMax,iMin;
    int jMax,jMin;
};
struct menuThreadParameters{
    int *x,*y,*n,*m,*tetriminoSize;
    bool *useStandartTetriminos;
};
struct gameSettings{
    int n,m,tetriminoSize;
};

struct threadParameterStruct{
    int *boardPtr,*tetriminoPtr,*x,*y,*score,*maxScore;
    int n, m, tetriminoSize;
    struct tetriminoBoundries* boundries;
};
bool gameIsRunning=false,menuIsRunning=true,programIsRunning=true;
char colors[7][12]={
    "\033[0;37m",
    "\033[0;31m",//green
    "\033[0;32m",//yellow
    "\033[0;33m",
    "\033[0;34m",
    "\033[0;35m",
    "\033[0;36m",
};
int standartTetriminos[7][5][5]={
    {//I
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {1,1,1,1,0},
    },
    {//J
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,1,0,0},
        {0,0,1,1,1},
        {0,0,0,0,0},
    },
    {//L
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,1,0,0},
        {1,1,1,0,0},
        {0,0,0,0,0},
    },
    {//T
        {0,0,0,0,0},
        {0,1,0,0,0},
        {1,1,1,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
    },
    {//O
        {0,0,0,0,0},
        {0,0,1,1,0},
        {0,0,1,1,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
    },
    {//s
        {0,0,0,0,0},
        {0,0,0,1,1},
        {0,0,1,1,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
    },
    {//z
        {0,0,0,0,0},
        {1,1,0,0,0},
        {0,1,1,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
    },


};
void drawBord(int* boardPtr,int* tetriminoPtr,int x,int y,int n,int m ,int tetriminoSize,int score,int maxScore){
    printf("\e[H\e[2J\e[3J");
    int i=0,j=0;
    for(i=0;i <m+2;i++)printf("-");
    printf("\n");
for(i =0; i<n;i++){
    printf("|");
    for(j =0;j<m;j++){
         if(i>=y && i<y+tetriminoSize && j>=x && j<x+tetriminoSize ){
        if(tetriminoPtr[tetriminoSize*(i-y)+j-x]!=0){
        printf("%s",colors[tetriminoPtr[tetriminoSize*(i-y)+j-x]]);
        printf("%c",tetriminoPtr[tetriminoSize*(i-y)+j-x] ? '*':' ');
        printf("\033[1;0m");
        }
        else{
            printf("%s",colors[boardPtr[m*i+j]]);
            printf("%c",boardPtr[m*i+j] ? '*':' ');
            printf("\033[1;0m");
        }
        }
        else{
        printf("%s",colors[boardPtr[m*i+j]]);
        printf("%c",boardPtr[m*i+j] ? '*':' ');
        printf("\033[1;0m");
        }
    }
    printf("| \n");
    
    
}
for(i=0 ;i <m+2;i++)printf("-");
printf("\nScore = %d\nMax Score = %d",score,maxScore);
}

void generateTetrimino(int* tetriminoPtr,int *x,int *y,int m,int tetriminoSize,struct tetriminoBoundries* boundries,bool useStandartTetrimino){
    int color =rand()%5 +1;
    int deltai=0,deltaj=0;
    int lenght = rand()%(tetriminoSize-1)+1;
    int standartTetriminoIndex= rand()%7;
    int i=tetriminoSize/2,j=tetriminoSize/2,p=0,l;
    if(!useStandartTetrimino){
        boundries->iMax=i;
        boundries->iMin=i;
        boundries->jMax=j;
        boundries->jMin=j;
        for(p=0;p<tetriminoSize*tetriminoSize;p++)tetriminoPtr[p]=0;
        tetriminoPtr[i*tetriminoSize+j]=color;
        for(l =0;l<lenght;l++){
            deltai= rand()%3 -1;
            deltaj= rand()%3 -1;
            while(i+deltai>=tetriminoSize || i+deltai<0)deltai= rand()%3 -1;
            while(j+deltaj>=tetriminoSize || j+deltaj<0)deltaj= rand()%3 -1;
            i+=deltai;
            j+=deltaj;
            if(i>boundries->iMax)boundries->iMax=i;//not sure
            if(i<boundries->iMin)boundries->iMin=i;
            if(j>boundries->jMax)boundries->jMax=j;
            if(j<boundries->jMin)boundries->jMin=j;
            tetriminoPtr[i*tetriminoSize+j]=color;
        }
    } 
    else{
        boundries->iMax=2;
        boundries->iMin=2;
        boundries->jMax=2;
        boundries->jMin=2;
        for(p=0;p<5;p++){
            for(l=0;l<5;l++){
                tetriminoPtr[p*5+l]=standartTetriminos[standartTetriminoIndex][p][l]*color;
                if(standartTetriminos[standartTetriminoIndex][p][l]==1){
                    if(p>boundries->iMax)boundries->iMax=p;//not sure
                    if(p<boundries->iMin)boundries->iMin=p;
                    if(l>boundries->jMax)boundries->jMax=l;
                    if(l<boundries->jMin)boundries->jMin=l;
                }
            }
        }
    }
    *x=(m-tetriminoSize)/2;
    *y=-boundries->iMin;
}

void drawTetrimino(int* tetriminoPtr,int tetriminoSize,struct tetriminoBoundries* boundries){ //mostly for debug purpose
printf("\n");
int i=0,j=0;
for( i =0; i<tetriminoSize;i++){
    
    for(j =0;j<tetriminoSize;j++){
       printf("%d",tetriminoPtr[i*tetriminoSize+j]);
    }
    printf("\n");
}
printf("i Max %d \ni Min %d \nj Max %d \n j Min %d \n",boundries->iMax,boundries->iMin,boundries->jMax,boundries->jMin);
}

void placeTetrimino(int* boardPtr,int* tetriminoPtr,int x,int y,int m ,int tetriminoSize){
    int i,j;
    for(i=0;i<tetriminoSize;i++)
    {
        for(j=0; j<tetriminoSize;j++){
            if(tetriminoPtr[i*tetriminoSize+j]!=0)boardPtr[(i+y)*m+j+x]=tetriminoPtr[i*tetriminoSize+j];
        }
    }
}

bool checkForTetriminoCollision(int* boardPtr,int* tetriminoPtr,int x,int y,int n, int m,int tetriminoSize,struct tetriminoBoundries* boundries,int xMovement,int yMovement){
    int i,j;
    if(y+boundries->iMax==n-1&&yMovement>0) return true;
    for(i=boundries->iMin;i<=boundries->iMax;i++)
    {
        for(j=boundries->jMin; j<=boundries->jMax;j++){
            if(tetriminoPtr[i*tetriminoSize+j]!=0){
                if(((yMovement!=0)&&(i+y+yMovement>=n)||(i+y+yMovement<0))||(xMovement!=0&&((j+x+xMovement<0)||(j+x+xMovement>=m))))return true;
                if(boardPtr[m*(i+y+yMovement)+j+x+xMovement]!=0)
                {
                    if(y+boundries->iMin==0)
                    {
                        gameIsRunning=false;
                        menuIsRunning=true;
                    }
                    return true;
                }
            }
            
        }
    }
    return false;
}

bool checkForTetriminoCollisionTurned(int* boardPtr,int *tetriminoToCheckFor,struct tetriminoBoundries bounds,int x,int y,int n,int m,int tetriminoSize){
    int i,j;
    if(y+bounds.iMax>=n) return true;
    for(i=0;i<tetriminoSize;i++)
    {
        for(j=0; j<tetriminoSize;j++){
            if(tetriminoToCheckFor[i*tetriminoSize+j]!=0){
                if(i+y>=n||i+y<0||j+x<0||j+x>=m)return true;
                if(boardPtr[m*(i+y)+j+x]!=0) return true;
            }
            
        }
    }
    return false;
}

int checkAndDeleteFinishedLines(int* boardPtr,struct tetriminoBoundries* boundries,int x,int y,int m){
    int score=0;
    int i,j,l;
    for(i=boundries->iMin+y;i<=boundries->iMax+y;i++)
    {
        bool lineIsFinished=true;
        for(j=0; j<m;j++){
            if(boardPtr[m*(i)+j]==0)lineIsFinished=false;
        }
         if(lineIsFinished){
            score++;
            for(l=i;l>0;l--){
                for(j=0; j<m;j++){
                    boardPtr[l*m+j]=  boardPtr[(l-1)*m+j];
                }
            }
        }
    }
    return score;
}

void TurnTetrimino(int* boardPtr,int* tetriminoPtr,int x,int y,int n, int m, int tetriminoSize,struct tetriminoBoundries* boundries,bool rotateClockWise){
    // a[i][j]=b[j][k-i-1] clockwise
    // a[i][j]=c[k-j-1][i] anti clockwise
    int temp[tetriminoSize][tetriminoSize];
    int i,j;
    struct tetriminoBoundries tempBounds;
    for (i = 0; i < tetriminoSize; i++)
  		for (j = 0; j < tetriminoSize; j++)
    		temp[i][j] = 0;
    for(i =boundries->iMin;i<=boundries->iMax;i++)
    {
        for(j=boundries->jMin; j<=boundries->jMax;j++)
        {
            if(rotateClockWise){
                temp[j][tetriminoSize-i-1]=tetriminoPtr[i*tetriminoSize+j];
                printf("%d",temp[j][i]);
            }
            else{
                temp[tetriminoSize-j-1][i]=tetriminoPtr[i*tetriminoSize+j];
                printf("%d",temp[j][i]);
            }
            
        }
        printf("\n");
    }
    if(rotateClockWise){
            tempBounds.jMax=tetriminoSize-1-boundries->iMin;
            tempBounds.jMin=tetriminoSize-1-boundries->iMax;
            tempBounds.iMin=boundries->jMin;
            tempBounds.iMax=boundries->jMax;
    }
    else{
            tempBounds.iMax=tetriminoSize-1-boundries->jMin;
            tempBounds.iMin=tetriminoSize-1-boundries->jMax;
            tempBounds.jMin=boundries->iMin;
            tempBounds.jMax=boundries->iMax;
    }
    if(!checkForTetriminoCollisionTurned(boardPtr,(int*)&temp,tempBounds,x,y,n,m,tetriminoSize)){
        
        for(i =0;i<tetriminoSize;i++)
        {
            for(j=0; j<tetriminoSize;j++)
            {
                tetriminoPtr[i*tetriminoSize+j]=temp[i][j];
            }
        }
        *boundries=tempBounds;
        
       
    }
}

void *inputThread(void* argsvoidptr)
{ 
    int input=0;
    struct threadParameterStruct* args= (struct threadParameterStruct*)argsvoidptr;
    int *boardPtr=args->boardPtr ,*tetriminoPtr=args->tetriminoPtr,*x=args->x,*y=args->y,*score=args->score,*maxScore=args->maxScore;
    int n=args->n,m=args->m,tetriminoSize=args->tetriminoSize;
    struct tetriminoBoundries* boundries=args->boundries;
    while(gameIsRunning){
        input=getch();
        switch (input)
        {
        case 'a':
            if(!checkForTetriminoCollision(boardPtr,tetriminoPtr,*x,*y,n,m,tetriminoSize,boundries,-1,0)){
                *x=*x-1;
                drawBord(boardPtr,tetriminoPtr,*x,*y,n,m,tetriminoSize,*score,*maxScore);
            } 
            break;
        case 'd':
            if(!checkForTetriminoCollision(boardPtr,tetriminoPtr,*x,*y,n,m,tetriminoSize,boundries,1,0)){
                *x=*x+1;
                drawBord(boardPtr,tetriminoPtr,*x,*y,n,m,tetriminoSize,*score,*maxScore);
            }
            break;
        case 's':
            if(!checkForTetriminoCollision(boardPtr,tetriminoPtr,*x,*y,n,m,tetriminoSize,boundries,0,1)){
                *y=*y+1;
                drawBord(boardPtr,tetriminoPtr,*x,*y,n,m,tetriminoSize,*score,*maxScore);
            }
        break;
        case 'e':
            TurnTetrimino(boardPtr,tetriminoPtr,*x,*y,n,m,tetriminoSize,boundries,1);
            drawBord(boardPtr,tetriminoPtr,*x,*y,n,m,tetriminoSize,*score,*maxScore);
        break;
        case 'w':
            TurnTetrimino(boardPtr,tetriminoPtr,*x,*y,n,m,tetriminoSize,boundries,0);
            drawBord(boardPtr,tetriminoPtr,*x,*y,n,m,tetriminoSize,*score,*maxScore);
        break;
        case 'p':
        gameIsRunning=0;
        menuIsRunning=1;
        break;
        default:
            break;
        }
    }
    pthread_exit(NULL);
}
void renderMainMenu(int index,int n,int m,int tetriminoSize,bool useStandartTetrimino){
    printf("\e[H\e[2J\e[3J");
    printf("%cStart\n",index ? ' ':'>');
    printf("%cBoard height= %d\n",index-1 ? ' ' : '>',n);
    printf("%cBoard lenght%d\n",index-2 ? ' ':'>',m);
    printf("%cTetrimino types %cStandart %cRandomly generate\n",index-3 ? ' ' : '>',useStandartTetrimino ? '*': ' ',useStandartTetrimino ? ' ': '*');
    printf("%cTetrimino max size %d\n",index-4 ? ' ' : '>',tetriminoSize);
    printf("%cSave settings\n",index-5 ? ' ' : '>');
    printf("%cExit game\n",index-6 ? ' ' : '>');
}
void *menuInputThread(void* args){
    int userInput=0;
    struct menuThreadParameters* parameters=(struct menuThreadParameters*)args;
    int *x=parameters->x,*y=parameters->y,*n=parameters->n,*m=parameters->m,*tetriminoSize=parameters->tetriminoSize;
    bool *useStandartTetrimino= parameters->useStandartTetriminos;
    FILE *filePtr;
    while(menuIsRunning){
        userInput=getch();
        switch (userInput)
        {
        case 'w':
        if(*y>0){
            *y+=-1;
            renderMainMenu(*y,*n,*m,*tetriminoSize,*useStandartTetrimino);
        }
            break;
        case 's':
            if(*y<6){
            *y+=1;
            renderMainMenu(*y,*n,*m,*tetriminoSize,*useStandartTetrimino);
        }
        break;
        case 'a':
            switch (*y)
            {
            case 1:
            if(*n>2){
                *n=*n-1;
                renderMainMenu(*y,*n,*m,*tetriminoSize,*useStandartTetrimino);
            }
                break;
            case 2:
            if(*m>2){
                *m=*m-1;
                renderMainMenu(*y,*n,*m,*tetriminoSize,*useStandartTetrimino);
            }
                break;
            case 3:
                *useStandartTetrimino=!*useStandartTetrimino;
                if(useStandartTetrimino) *tetriminoSize=5;
                renderMainMenu(*y,*n,*m,*tetriminoSize,*useStandartTetrimino);
                break;
            case 4:
            if(*tetriminoSize>1 && !*useStandartTetrimino){
                *tetriminoSize=*tetriminoSize-1;
                renderMainMenu(*y,*n,*m,*tetriminoSize,*useStandartTetrimino);
            }
                break;
            }
        break;
        case 'd':
            switch (*y)
            {
            case 1:
            *n=*n+1;
            renderMainMenu(*y,*n,*m,*tetriminoSize,*useStandartTetrimino);
                break;
            case 2:
            *m=*m+1;
            renderMainMenu(*y,*n,*m,*tetriminoSize,*useStandartTetrimino);
                break;
            case 3:
                *useStandartTetrimino=!*useStandartTetrimino;
                if(useStandartTetrimino) *tetriminoSize=5;
                renderMainMenu(*y,*n,*m,*tetriminoSize,*useStandartTetrimino);
                
                break;
            case 4:
            
            if(!*useStandartTetrimino){
            *tetriminoSize=*tetriminoSize+1;
            renderMainMenu(*y,*n,*m,*tetriminoSize,*useStandartTetrimino);
            }
                break;
            }
        break;

        case 13:
            switch (*y)
            {
            case 0:
            menuIsRunning=0;
            gameIsRunning=1;
                break;
            case 5:
            filePtr=fopen("config.humsi","w");
            fprintf(filePtr,"%d %d %d %d",*n,*m,*useStandartTetrimino,*tetriminoSize);
            fclose(filePtr);
            break;
            case 6:
                menuIsRunning=0;
                gameIsRunning=0;
                programIsRunning=0;
            break;
            }
            
        }
    }
    pthread_exit(NULL);
}

struct menuThreadParameters mainMenu()
{
    pthread_t menuThreadId;
    int x=0,y=0,n=2,m=2,tetriminoSize=1;
    bool useStandartTetrimino=false;
    struct menuThreadParameters* args=(struct menuThreadParameters *)malloc(sizeof(struct menuThreadParameters));
    FILE *filePtr=fopen("config.humsi","r");
    if(filePtr!=NULL)fscanf(filePtr,"%d %d %d %d",&n,&m,&useStandartTetrimino,&tetriminoSize);
    fclose(filePtr);
    
    args->x=(int*)&x;
    args->y=(int*)&y;
    args->n=(int*)&n;
    args->m=(int*)&m;
    args->tetriminoSize=(int*)&tetriminoSize;
    args->useStandartTetriminos=(bool*)&useStandartTetrimino;
    pthread_create(&menuThreadId, NULL, menuInputThread, (void*)args);
    renderMainMenu(y,n,m,tetriminoSize,useStandartTetrimino);
    while(menuIsRunning){
        sleep(1);
    }
    return *args;
}
int main(){
    srand(time(NULL)); 
    int n=10,m=10,x=0,y=0,score=0,tetriminoSize,i,j,maxScore=0;
     FILE *filePtr;
    bool useStandartTetrimino;
    while(programIsRunning){
        struct menuThreadParameters settings= mainMenu();
        if(gameIsRunning){
            n=*settings.n;
            m=*settings.m;
            tetriminoSize=*settings.tetriminoSize;
            useStandartTetrimino=*settings.useStandartTetriminos;
            int board[n][m],tetrimino[tetriminoSize][tetriminoSize];
            filePtr=fopen("MaxScore.humus","r");
            if(filePtr!=NULL)fscanf(filePtr,"%d",&maxScore);
            fclose(filePtr);
            for (i=0;i<n;i++) for(j=0;j<m;j++)board[i][j]=0;
            pthread_t thread_id;
            struct threadParameterStruct* parameter=(struct threadParameterStruct *)malloc(sizeof(struct threadParameterStruct));
            struct tetriminoBoundries boundries;
            parameter->n=n;
            parameter->m=m;
            parameter->x=(int*)&x;
            parameter->y=(int*)&y;
            parameter->maxScore=(int*)&maxScore;
            parameter->score=(int*)&score;
            parameter->boardPtr=(int*)&board;
            parameter->tetriminoPtr=(int*)&tetrimino;
            parameter->tetriminoSize=tetriminoSize;
            parameter->boundries=(struct tetriminoBoundries*)&boundries;
            
            pthread_create(&thread_id, NULL, inputThread, (void*)parameter);
            generateTetrimino((int*)&tetrimino,(int*)&x,(int*)&y,m,tetriminoSize,&boundries,useStandartTetrimino);
            score=0;
            while (gameIsRunning)
            {
                if(checkForTetriminoCollision((int*)&board,(int*)&tetrimino,x,y,n,m,tetriminoSize,&boundries,0,1)){
                    placeTetrimino((int*)&board,(int*)&tetrimino,x,y,m,tetriminoSize);
                    score+=checkAndDeleteFinishedLines((int*)&board,&boundries,x,y,m);
                    generateTetrimino((int*)&tetrimino,(int*)&x,(int*)&y,m,tetriminoSize,&boundries,useStandartTetrimino);
                }
                else y++;
                drawBord((int*)&board,(int*)&tetrimino,x,y,n,m,tetriminoSize,score,maxScore);
                sleep(1);
                
            }
            if(score>maxScore){
                maxScore=score;
                filePtr=fopen("MaxScore.humus","w");
                fprintf(filePtr,"%d",maxScore);
                fclose(filePtr);
            }
            
        }
    }
}

