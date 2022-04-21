/*
 * Copyright (C) Rida Bazzi, 2022
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <iostream>
#include <cstdlib>
#include "parser.h"
#include <map>
#include <vector>
#include <string>
#include <stdlib.h>
using namespace std;



void Parser::execute_program(struct stmt * start) {
    struct stmt * pc;
    pc = start;
    while (pc != NULL) {
        switch (pc->statement_type) {
            case 0:
                switch (pc->op) {
                    case 0:
                        mem[pc->LHS] = mem[pc->op1] + mem[pc->op2];
                        //printf("%d is: %d\n", pc->LHS, mem[pc->LHS]);
                        //printf("%d is: %d\n", pc->op1, mem[pc->op1]);
                        //printf("%d is: %d\n", pc->op2, mem[pc->op2]);                        
                        break;
                    case 1:
                        mem[pc->LHS] = mem[pc->op1] - mem[pc->op2];
                        //printf("%d is: %d\n", pc->LHS, mem[pc->LHS]);                         
                        break;
                    case 3:
                        mem[pc->LHS] = mem[pc->op1] * mem[pc->op2];
                        //printf("%d is: %d\n", pc->LHS, mem[pc->LHS]); 
                        break;
                    case 4:
                        mem[pc->LHS] = mem[pc->op1] / mem[pc->op2];
                        //printf("%d is: %d\n", pc->LHS, mem[pc->LHS]); 
                        break;
                    case 2:
                        mem[pc->LHS] = mem[pc->op1];
                        break;
                }
                break;
            case 2:
                cout << mem[pc->op1] << " ";
                break;
            case 1:
                mem[pc->op1] = inputs[next_input];
                next_input++;
                break;
            case 3:
                // Q3: What should we do in order to execute a procudure?
                for (int i = 0; i < pc_table[pc->pc_name].size(); i++)
                    execute_program(pc_table[pc->pc_name][i]);

                break;
            case 4:
                // Q4: How do we utilize the repeat number?
                int the_c = mem[location[pc->count_id]];
                for (int m = 0; m < the_c;m++)
                {
                    string the_p_name = pc->pc_name; 
                    for (int i = 0; i < pc_table[the_p_name].size(); i++)
                        execute_program(pc_table[the_p_name][i]);
                    //execute_program(pc_table[pc->pc_name]);
                }
                //printf("0 is: %d\n", mem[0]);
                break;
        }
        pc = pc->next;
    }
}

void Parser::syntax_error()
{
    cout << "SYNTAX ERROR !!!\n";
    exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

// This function simply reads and prints all tokens
// I included it as an example. You should compile the provided code
// as it is and then run ./a.out < tests/test0.txt to see what this 
// function does
void Parser::readAndPrintAllInput()
{
    Token t;

    // get a token
    t = lexer.GetToken();

    // while end of input is not reached
    while (t.token_type != END_OF_FILE) 
    {
        t.Print();         	// pringt token
        t = lexer.GetToken();	// and get another one
    }
        
    // note that you should use END_OF_FILE and not EOF
}

//the parsers 
void Parser::parse_input()
{
    //input --> input_program + inputs
    parse_input_program();
    parse_inputs();
    expect(END_OF_FILE);
    return;
}

void Parser::parse_input_program()
{
    Token t = lexer.peek(1);
    if (t.token_type == MAIN)       //input_program --> main
    {
        parse_main();
    }
    else if (t.token_type == PROC)  //input_program --> proc_decl_section + main
    {
        parse_proc_decl_section();
        parse_main();
    }
    else 
        syntax_error();

    //check if it is followed by NUM
    t = lexer.peek(1);
    if (t.token_type == NUM)
        return;
    else
        syntax_error();
    
}
void Parser::parse_proc_decl_section()
{
    parse_proc_decl();
    //check if it is followed by proc_decl_section
    Token t0 = lexer.peek(1);
    if (t0.token_type == PROC)
        parse_proc_decl_section();
    else if (t0.token_type == MAIN)
        return;
    else 
        syntax_error();

}

void Parser::parse_proc_decl()
{
    expect(PROC);
    name = parse_procedure_name();
    parse_procedure_body();
    pc_table.insert(pair<string, vector<struct stmt*>>(name, statements));
    statements.clear();
    expect(ENDPROC);
    //if ... return
    Token t0 = lexer.peek(1);
    if((t0.token_type == PROC) | (t0.token_type == MAIN))
        return;
    else 
        syntax_error();
}

string Parser::parse_procedure_name()
{
    
    Token t = lexer.peek(1);

    //struct stmt* st = new stmt;
    name = t.lexeme;

    if (t.token_type == ID)
    {
        //pc_table.insert(pair<string, stmt*>(name, st));
        expect(ID);
    }
    else if (t.token_type == NUM)
    {
        //pc_table.insert(pair<string, stmt*>(name, st));
        expect(NUM);
    }
    else
        syntax_error();
    
    Token t0 = lexer.peek(1);
    if((t0.token_type == SEMICOLON) | (t0.token_type == ID) | (t0.token_type == NUM) | (t0.token_type == INPUT) | (t0.token_type == OUTPUT) | (t0.token_type == DO))
        return name;
    else 
        syntax_error();
}

void Parser::parse_procedure_body()
{
    parse_statement_list();
    //if... return;
    Token t0 = lexer.peek(1);
    if((t0.token_type == NUM) | (t0.token_type == ENDPROC))
        return;
    else 
        syntax_error();
}
void Parser::parse_statement_list()
{
    parse_statement();
    Token t0 = lexer.peek(1);
    Token t2 = lexer.peek(2);
    if((t0.token_type == ID) | (t0.token_type == INPUT) | (t0.token_type == OUTPUT) | (t0.token_type == DO))
    {

        parse_statement_list();
    }
    //if... return
    else if (t0.token_type == NUM)
    {
        //it is follosed by statement_list
        if (t2.token_type == SEMICOLON)
            {
                parse_statement_list();
            }
        else //it is inputs (end statement list)
            return;
    }
    
    else if (t0.token_type == ENDPROC)
        return;
    else 
        syntax_error();
}
void Parser::parse_statement()
{
    Token t1 = lexer.peek(1);
    Token t2 = lexer.peek(2);
    if (t1.token_type == INPUT)
    {
        statements.push_back(parse_input_statement());
        
    }
    else if (t1.token_type == OUTPUT)
    {
        statements.push_back(parse_output_statement());
    }
    else if (t1.token_type == DO)
    {
        statements.push_back(parse_do_statement());
    }
    else if ((t1.token_type == ID) & (t2.token_type == EQUAL))
    {
        statements.push_back(parse_assign_statement());
    }
    //???
    else if (((t1.token_type == ID) | (t2.token_type == NUM)) & (t2.token_type == SEMICOLON))
    {
        string proc_name = parse_procedure_invocation();
        stmt* st = new stmt;
        st->statement_type = 3;
        st->op = 4;
        st->pc_name = proc_name;
        st->op1 = -1;
        st->op2 = -1;
        st->LHS = -1;
        st->next = NULL;
        statements.push_back(st);
    }
    else 
        syntax_error();

    Token t0 = lexer.peek(1);
    if((t0.token_type == NUM) | (t0.token_type == ENDPROC))
        return;
    else if((t0.token_type == ID) | (t0.token_type == NUM) | (t0.token_type == INPUT) | (t0.token_type == OUTPUT) | (t0.token_type == DO))
        return;
    else
        syntax_error();    
}
struct stmt* Parser::parse_input_statement()
{
    //INPUT X;
    struct stmt* st = new stmt;
    expect(INPUT);

    Token t = lexer.peek(1);
    //insert the input to an input table
    //set the struct variable
    location.insert(pair<string, int>(t.lexeme, next_available++));
    mem.push_back(0);
    st->statement_type = 1;
    st->op1 = location[t.lexeme];

    st->pc_name = -1;
    st->LHS = -1;
    st->op = 4;
    st->op2 = -1;
    st->next = NULL;

    //how to connect a staement with the next?
    //st4->next = st5;
    expect(ID);
    expect(SEMICOLON);
    //if...return
    Token t0 = lexer.peek(1);
    if((t0.token_type == ID) | (t0.token_type == NUM) | (t0.token_type == ENDPROC) | (t0.token_type == INPUT) | (t0.token_type == OUTPUT) | (t0.token_type == DO))
        return st;
    else
        syntax_error();
} 
struct stmt* Parser::parse_output_statement()
{
    Token t = lexer.peek(1);

    expect(OUTPUT);
    //set the stmt variables
    t = lexer.peek(1);
    stmt * st9 = new stmt;
    st9->statement_type = 2; //output
    st9->LHS = -1;
    st9->op = 4;
    if (location.find(t.lexeme) != location.end())
        {
            st9->op1 = location[t.lexeme];
        }
        else
        {
            location.insert(pair<string, int>(t.lexeme, next_available++));
            mem.push_back(0);
            st9->op1 = location[t.lexeme];
        }
    st9->op2 = -1;
    st9->next = NULL;
    //st8->next = st9;
    expect(ID);
    expect(SEMICOLON);
    Token t0 = lexer.peek(1);
    if((t0.token_type == ID) | (t0.token_type == NUM) | (t0.token_type == ENDPROC) | (t0.token_type == INPUT) | (t0.token_type == OUTPUT) | (t0.token_type == DO))
        return st9;
    else
        syntax_error();
}
string Parser::parse_procedure_invocation()
{
    string theName = parse_procedure_name();//it should return a name
    //stmt * st = new stmt;
    //st->statement_type = 3; //procedure
    // Q10: How do we store the name of the procedure?
    //st->pc_name = theName;
    //st->LHS = -1;
    //st->op = 4;
    //st->op1 = -1;
    //st->op2 = -1;
    expect(SEMICOLON);
    //if...return
    Token t0 = lexer.peek(1);
    if((t0.token_type == ID) | (t0.token_type == NUM) | (t0.token_type == ENDPROC) | (t0.token_type == INPUT) | (t0.token_type == OUTPUT) | (t0.token_type == DO))
        return theName;
    else
        syntax_error();

}
struct stmt* Parser::parse_do_statement()
{
    expect(DO);
    Token t = lexer.peek(1);
    stmt * st8 = new stmt;
    st8->statement_type = 4;
    // Q11: How do we store the number of repeat?
    //st8->pc_name = "INCX";
    st8->LHS = -1;
    st8->op = 4;
    st8->op1 = -1;
    st8->op2 = -1;
    st8->next = NULL;
    st8->count_id = t.lexeme;
    //cout << st8->count << "\n";
    
    stmt * st_proc = new stmt;
    expect(ID);
    st8->pc_name = parse_procedure_invocation();//what should it return?
    Token t0 = lexer.peek(1);
    if((t0.token_type == ID) | (t0.token_type == NUM) | (t0.token_type == ENDPROC) | (t0.token_type == INPUT) | (t0.token_type == OUTPUT) | (t0.token_type == DO))
        return st8;
    else
        syntax_error();
}
struct stmt* Parser::parse_assign_statement()
{
    Token t = lexer.peek(1);
    stmt * st = new stmt;
    //assign location of ID
    if (location.find(t.lexeme) != location.end())
        {
            st->LHS = location[t.lexeme];
        }
    else
        {
            location.insert(pair<string, int>(t.lexeme, next_available++));
            mem.push_back(0);
            st->LHS = location[t.lexeme];
        }
    expect(ID);
    expect(EQUAL);
    stmt * expr = new stmt;
    expr = parse_expr();
    st->statement_type = 0;
    st->op = expr->op;
    st->op1 = expr->op1;
    st->op2 = expr->op2;
    st->next = NULL;
    expect(SEMICOLON);
    //if...return
    Token t0 = lexer.peek(1);
    if((t0.token_type == ID) | (t0.token_type == NUM) | (t0.token_type == ENDPROC) | (t0.token_type == INPUT) | (t0.token_type == OUTPUT) | (t0.token_type == DO))
        return st;
    else
        syntax_error();
}
struct stmt* Parser::parse_expr()
{
    stmt* st = new stmt;
    //index1 = 
    Token t0 = lexer.peek(1);
    parse_primary();
    if (t0.token_type == NUM)
    {
        //int int1 = stoi(t0.lexeme);
        //int int2 = mem[0];
        //int int3 = mem[1];
        mem.push_back(stoi(t0.lexeme));
        next_available++;
        st->op1 = next_available - 1;
    }
    else if (t0.token_type == ID)
    {
        //it is in the mem table
        if (location.find(t0.lexeme) != location.end())
        {
            st->op1 = location[t0.lexeme];
        }
        else
        {
            location.insert(pair<string, int>(t0.lexeme, next_available++));
            st->op1 = location[t0.lexeme];
            mem.push_back(0);
        }
    }
    Token t = lexer.peek(1);
    //if is.operator(t.token_type);
    if ((t.token_type == PLUS) | (t.token_type == DIV) | (t.token_type == MINUS) | (t.token_type == MULT))
        {
            //op
            st->op = parse_operator();
            //index2 =
            t0 = lexer.peek(1);
            if (t0.token_type == NUM)
            {
                
                mem.push_back(stoi(t0.lexeme));
                
                next_available++;
                st->op2 = next_available - 1;

            }
            else if (t0.token_type == ID)
            {
                //it is in the mem table
                if (location.find(t0.lexeme) != location.end())
                {
                    st->op2 = location[t0.lexeme];
                }
                else
                {
                    location.insert(pair<string, int>(t0.lexeme, next_available++));
                    mem.push_back(0);
                    st->op2 = location[t0.lexeme];
                }
            }
            parse_primary();
        }
    else
    {
        st->op2 = -1;
        st->op = 2;
    }
        
    
    //check if it is followed by semicolon
    t = lexer.peek(1);
    if (t.token_type == SEMICOLON) 
        return st;
    else
        syntax_error(); 

}
int Parser::parse_operator()
{
    Token t = lexer.peek(1); //check if it is +/-/*/div
    int oper;
    if (t.token_type == PLUS)
    {
        expect(PLUS);
        oper = 0;
    }
    else if (t.token_type == MINUS)
    {
        expect(MINUS);
        oper = 1;
    }
    else if (t.token_type == MULT)
    {
        expect(MULT);
        oper = 3;
    }
    else if (t.token_type == DIV)
    {
        expect(DIV);
        oper = 4;
    }
    else
        syntax_error();
    t = lexer.peek(1);
    if ((t.token_type == ID) | (t.token_type == NUM)) //check if it is followed by primary --> ID/NUM
        return oper;
    else
        syntax_error();
}

