#pragma once
#include <stdbool.h>

enum Tokentype {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    OPEN_BRACE,
    CLOSE_BRACE
};

enum Error {
    UNKNOWN_CHAR,
    UNBALANCED_BRACKETS,
    HANGING_OP,
    DIV_BY_ZERO,
    NO_ARGS,
};

struct Token {
    int type;
    int extra;
};

struct Node {
    struct Token value;
    struct Node* left;
    struct Node* right;
};

struct Token create_token(int type, int extra);
int tokenize(struct Token tokenlist[50], char input[100], int* e);
void print_token_type(struct Token token);
bool is_num(char c);
struct Node* create_node(struct Token value);
void print_node(struct Node* node, int depth);
void free_node(struct Node* node);
struct Node* build_AST(struct Token tokenlist[50],int tp, int* e);
int eval_node(struct Node* node, int* e);
int eval(char input[100], int* e);  
void print_err(int e);

    
