#include "emsim.h"
#include <omp.h>

void playGroups(team_t* teams)
{
 static const int cNumTeamsPerGroup = NUMTEAMS / NUMGROUPS;
  int g;
#pragma omp parallel
  {
     #pragma omp for schedule(dynamic)
	  for (g = 0; g < NUMGROUPS; ++g) {
	    for (int i =  g * cNumTeamsPerGroup; i < (g+1) * cNumTeamsPerGroup; ++i) {
	      for (int j = (g+1) * cNumTeamsPerGroup - 1; j > i; --j) {
			int goalsI=0, goalsJ=0;
		// team i plays against team j in group g
		playGroupMatch(g, teams + i, teams + j, &goalsI, &goalsJ);
		teams[i].goals += goalsI - goalsJ;
		teams[j].goals += goalsJ - goalsI;
		if (goalsI > goalsJ)
		  teams[i].points += 3;
		else if (goalsI < goalsJ)
		  teams[j].points += 3;
		else {
		  teams[i].points += 1;
		  teams[j].points += 1;
		}
	      }
	    }
	  }
    }
}

void playFinalRound(int numGames, team_t** teams, team_t** successors)
{
  int i;
  #pragma omp parallel
  {
	#pragma omp for schedule(dynamic)

	  for (i = 0; i < numGames; ++i) {
	    int goals1 = 0, goals2 = 0;
	    team_t* team1;
	    team_t* team2;
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
} 
