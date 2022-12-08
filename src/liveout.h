
#pragma once
#include "cfg.h"
#include <vector>

// grab all used variables from a ast node
std::unordered_set<std::string> getUEVar(AstNode* node);

class LiveOut{
    private:
        std::unordered_set<std::string> varDomain;
        std::unordered_map<int, CFGNode*>& cfgBlocks;

        std::unordered_set<std::string> singleLiveOut(CFGNode* node);
        bool updateLiveOut(CFGNode* node);
    public:
        LiveOut(std::unordered_map<int, CFGNode*>& blocks): cfgBlocks(blocks) {};
        
        void prepCFG();
        void computeLiveOut();
        
};