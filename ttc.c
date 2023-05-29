#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void abort_(const char *message, const char *file, int line) {
  fprintf(stderr, "%s:%d: %s\n", file, line, message);
  exit(1);
}

#define TTC_ABORT(message) abort_(message, __FILE__, __LINE__)
#define TTC_ASSERT(expr) \
  if (!(expr)) TTC_ABORT("Assertion failed: " #expr)

#define ENUMERATE_NUMBERS(O) \
  O('0') O('1') O('2') O('3') O('4') O('5') O('6') O('7') O('8') O('9')

#define ENUMERATE_ALPHA(O) \
  O('a')                   \
  O('b')                   \
  O('c')                   \
  O('d')                   \
  O('e')                   \
  O('f')                   \
  O('g')                   \
  O('h')                   \
  O('i')                   \
  O('j')                   \
  O('k')                   \
  O('l')                   \
  O('m')                   \
  O('n')                   \
  O('o')                   \
  O('p')                   \
  O('q')                   \
  O('r')                   \
  O('s')                   \
  O('t')                   \
  O('u')                   \
  O('v')                   \
  O('w')                   \
  O('x')                   \
  O('y')                   \
  O('z')                   \
  O('A')                   \
  O('B')                   \
  O('C')                   \
  O('D')                   \
  O('E')                   \
  O('F')                   \
  O('G')                   \
  O('H')                   \
  O('I')                   \
  O('J')                   \
  O('K')                   \
  O('L')                   \
  O('M')                   \
  O('N')                   \
  O('O')                   \
  O('P')                   \
  O('Q')                   \
  O('R')                   \
  O('S')                   \
  O('T')                   \
  O('U')                   \
  O('V')                   \
  O('W')                   \
  O('X')                   \
  O('Y')                   \
  O('Z')

#define CASE(x) case x:
#define CASE_NUMBERS ENUMERATE_NUMBERS(CASE)
#define CASE_ALPHA ENUMERATE_ALPHA(CASE)

enum token_type {
  EOF_TOKEN = 0,
  NEWLINE_TOKEN,
  NUMBER_TOKEN,
  IDENT_TOKEN,
  STRING_TOKEN,

  // KEYWORDS
  LABEL_TOKEN = 100,
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
  EQ_TOKEN = 200,
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

const char *ttc_keywords[] = {
    "LABEL", "GOTO",  "PRINT", "INPUT",  "LET",      "IF",
    "THEN",  "ENDIF", "WHILE", "REPEAT", "ENDWHILE",
};

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

void skip_whitespace(lexer *l) {
  while (l->last == ' ' || l->last == '\t' || l->last == '\r') {
    skip(l);
  }
}

void skip_comment(lexer *l) {
  if (peek(l) == '#') {
    while (l->last != '\n') {
      skip(l);
    }
    // if (l->last == '\n') {
    //   skip(l);
    // }
  }
}

token_type check_if_keyword(const char *start, unsigned int len) {
  for (unsigned int i = 0; i < sizeof(ttc_keywords) / sizeof(ttc_keywords[0]);
       i++) {
    if (strncmp(start, ttc_keywords[i], len) == 0) {
      return (token_type)(LABEL_TOKEN + i);
    }
  }
  return IDENT_TOKEN;
}

token get_token(lexer *l) {
  skip_whitespace(l);
  skip_comment(l);
  token t = {EOF_TOKEN, l->input + l->pos - 1, 1};

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
        TTC_ABORT("Expected '=' after '!'");
      }
      break;

    case '"':
      t.type = STRING_TOKEN;
      skip(l);
      while (l->last != '"') {
        skip(l);
        if (peek(l) == '\n' || peek(l) == '\r' || peek(l) == '\t' ||
            peek(l) == '\\' || peek(l) == '%') {
          TTC_ABORT("Invalid character in string");
        }
      }
      skip(l);
      t.len = l->input + l->pos - 1 - t.start;
      break;

      CASE_NUMBERS
      t.type = NUMBER_TOKEN;
      while (isdigit(peek(l))) {
        skip(l);
      }
      if (peek(l) == '.') {
        skip(l);
        if (!isdigit(peek(l))) {
          TTC_ABORT("Expected digit after '.'");
        }
        while (isdigit(peek(l))) {
          skip(l);
        }
      }
      t.len = l->input + l->pos - 1 - t.start;
      break;

      CASE_ALPHA
      while (isalnum(peek(l))) {
        skip(l);
      }
      t.len = l->input + l->pos - 1 - t.start;
      t.type = check_if_keyword(t.start, t.len);
      break;

    default:
      TTC_ABORT("Unknown token");
  }
  return t;
}

int main(void) {
  lexer l = new_lexer("+- foo LABEL \"bar\" 123 9.8654 */");
  token t = get_token(&l);

  while (t.type != EOF_TOKEN) {
    printf("Token: %d\n", t.type);
    t = get_token(&l);
  }
  return 0;
}