#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "main.h"


int main() {

    while (true) {
        char input[100];
        int e = -1;
        printf("> ");
        scanf_s("%[^\n]%*c", input);
        if (input[0] == 'e') {
            break;
        }
        printf("%d \n",eval(input,&e));

    }
    return 0;
}

bool is_num(char c) {
    return (c >= '0' && c <= '9');
}

int tokenize(struct Token tokenlist[50], char input[100],int* e) {
    int cp = 0; // It stands for char pointer
    int tp = 0;
    int bracket_depth = 0;
    size_t len = strlen(input);
    while (cp < len) {
        char c = input[cp];
        struct Token token;
        if (is_num(c)) {
            int num = 0;
            while (is_num(c) && cp < len) {
                num *= 10;
                num += c - '0';
                cp += 1;
                c = input[cp];
                
            }
            token.type = NUMBER;
            token.extra = num;
            tokenlist[tp] = token;
            tp += 1;
        }
        struct Token token2;
        token2.extra = 0;
        switch (c) {
            case '+':
                token2.type = PLUS;
                tokenlist[tp] = token2;
                tp += 1;
                break;
            case '-':
                token2.type = MINUS;
                tokenlist[tp] = token2;
                tp += 1;
                break;
            case '*':
                token2.type = MULTIPLY;
                tokenlist[tp] = token2;
                tp += 1;
                break;
            case '/':
                token2.type = DIVIDE;
                tokenlist[tp] = token2;
                tp += 1;
                break;
            case '(':
                ++bracket_depth;
                token2.type = OPEN_BRACE;
                tokenlist[tp] = token2;
                tp += 1;
                break;
            case ')':
                --bracket_depth;
                token2.type = CLOSE_BRACE;
                tokenlist[tp] = token2;
                tp += 1;
                break;
            case ' ':
                break;
            case '\n':
                break;
            case '\r':
                break;
            case 0:
                break;
            default:
                //printf("Invalid character encountered:");
                //printf("%d", (int) c);
                *e = UNKNOWN_CHAR;
                return -1;
                break;
        }

        cp += 1;
    }
    if (bracket_depth != 0) {
        //printf("Unbalanced brackets");
        *e = UNBALANCED_BRACKETS;
        
        return -1;
    }
    return tp;
}

void print_token_type(struct Token token) {
    switch (token.type) {
        case NUMBER:
            printf("Number: %d", token.extra);
            break;
        case PLUS:
            printf("Plus");
            break;
        case MINUS:
            printf("Minus");
            break;
        case MULTIPLY:
            printf("Multiply");
            break;
        case DIVIDE:
            printf("Divide");
            break;
        case OPEN_BRACE:
            printf("Open bracket");
            break;
        case CLOSE_BRACE:
            printf("Close bracket");
            break;
    }
}

struct Node* create_node(struct Token value) {
    struct Node* node = malloc(sizeof(struct Node));
    if (node != NULL) {
        node->value = value;
        node->left = NULL;
        node->right = NULL;

    }
    return node; 
}

void print_node(struct Node* node, int depth) {
    if (node == NULL) {
        return;
    }
    for (int i = 0; i < depth; ++i) {
        printf("  ");
    }
    print_token_type(node->value);
    printf("\n");
    print_node(node->left, depth + 1);
    print_node(node->right, depth + 1);
}

struct Token create_token(int type, int extra) {
    struct Token t;
    t.type = type;
    t.extra = extra;
    return t;

}

void free_node(struct Node* node) {
    if (node == NULL) {
        return;
    }
    free_node(node->left);
    free_node(node->right);
    free(node);
}

struct Node* build_AST(struct Token tokenlist[50],int tp, int* e) {
    struct Node* nodelist[50];
    
    for (int i = 0; i < tp; i++) {
        nodelist[i] = create_node(tokenlist[i]);
    }

    // BRACKETS WOOO
    int start = -1;
    int end = -1;
    int bdepth = 0;
    for (int i = 0; i < tp; i++) {
        if (nodelist[i] == NULL) {
            continue;
        }
        if (nodelist[i]->value.type == OPEN_BRACE) {
            if (bdepth == 0) {
                start = i+1;
            }
            //printf("WE HIT A OPEN BRACKET OF DEPTH: %d \n",bdepth);
            bdepth++;
        }
        if (nodelist[i]->value.type == CLOSE_BRACE) {
            bdepth--;
            //printf("WE HIT A CLOSED BRACKET OF DEPTH: %d \n",bdepth);
            if (bdepth == 0) {
                end = i;
            }
        }

        // Since we didnt modify the array, indicies of nodelist would work on tokenlist for their associated token values
        if (end != -1) {
            
            struct Token newtokenlist[50];
            int l = 0;
            for (int j = start; j < end; j++) {
                newtokenlist[l] = tokenlist[j];
                nodelist[j] = NULL;
                l++;
            }
            nodelist[end] = NULL;
            //printf("TOKENS INSIDE BRACKETS:\n");
            
            //for (int i = 0; i < l; i++) {

                //print_token_type(newtokenlist[i]);
                //printf("\n");
            //}
            nodelist[start-1] = build_AST(newtokenlist, l,e);
            start = -1;
            end = -1;
            bdepth = 0;
        }
         
    }
    //printf("\n");
    //printf("NODES AS A RESULT OF BRACKETING\n");
    

