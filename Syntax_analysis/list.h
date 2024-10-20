struct alpha_token_t {
  unsigned int     numline;
  unsigned int     numToken;
  char          *content;
  char          *type;
  struct alpha_token_t *alpha_yylex;
};
typedef struct alpha_token_t* lex_list;

lex_list insert_node(lex_list list, unsigned int numline,unsigned int numToken,char *content, char *type);

void print_lex(lex_list list);

void destroy(lex_list list);
