/*-----------------------------------------*/
/* Vysoké učení technické v Brně -         */
/* Fakulta informačních technologií (FIT)  */
/*                                         */
/* Projekt č.2 do předmětu IOS             */
/*                                         */
/* Implementace "River Crossing Problem"   */
/* Autor: Michal Rein                      */
/* Login: xreinm00                         */
/*                                         */
/*-----------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdbool.h>


/*Struktura pro ukládání parametrů*/
typedef struct {

  int p;
  int h;
  int s;
  int r;
  int w;
  int c;

} parameters;

/*Ukazatele pro sdílenou paměť*/
int
  *sh_hackers_waiting,
  *sh_serfs_waiting,
  *sh_wait_room,
  *sh_hackers_index,
  *sh_serfs_index,
  *sh_molo_counter,
  *sh_action_counter,
  *sh_does_captain_exists,
  *sh_boat_seats,
  *sh_hackers_waiting_for_boarding,
  *sh_serfs_waiting_for_boarding;

/*Proměnné pro sdílenou paměť*/
int shm_0, shm_1, shm_2, shm_3, shm_4, shm_5, shm_6, shm_7, shm_8, shm_9;

/*Výstupní log*/
FILE *f_output;

/*Semafory*/
sem_t *sem_molo = NULL,
      *sem_counter_blocker = NULL,
      *sem_index = NULL,
      *sem_waiting_hackers = NULL,
      *sem_waiting_serfs = NULL,
      *sem_landing = NULL,
      *sem_boat_away = NULL,
      *sem_barricade = NULL,
      *sem_log = NULL,
      *sem_release_captain = NULL;

/*Prototypy funkcí*/
 int parse_args(int argc, char *argv[], parameters *pms);
 int access_pier(char *group, int id, parameters *params, FILE *f_output);
 int start_board(char *group, int free);
 void leave_pier(char *group, int id, parameters *params, FILE *f_output);
 int create_hackers(parameters *params, FILE *f_output);
 int create_serfs(parameters *params, FILE *f_output);
 int init_resources(parameters *params);
 void release_resources(FILE *f_output);
 void counter(char *group, int action, int molo, int hackers, int serfs);
 void print_help();

/* MAIN */
int main(int argc, char *argv[])
{


  parameters *params, init;
  params = &init;

  /*Otevření souboru pro výstup*/
  if ((f_output = fopen("proj2.out", "w+")) == NULL){
    fprintf(stderr, "Nelze otevřít soubor proj2.out!\n");
    return 1;
  }

  setbuf(f_output, NULL);

  /*Projde a otestuje všechny přiložené parametry*/
  if ( parse_args(argc, argv, params) == 1){
    print_help();
    return 1;
  }

  /*Inicializace semaforů a pomocných proměnných*/
  if (init_resources(params) == 1)
  {
    fprintf(stderr, "Nastala chyba při inicializaci zdrojů!\n");
    release_resources(f_output);
    return 3;
  }

  *sh_molo_counter = 0;
  *sh_hackers_index = 1;
  *sh_serfs_index = 1;
  *sh_action_counter = 1;
  *sh_serfs_waiting = 0;
  *sh_hackers_waiting = 0;
  *sh_does_captain_exists = 0;
  *sh_boat_seats = 0;
  *sh_hackers_waiting_for_boarding = 0;
  *sh_serfs_waiting_for_boarding = 0;

  /*V této části se parent proces rozdělí*/
  pid_t pid_hackers, pid_serfs;

   /*HACKERS*/
   pid_hackers = fork();

   if (pid_hackers < 0){
     fprintf(stderr, "Nepodařilo se vytvořit procesy HACK!\n");
     release_resources(f_output);
     return 2;
   }

   /*Child volá funkci create_hackers(), kde se začnou vytvářet procesy HACK*/
   if (pid_hackers == 0)
   {
     create_hackers(params, f_output);
   }

   /*Parrent pokračuje a vytvoří hlavu skupiny SERF*/
   if (pid_hackers > 0)
   {

     /*SERFS*/

     /*Randomizace funkce rand()*/
     srand(time(0));

     pid_serfs = fork();

     if (pid_serfs < 0){
       fprintf(stderr, "Nepodařilo se vytvořit procesy SERF!\n");
       release_resources(f_output);
       return 2;
     }

     /*Volání funkce create_serfs(), kde se začnou vytvářet procesy SERF*/
     if (pid_serfs == 0)
     {
       create_serfs(params, f_output);
     }
   }

   /*Parrent čeká na všechny ostatní procesy*/
    for (int w = 0; w < ((params -> p) * 2); w++)
    {                   
        wait(NULL);
    }

    waitpid(pid_hackers, NULL, 0);
    waitpid(pid_serfs, NULL, 0);
    

   /*Uklidí všechny naalokované zdroje (sdílenou paměť, semafory), zavře výstupní soubor*/
   release_resources(f_output);

  
  return 0;
}

