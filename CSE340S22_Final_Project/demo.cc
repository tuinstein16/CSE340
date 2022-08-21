#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "execute.h"
#include "lexer.h"
#include <iostream>
#include <map>
#include <vector>

using namespace std;

LexicalAnalyzer lexer;

struct expr{
    Token operand1;
    ArithmeticOperatorType expr_op;
    Token operand2;
};

struct cases{
    int case_num;
    InstructionNode* case_body;
};

struct condition{
    Token operand1;
    ConditionalOperatorType condition_op;
    Token operand2;
};

vector<struct InstructionNode*> instructions;
//vector<struct InstructionNode*> cases_list;
map<string , int> location;
void syntax_error();
Token expect(TokenType expected_type);
//void parse_generate_intermediate_representation();

InstructionNode* parse_program();
void parse_var_section();
void parse_id_list();
InstructionNode* parse_body();
InstructionNode* parse_stmt_list();
InstructionNode* parse_stmt();
InstructionNode* parse_assign_stmt();
expr* parse_expr();
Token parse_primary();
Token parse_op();
InstructionNode* parse_output_stmt();
InstructionNode* parse_input_stmt();
InstructionNode* parse_while_stmt();
InstructionNode* parse_if_stmt();
condition* parse_condition();
ConditionalOperatorType parse_relop();
InstructionNode* parse_switch_stmt();
InstructionNode* parse_for_stmt();
vector<cases*> parse_case_list();
cases* parse_case();
InstructionNode* parse_default_case();
void parse_inputs();
void parse_num_list();

InstructionNode* parse_program(){
    parse_var_section();
    InstructionNode* stmt = parse_body();
    parse_inputs();
    expect(END_OF_FILE);
    //cout << "Great\n"; 
    return stmt;
}

void parse_var_section(){
    parse_id_list();
    expect(SEMICOLON);
}

