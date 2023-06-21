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

#define URL_TABLE "urls"
#define KEY_TABLE "keys"

#define DEBUG 1

const int PORT = 8744;
const int MAX_DATA_SIZE = 1024;

const int KEY_SIZE = 8;

const int ID_MAX_ATTEMPTS = 250;
const int ID_SIZE = 4;

const char VALID_CHARS[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
};

#endif // __CONFIG_H__

