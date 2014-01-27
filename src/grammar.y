%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <float.h>
#include "brutCode.h"
#include "util.h"
#include "torcsVars.h"

#include "hashtable.h"
#include "symbol.h"
#include "expression.h"
#include "type.h"
#include "str.h"
#include "environment.h"
#include "statement.h"

#define BUFSIZE 1024
#define IGNOREDLABEL 0



  char error[BUFSIZE] = "";
  char buffer[BUFSIZE] = "";

  char driveParameters[] = "(i32 %index, %struct.CarElt* %car, %struct.Situation* %s)";
  int labelNum = 1;
  int regNum = 1;
  extern int yylineno;
  int yylex ();
  int yyerror ();

  struct environment * environment, *params = NULL;


  void reportError()
  {
    yyerror(error);
    exit(EXIT_FAILURE);
  }


  struct symbol_s * getSymbol (char * name)
  {
    struct symbol_s * s;
    s = getSymbolFromEnv(params, name);
    if (NULL != s)
    {
      return s;
    }
    if (NULL == (s = getSymbolFromEnv(environment , name)))
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
    char car;
    float flt;
    struct String * string; 
    int nat;
    struct symbol_s * sym;
    struct expression_s * expr;
    struct argument_expression_list_s * ael;
    struct symbol_list * slst;
    struct statement * stm;
    struct statement_list * lstm;
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
  %type <car> assignment_operator
  %type <expr> primary_expression expression postfix_expression unary_expression 
  %type <expr> additive_expression comparison_expression multiplicative_expression
  %type <ael> argument_expression_list
  %type <sym> declarator parameter_declaration
  %type <slst> declarator_list parameter_list
  %type <lstm> statement_list
  %type <stm> compound_statement expression_statement statement selection_statement
  %type <stm> iteration_statement jump_statement
  %type <string> function_definition external_declaration

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
    float f = atof($1);
    sprintf(buffer, "\t%%r%d = fadd float %.17g, 0.000000 \n", regNum, f);
    $$->code = newString(buffer);
    $$->regNum = regNum++;
    free($1); 
  }

  | '(' expression ')' { $$ = $2; }
  | IDENTIFIER '(' ')' { 
    $$ = createExpr();
    struct symbol_s * s;
    s = getSymbol($1);
    if(s->params != NULL)
    {
      sprintf(error, "function %s expects arguments ; none found", $1);
    }
    $$->regNum = regNum;
    sprintf(buffer, "\t%%r%d = call %s @%s()\n", regNum++, typeToLLVM(s->type) ,s->name);
    $$-> code = newString(buffer);
    free($1);
  //TODO : Ajouter la déclaration de la fonction  à la fin du fichier.
  }
  | IDENTIFIER '(' argument_expression_list ')' 
  {
    struct symbol_s *s = getSymbol($1);
    struct symbol_list * sl = s->params;
    struct argument_expression_list_s *el = $3;
    struct expression_s *e = NULL;
    string * fcall = NULL;
    int count = 0;
    $$ = createExpr();

  /*TODO : traitement particulier pour les fonctions de TORCC*/

    sprintf(buffer, "\t%%r%d = call %s @%s (", regNum++, typeToLLVM(s->type) ,$1);
      fcall = newString(buffer);
  /* checking args match function prototype */

      while(sl != NULL)
      {
        if(count != 0)
        {
          fcall = appendString(fcall, ", ");
        }
        count++;
        if(el == NULL)
        {
          sprintf(error, "function %s expects more arguments, %d found", $1, count);
          reportError();
        }
        if (sl->symbol->type != el->expr->type)
        {
          fprintf(stderr, "Warning: implicit conversion of argument %d of function %s from %s to %s\n",
           count, $1, typeToCType(el->expr->type), typeToCType(sl->symbol->type));
          e = convertExpression(sl->symbol->type, el->expr, &regNum);
          $$->code = concatString($$->code, e->code);
          sprintf(buffer, "%s %%r%d", typeToLLVM(e->type), e->regNum);
          freeExpression(e);
        }
        else
        {
          sprintf(buffer, "%s %%r%d", typeToLLVM(el->expr->type), el->expr->regNum);
          $$->code = concatString($$->code, el->expr->code);
        }
        freeExpression(el->expr);
        fcall = appendString(fcall, buffer);

        sl = sl->next;
        el = el->next;
      }
      $$->code = appendString(concatString($$->code, fcall), ")\n");
      $$->type = s->type;
    }
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
          sprintf(buffer, "\t%%r%d = fadd float 1.000000, %%r%d \n", regNum, s->regNum);
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
      $$->type = s->type;
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
          sprintf(buffer, "\t%%r%d = fsub float %%r%d, 1.000000 \n", regNum, s->regNum);
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
      $$->type = s->type;
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
      if ($2->symbol == NULL || !(isWritable($2->symbol)))
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
           sprintf(buffer, "\t%%r%d = fadd float 1.000000, %%r%d \n", regNum, s->regNum);
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
       $$->type = s->type;
       $$->code = newString(buffer);
       free($2);
     }
   }
   | DEC_OP unary_expression {
    if ($2->symbol == NULL || !isWritable($2->symbol))
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
         sprintf(buffer, "\t%%r%d = fsub float %%r%d, 1.000000 \n", regNum, s->regNum);
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
     $$->type = s->type;
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
    sprintf(buffer, "\t%%r%d = fsub float 0.000000, %%r%d \n", regNum, $2->regNum);
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
  $$ = createMulExpr("mul", $1, $3, &regNum);
  free($1);
  free($3);

}

