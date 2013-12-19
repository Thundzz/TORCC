%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "map.h"
#include "aux.h"

#define BUFSIZE 100 
  int regNum = 1;
  extern int yylineno;
  int yylex ();
  int yyerror ();

  

  void initTorcsVariables()
  {
    map_set_val("$accel",  regNum++);
    map_set_type("$accel",  TYPE_FLOAT);
    
    
  }


  char * strFusion (char * str, char * toAppend)
  {
    int newSize = strlen(str) + strlen (toAppend);
    char * catStr =  strndup (str, newSize) ;
    strcat ( catStr, toAppend);
    free(str);
    free(toAppend);

    return (catStr);
  }

  %}

%token <str> IDENTIFIER
%token <str> CONSTANTF 
%token <str> CONSTANTI
%token INC_OP DEC_OP LE_OP GE_OP EQ_OP NE_OP
%token SUB_ASSIGN MUL_ASSIGN ADD_ASSIGN
%token TYPE_NAME
%token <nat> INT FLOAT VOID
%token IF ELSE WHILE RETURN FOR

%type <nat> type_name
%type <str> compound_statement declarator postfix_expression unary_expression expression  parameter_list primary_expression multiplicative_expression additive_expression comparison_expression

%union{
  char * str;
  float flt;
  int nat;
 }

%start program
%%


primary_expression
: IDENTIFIER { $$ = $1 ;}
| CONSTANTI { $$ = $1 ; map_set_type($1, TYPE_INT);  }
| CONSTANTF { $$ = $1 ; map_set_type($1, TYPE_FLOAT); puts($1); }
| '(' expression ')' {}
| IDENTIFIER '(' ')'
| IDENTIFIER '(' argument_expression_list ')'
| IDENTIFIER INC_OP
| IDENTIFIER DEC_OP
;

postfix_expression
: primary_expression {$$ = $1; puts($1); }
| postfix_expression '[' expression ']'
;

argument_expression_list
: expression 
| argument_expression_list ',' expression
;

unary_expression
: postfix_expression {$$ = $1; puts($1); puts("test");}
| INC_OP unary_expression {}
| DEC_OP unary_expression {}
| unary_operator unary_expression {}
;

unary_operator
: '-'
;

multiplicative_expression
: unary_expression {$$ = $1;}
| multiplicative_expression '*' unary_expression
| multiplicative_expression '/' unary_expression
;

additive_expression
: multiplicative_expression {$$ = $1;puts($1); }
| additive_expression '+' multiplicative_expression
| additive_expression '-' multiplicative_expression
;

comparison_expression
: additive_expression {$$ = $1;puts($1); }
| additive_expression '<' additive_expression
| additive_expression '>' additive_expression
| additive_expression LE_OP additive_expression
| additive_expression GE_OP additive_expression
| additive_expression EQ_OP additive_expression
| additive_expression NE_OP additive_expression
;

expression
: unary_expression assignment_operator comparison_expression {
  int val = map_get_val($1);
  if(val == VAL_UNDEF)
  {
    fprintf(stderr, "error: undeclared identifier %s", $1);
    exit(EXIT_FAILURE);
  }
  else
  {
    char *regBuf = strndup("", BUFSIZE);
    snprintf(regBuf, BUFSIZE, "%%%d = ", regNum++);
 
    int rType = map_get_type($1), lType = map_get_type($3);
    if(rType != lType)
    {
      fprintf(stderr, "%d %d\n", map_get_type("$accel"),  map_get_type($3) );
      puts($3);
      fprintf(stderr, "Bouuuuuh (implicit cast not supported (yet)) : assigning a %d to a %d\n", rType, lType);
      exit(EXIT_FAILURE);
    }
    char * LLVMRType = typeToLLVM(rType);
    char *left = strFusion(regBuf, LLVMRType);
    left = strFusion(left, strdup(" "));
    $$ = strFusion(left, $3);
  }
 }
| comparison_expression
;

assignment_operator
: '='
| MUL_ASSIGN
| ADD_ASSIGN
| SUB_ASSIGN
;

declaration
: type_name declarator_list ';'
;

declarator_list
: declarator
| declarator_list ',' declarator
;

type_name
: VOID {$$ = VOID;}
| INT    {$$ = INT;}
| FLOAT  {$$ = FLOAT;}
;

declarator
: IDENTIFIER  {$$ = $1;} 
| '(' declarator ')' {}
| declarator '[' CONSTANTI ']'
| declarator '[' ']'
| declarator '(' parameter_list ')' {$$ =  $3;}
| declarator '(' ')'  {$$ = strFusion ( strFusion ($1, strdup("(")), strdup(")")) ;}
;

parameter_list
: parameter_declaration {}
| parameter_list ',' parameter_declaration
;

parameter_declaration
: type_name declarator
;

statement
: compound_statement
| expression_statement 
| selection_statement
| iteration_statement
| jump_statement
;

compound_statement
: '{' '}' {}
| '{' statement_list '}'  {}
| '{' declaration_list statement_list '}' {}
;

declaration_list
: declaration
| declaration_list declaration
;

statement_list
: statement
| statement_list statement
;

expression_statement
: ';'
| expression ';'
;

selection_statement
: IF '(' expression ')' statement
| IF '(' expression ')' statement ELSE statement
| FOR '(' expression_statement expression_statement expression ')' statement
;

iteration_statement
: WHILE '(' expression ')' statement
;

jump_statement
: RETURN ';'
| RETURN expression ';'
;

program
: external_declaration
| program external_declaration
;

external_declaration
: function_definition
| declaration
;

function_definition
: type_name declarator compound_statement {printf("define %s @%s(%s){%s}", typeToLLVM($1), $2, "[arguments]", "[body]" );}
;

%%
#include <stdio.h>
#include <string.h>

extern int column;
extern int yylineno;
extern FILE *yyin;

char *file_name = NULL;

char * typeToLLVM (int CType)
{
  switch(CType){
  case TYPE_INT:
    return strdup("i32");
    break;
  case TYPE_VOID:
    return strdup("void");
    break;
  case TYPE_FLOAT:
    return strdup("f32");
    break;
  default:
    perror ("Wrong type given somewhere.");
    exit(EXIT_FAILURE);
  }
}

int yyerror (char *s) {
  fflush (stdout);
  fprintf (stderr, "%s:%d:%d: %s\n", file_name, yylineno, column, s);
  return 0;
}


int main (int argc, char *argv[]) {
  FILE *input = NULL;



  if (argc==2) {
    input = fopen (argv[1], "r");
    file_name = strdup (argv[1]);
    if (input) {
      yyin = input;
    }
    else {
      fprintf (stderr, "%s: Could not open %s\n", *argv, argv[1]);
      return 1;
    }
  }
  else {
    fprintf (stderr, "%s: error: no input file\n", *argv);
    return 1;
  }

  map_init();
  initTorcsVariables();
  yyparse ();
  map_end();
  
  free (file_name);


  return 0;
}