void parse_id_list(){
    Token t = lexer.peek(2);
    if (t.token_type == COMMA){
        expect(ID);
        expect(COMMA);
        parse_id_list();
    }
    else if (t.token_type == SEMICOLON){
        expect(ID);
    }
    else 
        syntax_error();
    
}
InstructionNode* parse_body(){
    expect(LBRACE);
    InstructionNode* stmt = parse_stmt_list();
    expect(RBRACE);
    return stmt;
}
InstructionNode* parse_stmt_list(){
    struct InstructionNode* instl1; // instruction list for stmt
    struct InstructionNode* instl2; // instruction list for stmt list

    instl1 = parse_stmt();
    Token t = lexer.peek(1);
    if (t.token_type == RBRACE){
        return instl1;
    }
    else {
        instl2 = parse_stmt_list();
        struct InstructionNode* current = instl1;
        while (current->next != nullptr){
            current = current->next;
        }
        current->next = instl2;
    }
    return instl1;
}
InstructionNode* parse_stmt(){
    Token t1 = lexer.peek(1);
    Token t2 = lexer.peek(2);
    struct InstructionNode* instl;

    if (t2.token_type == EQUAL){ //assign_stmt
        instl = parse_assign_stmt();
    }
    else if (t1.token_type == WHILE){
        instl = parse_while_stmt();
    }
    else if (t1.token_type == IF){
        instl = parse_if_stmt();
        
    }
    else if (t1.token_type == SWITCH){
        instl = parse_switch_stmt();
        //instl = nullptr;
    }
    else if (t1.token_type == FOR){
        instl = parse_for_stmt();
    }
    else if (t1.token_type == OUTPUT){
        instl = parse_output_stmt();
    }
    else if (t1.token_type == INPUT){
        instl = parse_input_stmt();
    }
    else {
        syntax_error();
    }

    return instl;
}
InstructionNode* parse_assign_stmt(){
    InstructionNode* st = new InstructionNode;
    st->type = ASSIGN;
    Token t = lexer.peek(1);
    if (location.find(t.lexeme) != location.end())
        {
            st->assign_inst.left_hand_side_index = location[t.lexeme];
        }
    else
        {
            location.insert(pair<string, int>(t.lexeme, next_available++));
            mem[next_available-1] = 0;
            st->assign_inst.left_hand_side_index = location[t.lexeme];
        }
    expect(ID);
    expect(EQUAL);
    Token t1 = lexer.peek(1);
    Token t2 = lexer.peek(2);
    if (t2.token_type == SEMICOLON){//primary
        Token prim = parse_primary();
        if (prim.token_type == NUM)
        {
            mem[next_available] = stoi(prim.lexeme);
            next_available++;
            st->assign_inst.operand1_index = next_available - 1;
        }
        else if (prim.token_type == ID)
        {
            //it is in the mem table
            if (location.find(prim.lexeme) != location.end())
            {
                st->assign_inst.operand1_index = location[prim.lexeme];
            }
            else
            {
                location.insert(pair<string, int>(prim.lexeme, next_available++));
                st->assign_inst.operand1_index = location[prim.lexeme];
                mem[next_available-1] = 0;
            }
        }
        st->assign_inst.op = OPERATOR_NONE;
    }
    else {
        expr* the_expr = parse_expr();
        Token t11 = the_expr->operand1;
        Token t22 = the_expr->operand2;
        if (t11.token_type == NUM)
        {
            mem[next_available] = stoi(t11.lexeme);
            next_available++;
            st->assign_inst.operand1_index = next_available - 1;
        }
        else if (t11.token_type == ID)
        {
            //it is in the mem table
            if (location.find(t11.lexeme) != location.end())
            {
                st->assign_inst.operand1_index = location[t11.lexeme];
            }
            else
            {
                location.insert(pair<string, int>(t11.lexeme, next_available++));
                st->assign_inst.operand1_index = location[t11.lexeme];
                mem[next_available-1] = 0;
            }
        }
        st->assign_inst.op = the_expr->expr_op;

        if (t22.token_type == NUM)
        {
            mem[next_available] = stoi(t22.lexeme);
            next_available++;
            st->assign_inst.operand2_index = next_available - 1;
        }
        else if (t22.token_type == ID)
        {
            //it is in the mem table
            if (location.find(t22.lexeme) != location.end())
            {
                st->assign_inst.operand2_index = location[t22.lexeme];
            }
            else
            {
                location.insert(pair<string, int>(t22.lexeme, next_available++));
                st->assign_inst.operand2_index = location[t22.lexeme];
                mem[next_available-1] = 0;
            }
        }

    }
    st->next = nullptr;
    //instructions.push_back(st);
    expect(SEMICOLON);
    return st;
}
expr* parse_expr(){
    expr* new_expr = new expr;
    new_expr->operand1 = parse_primary();
    Token t = parse_op();
    if (t.token_type == PLUS){
        new_expr->expr_op = OPERATOR_PLUS;
    }
    else if (t.token_type == MINUS){
        new_expr->expr_op = OPERATOR_MINUS;
    }
    else if (t.token_type == MULT){
        new_expr->expr_op = OPERATOR_MULT;
    }
    else {
        new_expr->expr_op = OPERATOR_DIV;
    }
    new_expr->operand2 = parse_primary();
    return new_expr;

}

