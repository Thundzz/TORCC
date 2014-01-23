%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "brutCode.h"
#include "aux.h"
#include "torcsVars.h"

#include "hashtable.h"
#include "symbol.h"
#include "expression.h"
#include "type.h"
#include "str.h"


#define BUFSIZE 1024




  char error[BUFSIZE] = "";
  char buffer[BUFSIZE] = "";
  int regNum = 1;
  extern int yylineno;
  int yylex ();
  int yyerror ();

  hashtable_t * table;


  void reportError()
  {
    yyerror(error);
    exit(EXIT_FAILURE);
  }

  char * strFusion (char * str, char * toAppend)
  {
    int newSize = strlen(str) + strlen (toAppend) +1  ;
    str =  realloc (str, newSize) ;
    strncat ( str, toAppend, strlen(toAppend));
    free(toAppend);

    return (str);
  }
  struct symbol_s * getSymbol (char * name)
  {
    struct symbol_s * s;
    if (NULL == (s = ht_get(table, name)))
    { 
      sprintf(error, "error: Undeclared identifier %s", name);
      reportError();
    }
    return (s);
  }


  %}



  %error-verbose
  %union{
    char * str;
    float flt;
    int nat;
    struct symbol_s * sym;
    struct expression_s * expr;
    struct argument_expression_list_s * ael;
  }


  %token <str> IDENTIFIER
  %token <str> CONSTANTF
  %token <str> CONSTANTI
  %token INC_OP DEC_OP LE_OP GE_OP EQ_OP NE_OP
  %token SUB_ASSIGN MUL_ASSIGN ADD_ASSIGN
  %token TYPE_NAME
  %token <nat> INT FLOAT VOID
  %token IF ELSE WHILE RETURN FOR

  %type <nat> type_name
  %type <str> compound_statement declarator parameter_list additive_expression comparison_expression expression_statement statement statement_list
  %type <expr> primary_expression expression postfix_expression unary_expression multiplicative_expression 
  %type <ael> argument_expression_list



  %start program
  %%

  primary_expression
  : IDENTIFIER { 
    $$ = createExpr();
    $$->symbol= getSymbol($1);
    $$->type = $$->symbol->type;
    $$->isPtr = (isPtr($$->symbol) || isArray($$->symbol)); 
    $$->regNum = $$->symbol->regNum;
    free($1);
  }

  | CONSTANTI {
    $$ = createExpr();
    $$-> type = TYPE_INT;
    sprintf(buffer, "\t%%r%d = add i32 0, %d \n", regNum, atoi($1));
    $$->code = newString(buffer);
    $$->regNum = regNum++;
    free($1); 
  }

  | CONSTANTF {
   $$ = createExpr();
   $$-> type = TYPE_FLOAT;
   sprintf(buffer, "\t%%r%d = fadd float 0, %f \n", regNum, atof($1));
   $$->code = newString(buffer);
   $$->regNum = regNum++;
   free($1); 
 }

 | '(' expression ')' { $$ = $2; }
 | IDENTIFIER '(' ')' { 
   $$ = createExpr();
   struct symbol_s * s;
   s = getSymbol($1);
   $$->regNum = regNum;
   sprintf(buffer, "\t%%r%d = call %s @%s()\n", regNum++, typeToLLVM(s->type) ,s->name);
   $$-> code = newString(buffer);
   free($1);
  //TODO : Ajouter la déclaration de la fonction  à la fin du fichier.
 }