| multiplicative_expression '/' unary_expression 
{
  $$ = createMulExpr("div", $1, $3, &regNum);
  free($1);
  free($3);
}
;

additive_expression
: multiplicative_expression { $$ = $1; }
| additive_expression '+' multiplicative_expression 
{
  $$ = createMulExpr("add", $1, $3, &regNum);
  free($1);
  free($3);
}
| additive_expression '-' multiplicative_expression
{
  $$ = createMulExpr("sub", $1, $3, &regNum);
  free($1);
  free($3);
}
;

comparison_expression
: additive_expression {$$ = $1; }
| additive_expression '<' additive_expression {
  $$ = createComparisonExpr("cmp slt", $1, $3, &regNum);
  free($1);
  free($3);
}
| additive_expression '>' additive_expression {
  $$ = createComparisonExpr("cmp sgt", $1, $3, &regNum);
  free($1);
  free($3);
}
| additive_expression LE_OP additive_expression
{
  $$ = createComparisonExpr("cmp sle", $1, $3, &regNum);
  free($1);
  free($3);
}
| additive_expression GE_OP additive_expression
{
  $$ = createComparisonExpr("cmp sge", $1, $3, &regNum);
  free($1);
  free($3);
}
| additive_expression EQ_OP additive_expression
{
  $$ = createComparisonExpr("cmp eq", $1, $3, &regNum);
  free($1);
  free($3);
}
| additive_expression NE_OP additive_expression
{
  $$ = createComparisonExpr("cmp neq", $1, $3, &regNum);
  free($1);
  free($3);
}
;

expression
: unary_expression assignment_operator comparison_expression {

  if ($1-> symbol == NULL || !isWritable($1->symbol))
  {
    sprintf(error, "Expression is not assignable");
    reportError();
  }
  else
  {
    $$ = createFinalExpr($1, $2, $3, &regNum);
  }
}
| comparison_expression { $$ = $1;}
;

assignment_operator
: '=' { $$ = '=' ;}
| MUL_ASSIGN {$$ = '*' ;} 
| ADD_ASSIGN {$$ = '+' ;}
| SUB_ASSIGN {$$ = '-' ;}
;

declaration
: type_name declarator_list ';' 
{
  struct symbol_list * l = $2;

  //liberation de la liste de parametres produite si une fonction etait declaree
  if(params != environment)
  {
    freeEnvironment(params, DONTFREESYMBOLS);
    params = NULL;
  }
  while(l != NULL)
  {
    if(isPtr(l->symbol))
    {
      sprintf(error, "size unknown for array %s", l->symbol->name);
      reportError();
    }
    if($1 != TYPE_INT && $1 != TYPE_FLOAT && !isFunction(l->symbol))
    {
      sprintf(error, "forbidden type for declaration of %s (void or bad type)", l->symbol->name);
      reportError();
    }
    l->symbol->type = $1;
    setSymbol(environment, l->symbol, l->symbol->name);
    l = l->next;
  }
}
;

declarator_list
: declarator 
{
  $$ = createList($1);
}
| declarator_list ',' declarator
{
  $$ = $1;
  addSymbolInList($$, $3);
}
;

type_name
: VOID {$$ = TYPE_VOID;}
| INT    {$$ = TYPE_INT;}
| FLOAT  {$$ = TYPE_FLOAT;}
;