Token parse_primary(){
    Token t = lexer.peek(1);
    if (t.token_type == ID)
        expect(ID);
    else{
        expect(NUM);
    }
    return t;
}
Token parse_op(){
    Token t = lexer.peek(1);
    if (t.token_type == PLUS){
        expect(PLUS);
    }
    else if (t.token_type == MINUS){
        expect(MINUS);
    }
    else if (t.token_type == MULT){
        expect(MULT);
    }
    else {
        expect(DIV);
    }
    return t;
}
InstructionNode* parse_output_stmt(){
    InstructionNode *st = new InstructionNode;
    st->type = OUT;
    expect(OUTPUT);
    Token t = lexer.peek(1);
    if (location.find(t.lexeme) != location.end())
    {
        st->output_inst.var_index = location[t.lexeme];
    }
    else
    {
        location.insert(pair<string, int>(t.lexeme, next_available++));
        st->output_inst.var_index = location[t.lexeme];
        mem[next_available-1] = 0;
    }
    expect(ID);
    //instructions.push_back(st);
    st->next = nullptr;
    expect(SEMICOLON);
    return st;
}
InstructionNode* parse_input_stmt(){
    InstructionNode *st = new InstructionNode;
    st->type = IN;
    expect(INPUT);
    Token t = lexer.peek(1);
    if (location.find(t.lexeme) != location.end())
    {
        st->input_inst.var_index = location[t.lexeme];
    }
    else
    {
        location.insert(pair<string, int>(t.lexeme, next_available++));
        st->input_inst.var_index = location[t.lexeme];
        mem[next_available-1] = 0;
    }
    expect(ID);
    //instructions.push_back(st);
    st->next = nullptr;
    expect(SEMICOLON);
    return st;
}
InstructionNode* parse_while_stmt(){
    InstructionNode *st = new InstructionNode;
    st->type = CJMP;
    expect(WHILE);

    condition* cd = parse_condition();
    st->cjmp_inst.condition_op = cd->condition_op;
    Token t11 = cd->operand1;
    Token t22 = cd->operand2;
        if (t11.token_type == NUM)
        {
            mem[next_available] = stoi(t11.lexeme);
            next_available++;
            st->cjmp_inst.operand1_index = next_available - 1;
        }
        else if (t11.token_type == ID)
        {
            //it is in the mem table
            if (location.find(t11.lexeme) != location.end())
            {
                st->cjmp_inst.operand1_index = location[t11.lexeme];
            }
            else
            {
                location.insert(pair<string, int>(t11.lexeme, next_available++));
                st->cjmp_inst.operand1_index = location[t11.lexeme];
                mem[next_available-1] = 0;
            }
        }
        

        if (t22.token_type == NUM)
        {
            mem[next_available] = stoi(t22.lexeme);
            next_available++;
            st->cjmp_inst.operand2_index = next_available - 1;
        }
        else if (t22.token_type == ID)
        {
            //it is in the mem table
            if (location.find(t22.lexeme) != location.end())
            {
                st->cjmp_inst.operand2_index = location[t22.lexeme];
            }
            else
            {
                location.insert(pair<string, int>(t22.lexeme, next_available++));
                st->cjmp_inst.operand2_index = location[t22.lexeme];
                mem[next_available-1] = 0;
            }
        }
    
    st->next = parse_body();

    InstructionNode* jmp1 = new InstructionNode; 
    jmp1->type = JMP;
    jmp1->jmp_inst.target = st;
    jmp1->next = nullptr;
    InstructionNode *current = st;
    while (current->next != nullptr){
        current = current->next;
    }
    current->next = jmp1;

    InstructionNode* noop1 = new InstructionNode; 
    noop1->type = NOOP;
    noop1->next = nullptr;

    current = jmp1;
    while (current->next != nullptr){
        current = current->next;
    }
    current->next = noop1;
    st->cjmp_inst.target = noop1;

    return st;
}
InstructionNode* parse_if_stmt(){
    InstructionNode *st = new InstructionNode;
    st->type = CJMP;
    expect(IF);
    condition* cd = parse_condition();
    st->cjmp_inst.condition_op = cd->condition_op;
    Token t11 = cd->operand1;
    Token t22 = cd->operand2;
        if (t11.token_type == NUM)
        {
            mem[next_available] = stoi(t11.lexeme);
            next_available++;
            st->cjmp_inst.operand1_index = next_available - 1;
        }
        else if (t11.token_type == ID)
        {
            //it is in the mem table
            if (location.find(t11.lexeme) != location.end())
            {
                st->cjmp_inst.operand1_index = location[t11.lexeme];
            }
            else
            {
                location.insert(pair<string, int>(t11.lexeme, next_available++));
                st->cjmp_inst.operand1_index = location[t11.lexeme];
                mem[next_available-1] = 0;
            }
        }
        

        if (t22.token_type == NUM)
        {
            mem[next_available] = stoi(t22.lexeme);
            next_available++;
            st->cjmp_inst.operand2_index = next_available - 1;
        }
        else if (t22.token_type == ID)
        {
            //it is in the mem table
            if (location.find(t22.lexeme) != location.end())
            {
                st->cjmp_inst.operand2_index = location[t22.lexeme];
            }
            else
            {
                location.insert(pair<string, int>(t22.lexeme, next_available++));
                st->cjmp_inst.operand2_index = location[t22.lexeme];
                mem[next_available-1] = 0;
            }
        }
    
    st->next = parse_body();
    InstructionNode *current = st;
    while(current->next != nullptr){
        current = current->next;
    }
    InstructionNode *noop1 = new InstructionNode;
    noop1->type = NOOP;
    noop1->next = nullptr;
    current->next = noop1;

    st->cjmp_inst.target = noop1;
    return st;
}
condition* parse_condition(){
    condition* cd = new condition;
    cd->operand1 = parse_primary();
    cd->condition_op = parse_relop();
    cd->operand2 = parse_primary();

    return cd;
}