void Parser::parse_primary()
{
    Token t = lexer.peek(1);
    if (t.token_type == ID)
    {
        expect(ID);
    }
    else if (t.token_type == NUM)
    {
        expect(NUM);
    }
        
    else
        syntax_error();

    //check if it is followed by semicolon
    Token t1 = lexer.peek(1);
    if ((t1.token_type == SEMICOLON) | (t1.token_type == PLUS) | (t1.token_type == DIV) | (t1.token_type == MINUS) | (t1.token_type == MULT)) 
        return;
    else
        syntax_error();
     
}

void Parser::parse_main()
{
    expect(MAIN);        //consume MAIN
    parse_procedure_body();
    
    //check if it is followed by NUM
    Token t = lexer.peek(1);
    if (t.token_type == NUM)
        return;
    else
        syntax_error();

}

void Parser::parse_inputs()
{
    inputs.push_back(stoi(expect(NUM).lexeme));        //consume NUM     
    Token t = lexer.peek(1);        //check if the next token is NUM of EOF
    if (t.token_type == NUM)
    {
        parse_inputs();     //inputs --> NUM + inputs
    } 
    else if (t.token_type == END_OF_FILE)
        return;     //inputs --> NUM
    else
        syntax_error();

}

int main()
{
    // note: the parser class has a lexer object instantiated in it (see file
    // parser.h). You should not be declaring a separate lexer object. 
    // You can access the lexer object in the parser functions as shown in 
    // the example  method Parser::readAndPrintAllInput()
    // If you declare another lexer object, lexical analysis will 
    // not work correctly
    Parser parser;
    //x77 =1;
    //parser.readAndPrintAllInput();
    parser.parse_input();
    for (next_st = 0; next_st< statements.size(); next_st++)
    {
        parser.execute_program(statements[next_st]);
    }

	
}
