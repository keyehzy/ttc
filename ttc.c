#include <stdio.h>
#include <string.h>

struct lexer {
    const char *input;
    unsigned int len;
    unsigned int pos;
    char last;
};
typedef struct lexer lexer;

void skip(lexer *l) {
    if (l->pos >= l->len) {
        l->last = '\0';
    }
    l->last = l->input[l->pos++];
}

char peek(lexer *l) {
    if (l->pos >= l->len) {
        return '\0';
    }
    return l->input[l->pos];
}

lexer new_lexer(const char *input) {
    lexer l = {input, strlen(input), 0, '\0'};
    skip(&l);
    return l;
}

int main(void) {
    lexer l = new_lexer("LET foobar = 123");

    while(l.last != '\0') {
        printf("%c\n", l.last);
        skip(&l);
    }
    return 0;
}