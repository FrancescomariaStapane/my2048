#include  "storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "game.h"
#include "renderer.h"

sqlite3 *db;
// #define TABLE "LEADERBOARD"
static int printQueryResult(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i = 0; i<argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

static int loadBoardState(void *userState, int argc, char **argv, char **azColName) {
    if (argc != 1 || !argv[0])
        return -1;
    strToBoardState(&((UserState*)userState)->state, argv[0]);
    return 0;
}

static int copyCheck(void *check, int argc, char **argv, char **azColName) {
    if (argc != 1 || !argv[0])
        return -1;
    *(bool*)check = !strcmp(argv[0], "1");
    return 0;
}

static int loadLeaderboard(void *leaderboard, int argc, char **argv, char **azColName) {
    Leaderboard* lb = leaderboard;
    if (lb->iterator > lb-> nOfUsers || argc != 3 || !argv[0] || !argv[1] || !argv[2])
        return -1;
    strcpy(lb->users[lb->iterator].username, argv[0]);
    lb->users[lb->iterator].bestScore = atoi(argv[1]);
    lb->users[lb->iterator].bestTile = atoi(argv[2]);
    lb->iterator++;
    return 0;
}

static int initLeaderBoard(void *leaderBoard, int argc, char **argv, char **azColName) {
    if (argc != 1 || !argv[0])
        return -1;
    Leaderboard *lb = leaderBoard;
    lb -> nOfUsers = atoi(argv[0]);
    lb -> iterator = 0;
    lb -> users = malloc(lb -> nOfUsers * sizeof(UserHighScore));
    return 0;
}

static int loadPersonalBest(void *bestState, int argc, char **argv, char **azColName) {
    if (argc != 2 || !argv[0] || !argv[1])
        return -1;
    UserState *bs = bestState;
    bs->bestScore = atoi(argv[0]);
    bs->bestTile = atoi(argv[1]);
    return 0;
}


int openDb() {
    char *zErrMsg = 0;
    int rc = 0;
    char workingDir[4096];
    getWorkingDir(workingDir);
    char* dbFile = malloc(sizeof(char) * ( strlen(workingDir) + 16));
    strcpy(dbFile,workingDir);
    strcat(dbFile,"/users.db");
    rc = sqlite3_open(dbFile, &db);
    free(dbFile);
    if( rc != SQLITE_OK ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    char* sql = "CREATE TABLE IF NOT EXISTS LEADERBOARD("  \
      "NAME TEXT PRIMARY KEY   NOT NULL," \
      "BOARD_STATE            TEXT     NOT NULL," \
      "BEST_SCORE        INT," \
      "BEST_TILE         INT );";
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr,("%s\n"),zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    return 0;
}
void closeDb() {
    sqlite3_close(db);
}

int addUser(UserState user) {
    char sql[4096];
    char *zErrMsg = 0;
    int rc = 0;
    char* boardValues = malloc (sizeof(char)* 2 * user.state.n_cols * user.state.n_rows);
    boardStateToStr(user.state, boardValues);
    sprintf(sql, "INSERT INTO LEADERBOARD (NAME,BOARD_STATE,BEST_SCORE,BEST_TILE) "\
        "VALUES ('%s', '%s', %d, %d); ", user.username, "4;4;0;2;0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0", user.bestScore, user.bestTile);
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr,("%s\n"),zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    free(boardValues);
    return 0;
}

int updateBest(UserState state) {
    char sql[8192];
    char *zErrMsg = 0;
    int rc = 0;
    sprintf(sql, "UPDATE LEADERBOARD"\
        " SET BEST_SCORE = %d, BEST_TILE = %d WHERE NAME = '%s'; ",state.bestScore, state.bestTile, state.username);
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr,("%s\n"),zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    return 0;
}

int updateBoardState(UserState userState) {
    char sql[8192];
    char buf[4096];
    char *zErrMsg = 0;
    int rc = 0;
    boardStateToStr(userState.state,buf);
    sprintf(sql, "UPDATE LEADERBOARD"\
        " SET BOARD_STATE = '%s' WHERE NAME = '%s'; ", buf, userState.username);
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr,("%s\n"),zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    return 0;
}

int getSavedBoardState(UserState* state) {
    char sql[4096];
    char *zErrMsg = 0;
    int rc = 0;
    sprintf(sql, "SELECT BOARD_STATE FROM LEADERBOARD WHERE NAME = '%s';", state->username);
    rc = sqlite3_exec(db, sql, loadBoardState, state, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr,("%s\n"),zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    return 0;
}

int getPersonalBest(char* username, int* bestScore, int* bestTile) {
    char sql[4096];
    sprintf(sql, "SELECT BEST_SCORE,BEST_TILE FROM LEADERBOARD WHERE NAME ='%s';",username);
    char *zErrMsg = 0;
    int rc = 0;
    zErrMsg = 0;
    UserState bestState;
    rc = sqlite3_exec(db, sql, loadPersonalBest, &bestState, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr,("%s\n"),zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    *bestScore = bestState.bestScore;
    *bestTile = bestState.bestTile;
    return 0;
}
int getLeaderBoard(Leaderboard* leaderboard) {
    char *sql = "SELECT COUNT(*) FROM LEADERBOARD;";
    char *zErrMsg = 0;
    int rc = 0;
    rc = sqlite3_exec(db, sql, initLeaderBoard, leaderboard, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr,("%s\n"),zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    sql = "SELECT NAME,BEST_SCORE,BEST_TILE FROM LEADERBOARD ORDER BY BEST_SCORE DESC;";
    zErrMsg = 0;
    rc = sqlite3_exec(db, sql, loadLeaderboard, leaderboard, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr,("%s\n"),zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    return 0;
}

int checkUserExists(char* username, bool* check) {
    char sql[4096];
    sprintf(sql, "SELECT COUNT(*) FROM LEADERBOARD WHERE NAME = '%s';", username);
    char *zErrMsg = 0;
    int rc = 0;
    rc = sqlite3_exec(db, sql, copyCheck, check, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr,("%s\n"),zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    return 0;
}

// int main(int argc, char* argv[]) {
//
//     BoardState state;
//     newBoardState(&state,3,5);
//     initGame(&state);
//     char buf[4096];
//     boardStateToStr(state,buf);
//     printf("%s\n",buf);
// }