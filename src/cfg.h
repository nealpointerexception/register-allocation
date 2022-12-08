#pragma once

#include <vector>
#include <stack>
#include <string>
#include <unordered_set>
#include "ast.h"

using namespace antlrcpp;
using namespace antlr4;

class CFGNode
{
public:
	int id;
	NodeType type;
	AstNode *astNode;

	std::unordered_set<std::string> varkill {};
	std::unordered_set<std::string> uevar {};
	std::unordered_set<std::string> liveout {};

	std::vector<CFGNode *> children;
	std::vector<CFGNode *> parents;

	CFGNode(){};
	CFGNode(int id, std::vector<CFGNode *> parents, AstNode* node) : 
		id(id), 
		parents(parents),
		astNode(node)
		{};

	void insertChild(CFGNode* node);
	void insertParent(CFGNode* node);

	
};

class CFGCreator
{
private:
	int rid = 0;
	std::unordered_map<int, CFGNode*> cfgBlocks;
	void registerCFGBlock(CFGNode *);
	void resolveChildren();
	std::vector<CFGNode *> traverse(AstNode *node, std::vector<CFGNode *> parents);
public:
	CFGNode *genCFG(AstNode *root);
	void outputCFG(std::string filename);
	std::unordered_map<int, CFGNode*>& getCFGBlocks() {return cfgBlocks;};
	
};