    /*
    for (int i = 0; i < tp; i++) {
        if (nodelist[i] == NULL) {
            continue;
        }
        print_token_type(nodelist[i]->value);
        printf("\n");
    }
    */
    

    // Multiply and divide 
    for (int i = 0; i < tp; i++) {
        if (nodelist[i] == NULL) {
            continue;
        }
        if (nodelist[i]->left != NULL || nodelist[i]->right != NULL) {
            continue;
        }
        //print_token_type(nodelist[i]->value)      ;
        if (nodelist[i]->value.type == MULTIPLY || nodelist[i]->value.type == DIVIDE) {
            if (i == 0 || i == tp-1) {
                *e = HANGING_OP;
                return nodelist[0];
            }
            else {
                // Get nearest non-null left neighbour
                int a = 1;
                int b = 1;
                while (nodelist[i-a] == NULL) {
                    a += 1;
                    if (i-a < 0) {
                        //printf("ERROR: HANGING MULTIPLICATION/DIVISION OPERATOR");
                        *e = HANGING_OP;
                        return nodelist[0];
                        a = -1;
                        break; 
                    }
                }
                while (nodelist[i+b] == NULL) {
                    b += 1;
                    if (i+b == tp-1 ) {
                        //printf("ERROR: HANGING MULTIPLICATION/DIVISION OPERATOR");
                        *e = HANGING_OP;
                        return nodelist[0];
                        a = -1;
                        break; 
                    }
                }

                if (a != -1) {

                    nodelist[i]->left = nodelist[i-a];
                    nodelist[i]->right = nodelist[i+b];
                    nodelist[i-a] = NULL;
                    nodelist[i+b] = NULL;
                }

            }

        }
    }
    
    // Add and subtract
    for (int i = 0; i < tp; i++) {
        if (nodelist[i] == NULL) {
            continue;
        }
        if (nodelist[i]->left != NULL || nodelist[i]->right != NULL) {
            continue;
        }
        //print_token_type(nodelist[i]->value)      ;
        if (nodelist[i]->value.type == PLUS || nodelist[i]->value.type == MINUS) {
            if (i == 0 || i == tp-1) {
                *e = HANGING_OP;
                return nodelist[0];
            }
            else {
                // Get nearest non-null left neighbour
                int a = 1;
                int b = 1;
                while (nodelist[i-a] == NULL) {
                    a += 1;
                    if (i-a == 0) {
                        //printf("ERROR: HANGING ADD/SUB OPERATOR");
                        *e = HANGING_OP;
                        return nodelist[0];
                        a = -1;
                        break; 
                    }
                }
                while (nodelist[i+b] == NULL) {
                    b += 1;
                    if (i+b == tp-1 ) {
                        printf("ERROR: HANGING ADD/SUB OPERATOR");
                        *e = HANGING_OP;
                        return nodelist[0];
                        a = -1;
                        break; 
                    }
                }

                if (a != -1) {

                    nodelist[i]->left = nodelist[i-a];
                    nodelist[i]->right = nodelist[i+b];
                    nodelist[i-a] = NULL;
                    nodelist[i+b] = NULL;
                }

            }

        }
    }
    for (int i = 0; i < tp; i++) {
        if (nodelist[i] != NULL) {
            return nodelist[i];
        }
    }
    
    return nodelist[0];
}

int eval_node(struct Node* node,int* e) {
    if (*e != -1) {
        return -1;
    }
    if (node->value.type != NUMBER) {

        if (node->left == NULL || node->right == NULL) {
            *e = NO_ARGS;
            return -1;
        }
    }
    
    switch (node->value.type) {
        case NUMBER:
            return node->value.extra;
            break;
        case PLUS:
            return eval_node(node->left,e) + eval_node(node->right,e);
            break;
        case MINUS:
            return eval_node(node->left,e) - eval_node(node->right,e);
            break;
        case MULTIPLY:
            return eval_node(node->left,e) * eval_node(node->right,e);
            break;
        case DIVIDE: ;
            int divisor = eval_node(node->right, e);
            if (divisor == 0) {
                *e = DIV_BY_ZERO;
                return -1;
            }

            return eval_node(node->left,e) / divisor;
            break;
        case OPEN_BRACE:
            break;
        case CLOSE_BRACE:
            break;
    }
    return 0;
}

void print_err(int e) {
    printf("ERROR: ");

    switch (e) {

        case UNKNOWN_CHAR:
            printf("Unknown character encountered\n");
            break;
        case UNBALANCED_BRACKETS:
            printf("Unbalanced brackets\n");
            break;
        case HANGING_OP:
            printf("Hanging operator\n");
            break;
        case DIV_BY_ZERO:
            printf("Divide by zero error\n");
            break;
        case NO_ARGS:
            printf("Insufficient Arguments\n");
            break;
    }
}

int eval(char input[100],int* e) {

    struct Token tokenlist[50];
    int tp = tokenize(tokenlist,input,e);
    if (*e != -1) {
        print_err(*e);
        return -1;
    }

    struct Node* root = build_AST(tokenlist,tp,e);
    if (*e != -1) {
        print_err(*e);
        return -1;
    }
    int ans = eval_node(root,e);
    if (*e != -1) {
        print_err(*e);
        return -1;
    }
    free_node(root);
    return ans;

}

