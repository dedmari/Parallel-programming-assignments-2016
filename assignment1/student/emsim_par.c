#include "emsim.h"
#include <pthread.h>


struct groupDetails
{
	int groupMin;
	int groupMax;
	int numSuccessors;
  	int teamsPerGroup;
	team_t* teams;
	team_t** successors; //check it
	team_t** bestThirds; // check it
	char padding[64];
	
};

void* groupInterface(void *str)
{
	struct groupDetails *grpstrcut = str;
	int g;
	for (g = grpstrcut->groupMin; g <grpstrcut->groupMax; g++) {
	playGroup(g,grpstrcut->teams + (g * grpstrcut->teamsPerGroup),
              grpstrcut->teamsPerGroup,
              grpstrcut->successors + g * 2,
              grpstrcut->successors + (grpstrcut->numSuccessors - (g * 2) - 1),grpstrcut->bestThirds + g);
}
return NULL;
}


void playEM(team_t* teams, int numThreads)
{
	//printf("%d\n",teams->id);
	  static const int cInitialNumSuccessors = NUMGROUPS * 2 + NUMTHIRDS;
	  static const int cTeamsPerGroup = NUMTEAMS / NUMGROUPS;
	  team_t* successors[NUMGROUPS * 2 + NUMTHIRDS] = {0};
	  team_t* bestThirds[NUMGROUPS];
	  
	  int numSuccessors = cInitialNumSuccessors;
	  int	curBestThird = 0;
	  int g=0;
	  int groupsPerThread = NUMGROUPS / numThreads;
	  int groupRest       = NUMGROUPS % numThreads;
	
  struct groupDetails *gx;
  gx = malloc(numThreads * sizeof(*gx));
	pthread_t *thread_gr;
	thread_gr = malloc(numThreads * sizeof(*thread_gr));

  // play groups
  initialize();

	
	
  for (int t = 0; t < numThreads; ++t) {
	
    gx[t].groupMin = g;
    g += groupsPerThread;
    if(groupRest > 0)
	{
		g++;
		--groupRest;
	}
     gx[t].groupMax = g;
     gx[t].teams = teams;
     gx[t].numSuccessors = numSuccessors;
     gx[t].teamsPerGroup = cTeamsPerGroup;
     gx[t].successors = successors;
     gx[t].bestThirds = bestThirds;
    
    pthread_create(thread_gr+t,NULL,&groupInterface,gx+t);
	
  }
  for (int t = 0; t < numThreads; ++t) {
	pthread_join(thread_gr[t],NULL);
  }
  free(thread_gr);
  free (gx);
  // fill best thirds
  sortTeams(NUMGROUPS, bestThirds);
  for (g = 0; g < numSuccessors; ++g) {
    if (successors[g] == NULL) {
      successors[g] = bestThirds[curBestThird++];
    }
  }
	
  // play final rounds
  while (numSuccessors > 1) {
    playFinalRound(numSuccessors / 2, successors, successors);
    numSuccessors /= 2;
  }
}
	struct pthread_args_match{
		int numGames;
		int gameNo;
		team_t* team1;
		team_t* team2;
		int goal1;
		int goal2;
		char padding[64];
	};

	void* playMatchInPar(void* ptr) {
		struct pthread_args_match *args = (ptr);
		playFinalMatch(args->numGames, args->gameNo,
		args->team1, args->team2, &(args->goal1), &(args->goal2));
		return
		NULL;
	}

void playFinalRound(int numGames, team_t** teams, team_t** successors1)
{
  //team_t* team1;
  //team_t* team2;
  int i;
  pthread_t *threads;
  threads = malloc(numGames * sizeof(*threads));
  struct pthread_args_match *args;
  args = malloc(numGames * sizeof(*args));

  for (i = 0; i < numGames; ++i) {
    args[i].numGames = numGames;
    args[i].team1 = teams[i*2];
    args[i].team2 = teams[i*2+1];
    args[i].goal1 = 0;
    args[i].goal2 = 0;
    args[i].gameNo = i;
    pthread_create(threads+i,NULL,&playMatchInPar,args+i);
    
  }
	for(i = 0; i < numGames; ++i){
	pthread_join(threads[i],NULL);
       }
  for (i = 0; i < numGames; ++i) {	
    if (args[i].goal1 > args[i].goal2)
      successors1[i] = args[i].team1;
    else if (args[i].goal1 < args[i].goal2)
      successors1[i] = args[i].team2;
    else
    {
      playPenalty(args[i].team1, args[i].team2, &args[i].goal1, &args[i].goal2);
      if (args[i].goal1 > args[i].goal1)
        successors1[i] = args[i].team1;
      else
        successors1[i] = args[i].team2;
    }
  }
	
	free(threads);
	free(args);
}
