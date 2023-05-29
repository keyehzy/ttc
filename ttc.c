#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void abort_(const char *message, const char *file, int line) {
  fprintf(stderr, "%s:%d: %s\n", file, line, message);
  __builtin_trap();
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

lexer new_lexer(const char *input, unsigned int len) {
  lexer l = {input, len, 0, '\0'};
  skip(&l);
  return l;
}

lexer new_lexer_from_cstr(const char *input) {
  lexer l = {input, strlen(input), 0, '\0'};
  skip(&l);
  return l;
}

void skip_whitespace(lexer *l) {
  while (peek(l) == ' ' || peek(l) == '\t' || peek(l) == '\r') {
    skip(l);
  }
}

void skip_comment(lexer *l) {
  if (peek(l) == '#') {
    while (peek(l) != '\n') {
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

  switch (peek(l)) {
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
      while (peek(l) != '"') {
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
      // TTC_ABORT("Unknown token");
      break;
  }
  return t;
}

struct emitter {
  char *header;
  unsigned int header_len;
  unsigned int header_pos;

  char *body;
  unsigned int body_len;
  unsigned int body_pos;
};
typedef struct emitter emitter;

void emit_header(emitter *e, const char *str, unsigned int len) {
  if (e->header_pos + len >= e->header_len) {
    e->header_len = (e->header_len + len) * 2;
    e->header = realloc(e->header, e->header_len);
    TTC_ASSERT(e->header);
  }
  memcpy(e->header + e->header_pos, str, len);
  e->header_pos += len;
}

void emit_body(emitter *e, const char *str, unsigned int len) {
  if (e->body_pos + len >= e->body_len) {
    e->body_len = (e->body_len + len) * 2;
    e->body = realloc(e->body, e->body_len);
    TTC_ASSERT(e->body);
  }
  memcpy(e->body + e->body_pos, str, len);
  e->body_pos += len;
}

void emit_header_cstr(emitter *e, const char *str) {
  emit_header(e, str, strlen(str));
}

void emit_body_cstr(emitter *e, const char *str) {
  emit_body(e, str, strlen(str));
}

void emitter_write_to_file(emitter *e, FILE *f) {
  if (!f) {
    TTC_ABORT("Failed to open file for writing");
  }
  fwrite(e->header, 1, e->header_pos, f);
  fwrite(e->body, 1, e->body_pos, f);
  fclose(f);
}

struct token_set {
  token tokens[256];
  unsigned int len;
};
typedef struct token_set token_set;

void token_set_add(token_set *s, token t) {
  TTC_ASSERT(s->len < 256);
  for (unsigned int i = 0; i < s->len; i++) {
    if (!strncmp(s->tokens[i].start, t.start, t.len)) {
      return;
    }
  }
  s->tokens[s->len++] = t;
}

int token_set_contains(token_set *s, token t) {
  for (unsigned int i = 0; i < s->len; i++) {
    if (!strncmp(s->tokens[i].start, t.start, t.len)) {
      return 1;
    }
  }
  return 0;
}

int token_set_equals(token_set *a, token_set *b) {
  if (a->len != b->len) {
    return 0;
  }
  for (unsigned int i = 0; i < a->len; i++) {
    if (!token_set_contains(b, a->tokens[i])) {
      return 0;
    }
  }
  return 1;
}

struct parser {
  lexer *l;
  token cur_token;
  token next_token;

  token_set symbols;
  token_set labels;
  token_set gotoes;

  emitter e;
};
typedef struct parser parser;

int check(parser *p, token_type type) { return p->cur_token.type == type; }

void next_token(parser *p) {
  p->cur_token = p->next_token;
  p->next_token = get_token(p->l);
}

void match(parser *p, token_type type) {
  if (p->cur_token.type == type) {
    next_token(p);
  } else {
    TTC_ABORT("Expected token");
  }
}

parser new_parser(lexer *l) {
  parser p = {0};
  p.l = l;
  next_token(&p);
  next_token(&p);
  return p;
}

void primary(parser *p) {
  if (check(p, NUMBER_TOKEN)) {
    emit_body(&p->e, p->cur_token.start, p->cur_token.len);
    next_token(p);
  } else if (check(p, IDENT_TOKEN)) {
    if (!token_set_contains(&p->symbols, p->cur_token)) {
      TTC_ABORT("Reference to undeclared variable");
    }
    emit_body(&p->e, p->cur_token.start, p->cur_token.len);
    next_token(p);
  } else {
    TTC_ABORT("Unexpected token");
  }
}

void unary(parser *p) {
  if (check(p, PLUS_TOKEN) || check(p, MINUS_TOKEN)) {
    emit_body(&p->e, p->cur_token.start, p->cur_token.len);
    next_token(p);
  }
  primary(p);
}

void term(parser *p) {
  unary(p);
  while (check(p, ASTERISK_TOKEN) || check(p, SLASH_TOKEN)) {
    emit_body(&p->e, p->cur_token.start, p->cur_token.len);
    next_token(p);
    unary(p);
  }
}

void expression(parser *p) {
  term(p);
  while (check(p, PLUS_TOKEN) || check(p, MINUS_TOKEN)) {
    emit_body(&p->e, p->cur_token.start, p->cur_token.len);
    next_token(p);
    term(p);
  }
}

int check_if_comparsion(parser *p) {
  token_type t = p->cur_token.type;
  return t == EQEQ_TOKEN || t == NOTEQ_TOKEN || t == GT_TOKEN ||
         t == GTEQ_TOKEN || t == LT_TOKEN || t == LTEQ_TOKEN;
}

void comparison(parser *p) {
  expression(p);

  if (check_if_comparsion(p)) {
    emit_body(&p->e,p->cur_token.start, p->cur_token.len);
    next_token(p);
    expression(p);
  } else {
    TTC_ABORT("Expected comparison operator");
  }

  while (check_if_comparsion(p)) {
    emit_body(&p->e,p->cur_token.start, p->cur_token.len);
    next_token(p);
    expression(p);
  }
}

void newline(parser *p) {
  match(p, NEWLINE_TOKEN);
  while (check(p, NEWLINE_TOKEN)) {
    next_token(p);
  }
}

void statement(parser *p) {
  switch (p->cur_token.type) {
    case PRINT_TOKEN:
      next_token(p);

      if (check(p, STRING_TOKEN)) {
        emit_body_cstr(&p->e, "printf(");
        emit_body(&p->e, p->cur_token.start, p->cur_token.len);
        emit_body_cstr(&p->e, ");");
        next_token(p);
      } else {
        emit_body_cstr(&p->e, "printf(\"%.2f\\n\", (float)(");
        expression(p);
        emit_body_cstr(&p->e, "));");
      }
      break;

    case IF_TOKEN:
      next_token(p);
      emit_body_cstr(&p->e, "if (");
      comparison(p);

      match(p, THEN_TOKEN);
      newline(p);
      emit_body_cstr(&p->e, "){");

      while (p->cur_token.type != ENDIF_TOKEN) {
        statement(p);
      }

      match(p, ENDIF_TOKEN);
      emit_body_cstr(&p->e, "}");
      break;

    case WHILE_TOKEN:
      next_token(p);
      emit_body_cstr(&p->e, "while (");
      comparison(p);

      match(p, REPEAT_TOKEN);
      newline(p);
      emit_body_cstr(&p->e, "){");

      while (p->cur_token.type != ENDWHILE_TOKEN) {
        statement(p);
      }

      match(p, ENDWHILE_TOKEN);
      emit_body_cstr(&p->e, "}");
      break;

    case LABEL_TOKEN:
      next_token(p);
      if (token_set_contains(&p->labels, p->cur_token)) {
        TTC_ABORT("Label already defined");
      }
      token_set_add(&p->labels, p->cur_token);
      emit_body(&p->e, p->cur_token.start, p->cur_token.len);
      emit_body_cstr(&p->e, ":");
      match(p, IDENT_TOKEN);
      break;

    case GOTO_TOKEN:
      next_token(p);
      token_set_add(&p->gotoes, p->cur_token);
      emit_body_cstr(&p->e, "goto ");
      emit_body(&p->e, p->cur_token.start, p->cur_token.len);
      emit_body_cstr(&p->e, ";");
      match(p, IDENT_TOKEN);
      break;

    case LET_TOKEN:
      next_token(p);

      if (!token_set_contains(&p->symbols, p->cur_token)) {
        token_set_add(&p->symbols, p->cur_token);
        emit_header_cstr(&p->e, "float ");
        emit_header(&p->e, p->cur_token.start, p->cur_token.len);
        emit_header_cstr(&p->e, ";");
      }

      emit_body(&p->e, p->cur_token.start, p->cur_token.len);
      emit_body_cstr(&p->e, "=");

      match(p, IDENT_TOKEN);
      match(p, EQ_TOKEN);
      expression(p);
      emit_body_cstr(&p->e, ";");
      break;

    case INPUT_TOKEN:
      next_token(p);

      if (!token_set_contains(&p->symbols, p->cur_token)) {
        token_set_add(&p->symbols, p->cur_token);
        emit_header_cstr(&p->e, "float ");
        emit_header(&p->e, p->cur_token.start, p->cur_token.len);
        emit_header_cstr(&p->e, ";");
      }

      emit_body_cstr(&p->e, "if (0 == scanf(\"%f\", &");
      emit_body(&p->e, p->cur_token.start, p->cur_token.len);
      emit_body_cstr(&p->e, ")) {");
      emit_body(&p->e, p->cur_token.start, p->cur_token.len);
      emit_body_cstr(&p->e, "=0;");
      emit_body_cstr(&p->e, "scanf(\"%*s\");");
      emit_body_cstr(&p->e, "}");

      match(p, IDENT_TOKEN);
      break;

    default:
      TTC_ABORT("Unknown statement");
  }

  newline(p);
}

void program(parser *p) {
  emit_header_cstr(&p->e, "#include <stdio.h>\n");
  emit_body_cstr(&p->e, "int main(void) {");

  while (check(p, NEWLINE_TOKEN)) {
    next_token(p);
  }

  while (p->cur_token.type != EOF_TOKEN) {
    statement(p);
  }

  emit_body_cstr(&p->e, "return 0;}\n");

  TTC_ASSERT(token_set_equals(&p->gotoes, &p->labels));
}

int main(int argc, char **argv) {
  if (argc != 2) {
    TTC_ABORT("Expected one argument");
  }

  FILE *f = fopen(argv[1], "r");
  if (!f) {
    TTC_ABORT("Could not open file");
  }

  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *input = malloc(size + 1);
  fread(input, size, 1, f);
  input[size] = '\0';
  fclose(f);

  lexer l = new_lexer(input, size);
  parser p = new_parser(&l);
  program(&p);

  emitter_write_to_file(&p.e, stdout);
  return 0;
}