declarator
: IDENTIFIER  {$$ = newSymbol($1, regNum++, TYPE_UNDEF, IS_WR);}
| '(' declarator ')' {$$ = $2;}
| declarator '[' CONSTANTI ']'
{
  int n = atoi($3);
  if(n == 0)
  {
    sprintf(error, "ISO C forbids 0-size arrays.");
    reportError();
  }
  $$ = $1;
  $$->flags = ($$->flags) | IS_WR | IS_WR;
  $$->arrayArity = n;
}
| declarator '[' ']' 
{
  $$ = $1;
  $$->flags = ($$->flags) | IS_WR | IS_PTR;
}
| declarator '(' parameter_list ')' 
{
  $$ = $1;
  $$->params = $3;
  $$->flags = $$->flags | IS_FUNC;
  struct symbol_list * lst = $3;
  int count =0;
  $$->paramsCode = newString("(");
    while(lst != NULL)
    {
      if(count != 0)
      {
        $$->paramsCode = appendString($$->paramsCode, ", ");
      }
      sprintf(buffer, "%s %%r%d", typeToLLVM(lst->symbol->type), lst->symbol->regNum );
      $$->paramsCode = appendString($$->paramsCode, buffer);
      count ++;
      lst = lst->next;
    }
    $$->paramsCode = appendString($$->paramsCode, ")");

  }
  | declarator '(' ')'  {
    $$ = $1;
    $$->flags = $$->flags | IS_FUNC;
  /* params already equal to NULL */ 
  }
  ;

  parameter_list
  : parameter_declaration
  {
    if (params != environment)
    {
      freeEnvironment(params, DONTFREESYMBOLS);
      params = NULL;
    }
    $$ = createList($1);
    params = createEnvironment();
    setSymbol(params, $1, $1->name);

  }
  | parameter_list ',' parameter_declaration
  {
    $$ = $1;
    addSymbolInList($1, $3);
    setSymbol(params, $3, $3->name);
  }
  ;

  parameter_declaration
  : type_name declarator {
    $$ = $2;
    switch($1)
    {
      case TYPE_VOID:
      if(!isFunction($2))
      {
        sprintf(error, "variable %s has incomplete type void", $2->name );
        reportError();
      }
      break;
      case TYPE_INT:
      case TYPE_FLOAT:
      $$->type = $1;
      break;
      default:
      sprintf(error, "symbol %s is error-typed", $2->name );
      reportError();
    }
  }
  ;

  statement
  : compound_statement {$$ = $1;}
  | expression_statement { $$ = $1;}
  | selection_statement {$$ = $1;}
  | iteration_statement {$$ = $1;}
  | jump_statement {$$ = $1;}
  ;

  block_overture
  : '{' {
    environment = englober( createEnvironment(), environment);
  }

  block_underture
  : '}' {
    environment = deglober(environment);
  }

  compound_statement
  : block_overture block_underture {$$ = createStatement();}
  | block_overture statement_list block_underture  {
    $$ = createStatement();
    $$->returns = $2->returns;
    $$->returnType = $2->returnType;
    $$->code = $2->code;
  }
  | block_overture declaration_list statement_list block_underture {
    $$ = createStatement();
    $$->returns = $3->returns;
    $$->returnType = $3->returnType;
    $$->code = $3->code;
  }
  ;

declaration_list /* aucun code ; referencement dans les tables fait au niveau des declaration's */
  : declaration 
  | declaration_list declaration
  ;

  statement_list 
    // TODO : Convertir le registre retourné vers le type de retour de la fonction.
  : statement {
    $$ = createStatementList();
    $$->code = $1->code ;
    $$->returns = $$-> lastStatementReturns = $1->returns;
    $$->returnType = $1 ->returnType;
    freeStatement($1);
  }
  | statement_list statement { 
    if($1->returnType != $2->returnType)
    {
      sprintf(error, "return types inconsistent.\n");
      reportError();
    }
    $$->returnType = $1->returnType;
    $$ = createStatementList();
    $$->code = concatString($1->code, $2->code);
    $$->returns = ($1->returns || ($$->lastStatementReturns = $2->returns));
    freeStatementList($1);
    freeStatement($2);
  }
  ;

  expression_statement
  : ';'
  {
    $$ = createStatement();
    $$->code = NULL;
  }
  | expression ';' 
  {
    $$ = createStatement();
    $$->code = $1->code;
    freeExpression($1);
  }
  ;

selection_statement /* TODO : Vérifier si tous les branchements conditionnels retournent */
  : IF '(' expression ')' statement
  {

    $$ -> returns =  $5 -> returns ;
    int labelTrue = labelNum++, labelFalse = labelNum++;
    $$ = createStatement();

    $$ -> code = appendBranch($3->code, $3->regNum, labelTrue, labelFalse );
    $$ -> code = appendLabel($$-> code, labelTrue);
    $$ -> code = concatString($$->code, $5->code );
    $$ -> code = appendLabel($$->code, labelFalse);

    freeExpression($3);
    freeStatement($5);
  }
  | IF '(' expression ')' statement ELSE statement
  {
    $$ -> returns =  $5 -> returns && $7 ->returns ;
    int labelTrue = labelNum++, labelFalse = labelNum++, labelEnd = labelNum ++;
    $$ = createStatement();

    $$ -> code = appendBranch($3->code, $3->regNum, labelTrue, labelFalse );
    $$ -> code = appendLabel($$-> code, labelTrue);
    $$ -> code = concatString($$->code, $5->code );
    $$ -> code = appendBranch($$-> code, 0, labelEnd, IGNOREDLABEL);
    $$ -> code = appendLabel($$->code, labelFalse);
    $$ -> code = concatString($$->code, $7->code);
    $$ -> code = appendLabel($$->code, labelEnd);

    freeExpression($3);
    freeStatement($5);
    freeStatement($7);
  }
  | FOR '(' expression_statement expression_statement expression ')' statement
  {
// TODO
  }
  ;

  iteration_statement