ConditionalOperatorType parse_relop(){
    Token t = lexer.peek(1);
    ConditionalOperatorType x;
    if (t.token_type == GREATER){
        expect(GREATER);
        x = CONDITION_GREATER;
    }
    else if (t.token_type == LESS){
        expect(LESS);
        x = CONDITION_LESS;
    }
    else {
        expect(NOTEQUAL);
        x = CONDITION_NOTEQUAL;
    }
    return x;
}
InstructionNode* parse_switch_stmt(){
    InstructionNode *st = new InstructionNode;
    st = nullptr;
    //InstructionNode *current = st;
    InstructionNode *previous = st;
    //st->type = CJMP;
    expect(SWITCH);
    //st->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
    Token t0 = lexer.peek(1);
    int t0_index = -1;
        if (t0.token_type == NUM)
        {
            mem[next_available] = stoi(t0.lexeme);
            next_available++;
            t0_index = next_available - 1;
        }
        else if (t0.token_type == ID)
        {
            //it is in the mem table
            if (location.find(t0.lexeme) != location.end())
            {
                t0_index = location[t0.lexeme];
            }
            else
            {
                location.insert(pair<string, int>(t0.lexeme, next_available++));
                t0_index = location[t0.lexeme];
                mem[next_available-1] = 0;
            }
        }
    
    expect(ID);
    expect(LBRACE);
    vector<cases*> case_list = parse_case_list();

    Token t = lexer.peek(1);
    if (t.token_type == DEFAULT){
        /*
        InstructionNode* current = st;
        while (current->next != nullptr){
            current = current->next;
        }
        current->next = */
        InstructionNode* def1 = parse_default_case(); 
        InstructionNode *noop = new InstructionNode;
        noop->type = NOOP;
        noop->next = nullptr;
        InstructionNode* curr11 = def1;
        while (curr11->next != nullptr){
            curr11 = curr11->next;
        }
        curr11->next = noop;
        while(case_list.size()>0){
            //current = nullptr;
            InstructionNode *newNode = new InstructionNode;
            newNode->type = CJMP;
            newNode->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
            newNode->cjmp_inst.operand1_index = t0_index;
            cases* new_case = case_list[0];
            case_list.erase(case_list.begin()+0);

            mem[next_available] = new_case->case_num;
            next_available++;
            newNode->cjmp_inst.operand2_index = next_available - 1;
            newNode->cjmp_inst.target = new_case->case_body;
            InstructionNode *curr1 = new_case->case_body;
            while(curr1->next != nullptr){
                curr1 = curr1->next;
            }
            curr1->next = noop;
            if (case_list.size() == 0){
                newNode->next = def1;
            }

            if (previous == nullptr){
                st = newNode;
                previous = newNode;
            }
            else if (previous != nullptr){
                previous->next = newNode;
                previous = previous->next;
            }

        }
        expect(RBRACE);
        return st;
    }

    InstructionNode *noop = new InstructionNode;
    noop->type = NOOP;
    noop->next = nullptr;

    while(case_list.size()>0){
        //current = nullptr;
        InstructionNode *newNode = new InstructionNode;
        newNode->type = CJMP;
        newNode->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
        newNode->cjmp_inst.operand1_index = t0_index;
        cases* new_case = case_list[0];
        case_list.erase(case_list.begin()+0);

        mem[next_available] = new_case->case_num;
        next_available++;
        newNode->cjmp_inst.operand2_index = next_available - 1;
        newNode->cjmp_inst.target = new_case->case_body;
        InstructionNode *curr1 = new_case->case_body;
        while(curr1->next != nullptr){
            curr1 = curr1->next;
        }
        curr1->next = noop;
        if (case_list.size() == 0){
            newNode->next = noop;
        }

        if (previous == nullptr){
            st = newNode;
            previous = newNode;
        }
        else if (previous != nullptr){
            previous->next = newNode;
            previous = previous->next;
        }

        //previous = newNode;
        //current = newNode;
    }
    
    expect(RBRACE);
    return st;
}
InstructionNode* parse_for_stmt(){
    expect(FOR);
    expect(LPAREN);
    InstructionNode* st0 = new InstructionNode;
    st0 = parse_assign_stmt();
    //InstructionNode* curr = st0;
    
    //while
    {
        InstructionNode *st = new InstructionNode;
        st0->next = st;
        st->type = CJMP;

        condition* cd = parse_condition();
        st->cjmp_inst.condition_op = cd->condition_op;
        Token t11 = cd->operand1;
        Token t22 = cd->operand2;
        if (t11.token_type == NUM){
            mem[next_available] = stoi(t11.lexeme);
            next_available++;
            st->cjmp_inst.operand1_index = next_available - 1;
        }
        else if (t11.token_type == ID){
            //it is in the mem table
            if (location.find(t11.lexeme) != location.end()){
                st->cjmp_inst.operand1_index = location[t11.lexeme];
            }
            else{
                location.insert(pair<string, int>(t11.lexeme, next_available++));
                st->cjmp_inst.operand1_index = location[t11.lexeme];
                mem[next_available-1] = 0;
            }
        }
            

        if (t22.token_type == NUM){
            mem[next_available] = stoi(t22.lexeme);
            next_available++;
            st->cjmp_inst.operand2_index = next_available - 1;
        }
        else if (t22.token_type == ID){
            //it is in the mem table
            if (location.find(t22.lexeme) != location.end()){
                st->cjmp_inst.operand2_index = location[t22.lexeme];
            }
            else{
                location.insert(pair<string, int>(t22.lexeme, next_available++));
                st->cjmp_inst.operand2_index = location[t22.lexeme];
                mem[next_available-1] = 0;
            }
        }

        expect(SEMICOLON);
        InstructionNode* st1 = new InstructionNode;
        st1 = parse_assign_stmt();
        expect(RPAREN);
        
        st->next = parse_body();

        InstructionNode* jmp1 = new InstructionNode; 
        jmp1->type = JMP;
        jmp1->jmp_inst.target = st;
        jmp1->next = nullptr;
        InstructionNode *current = st;
        while (current->next != nullptr){
            current = current->next;
        }
        current->next = st1; //the assignment is after the body

        InstructionNode* noop1 = new InstructionNode; 
        noop1->type = NOOP;
        noop1->next = nullptr;
        
        st1->next = jmp1;

        current = jmp1;
        while (current->next != nullptr){
            current = current->next;
        }
        current->next = noop1;
        st->cjmp_inst.target = noop1;
    }

    return st0;
}

