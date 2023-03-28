#include <stdio.h>
#include "sqlite/sqlite3.h"

int main(int argc, char **argv) {
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(argv[1], &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    sqlite3_stmt *stmt;
    const char *sql = "SELECT name, sql FROM sqlite_master WHERE type='table'";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *table_name = (const char *) sqlite3_column_text(stmt, 0);
        const char *table_schema = (const char *) sqlite3_column_text(stmt, 1);
        printf("Table: %s\n", table_name);
        printf("Schema: %s\n", table_schema);

        const char *select_sql = sqlite3_mprintf("SELECT * FROM %Q", table_name);
        sqlite3_stmt *select_stmt;
        rc = sqlite3_prepare_v2(db, select_sql, -1, &select_stmt, 0);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return 1;
        }

        int num_cols = sqlite3_column_count(select_stmt);

        for (int i = 0; i < num_cols; i++) {
            const char *col_name = sqlite3_column_name(select_stmt, i);
            printf("%s\t", col_name);
        }

        printf("\n");

        while (sqlite3_step(select_stmt) == SQLITE_ROW) {
            for (int i = 0; i < num_cols; i++) {
                const char *col_value = (const char *) sqlite3_column_text(select_stmt, i);
                printf("%s\t", col_value);
            }

            printf("\n");
        }

        sqlite3_finalize(select_stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    printf("Conversion successful\n");

    return 0;
}