/*---------------------------------------------------------------------------------*/
/*  create_hackers():                                                              */
/*- Funkce vytváří nové procesy HACK, v náhodných intervalech daných proměnnou 'h' */
/*- Každý proces po vytvoření ohlásí svoji existenci a volá funkci access_pier()   */
/*---------------------------------------------------------------------------------*/
int create_hackers(parameters *params, FILE *f_output)
{
  pid_t hacker;

  /*Vytvoří tolik procesů, kolik je předepsáno parametrem 'P' */
  for(int i = 0; i < params -> p; i++){

     /* Uspí vytvoření procesu na dobu <0,H> ms*/
    if(params -> h != 0){
      usleep(rand() % (params -> h) * 1000);
    }

    /*Vytvoření nového procesu HACK*/
    hacker = fork();

    if (hacker < 0){
      fprintf(stderr, "Chyba při vytváření některého z procesů HACK!\n");
      return 3;
    }
    if (hacker == 0){

      sem_wait(sem_index); //pro zaručení unikátního indexu každého procesu

        sem_wait(sem_log);
          //printf("%d: HACK %d: starts\n", *sh_action_counter, *sh_hackers_index);
          fprintf(f_output, "%d: HACK %d: starts\n", *sh_action_counter, *sh_hackers_index);

          int id = *sh_hackers_index; //uložení indexu pro předání do dalších funkcí
          ++*sh_hackers_index;        //inkrementace indexu pro další proces
          counter("Hackers",1,0,0,0); //inkrementace sh_action_counter
        sem_post(sem_log);

      sem_post(sem_index);

      /*Proces vstupuje na molo*/
      access_pier("Hackers", id, params, f_output);
    }
  }

  exit(0);
}

/*---------------------------------------------------------------------------------*/
/*  create_serfs():                                                                */
/*- Funkce vytváří nové procesy SERF, v náhodných intervalech daných proměnnou 's' */
/*- Každý proces po vytvoření ohlásí svoji existenci a volá funkci access_pier()   */
/*---------------------------------------------------------------------------------*/
int create_serfs(parameters *params, FILE *f_output)
{
  pid_t serf;

  /*Vytvoří tolik procesů, kolik je předepsáno parametrem 'P' */
  for(int i = 0; i < params -> p; i++){

    /* Uspí vytvoření procesu na dobu <0,H> ms*/
    if(params -> s != 0){
      usleep(rand() % (params -> s) * 1000);
    }

    /*Vytvoření nového procesu SERF*/
    serf = fork();

    if (serf < 0){
      fprintf(stderr, "Chyba při vytváření některého z procesů SERF!\n");
      return 3;
    }

    /*Nový proces SERF*/
    if (serf == 0){

      sem_wait(sem_index);//pro zaručení unikátního indexu každého procesu

        sem_wait(sem_log);
          //printf("%d: SERF %d: starts\n", *sh_action_counter, *sh_serfs_index);
          fprintf(f_output, "%d: SERF %d: starts\n", *sh_action_counter, *sh_serfs_index);

          int id = *sh_serfs_index; //uložení indexu pro předání do dalších funkcí
          ++*sh_serfs_index;        //inkrementace indexu pro další proces
          counter("Serfs",1,0,0,0); //inkrementace sh_action_counter
        sem_post(sem_log);

      sem_post(sem_index);

      /*Proces vstupuje na molo*/
      access_pier("Serfs", id, params, f_output);
    }
  }
  exit(0);
}

