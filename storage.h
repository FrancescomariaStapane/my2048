#ifndef LIBSTORAGE_STORAGE_H
#define LIBSTORAGE_STORAGE_H

#include <sqlite3.h>

#include "game.h"

int openDb();
void closeDb();
int addUser(UserState user);
int updateBest(UserState state);
int updateBoardState(UserState userState);
int getSavedBoardState(UserState* state);
int getLeaderBoard(Leaderboard* leaderboard);
int checkUserExists(char* username, bool* check);


#endif


