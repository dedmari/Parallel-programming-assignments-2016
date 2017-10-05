#include "emsim.h"
#include <pthread.h>

//GLOBALS
int g_index;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//global variables for final round
int fr_index;
pthread_mutex_t mutex_fr = PTHREAD_MUTEX_INITIALIZER;
team_t** successors_fr;
typedef struct
{
	int groupNo;
	team_t* teams1;
	team_t* teams2;
	int goals1;
  	int goals2;
	//int i;
	//int j;
}t_args;


void* pGroups(void *ptr)
{
	t_args *m_args = ptr;
	int l_index; 
  	while (1){
		pthread_mutex_lock(&mutex);
		if (g_index == 36){
			pthread_mutex_unlock(&mutex);
			break;
		}

		l_index = g_index;
		g_index++;
		pthread_mutex_unlock(&mutex);
		playGroupMatch(m_args[l_index].groupNo, m_args[l_index].teams1 , m_args[l_index].teams2, &(m_args[l_index].goals1), &(m_args[l_index].goals2));
		m_args[l_index].teams1[0].goals += m_args[l_index].goals1 - m_args[l_index].goals2;
        	m_args[l_index].teams2[0].goals += m_args[l_index].goals2 - m_args[l_index].goals1;
        	if (m_args[l_index].goals1 > m_args[l_index].goals2)
          		m_args[l_index].teams1[0].points += 3;
        	else if (m_args[l_index].goals1 < m_args[l_index].goals2)
          		m_args[l_index].teams2[0].points += 3;
       		else {
          		m_args[l_index].teams1[0].points += 1;
          		m_args[l_index].teams2[0].points += 1;
        	}
		
    	}
	return NULL;
}

void playGroups(team_t* teams, int numWorker)
{
	static const int cNumTeamsPerGroup = NUMTEAMS / NUMGROUPS;
        int g, i, j, goalsI=0, goalsJ=0;
	numWorker++;
	pthread_t *thread_pg;
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
			thread_args[s_count].teams1 = teams + i;
			thread_args[s_count].teams2 = teams + j;
			//thread_args[s_count].i = i;
			//thread_args[s_count].j = j;
			thread_args[s_count].goals1 = goalsI;
			thread_args[s_count].goals2 = goalsJ;

			s_count++;
      		 }
    	  }
  	}


  	// assign the arguments
  	for (int t = 0; t < numWorker; ++t) {
		pthread_create(thread_pg+t,NULL,&pGroups,thread_args);
  	}

  	for (int x = 0; x < numWorker; x++) {
    	  pthread_join(thread_pg[x], NULL);
  	}
	free(thread_pg);
        free (thread_args);
}


//code for final round
typedef struct
{
	int numGames;
	team_t* team1;
	team_t* team2;
	int goals1;
  	int goals2;
	team_t* successors;
        int i;
}fr_args;

void* fGroups(void *ptr)
{
	fr_args *m_args = ptr;
	int l_index=0; 
	while (1){
		pthread_mutex_lock(&mutex_fr);
		if (fr_index == m_args[0].numGames){
			pthread_mutex_unlock(&mutex_fr);
			break;
		}
		l_index = fr_index;
		fr_index++;
		
		pthread_mutex_unlock(&mutex_fr);

		    playFinalMatch(m_args[l_index].numGames, m_args[l_index].i, m_args[l_index].team1, m_args[l_index].team2, &(m_args[l_index].goals1), &(m_args[l_index].goals2));
		    if (m_args[l_index].goals1 > m_args[l_index].goals2)
		      successors_fr[m_args[l_index].i] = m_args[l_index].team1;
		    else if (m_args[l_index].goals1 < m_args[l_index].goals2)
		      successors_fr[m_args[l_index].i]= m_args[l_index].team2;
		    else 
		   {
		      playPenalty(m_args[l_index].team1, m_args[l_index].team2, &(m_args[l_index].goals1), &(m_args[l_index].goals2));
		      if (m_args[l_index].goals1 > m_args[l_index].goals2)
			successors_fr[m_args[l_index].i] = m_args[l_index].team1;
		      else
			successors_fr[m_args[l_index].i] = m_args[l_index].team2;
		    }
	}
	
        return NULL;
}

pthread_t *thread_pf;


void playFinalRound(int numGames, team_t** teams, team_t** successors, int numWorker)
{
         successors_fr = successors;
       	 
	 fr_args *thread_args = (fr_args*) malloc(numGames * sizeof(*thread_args));
	 int i, goals1 = 0, goals2 = 0;
	if(numGames == 8){
	  thread_pf = malloc(2 * sizeof(*thread_pf));
           
         }
	 for (i = 0; i < numGames; ++i) {
	    thread_args[i].team1 = teams[i*2];
	    thread_args[i].team2 = teams[i*2+1];
	    thread_args[i].successors = successors[i];
	    thread_args[i].goals1 = goals1;
	    thread_args[i].goals2 = goals2;
	    thread_args[i].numGames = numGames;
	    thread_args[i].i = i;		

	  }
	
	
	  	
        for (int t = 0; t < 2; ++t) {
			pthread_create(thread_pf+t,NULL,&fGroups,thread_args);
        }
        for (int x = 0; x < 2; x++) {
    	  	pthread_join(thread_pf[x], NULL);
  	}
	
  	
        if(numGames == 1)
	   free(thread_pf);
        free (thread_args);
        //pthread_mutex_destroy(&mutex_fr);
	fr_index=0;
        
} 


