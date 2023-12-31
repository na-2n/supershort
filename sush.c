#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <microhttpd.h>
#include <sqlite3.h>

#include "config.h"

int countch(const char *str, const char ch)
{
    const char *tmp = str;
    int cnt = 0;

    for (; *tmp != '\0'; tmp++) {
        if (*tmp == ch) {
            cnt++;
        }
    }

    return cnt;
}

int urlcmp(const char *url, const char *cmp)
{
    char *tmp = (char *)url;

    for (; *tmp != '\0'; tmp++) {
        if (*tmp == '?') {
            *tmp = '\0';
            break;
        }
    }

    int ret = strcmp(url, cmp);
    *tmp = '?';

    return ret;
}

int db_checkid(struct sqlite3 *db, const char *id)
{
    struct sqlite3_stmt *st = NULL;

    sqlite3_prepare_v2(db, "SELECT 1 FROM " URL_TABLE " WHERE id = ?", -1, &st, NULL);
    sqlite3_bind_text(st, 1, id, strlen(id), SQLITE_TRANSIENT);
    int ret = sqlite3_step(st) == SQLITE_ROW;
    sqlite3_finalize(st);

    return ret;
}

int db_checkkey(struct sqlite3 *db, const char *key)
{
    struct sqlite3_stmt *st = NULL;

    sqlite3_prepare_v2(db, "SELECT 1 FROM " KEY_TABLE " WHERE key = ?", -1, &st, NULL);
    sqlite3_bind_text(st, 1, key, strlen(key), SQLITE_TRANSIENT);
    int ret = sqlite3_step(st) == SQLITE_ROW;
    sqlite3_finalize(st);

    return ret;
}

int simple_response(struct MHD_Connection *conn, unsigned int status, size_t size, void *data)
{
    if (conn == NULL) {
        fprintf(stderr, "[ERR] simple_respones: connection is null\n");
        return MHD_NO;
    }

    struct MHD_Response *resp = MHD_create_response_from_buffer(size, data, MHD_RESPMEM_PERSISTENT);
    if (resp == NULL) {
        fprintf(stderr, "[ERR] simple_response: failed to create response\n");
    }

    int ret = MHD_queue_response(conn, status, resp);
    if (ret == MHD_NO) {
        fprintf(stderr, "[ERR] simple_response: failed to queue response\n");
    }

    MHD_destroy_response(resp);
    return ret;
}
#define blank_response(conn, status) simple_response(conn, status, 0, NULL);

