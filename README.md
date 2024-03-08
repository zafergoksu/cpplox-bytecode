# cpplox

An implementation for the compiler and virtual machine (VM) for the garbage-collected, object-orientated, dynamic programming language: 'Lox'.

A recursive-descent parser is a literal trnaslation of the grammar's rules straight into imperative code, each rule in
the grammar is a function/ code.

## Grammar

The grammar for Lox, where precedence is low to high in order (and `unary` is right associative):

```
program         ::= statement* EOF ;
declaration     ::= classDecl | funDecl | varDecl | statement;
statement       ::= exprStmt | forStmt | ifStmt | printStmt | returnStmt | whileStmt | block ;
exprStmt        ::= expression ";" ;
forStmt         ::= "for" "(" ( varDecl | exprStmt | ";" ) expression? ";" expression? ")" statement ;
ifStmt          ::= "if" "(" expression ")" statement ( "else" statement )? ;
printStmt       ::= "print" expression ";" ;
returnStmt      ::= "return" expression? ";" ;
whileStmt       ::= "while" "(" expression ")" statement ;
block           ::= "{" declaration* "}" ;

classDecl       ::= "class" IDENTIFIER ( "<" IDENTIFIER )? {" function* "}" ;
funDecl         ::= "fun" function ;
function        ::= IDENTIFIER "(" parameters? ")" block ;
parameters      ::= IDENTIFIER ( "," IDENTIFIER )* ;
varDecl         ::= "var" IDENTIFIER ( "=" expression )? ";" ;
expression      ::= assignment ;
assignment      ::= ( call "." )? IDENTIFIER "=" assignment | logic_or ;
logic_or        ::= logic_and ( "or" logic_and )* ;
logic_and       ::= equality ( "and" equality )* ;
equality        ::= comparison ( ( "!=" | "==" ) comparison )* ;
comparison      ::= term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term            ::= factor ( ( "-" | "+" ) factor )* ;
factor          ::= unary ( ( "/" | "*" ) unary )*;
unary           ::= ( "!" | "-" ) unary | call ;
call            ::= primary ( "(" arguments? ")" | "." IDENTIFIER )* ;
arguments       ::= expression ( "," expression )* ;
primary         ::= NUMBER | STRING | "true" | "false" | "this" | "nil" | IDENTIFIER | "(" expression ")" | "super" "." IDENTIFIER ;
```