vector<cases*> parse_case_list(){
    vector<cases*> the_cases;
    //InstructionNode *st = new InstructionNode;
        
    cases* case1 = parse_case();
    the_cases.push_back(case1);

    Token t = lexer.peek(1);
    if (t.token_type == CASE){
        vector<cases*> cases2 = parse_case_list();
        the_cases.insert(the_cases.end(), cases2.begin(), cases2.end());
    }

    return the_cases;

}

struct cases* parse_case(){
    struct cases* case1 = new struct cases;
    expect(CASE);
    Token t = expect(NUM);
    case1->case_num = stoi(t.lexeme);
    expect(COLON);
    case1->case_body = parse_body();

    return case1;

}

InstructionNode* parse_default_case(){
    expect(DEFAULT);
    expect(COLON);
    InstructionNode* st = parse_body();
    return st;
}
void parse_inputs(){
    parse_num_list();
}
void parse_num_list(){
    inputs.push_back(stoi(expect(NUM).lexeme));
    Token t = lexer.peek(1);
    if (t.token_type == NUM){
        parse_num_list();
    }
}

void syntax_error()
{
    cout << "SYNTAX ERROR !!!\n";
    exit(1);
}

Token expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

struct InstructionNode * parse_generate_intermediate_representation()
{
    struct InstructionNode * the_presentation = parse_program();

    return the_presentation;
}