| IDENTIFIER '(' argument_expression_list ')' {/*TODO*/}
 | IDENTIFIER INC_OP {
   $$ = createExpr();
   struct symbol_s * s = getSymbol($1);
   $$->regNum = s->regNum;
   $$->isPtr = (isPtr(s) || isArray(s)); 
   switch(s->type)
   {
    case TYPE_INT:
    sprintf(buffer, "\t%%r%d = add i32 1, %%r%d \n", regNum, s->regNum);
    break;
    case TYPE_FLOAT:
    if(!isPtr(s))
    {
      sprintf(buffer, "\t%%r%d = fadd float 1, %%r%d \n", regNum, s->regNum);
    }
    else
    {
      sprintf(buffer, "\t%%r%d = add i32 1, %%r%d \n", regNum, s->regNum);      
    }
    break;
    default:
    sprintf(error, "Identifier %s has invalid type for incrementation.", s->name);
    reportError();
  } 
  $$->symbol = NULL;
  s->regNum = regNum++;
  $$->code = newString(buffer);
  free($1);
}
| IDENTIFIER DEC_OP {
  $$ = createExpr();
  struct symbol_s * s = getSymbol($1);
  $$->regNum = s->regNum;
  $$->isPtr = (isPtr(s) || isArray(s)); 
  switch(s->type)
  {
    case TYPE_INT:
    sprintf(buffer, "\t%%r%d = sub i32 %%r%d, 1 \n", regNum, s->regNum);
    break;
    case TYPE_FLOAT:
    if(!isPtr(s))
    {
      sprintf(buffer, "\t%%r%d = fsub float %%r%d, 1 \n", regNum, s->regNum);
    }
    else
    {
      sprintf(buffer, "\t%%r%d = sub i32 %%r%d, 1 \n", regNum, s->regNum);
    }
    break;
    default:
    sprintf(error, "Identifier %s has invalid type for decrementation.", s->name);
    reportError();
  } 
  s->regNum = regNum++;
  $$->symbol = NULL;
  $$->code = newString(buffer);
  free($1);
}
;


postfix_expression
: primary_expression {$$ = $1; }
| postfix_expression '[' expression ']'  {

  $$ = createExpr();
  if(! ($1->isPtr))
  {
    sprintf(error, "Trying to reach something that is not a pointer or an array.");
    reportError();
  }
  else
  {
    sprintf(buffer, "\t%%r%d = load %s* $1 !ind\n\t!ind = !{i32 %%r%d}",
      regNum, typeToLLVM($1->type), $3->regNum );
    $$->code = appendString(concatString($1->code, $3->code), buffer);
    $$->regNum= regNum ++;
    $$->type =  $1-> type;
  }
  free ($1);
  free ($3);
} 
;

argument_expression_list
: expression  { $$ = createArgExprList($1) ;}
| argument_expression_list ',' expression {$$ = $1; appendArgExprList($$, $3);}
;

