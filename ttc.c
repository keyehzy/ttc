#include <stdio.h>
#include <string.h>

enum token_type {
  EOF_TOKEN,
  NEWLINE_TOKEN,
  NUMBER_TOKEN,
  IDENT_TOKEN,
  STRING_TOKEN,

  // KEYWORDS
  LABEL_TOKEN,
  GOTO_TOKEN,
  PRINT_TOKEN,
  INPUT_TOKEN,
  LET_TOKEN,
  IF_TOKEN,
  THEN_TOKEN,
  ENDIF_TOKEN,
  WHILE_TOKEN,
  REPEAT_TOKEN,
  ENDWHILE_TOKEN,

  // OPERATORS
  EQ_TOKEN,
  PLUS_TOKEN,
  MINUS_TOKEN,
  ASTERISK_TOKEN,
  SLASH_TOKEN,
  EQEQ_TOKEN,
  NOTEQ_TOKEN,
  LT_TOKEN,
  LTEQ_TOKEN,
  GT_TOKEN,
  GTEQ_TOKEN,
};
typedef enum token_type token_type;

struct token {
  token_type type;
  const char *start;
  unsigned int len;
};
typedef struct token token;

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

char peek(lexer *l) { return l->last; }

lexer new_lexer(const char *input) {
  lexer l = {input, strlen(input), 0, '\0'};
  skip(&l);
  return l;
}

token get_token(lexer *l) {
  token t = {EOF_TOKEN, l->input + l->pos, 1};

  while (peek(l) == ' ' || peek(l) == '\t' || peek(l) == '\r') {
    skip(l);
  }

  switch (l->last) {
    case '\0':
      skip(l);
      t.type = EOF_TOKEN;
      break;
    case '\n':
      t.type = NEWLINE_TOKEN;
      skip(l);
      break;
    case '+':
      t.type = PLUS_TOKEN;
      skip(l);
      break;
    case '-':
      t.type = MINUS_TOKEN;
      skip(l);
      break;
    case '*':
      t.type = ASTERISK_TOKEN;
      skip(l);
      break;
    case '/':
      t.type = SLASH_TOKEN;
      skip(l);
      break;
    case '=':
      t.type = EQ_TOKEN;
      skip(l);
      if (peek(l) == '=') {
        t.type = EQEQ_TOKEN;
        t.len = 2;
        skip(l);
      }
      break;
    case '>':
      t.type = GT_TOKEN;
      skip(l);
      if (peek(l) == '=') {
        t.type = GTEQ_TOKEN;
        t.len = 2;
        skip(l);
      }
      break;
    case '<':
      t.type = LT_TOKEN;
      skip(l);
      if (peek(l) == '=') {
        t.type = LTEQ_TOKEN;
        t.len = 2;
        skip(l);
      }
      break;
    case '!':
      skip(l);
      if (peek(l) == '=') {
        t.type = NOTEQ_TOKEN;
        t.len = 2;
        skip(l);
      } else {
        t.type = EOF_TOKEN;
      }
      break;
  }
  return t;
}

int main(void) {
  lexer l = new_lexer("+- */ < <= > >= == !=\n");
  token t = get_token(&l);

  while (t.type != EOF_TOKEN) {
    printf("Token: %d\n", t.type);
    t = get_token(&l);
  }
  return 0;
}