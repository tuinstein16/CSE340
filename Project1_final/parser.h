/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"
#include <map>
#include <vector>

using namespace std;
string name;
map<string , int> location;
//map<string, int> pc_location;
int next_available = 0;
int pc_next_available = 0;
vector<int> mem;
vector<int> inputs;
vector<struct stmt*> statements;
int next_input = 0;
int next_st;
// Q2: How could we create a procedure_table?
//vector<proc *> pc_table;
map<string , vector<struct stmt*>> pc_table;

struct stmt {
    int statement_type;  // 0 ASSIGN, 1 INPUT, 2 OUTPUT, 3 INVOKE, 4 DO.
    int LHS;
    int op;  // 0 PLUS, 1 MINUS, 2 DIV, 3 MULT, or 4 NOOP
    int op1;
    int op2;
    //relate procedure_table's pc_name with structure stmt?
    string pc_name;
    string count_id;

    struct stmt * next;
};

class Parser {
  public:
    void parse_input();
    void readAndPrintAllInput();

    //map<string , int> location;
    //map<string, int> pc_location;
    //int next_available = 0;
    //int pc_next_available = 0;
    //vector<int> mem;
    //vector<int> inputs;
    //int next_input = 0;
    //string name;
    // Q2: How could we create a procedure_table?
    //vector<proc *> pc_table;
    //map<string , stmt*> pc_table;

    //my code
    void execute_program(struct stmt * start);
    void parse_input_program();
    void parse_proc_decl_section();
    void parse_proc_decl();
    string parse_procedure_name();
    void parse_procedure_body();
    void parse_statement_list();
    void parse_statement();
    struct stmt* parse_input_statement();
    struct stmt* parse_output_statement();
    string parse_procedure_invocation();
    struct stmt* parse_do_statement();
    struct stmt* parse_assign_statement();
    struct stmt* parse_expr();
    int parse_operator();
    void parse_primary();
    void parse_main();
    void parse_inputs();
  private:
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);
};

#endif

