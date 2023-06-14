#ifndef __CONFIG_H__
#define __CONFIG_H__

#define HOME_PAGE \
    "<html>" \
        "<head>" \
            "<title>Home</title>" \
        "</head>" \
        "<body>" \
            "<p>Hello world!</p>" \
        "</body>" \
    "</html>"

#define NOT_FOUND_PAGE \
    "<html>" \
        "<head>" \
            "<title>404 Not Found</title>" \
        "</head>" \
        "<body>" \
            "<p>404 Not Found</p>" \
        "</body>" \
    "</html>"

#define DB_TABLE "short"
#define DEBUG 1

static const int PORT = 8744;
static const int MAX_DATA_SIZE = 1024;

static const int ID_MAX_ATTEMPTS = 250;
static const int ID_SIZE = 4;
static const char ID_CHARS[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
};

#endif // __CONFIG_H__