unary_expression
: postfix_expression { $$ = $1; }
| INC_OP unary_expression { 
  if ($2->symbol == NULL)
  {
    sprintf(error,"Expression is not assignable");
    reportError();
  }
  else{
   $$ = createExpr();
   struct symbol_s * s = $2->symbol ;
   $$->regNum = s->regNum;
   $$->isPtr = (isPtr(s) || isArray(s)); 
   switch(s->type)
   {
    case TYPE_INT:
    sprintf(buffer, "\t%%r%d = add i32 1, %%r%d \n", regNum, s->regNum);
    break;
    case TYPE_FLOAT:
    if(!isPtr(s))
    {
      sprintf(buffer, "\t%%r%d = fadd float 1, %%r%d \n", regNum, s->regNum);
    }
    else
    {
      sprintf(buffer, "\t%%r%d = add i32 1, %%r%d \n", regNum, s->regNum);      
    }
    break;
    default:
    sprintf(error, "Identifier %s has invalid type for incrementation.", s->name);
    reportError();
  } 
  $$->symbol = s;
  s->regNum = regNum;
  $$->regNum = regNum++;

  $$->code = newString(buffer);
  free($2);
}
}
| DEC_OP unary_expression {
  if ($2->symbol == NULL)
  {
    sprintf(error,"Expression is not assignable");
    reportError();
  }
  else{
   $$ = createExpr();
   struct symbol_s * s = $2->symbol;
   $$->regNum = s->regNum;
   $$->isPtr = (isPtr(s) || isArray(s)); 
   switch(s->type)
   {
    case TYPE_INT:
    sprintf(buffer, "\t%%r%d = sub i32 1, %%r%d \n", regNum, s->regNum);
    break;
    case TYPE_FLOAT:
    if(!isPtr(s))
    {
      sprintf(buffer, "\t%%r%d = fsub float %%r%d, 1 \n", regNum, s->regNum);
    }
    else
    {
      sprintf(buffer, "\t%%r%d = sub i32 %%r%d, 1 \n", regNum, s->regNum);      
    }
    break;
    default:
    sprintf(error, "Identifier %s has invalid type for decrementation.", s->name);
    reportError();
  } 
  $$->symbol = s;
  s->regNum = regNum;
  $$->regNum = regNum++;

  $$->code = newString(buffer);
  free($2);
}
}
| unary_operator unary_expression {
  $$ = createExpr();
  if($2->isPtr)
  {
    sprintf(error, "Unary operator - is forbidden for pointer expressions.");
  }
  switch($2->type)
  {
    case TYPE_INT:
    sprintf(buffer, "\t%%r%d = sub i32 0, %%r%d \n", regNum, $2->regNum);
    break;
    case TYPE_FLOAT:
    sprintf(buffer, "\t%%r%d = fsub float 0, %%r%d \n", regNum, $2->regNum);
    break;
    default:
    sprintf(error, "void or undef'd type-expression's opposite cannot be computed.");
    reportError();
    break;
  }
  $$->regNum = regNum++;
  $$->type = $2->type;
  $$->code = appendString($2->code, buffer);
  free($2);
}
;

unary_operator
: '-' 
;

multiplicative_expression
: unary_expression { $$ = $1;}
| multiplicative_expression '*' unary_expression {
  $$ = createExpr();
  char * operation;
  char * type;
  int result = regNum ++;
  int tmp = regNum;
  struct expression_s *floatingPoint, *i32;
  if ($1->type == TYPE_FLOAT && $3->type == TYPE_FLOAT)
  {
   operation = "fmul";
   $$ -> type = TYPE_FLOAT;
   type = "float";
   sprintf(buffer, "\t%%r%d = %s %s %%r%d, %%r%d \n", result, operation, type , $1->regNum, $3->regNum);
 }
 else if ($1->type == TYPE_INT && $3->type == TYPE_INT)
 {
  operation = "mul";
  $$ -> type = TYPE_INT;
  type = "i32";
  sprintf(buffer, "\t%%r%d = %s %s %%r%d, %%r%d \n", result, operation, type , $1->regNum, $3->regNum);
}
else 
{
  if ($1 ->type == TYPE_FLOAT)
  {
    floatingPoint = $1;
    i32 = $3;
  }
  else if ($3 ->type == TYPE_FLOAT)
  {
    floatingPoint = $3;
    i32 = $1;
  }
  operation = "fmul";
  $$ -> type = TYPE_FLOAT;
  type = "float";
  sprintf(buffer, "\t%%r%d = sitofp i32 %%r%d to float  \n"
    "\t%%r%d = %s %s %%r%d, %%r%d \n", tmp , i32->regNum, result, operation, type , tmp, floatingPoint->regNum);
}

$$ -> code = newString(buffer);
$$ -> regNum = regNum;
free($1);
free($3);

}
| multiplicative_expression '/' unary_expression
;

additive_expression
: multiplicative_expression {$$ = $1; }
| additive_expression '+' multiplicative_expression
| additive_expression '-' multiplicative_expression
;

comparison_expression
: additive_expression {$$ = $1; }
| additive_expression '<' additive_expression
| additive_expression '>' additive_expression
| additive_expression LE_OP additive_expression
| additive_expression GE_OP additive_expression
| additive_expression EQ_OP additive_expression
| additive_expression NE_OP additive_expression
;