static enum MHD_Result sush_handler(void *cls,
                                    struct MHD_Connection *conn,
                                    const char *url,
                                    const char *method,
                                    const char *version,
                                    const char *data,
                                    size_t *data_size,
                                    void **ptr)
{
    static int dummy;
    struct sqlite3 *db = (sqlite3 *)cls;
    struct sqlite3_stmt *st = NULL;

    if (&dummy != *ptr) {
        *ptr = &dummy;

        return MHD_YES;
    }
    *ptr = NULL;

    struct MHD_Response *resp;
    int ret;

#if DEBUG
    printf("[DBG] %s\n", url);
#endif

    if (strcmp(method, "POST") == 0) {
        // POSTing

        if (*data_size == 0) {
            return blank_response(conn, MHD_HTTP_BAD_REQUEST);
        }

        // TODO: figure out why we can't queue responses from within this 'if' block
        if (strcmp(url, "/s") == 0) {
            if (*data_size > MAX_DATA_SIZE) {
                return blank_response(conn, MHD_HTTP_CONTENT_TOO_LARGE);
            }

            const char *auth = MHD_lookup_connection_value(conn, MHD_HEADER_KIND, "Authorization");
            if (auth == NULL) {
#if DEBUG
                fprintf(stderr, "[DBG] client did not set auth header\n");
#endif
                return blank_response(conn, MHD_HTTP_UNAUTHORIZED);
            }

            if (!db_checkkey(db, auth)) {
#if DEBUG
                fprintf(stderr, "[DBG] client tried to use unknown key!\n");
#endif
                return blank_response(conn, MHD_HTTP_FORBIDDEN);
            }

            char *instr = malloc(*data_size + 1);
            memcpy(instr, data, *data_size);
            instr[*data_size] = '\0';

            char id[ID_SIZE + 1];
            int attempts = 0;
            do {
                if (attempts > ID_MAX_ATTEMPTS) {
                    fprintf(stderr, "[ERR] Maximum ID attempts reached! Are we out of possible IDs?");
                    return blank_response(conn, MHD_HTTP_INTERNAL_SERVER_ERROR);
                }

                for (int i = 0; i < ID_SIZE; i++) {
                    id[i] = VALID_CHARS[rand() % sizeof(VALID_CHARS)];
                }
                id[ID_SIZE] = '\0';
                attempts++;
            } while (db_checkid(db, id));

            sqlite3_prepare_v2(db, "INSERT INTO " URL_TABLE " VALUES (?, ?)", -1, &st, NULL);
            sqlite3_bind_text(st, 1, id, ID_SIZE, SQLITE_TRANSIENT);
            sqlite3_bind_text(st, 2, instr, *data_size, SQLITE_TRANSIENT);
            if (sqlite3_step(st) == SQLITE_DONE) {
                printf("[NEW] %s -> %s\n", id, instr);

                resp = MHD_create_response_from_buffer(ID_SIZE, id, MHD_RESPMEM_PERSISTENT);
                ret = MHD_queue_response(conn, MHD_HTTP_OK, resp);
            } else {
                fprintf(stderr, "[ERR] SQLite error @ %s -> %s\n", id, instr);

                const char *err = "SQLite error";
                resp = MHD_create_response_from_buffer(strlen(err), (void *)err, MHD_RESPMEM_PERSISTENT);
                ret = MHD_queue_response(conn, MHD_HTTP_INTERNAL_SERVER_ERROR, resp);
            }

            MHD_destroy_response(resp);
            sqlite3_finalize(st);
            free(instr);

            return ret;
        }
    } else if (strcmp(method, "GET") == 0) {
        // GETting

        if (*data_size != 0) {
            return blank_response(conn, MHD_HTTP_BAD_REQUEST);
        }

        if (strcmp(url, "/") == 0) {
            resp = MHD_create_response_from_buffer(strlen(HOME_PAGE), (void *)HOME_PAGE, MHD_RESPMEM_PERSISTENT);
            ret = MHD_queue_response(conn, MHD_HTTP_OK, resp);
            MHD_destroy_response(resp);

            return ret;
        } else {
            int cnt = countch(url, '/');
            if (cnt == 1 || (cnt == 2 && url[strlen(url)-1] == '/')) {
                char *part = malloc(strlen(url));
                strcpy(part, url+1);

                if (part[strlen(part)-1] == '/') {
                    part[strlen(part)-1] = '\0';
                }

                sqlite3_prepare_v2(db, "SELECT url FROM " URL_TABLE " WHERE id = ?", -1, &st, NULL);
                sqlite3_bind_text(st, 1, part, strlen(part), SQLITE_TRANSIENT);
                int _c = sqlite3_step(st);
                if (_c == SQLITE_ROW) {
                    const unsigned char *redir_url = sqlite3_column_text(st, 0);

#if DEBUG
                    printf("[DBG] found: /%s -> %s\n", part, redir_url);
#endif

                    resp = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
                    MHD_add_response_header(resp, "Location", (const char *)redir_url);
                    ret = MHD_queue_response(conn, MHD_HTTP_TEMPORARY_REDIRECT, resp);

                    MHD_destroy_response(resp);
                    sqlite3_finalize(st);
                    free(part);

                    return ret;
                }

#if DEBUG
                fprintf(stderr, "[DBG] not found (SQLi:%i): /%s\n", _c, part);
#endif

                sqlite3_finalize(st);
                free(part);
            }
        }
    }

    resp = MHD_create_response_from_buffer(strlen(NOT_FOUND_PAGE), (void*)NOT_FOUND_PAGE, MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(conn, MHD_HTTP_NOT_FOUND, resp);
    MHD_destroy_response(resp);

    return ret;
}

int main(int argc, char **argv)
{
    struct sqlite3 *db;
    int ec = sqlite3_open("sush.sqlite3", &db);
    if (ec != SQLITE_OK) {
        fprintf(stderr, "Failed to open SQLite database: %i\n", ec);

        return EXIT_FAILURE;
    }

    srand(time(NULL));

    struct MHD_Daemon *d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
                                            PORT,
                                            NULL,
                                            NULL,
                                            &sush_handler,
                                            db,
                                            MHD_OPTION_END);
    if (d == NULL) {
        return EXIT_FAILURE;
    }

    getc(stdin);
    MHD_stop_daemon(d);
    sqlite3_close(db);

    return EXIT_SUCCESS;
}
