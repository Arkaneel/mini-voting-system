#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

#define DB_FILE "votes.db"
#define CANDIDATES_COUNT 3

struct Candidate {
    int id;
    char name[50];
};

struct Candidate candidates[CANDIDATES_COUNT] = {
    {1, "Candidate 1"},
    {2, "Candidate 2"},
    {3, "Candidate 3"}
};

void print_candidates() {
    for (int i = 0; i < CANDIDATES_COUNT; i++) {
        printf("%d. %s\n", candidates[i].id, candidates[i].name);
    }
}

int main() {
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(DB_FILE, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    char *create_table_sql = "CREATE TABLE IF NOT EXISTS votes(candidate_id INT);";
    rc = sqlite3_exec(db, create_table_sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    printf("Please choose a candidate by entering a number:\n");
    print_candidates();

    int chosen_candidate_id;
    scanf("%d", &chosen_candidate_id);
    int chosen_candidate_index = -1;
    for (int i = 0; i < CANDIDATES_COUNT; i++) {
        if (candidates[i].id == chosen_candidate_id) {
            chosen_candidate_index = i;
            break;
        }
    }

    if (chosen_candidate_index == -1) {
        printf("Invalid candidate ID.\n");
        sqlite3_close(db);
        return 1;
    }

    char *insert_vote_sql;
    asprintf(&insert_vote_sql, "INSERT INTO votes(candidate_id) VALUES(%d);", chosen_candidate_id);
    rc = sqlite3_exec(db, insert_vote_sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    printf("Your vote has been recorded. Thank you for voting!\n");

    sqlite3_close(db);
    return 0;
}
