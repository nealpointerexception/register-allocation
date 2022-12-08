#include "ast.h"
#include <iostream>


void AstNode::adopt_child_r(AstNode* child){
    children.insert(children.begin(), child);
}

void AstNode::adopt_child(AstNode* child){
    children.push_back(child);
}

std::string AstNode::toString(){
    switch(type){
        case NodeType::IF:
        case NodeType::WHILE:
            return value + " (" + children.at(0)->toString() + ")";
        case NodeType::CMPOP:
        case NodeType::OP:
            return children.at(0)->toString() 
                    + " " 
                    + value 
                    + " " 
                    + children.at(1)->toString();
        case NodeType::VAR:
        case NodeType::NUM:
            return value;
        case NodeType::ROOT:
            return "ROOT";
        case NodeType::STAT_LIST:
            return "STAT_LIST";
        case NodeType::VARDECL:
            return children.at(0)->toString() + " = " + children.at(1)->toString();
        default:
            return "UNKWN";
    }
}


std::string nodetype2str(NodeType type){
    switch(type){
        case NodeType::CMPOP:
            return "CMPOP";
        case NodeType::IF:
            return "IF";
        case NodeType::NUM:
            return "NUM";
        case NodeType::OP:
            return "OP";
        case NodeType::ROOT:
            return "ROOT";
        case NodeType::STAT_LIST:
            return "STAT_LIST";
        case NodeType::VAR:
            return "VAR";
        case NodeType::VARDECL:
            return "VARDECL";
        case NodeType::WHILE:
            return "WHILE";
        default:
            return "UNKWN";
    }
}

int outputTree(AstNode* start, int num, std::ofstream& file){
    if(!num){
        file << "flowchart TD" << std::endl;
    }
    file << num << "[\"" << start->value  << "\"]" << std::endl;

    int myNum = num;
    for(auto* child : start->children){
        num++;
        file << myNum << "-->" << num << std::endl;
        num = outputTree(child, num, file);
    }
    return num;
}

void SimpleAst::exitExpr(simpleParser::ExprContext *ctx){
    AstNode* tempNode = new AstNode();
    if(ctx->children.size() == 1){
        // one child, either var or num
        if(ctx->NUM()){
            tempNode->type = NodeType::NUM;
            tempNode->value = ctx->NUM()->getText();
        }
        else{
            tempNode->type = NodeType::VAR;
            tempNode->value = ctx->ID()->getText();
        }
        // std::cout << "pushed" << std::endl;
        stack_machine.push(tempNode);
        return;
    }
    // we have an operation expression
    // adopt two items from the stack
    tempNode->adopt_child_r(stack_machine.top());
    stack_machine.pop();
    tempNode->adopt_child_r(stack_machine.top());
    stack_machine.pop();

    
    if(ctx->PLUS() || ctx->MINUS()){
        tempNode->type = NodeType::OP;
        tempNode->value = ctx->PLUS() ? ctx->PLUS()->getText() : ctx->MINUS()->getText();
    }
    else if(ctx->GT() || ctx->LT()){
        // std::cout << "cmp" << std::endl;
        tempNode->type = NodeType::CMPOP;
        tempNode->value = ctx->GT() ? ctx->GT()->getText() : ctx->LT()->getText(); 
    }
    // std::cout << "pushed op" << std::endl;
    stack_machine.push(tempNode);
}

void SimpleAst::exitStat_list(simpleParser::Stat_listContext *ctx){
    AstNode* tempNode = new AstNode("{", NodeType::STAT_LIST);
    while(true){
        if(!stack_machine.top()->isStatment){
            break;
        }
        tempNode->adopt_child_r(stack_machine.top());
        stack_machine.pop();
    }
    stack_machine.push(tempNode);
}

void SimpleAst::exitVardecl(simpleParser::VardeclContext *ctx){
    // children: target, value
    AstNode* tempNode = new AstNode("=", NodeType::VARDECL);
    tempNode->isStatment = true;

    tempNode->adopt_child(new AstNode(ctx->ID()->getText(), NodeType::VAR));
    tempNode->adopt_child(stack_machine.top());
    stack_machine.pop();
    // std::cout << "sanity 0 " << stack_machine.size() << std::endl;
    
    stack_machine.push(tempNode);
}

void SimpleAst::exitIf(simpleParser::IfContext *ctx){
    // children: expr, true block, [else block]
    // on stack should be [else block], true block and then expr
    AstNode* tempNode = new AstNode("if", NodeType::IF);
    tempNode->isStatment = true;

    if(ctx->ELSE() != nullptr){
        // adopt else block
        tempNode->adopt_child_r(stack_machine.top());
        stack_machine.pop();
    }

    assert(stack_machine.top()->type == NodeType::STAT_LIST);
    tempNode->adopt_child_r(stack_machine.top());
    stack_machine.pop();

    assert(stack_machine.top()->type == NodeType::CMPOP);
    tempNode->adopt_child_r(stack_machine.top());
    stack_machine.pop();

    stack_machine.push(tempNode);
}

void SimpleAst::exitWhile(simpleParser::WhileContext *ctx){
    // children: expr, true block
    // on stack should be block and then expr
    AstNode* tempNode = new AstNode("while", NodeType::WHILE);
    tempNode->isStatment = true;
    assert(stack_machine.top()->type == NodeType::STAT_LIST);

    tempNode->adopt_child_r(stack_machine.top());
    stack_machine.pop();

    assert(stack_machine.top()->type == NodeType::CMPOP);
    tempNode->adopt_child_r(stack_machine.top());
    stack_machine.pop();

    stack_machine.push(tempNode);
}

void SimpleAst::exitProgram(simpleParser::ProgramContext *ctx){
    root = new AstNode("root", NodeType::ROOT);
    while(!stack_machine.empty()){
        // this will likely be a statement list
        // std::cout << "root adpt: " << stack_machine.top()->value << std::endl;
        root->adopt_child_r(stack_machine.top());
        stack_machine.pop();
    }
    
}