: WHILE '(' expression ')' statement {//TODO
}
;

jump_statement // ->returns = 1
: RETURN ';' {
  $$ = createStatement();
  $$ ->returnType = TYPE_VOID;
  $$->returns = 1; 
  char bigbuffer[4*BUFSIZE] = "";
  getLLVMVarStoring(environment , bigbuffer );
  $$ -> code = appendString ($$->code, bigbuffer);
  hideRightString($$->code);
  $$ ->code = appendString($$->code, "\tret void\n");

}
| RETURN expression ';' {
  $$ = createStatement();
  $$ ->returns= 1;
  $$ ->returnType = $2->type;
  char bigbuffer[4*BUFSIZE] = "";
  getLLVMVarStoring(environment , bigbuffer );
  sprintf(buffer, "\tret %s %%r%d\n", typeToLLVM($2->type), $2->regNum);
  $$ ->code = concatString($2->code, $$->code);
  $$ ->code = appendString ($$->code , bigbuffer);
  hideRightString($$->code);
  $$ ->code = appendString( $$->code,  buffer);
}
;

program
: external_declaration {
  //printString(stdout, $1);
}
| program external_declaration
{
  //printString(stdout, $2); 
}
;

external_declaration
: function_definition {$$ = $1;}
| declaration {$$ = newString(NULL);}
;

function_definition
: type_name declarator compound_statement {
  if ($1 != $3->returnType)
  {
    sprintf(error, "function %s has return-type %s, but %s was found",
     $2->name, typeToCType($1), typeToCType($3->returnType));
    reportError();
  }
  $2->type = $1;
  /*TODO : verifier la compatibilite du prototype eventuel*/
  setSymbol(environment, $2, $2->name);

  sprintf(buffer, "define %s @%s", typeToLLVM($1), $2->name);
  $$ = newString(buffer);
  /*Definition des arguments*/
  if (strcmp($2->name, "drive") == 0)
  {
    if ($2->params != NULL)
    {
      sprintf(error, "parameters were found in function drive, which should have none.");
      reportError();
    }
    $$ = appendString($$, driveParameters);
  }
  else
  {
    $$ = concatString($$, $2->paramsCode);
  }


  $$ = appendString($$, "{\n");
// Importation des variables de TORCS
  if (strcmp($2->name, "drive") == 0)
  {
    char bigbuffer[4*BUFSIZE]= "";
    getLLVMVarLoading(environment , bigbuffer );
    $$ = appendString ($$, bigbuffer);
  }

//Generation du corps
  $$ = concatString($$, $3->code);
  if(! ($3->returns))
  {
    if($1 == TYPE_VOID)
    {
      char bigbuffer[4*BUFSIZE]= "";
      getLLVMVarStoring(environment , bigbuffer);
      $$= appendString($$ , bigbuffer);
      $$= appendString($$ ,"\tret void\n");
    }
    else
    {
      fprintf(stderr, "Warning: function %s has non-void return type and"
       " not every logical path return, return value defaults to 0.\n", $2->name );
      sprintf(buffer, "\tret %s 0\n",typeToLLVM($1));
      $$ = appendString($$, buffer);
    }
  }

  // Enregistrement des variables de TORCS faite dans le return


  $$ = appendString($$, "}\n");
  if(strcmp($2->name, "drive") == 0)
    printWholeString(stdout, $$);
  else
    printString(stdout, $$);
  destroyString($$);
  freeStatement($3); 

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
  environment = createEnvironment();
  struct symbol_s *s = newSymbol("$accel", 40037, TYPE_FLOAT, IS_WR | IS_TORCSIAN);
  setSymbol(environment, s, "$accel");

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
  regNum = initTorcsVars(environment);
  declareFunctionMasks(environment);

  puts(header);
  yyparse ();
  puts(end);
  fputs("Compilation successful.\n", stderr);
  /* map_end(); */
  freeEnvironment( environment , DONTFREESYMBOLS);
  environment = NULL;
  free (file_name);
  //yylex_destroy( yyscanner ));

  return 0;
}