expression
: unary_expression assignment_operator comparison_expression {
    /*
    int val = map_get_val($1); 
    if(val == VAL_UNDEF)
    {
      sprintf(error, "error: undeclared identifier %s", $1);
      yyerror(error);
      exit(EXIT_FAILURE);
    }
    else
    {
      
      char * regBuf = malloc(BUFSIZE * sizeof (char));
      map_set_val( $1, regNum);
      snprintf(regBuf, BUFSIZE, "%%reg%d = fadd ", regNum++);
      int rType = map_get_type($1), lType = map_get_type($3);
      if(rType != lType)
      {
        yyerror("implicit cast not supported (yet)");
        exit(EXIT_FAILURE);
      }
      else
      {
        char * LLVMRType = typeToLLVM(rType);
        char *left = strFusion( regBuf, LLVMRType);
        left = strFusion(left, strdup(" "));
        $$ = strFusion(strdup("\t") ,strFusion(left, $3));
        $$ = strFusion($$, strdup(", 1.0"));

      }
      
    }
  */
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
  : VOID {$$ = TYPE_VOID;}
  | INT    {$$ = TYPE_INT;}
  | FLOAT  {$$ = TYPE_FLOAT;}
  ;

  declarator
  : IDENTIFIER  {$$ = $1;}
  | '(' declarator ')' {}
  | declarator '[' CONSTANTI ']'
  | declarator '[' ']'
  | declarator '(' parameter_list ')' {$$ =  $1;}
  | declarator '(' ')'  {
    $$ = strFusion ( strFusion ($1, strdup("(")), strdup(")"));
  }
  ;

  parameter_list
  : parameter_declaration {}
  | parameter_list ',' parameter_declaration
  ;

  parameter_declaration
  : type_name declarator
  ;

  statement
  : compound_statement {$$ = $1;}
  | expression_statement { $$ = $1;}
  | selection_statement {}
  | iteration_statement {}
  | jump_statement {}
  ;

  block_overture
  : '{' {
  //engloberTable();
  }

  block_underture
  : '}' {
  //degloberTable();
  }

  compound_statement
  : block_overture block_underture {}
  | block_overture statement_list block_underture  {
    $$ = $2;
  }
  | block_overture declaration_list statement_list block_underture {}
  ;

  declaration_list
  : declaration
  | declaration_list declaration
  ;

  statement_list
  : statement {$$ = $1 ;}
  | statement_list statement { 
  }
  ;

  expression_statement
  : ';'  {}
  | expression ';' {

    /*$$ = strFusion ($1 , strdup("\n"));*/
  }
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
  : type_name declarator compound_statement {
    char parameters[] = "drive(i32 %index, %struct.CarElt* %car, %struct.Situation* %s)";
    char ctrl[] = "%ctrl = getelementptr %struct.CarElt* %car, i32 0, i32 5";
    char * type = typeToLLVM($1);
    //puts(header);
    printf("define %s @%s {\n\t%s\n%s%s%s\tret void\n}\n\n", type,  parameters, ctrl, getLLVMVarLoading() ,$3 ,  /*getLLVMVarStoring()*/ ""); 
    /*TODO : adapter LLVMVARSTORING pour utiliser la hashtable au lieu de la map*/
    //puts(end);
    free($2);free($3); 
  }
  ;

  %%
#include <stdio.h>
#include <string.h>

  extern int column;
  extern int yylineno;
  extern FILE *yyin;

  char *file_name = NULL;


  int yyerror (char *s) {
    fflush (stdout);
    fprintf (stderr, "%s:%d:%d: %s\n", file_name, yylineno, column, s);
    return 0;
  }


  int main (int argc, char *argv[]) {
    FILE *input = NULL;
    table = ht_create(1337);
    struct symbol_s *s = newSymbol("$accel", 123, TYPE_FLOAT);
    ht_set(table, "$accel", s);

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

  /*    map_init();*/
    regNum = initTorcsVars();

    yyparse ();
   /* map_end(); */
    ht_free( table );
    free (file_name);
    //yylex_destroy( yyscanner ));

    return 0;
  }
