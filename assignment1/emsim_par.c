#include "emsim.h"
#include <pthread.h>

//GLOBALS
int g_index;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
typedef struct
{
	int groupNo;
	team_t teams1;
	team_t teams2;
	int goals1;
  	int goals2;
}t_args;

void function parallel_pGroups(void *ptr)
{
	struct t_args *m_args = ptr;
	int l_index; 
  	while (True){
		pthread_mutex_lock(mutex);
		if (g_index == 36){
			pthread_mutex_unlock(&mutex);
			return NULL;
		}

		l_index = g_index;
		g_index++;
		pthread_mutex_unlock(&mutex);
		playGroupMatch(m_args[l_index].groupNo, m_args[l_index].team1 , m_args[l_index].team2, &(m_args[l_index].goals1), &(m_args[l_index].goals2));
		team1.goals += goals1 - goals2;
        	teams2.goals += goals2 - goals1;
        	if (goals1 > goals2)
          		teams1.points += 3;
        	else if (goals1 < goals2)
          		teams2.points += 3;
       		else {
          		teams1.points += 1;
          		teams2.points += 1;
        	}

    	}
	return NULL;
}

void playGroups(team_t* teams, int numWorker)
{
	static const int cNumTeamsPerGroup = NUMTEAMS / NUMGROUPS;
        int g, i, j, goalsI, goalsJ;
	numWorker++;
	pthread_t thread_pg;
	thread_pg = malloc(numWorker * sizeof(*thread_pg));
	/* Thread args */
        t_args *thread_args = (t_args*) malloc(36 * sizeof(*thread_args));
  	g_index = 0;
	int s_count = 0;
	for (g = 0; g < NUMGROUPS; ++g) {
    	  for (i =  g * cNumTeamsPerGroup; i < (g+1) * cNumTeamsPerGroup; ++i) {
      		for (j = (g+1) * cNumTeamsPerGroup - 1; j > i; --j) {
			//initializing structure for 36 matchs
			
			thread_args[s_count].groupNo = g;
			thread_args[s_count].team1 = teams + i;
			thread_args[s_count].team2 = teams + j;
			thread_args[s_count].goals1 = goalsI;
			thread_args[s_count].goals2 = goalsJ;
			s_count++;
      		 }
    	  }
  	}


  	// assign the arguments
  	for (t = 0; t < numWorker; ++t) {
   	  pthread_create(thread_pg+t, NULL, &parallel_pGroups, thread_args);
  	}

  	for (i = 0; i < numWorker; i++) {
    	  pthread_join(thread_pg[i], NULL);
  	}
	free(thread_pg);
        free (thread_args);
}

void playFinalRound(int numGames, team_t** teams, team_t** successors, int numWorker)
{
 //printf("number of workers in Final round= %d\n",numWorker);

	team_t* team1;
  team_t* team2;
  int i, goals1 = 0, goals2 = 0;

  for (i = 0; i < numGames; ++i) {
    team1 = teams[i*2];
    team2 = teams[i*2+1];
    playFinalMatch(numGames, i, team1, team2, &goals1, &goals2);

    if (goals1 > goals2)
      successors[i] = team1;
    else if (goals1 < goals2)
      successors[i] = team2;
    else {
      playPenalty(team1, team2, &goals1, &goals2);
      if (goals1 > goals2)
        successors[i] = team1;
      else
        successors[i] = team2;
    }
  }
} 