/*------------------------------------------------------------------------------*/
/* counter():                                                                   */
/*- Funkce slouží pro inkrementaci proměnných, ke kterým může přistupovat       */
/*  zároveň více procesů najednou --> je chráněná semaforem sem_counter_blocker */
/*- Každá proměnná má své místo, inkrementace proběhne, jestliže je proměnná    */
/*  nastavená na hodnotu '1'                                                    */
/*                                                                              */
/*  Pořadí proměnných:                                                          */
/*                                                                              */
/*  -  (*group identifikuje příslušníka skupiny)                                */
/*  -  action = pořadové číslo prováděné akce                                   */
/*  -  molo = počet osob na molu                                                */
/*  -  hackers = počet HACK na molu                                             */
/*  -  serfs = počet SERF na molu                                               */
/*------------------------------------------------------------------------------*/
void counter(char *group, int action, int molo, int hackers, int serfs)
{
  sem_wait(sem_counter_blocker);

    if (strcmp(group, "Hackers") == 0){

      if (action == 1) ++*sh_action_counter;
      if (molo == 1) ++*sh_molo_counter;
      if (hackers == 1) ++*sh_hackers_waiting;

    }

    if (strcmp(group, "Serfs") == 0){

      if (action == 1) ++*sh_action_counter;
      if (molo == 1) ++*sh_molo_counter;
      if (serfs == 1) ++*sh_serfs_waiting;

    }

  sem_post(sem_counter_blocker);

}

/*------------------------------------------------------------------------------*/
/* leave_pier()                                                                 */
/*- Funkce se volá tehdy, když proces zjistí , že je plná kapacita mola.        */
/*- Probíhá cyklicky tisk oznámení (leaves queue / is back), v závisloti        */
/*  na proměnné wait.                                                           */
/*- Doba uspání procesu závisí na proměnné 'w', přístupné z proměnné 'params'   */
/*------------------------------------------------------------------------------*/

