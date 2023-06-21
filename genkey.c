#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sqlite3.h>

#include "config.h"

void genkey(char **key)
{
    *key = malloc(KEY_SIZE + 1);
    (*key)[KEY_SIZE] = '\0';

    for (int i = 0; i < KEY_SIZE; i++) {
        (*key)[i] = VALID_CHARS[rand() % sizeof(VALID_CHARS)];
    }
}

int main(int argc, char **argv)
{
    if (argc == 1) {
        printf("usage: %s <user>\n", argv[0]);

        return EXIT_FAILURE;
    }
    int ret = EXIT_SUCCESS;

    struct sqlite3 *db;
    int ec = sqlite3_open("sush.sqlite3", &db);
    if (ec != SQLITE_OK) {
        fprintf(stderr, "Failed to open SQLite database: %i\n", ec);

        return EXIT_FAILURE;
    }

    srand(time(NULL));

    char *user = argv[1];
    char *key;
    genkey(&key);

    struct sqlite3_stmt *st;
    sqlite3_prepare_v2(db, "INSERT INTO " KEY_TABLE " VALUES (?, ?)", -1, &st, NULL);
    sqlite3_bind_text(st, 1, user, strlen(user), SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, key, KEY_SIZE, SQLITE_TRANSIENT);
    ec = sqlite3_step(st);
    if (ec != SQLITE_DONE) {
        fprintf(stderr, "SQLite error: %i\n", ec);
        ret = EXIT_FAILURE;
    } else {
        printf("%s\n", key);
    }

    sqlite3_finalize(st);
    sqlite3_close(db);
    free(key);
    return ret;
}

