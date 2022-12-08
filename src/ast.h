#pragma once

#include <vector>
#include <stack>
#include <string>
#include <unordered_set>
#include "simpleParserBaseListener.h"

enum NodeType{
    ROOT,
    VARDECL,
    WHILE,
    IF,
    STAT_LIST,
    VAR,
    NUM,
    OP,
    CMPOP
};

using namespace antlrcpp;

class AstNode{
    public:
        std::string value;
        NodeType type;
        bool isStatment {false};
        std::vector<AstNode*> children;

        AstNode() {};
        AstNode(std::string val, NodeType type): value(val), type(type) {};

        void adopt_child_r(AstNode* child);
        void adopt_child(AstNode* child);
        std::string toString() const;
        friend std::ostream& operator<<(std::ostream& os, const AstNode& an);
};

std::string nodetype2str(NodeType type);
int outputTree(AstNode* start, int num, std::ofstream& file);

class SimpleAst: public simpleParserBaseListener 
{
	private:
		AstNode* root;
        std::stack<AstNode*> stack_machine;

	public:
		AstNode* getAst() {return root;};
		void exitStat_list(simpleParser::Stat_listContext *ctx);
		void exitExpr(simpleParser::ExprContext *ctx);
        void exitVardecl(simpleParser::VardeclContext *ctx);
        void exitIf(simpleParser::IfContext *ctx);
        void exitWhile(simpleParser::WhileContext *ctx);
        void exitProgram(simpleParser::ProgramContext *ctx);

};

