#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE (1024 * 1024)  /* 1 MB hard limit for Phase 1 */

typedef enum {
    TOK_OPEN_TAG,
    TOK_CLOSE_TAG,
    TOK_TEXT
} TokenKind;

static const char *kind_name(TokenKind kind)
{
    switch (kind) {
        case TOK_OPEN_TAG:  return "OPEN_TAG";
        case TOK_CLOSE_TAG: return "CLOSE_TAG";
        case TOK_TEXT:      return "TEXT";
    }
    return "UNKNOWN";
}

static int is_all_whitespace(const char *s, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (!isspace((unsigned char)s[i]))
            return 0;
    }
    return 1;
}

/* Print token content, escaping whitespace so every byte is visible */
static void print_escaped(const char *s, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        switch (s[i]) {
            case '\n': printf("\\n"); break;
            case '\t': printf("\\t"); break;
            case '\r': printf("\\r"); break;
            default:   putchar(s[i]); break;
        }
    }
}

static void print_token(TokenKind kind, const char *start, size_t len)
{
    if (kind == TOK_TEXT && is_all_whitespace(start, len)) {
        printf("TEXT(whitespace): \"");
        print_escaped(start, len);
        printf("\"\n");
    } else if (kind == TOK_TEXT) {
        printf("TEXT: \"");
        print_escaped(start, len);
        printf("\"\n");
    } else {
        printf("%s: ", kind_name(kind));
        print_escaped(start, len);
        printf("\n");
    }
}

static void tokenize(const char *buf)
{
    const char *p = buf;

    while (*p != '\0') {
        if (*p == '<') {
            const char *start = p;
            /* scan to closing '>' */
            while (*p != '\0' && *p != '>')
                p++;
            if (*p == '>')
                p++;  /* include the '>' */
            TokenKind kind = (start[1] == '/') ? TOK_CLOSE_TAG : TOK_OPEN_TAG;
            print_token(kind, start, (size_t)(p - start));
        } else {
            const char *start = p;
            /* accumulate text until the next '<' or end of buffer */
            while (*p != '\0' && *p != '<')
                p++;
            print_token(TOK_TEXT, start, (size_t)(p - start));
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <html-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    static char buffer[MAX_SIZE];  /* static so it lives in BSS, not the stack */
    size_t n = fread(buffer, 1, MAX_SIZE - 1, fp);
    fclose(fp);

    buffer[n] = '\0';

    if (n == 0) {
        fprintf(stderr, "Error: file is empty\n");
        return EXIT_FAILURE;
    }

    tokenize(buffer);

    return EXIT_SUCCESS;
}