void leave_pier(char *group, int id, parameters *params, FILE *f_output)
{
  /*Proměnná, na které závisí celý čekací cyklus*/
  bool wait = true;

  /*Větev pro čekající procesy HACK*/
  if (strcmp(group, "Hackers") == 0)
  {
    /*Proces zde čeká a opakovaně testuje hodnotu semaforu sem_molo */
    while(wait == true){

      /*Proces opouští frontu na molu*/
      sem_wait(sem_log);
        //printf("%d: HACK %d: leaves queue: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
        fprintf(f_output, "%d: HACK %d: leaves queue: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
        counter("Hackers",1,0,0,0);
      sem_post(sem_log);

    /*Uspání na náhodnou dobu*/
    usleep(rand() % (params -> w) * 1000);

      /*Proces se probudil a jde se zkusit znova dostat na molo*/
      sem_wait(sem_log);
        //printf("%d: HACK %d: is back\n", *sh_action_counter, id);
        fprintf(f_output, "%d: HACK %d: is back\n", *sh_action_counter, id);
        counter("Hackers",1,0,0,0);
      sem_post(sem_log);

    /*Test semaforu sem_molo*/
    sem_wait(sem_barricade);
      int test_value;
        sem_getvalue(sem_molo, &test_value);

        /*Jestliže není semafor uzamčen, ihned si zabere místo v semaforu a odchází z funkce*/
        if (test_value > 0 ){
          wait = false;
          sem_wait(sem_molo);
        }
    sem_post(sem_barricade);

    }
  }

  /*Větev pro čekající procesy SERF*/
  if (strcmp(group, "Serfs") == 0)
  {
    /*Proces zde čeká a opakovaně testuje hodnotu semaforu sem_molo */
    while(wait == true){

      /*Proces opouští frontu na molu*/
      sem_wait(sem_log);
        //printf("%d: SERF %d: leaves queue: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
        fprintf(f_output, "%d: SERF %d: leaves queue: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
        counter("Serfs",1,0,0,0);
      sem_post(sem_log);

    /*Uspání na náhodnou dobu*/
    usleep(rand() % (params -> w) * 1000);

      /*Proces se probudil a jde se zkusit znova dostat na molo*/
      sem_wait(sem_log);
        //printf("%d: SERF %d: is back\n", *sh_action_counter, id);
        fprintf(f_output, "%d: SERF %d: is back\n", *sh_action_counter, id);
        counter("Serfs",1,0,0,0);
      sem_post(sem_log);

    /*Test semaforu sem_molo*/
    sem_wait(sem_barricade);
      int test_value;
        sem_getvalue(sem_molo, &test_value);

        /*Jestliže není semafor uzamčen, ihned si zabere místo v semaforu a odchází z funkce*/
        if (test_value > 0 ){
          wait = false;
          sem_wait(sem_molo);
        }
    sem_post(sem_barricade);

    }
  }
}

/*---------------------------------------------------------------------------------*/
/* access_pier()                                                                   */
/*- Alfa a omega celého programu                                                   */
/*- Procesy zde přistupují na molo, projdou naloděním a vyloděním                  */
/*- Při naloďování se jeden proces stane kapitánem, ten zajistí uvolnění mola,     */
/*  uspí se a následně vypustí svou posádku, přičemž poslední propuštěný umožní    */
/*  kapitánovi, aby se vylodil (tím je zajištěno, že se kap. vylodí jako poslední) */
/*- V případě naplení kapacity mola volají procesy funkci leave_pier a čekají,     */
/*  než se některá další skupina nalodí a uvolní tím místo na molu                 */
/*---------------------------------------------------------------------------------*/
int access_pier(char *group, int id, parameters *params, FILE *f_output)
{
  bool isCaptain = false;

  /*Část pro skupinu HACK*/
  if (strcmp(group, "Hackers") == 0){

      /*Testuje jestli nebyla překročena kapacita mola,
      v případě přeplnění proces opuští molo a uspí se*/
      if(*sh_molo_counter == params -> c)
      {
        leave_pier("Hackers", id, params, f_output);
      }
      else
      {
        sem_wait(sem_molo);
      }

        /*Proces úspěšně vstoupil na molo a čeká*/
        sem_wait(sem_log);
          counter("Hackers",0,0,1,0);
          //printf("%d: HACK %d: waits: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
          fprintf(f_output, "%d: HACK %d: waits: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
          counter("Hackers",1,1,0,0);
        sem_post(sem_log);

        /*Proces čeká, až se bude moci nalodit*/
        sem_wait(sem_boat_away); //čekající HACK
        *sh_hackers_waiting_for_boarding += 1;

        /*Kontrola, jestli už není možné vytvořit skupinu pro nalodění*/
        if (*sh_hackers_waiting_for_boarding >= 2 && *sh_serfs_waiting_for_boarding >= 2 && *sh_does_captain_exists == 0){
          *sh_does_captain_exists = 1;
          isCaptain = true;

          *sh_hackers_waiting -= 2;
          *sh_serfs_waiting -= 2;
          *sh_hackers_waiting_for_boarding -= 2;
          *sh_serfs_waiting_for_boarding -= 2;

          start_board("Hackers",2);

        }
        else if(*sh_hackers_waiting_for_boarding >= 4 && *sh_serfs_waiting_for_boarding < 2 && *sh_does_captain_exists == 0){
          *sh_does_captain_exists = 1;
          isCaptain = true;

          *sh_hackers_waiting -= 4;
          *sh_hackers_waiting_for_boarding -= 4;

          start_board("Hackers",4);

        }else{

        /*Uvolnění semaforu pro další proces*/
        sem_post(sem_boat_away);

        }

        /*Čekání na uvolnění naloděné posádky (zajišťuje proces kapitána)*/
        sem_wait(sem_waiting_hackers);



        if (isCaptain == true){

            /*Kapitán oznamuje, že se nalodil*/
            sem_wait(sem_log);
              //printf("%d: HACK %d: boards: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
              fprintf(f_output, "%d: HACK %d: boards: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
              counter("Hackers",1,0,0,0);
            sem_post(sem_log);

            *sh_boat_seats += 4;
            *sh_molo_counter -= 4;

            /*Umožní dalším procesům vstoupit na molo*/
            sem_wait(sem_barricade);
              for (int i = 0; i < 4; i++){
                  sem_post(sem_molo);
              }
            sem_post(sem_barricade);

          /*Uspání kapitána*/
          srand(time(0));
          usleep(rand() % (params -> r) * 1000);

          /*Uvolní svoji posádku*/
          sem_wait(sem_barricade);
            for (int j = 0; j < 4; j++){
              sem_post(sem_landing);
            }
          sem_post(sem_barricade);

          /*Čeká, až poslední proces z jeho posádky umožní vylodění kapitána*/
          sem_wait(sem_release_captain);

        }

        /*Procesy čekají, až kapitán uvolní semafor sem_landing a budou se moci vylodit*/
        sem_wait(sem_landing);

        if (isCaptain == true){

          sem_wait(sem_log);
            //printf("%d: HACK %d: captain exits: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
            fprintf(f_output, "%d: HACK %d: captain exits: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
            counter("Hackers",1,0,0,0);
          sem_post(sem_log);

          *sh_boat_seats -= 1;

          *sh_does_captain_exists = 0;

          /*Umožní dalšímu procesu, aby započal nalodění*/
          sem_post(sem_boat_away);

        }else{

            sem_wait(sem_log);
              //printf("%d: HACK %d: member exits: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
              fprintf(f_output, "%d: HACK %d: member exits: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
              counter("Hackers",1,0,0,0);
            sem_post(sem_log);

            sem_wait(sem_barricade);
              *sh_boat_seats -= 1;

              if (*sh_boat_seats == 1){
                sem_post(sem_release_captain);
              }

            sem_post(sem_barricade);

        }
  }

  if (strcmp(group, "Serfs") == 0){

    /*Testuje jestli nebyla překročena kapacita mola,
    v případě přeplnění proces opuští molo a uspí se*/
    if(*sh_molo_counter == params -> c)
    {
          leave_pier("Serfs", id, params, f_output);

    }else{

      sem_wait(sem_molo);

    }

        /*Proces oznamuje, že vstoupil na molo*/
          sem_wait(sem_log);
            counter("Serfs",0,0,0,1);
            //printf("%d: SERF %d: waits: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
            fprintf(f_output, "%d: SERF %d: waits: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
            counter("Serfs",1,1,0,0);
          sem_post(sem_log);

          sem_wait(sem_boat_away); //čekající SERF
          *sh_serfs_waiting_for_boarding += 1;

        /*Kontrola, jestli už není možné vytvořit skupinu pro nalodění*/
        if (*sh_serfs_waiting_for_boarding >= 2 && *sh_hackers_waiting_for_boarding >= 2 && *sh_does_captain_exists == 0){
          *sh_does_captain_exists = 1;
          isCaptain = true;

            *sh_hackers_waiting -= 2;
            *sh_serfs_waiting -= 2;
            *sh_hackers_waiting_for_boarding -= 2;
            *sh_serfs_waiting_for_boarding -= 2;

          start_board("Serfs",2);

        }
        else if(*sh_serfs_waiting_for_boarding >= 4 && *sh_hackers_waiting_for_boarding < 2 && *sh_does_captain_exists == 0){
          *sh_does_captain_exists = 1;
          isCaptain = true;

            *sh_serfs_waiting -= 4;
            *sh_serfs_waiting_for_boarding -= 4;

          start_board("Serfs",4);

        }
        else
        {
          sem_post(sem_boat_away);
        }


        /*Čekání na uvolnění naloděné posádky (zajišťuje proces kapitána)*/
        /*(v závisloti na rozpoložení se uvolní 2 nebo 4 serfs)          */

        sem_wait(sem_waiting_serfs);

        if (isCaptain == true){

            /*Kapitán oznamuje, že se nalodil*/
            sem_wait(sem_log);
              //printf("%d: SERF %d: boards: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
              fprintf(f_output, "%d: SERF %d: boards: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
              counter("Serfs",1,0,0,0);
            sem_post(sem_log);

            *sh_boat_seats += 4;
            *sh_molo_counter -= 4;

            /*Umožní dalším procesům vstoupit na molo*/
            sem_wait(sem_barricade);
              for (int i = 0; i < 4; i++){
                  sem_post(sem_molo);
              }
            sem_post(sem_barricade);

          /*Uspání kapitána*/
          srand(time(0));
          usleep(rand() % (params -> r) * 1000);

          /*Po probuzení uvolní kapitán semafor landing pro velikost své posádky*/
          sem_wait(sem_barricade);
            for (int j = 0; j < 4; j++){
              sem_post(sem_landing);
            }
          sem_post(sem_barricade);

          /*Kapitán čeká, až se celá jeho posádka vylodí a umožní mu odejít*/
          sem_wait(sem_release_captain);

        }

        /*Procesy čekají, až kapitán uvolní semafor sem_landing a budou se moci vylodit*/
        sem_wait(sem_landing);

        /*Kapitán opouští loď jako poslední*/
        if (isCaptain == true){

            sem_wait(sem_log);

              //printf("%d: SERF %d: captain exits: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
              fprintf(f_output, "%d: SERF %d: captain exits: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
              counter("Serfs",1,0,0,0);

            sem_post(sem_log);


          *sh_boat_seats -= 1;

          /*Uvolnění lodě pro nalodění další posádky*/
          *sh_does_captain_exists = 0;
          sem_post(sem_boat_away);

        }else{

            /*Posádka opouští loď*/
            sem_wait(sem_log);

              //printf("%d: SERF %d: member exits: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
              fprintf(f_output, "%d: SERF %d: member exits: %d: %d\n", *sh_action_counter, id, *sh_hackers_waiting, *sh_serfs_waiting);
              counter("Serfs",1,0,0,0);

            sem_post(sem_log);

            /*Kontroluje, jestli už nezbyl v lodi pouze kapitán, případně jej propustí*/
            sem_wait(sem_barricade);
            *sh_boat_seats -= 1;

            if (*sh_boat_seats == 1){
              sem_post(sem_release_captain);
            }
            sem_post(sem_barricade);

        }
  }

  exit(0);

}

/*------------------------------------------------------------------------------*/
/* start_board                                                                  */
/*- Funkce slouží k vyhodnocení typu posádky a uvolnění patřičných semaforů     */
/*------------------------------------------------------------------------------*/
int start_board(char *group, int free)
{

  /*Pro kapitána ze skupiny HACK*/
  if (strcmp(group, "Hackers") == 0){

    /*Uvolní semafory, kde čekají členové jeho posádky (kombinovaná skupina)*/
    if (free == 2){
      for(int i = 0; i < free; i++){
        sem_post(sem_waiting_hackers);
        sem_post(sem_waiting_serfs);
      }
    }

    /*Uvolní semafory, kde čekají členové jeho posádky (pouze členové skupiny HACK)*/
    if (free == 4){
      for(int i = 0; i < free; i++){
        sem_post(sem_waiting_hackers);
      }
    }
  }

  /*Pro kapitána ze skupiny SERF*/
  if (strcmp(group, "Serfs") == 0){

    /*Uvolní semafory, kde čekají členové jeho posádky (kombinovaná skupina)*/
    if (free == 2){
      for(int i = 0; i < free; i++){
        sem_post(sem_waiting_hackers);
        sem_post(sem_waiting_serfs);
      }
    }

    /*Uvolní semafory, kde čekají členové jeho posádky (pouze členové skupiny SERF)*/
    if (free == 4){
      for(int i = 0; i < free; i++){
        sem_post(sem_waiting_serfs);
      }
    }
  }

  return 0;
}

/*-------------------------------------------------------------------------------*/
/* parse_args()                                                                  */
/*- Funkce prochází všechny zadané parametry a ukládá je do struktury parameters */
/*-------------------------------------------------------------------------------*/
int parse_args(int argc, char *argv[], parameters *pms)
{

  char *strtoul_error;

  if (argc == 7)
  {
    /*Zapsání a kontrola parametru P */
    pms -> p = strtol(argv[1], &strtoul_error, 10);
    if (*strtoul_error != 0){
      fprintf(stderr, "Parametr P není číslo!\n");
      return 1;
    }
    else if ( ! ( (pms -> p) % 2 == 0 && (pms -> p) >= 2 ) )
    {
      fprintf(stderr, "Parametr P nesmí odporovat podmínce: P >= 2 && (P %% 2) == 0!\n");
      return 1;
    }

    /*Zapsání a kontrola parametru H */
    pms -> h = strtol(argv[2], &strtoul_error, 10);
    if (*strtoul_error != 0){
      fprintf(stderr, "Parametr H není číslo!\n");
      return 1;
    }
    else if ( ! ( (pms -> h) >= 0 && (pms -> h) <= 2000 ) )
    {
      fprintf(stderr, "Parametr H nesmí odporovat podmínce: H >= 0 && H <= 2000!\n");
      return 1;
    }

    /*Zapsání a kontrola parametru S */
    pms -> s = strtol(argv[3], &strtoul_error, 10);
    if (*strtoul_error != 0){
      fprintf(stderr, "Parametr S není číslo!\n");
      return 1;
    }
    else if ( ! ( (pms -> s) >= 0 && (pms -> s) <= 2000 ) )
    {
      fprintf(stderr, "Parametr S nesmí odporovat podmínce: S >= 0 && S <= 2000!\n");
      return 1;
    }

    /*Zapsání a kontrola parametru R */
    pms -> r = strtol(argv[4], &strtoul_error, 10);
    if (*strtoul_error != 0){
      fprintf(stderr, "Parametr R není číslo!\n");
      return 1;
    }
    else if ( ! ( (pms -> r) >= 0 && (pms -> r) <= 2000 ) )
    {
      fprintf(stderr, "Parametr R nesmí odporovat podmínce: R >= 0 && R <= 2000!\n");
      return 1;
    }

    /*Zapsání a kontrola parametru W */
    pms -> w = strtol(argv[5], &strtoul_error, 10);
    if (*strtoul_error != 0){
      fprintf(stderr, "Parametr W není číslo!\n");
      return 1;
    }
    else if ( ! ( (pms -> w) >= 20 && (pms -> w) <= 2000 ) )
    {
      fprintf(stderr, "Parametr W nesmí odporovat podmínce: W >= 20 && W <= 2000!\n");
      return 1;
    }

    /*Zapsání a kontrola parametru C */
    pms -> c = strtol(argv[6], &strtoul_error, 10);
    if (*strtoul_error != 0){
      fprintf(stderr, "Parametr C není číslo!\n");
      return 1;
    }
    else if ( ! ((pms -> c) >= 5) )
    {
      fprintf(stderr, "Parametr C nesmí odporovat podmínce: C >= 5!\n");
      return 1;
    }

  }
  /*Málo zadaných parametrů*/
  else if (argc < 7)
  {
    fprintf(stderr, "Chybí některý z vyžadovaných argumentů!\n");
    return 1;
  }
  /*Zadáno moc parametrů*/
  else if (argc > 7)
  {
    fprintf(stderr, "Bylo zadáno příliš mnoho argumentů!\n");
    return 1;
  }

  return 0;
}

/*------------------------------------------------------------------------------*/
/* init_resources()                                                             */
/*- Funkce alokuje sdílenou paměť a semafory                                    */
/*------------------------------------------------------------------------------*/
int init_resources(parameters *params)
{

  /*Otevření semaforů*/

  if ( ((sem_molo = sem_open("/xreinm00_semafor_molo", O_CREAT | O_EXCL, 0644, params -> c)) == SEM_FAILED)
    || ((sem_counter_blocker = sem_open("/xreinm00_semafor_counter_blocker", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
    || ((sem_index = sem_open("/xreinm00_semafor_index", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
    || ((sem_waiting_hackers = sem_open("/xreinm00_semafor_waiting_hackers", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
    || ((sem_waiting_serfs = sem_open("/xreinm00_semafor_waiting_serfs", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
    || ((sem_landing = sem_open("/xreinm00_semafor_landing", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
    || ((sem_boat_away = sem_open("/xreinm00_semafor_boat_away", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
    || ((sem_barricade = sem_open("/xreinm00_semafor_barricade", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
    || ((sem_log = sem_open("/xreinm00_semafor_log", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
    || ((sem_release_captain = sem_open("/xreinm00_semafor_release_captain", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED))
    {
      return 1;
    }

  if ( ((shm_0 = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0644)) == -1)
    || ((shm_1 = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0644)) == -1)
    || ((shm_2 = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0644)) == -1)
    || ((shm_3 = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0644)) == -1)
    || ((shm_4 = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0644)) == -1)
    || ((shm_5 = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0644)) == -1)
    || ((shm_6 = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0644)) == -1)
    || ((shm_7 = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0644)) == -1)
    || ((shm_8 = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0644)) == -1)
    || ((shm_9 = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0644)) == -1))
  {
    return 1;
  }

  if ( ((sh_molo_counter = (int *) shmat(shm_0, NULL, 0)) == (void *) -1)        //žátěž mola
    || ((sh_hackers_index = (int *) shmat(shm_1, NULL, 0)) == (void *) -1)       //indexování hackerů
    || ((sh_serfs_index = (int *) shmat(shm_2, NULL, 0)) == (void *) -1)         //indexování serfů
    || ((sh_action_counter = (int *) shmat(shm_3, NULL, 0)) == (void *) -1)      //počítadlo akcí
    || ((sh_hackers_waiting = (int *) shmat(shm_4, NULL, 0)) == (void *) -1)     //počet hackerů čekajících na molu
    || ((sh_serfs_waiting = (int *) shmat(shm_5, NULL, 0)) == (void *) -1)       //počet serfů čekajících na molu
    || ((sh_does_captain_exists = (int *) shmat(shm_6, NULL, 0)) == (void *) -1) //značí jestli již někde existuje kapitán
    || ((sh_boat_seats = (int *) shmat(shm_7, NULL, 0)) == (void *) -1)          //počet pasažérů na lodi
    || ((sh_hackers_waiting_for_boarding = (int *) shmat(shm_8, NULL, 0)) == (void *) -1)  //počet hackerů připravených se nalodit
    || ((sh_serfs_waiting_for_boarding = (int *) shmat(shm_9, NULL, 0)) == (void *) -1))    //počet serfů připravených se nalodit
  {
    return 1;
  }

  return 0;

}

/*------------------------------------------------------------------------------*/
/* release_resources()                                                          */
/*- Funkce uvolní všechny alokované zdroje                                      */
/*------------------------------------------------------------------------------*/
void release_resources(FILE *f_output)
{

  /*Zavře soubor proj2.out*/
  if ((fclose(f_output)) == EOF)
  {
    fprintf(stderr, "Došlo k chybě při zavírání souboru proj2.out!\n");
  }

  /*Zavře semafory*/
  if ( ((sem_close(sem_molo)) == -1)
    || ((sem_close(sem_counter_blocker)) == -1)
    || ((sem_close(sem_index)) == -1)
    || ((sem_close(sem_waiting_hackers)) == -1)
    || ((sem_close(sem_waiting_serfs)) == -1)
    || ((sem_close(sem_landing)) == -1)
    || ((sem_close(sem_boat_away)) == -1)
    || ((sem_close(sem_barricade)) == -1)
    || ((sem_close(sem_log)) == -1)
    || ((sem_close(sem_release_captain)) == -1))
  {
    fprintf(stderr, "Došlo k chybě při zavírání semaforů!\n");
  }

  if ( ((sem_unlink("/xreinm00_semafor_molo")) == -1)
    || ((sem_unlink("/xreinm00_semafor_counter_blocker")) == -1)
    || ((sem_unlink("/xreinm00_semafor_index")) == -1)
    || ((sem_unlink("/xreinm00_semafor_waiting_serfs")) == -1)
    || ((sem_unlink("/xreinm00_semafor_waiting_hackers")) == -1)
    || ((sem_unlink("/xreinm00_semafor_landing")) == -1)
    || ((sem_unlink("/xreinm00_semafor_boat_away")) == -1)
    || ((sem_unlink("/xreinm00_semafor_barricade")) == -1)
    || ((sem_unlink("/xreinm00_semafor_log")) == -1)
    || ((sem_unlink("/xreinm00_semafor_release_captain")) == -1))
  {
    fprintf(stderr, "Došlo k chybě při unlinkování semaforů!\n");
  }

  if ( ((shmctl(shm_0, IPC_RMID, NULL)) == -1)
  || ((shmctl(shm_1, IPC_RMID, NULL)) == -1)
  || ((shmctl(shm_2, IPC_RMID, NULL)) == -1)
  || ((shmctl(shm_3, IPC_RMID, NULL)) == -1)
  || ((shmctl(shm_4, IPC_RMID, NULL)) == -1)
  || ((shmctl(shm_5, IPC_RMID, NULL)) == -1)
  || ((shmctl(shm_6, IPC_RMID, NULL)) == -1)
  || ((shmctl(shm_7, IPC_RMID, NULL)) == -1)
  || ((shmctl(shm_8, IPC_RMID, NULL)) == -1)
  || ((shmctl(shm_9, IPC_RMID, NULL)) == -1))
  {
    fprintf(stderr, "Došlo k chybě při uvolňování sdílené paměti!\n");
  }

  if ( ((shmdt(sh_molo_counter)) == -1)
  || ((shmdt(sh_hackers_index)) == -1)
  || ((shmdt(sh_serfs_index)) == -1)
  || ((shmdt(sh_serfs_waiting)) == -1)
  || ((shmdt(sh_hackers_waiting)) == -1)
  || ((shmdt(sh_action_counter)) == -1)
  || ((shmdt(sh_does_captain_exists)) == -1)
  || ((shmdt(sh_boat_seats)) == -1)
  || ((shmdt(sh_hackers_waiting_for_boarding)) == -1)
  || ((shmdt(sh_serfs_waiting_for_boarding)) == -1))
  {
    fprintf(stderr, "Došlo k chybě při uvolňování sdílené paměti!\n");
  }

}

/*------------------------------------------------------------------------------*/
/* print_help()                                                                 */
/*- Funkce pro vypsání pomoci při špatném zadání parametrů                      */
/*------------------------------------------------------------------------------*/
void print_help(){
  fprintf(stderr, "\nParametry prosím zadávejte následovně v tomto pořadí: P H S R W C\n");
  fprintf(stderr, "Přičemž,\n");
  fprintf(stderr, "P volte v rozmezí: P >= 2 && (P %% 2) == 0\n");
  fprintf(stderr, "H volte v rozmezí: H >= 0 && H <= 2000\n");
  fprintf(stderr, "S volte v rozmezí: S >= 0 && S <= 2000\n");
  fprintf(stderr, "R volte v rozmezí: R >= 0 && R <= 2000\n");
  fprintf(stderr, "W volte v rozmezí: W >= 20 && W <= 2000\n");
  fprintf(stderr, "C volte v rozmezí: C >= 5\n");
}
