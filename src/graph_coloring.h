#pragma once
#include "cfg.h"

typedef std::pair<std::string, std::unordered_set<std::string>> graphPair;
class GraphColoring{
    private:
        // std::unordered_set<std::string> problematic;
        std::unordered_map<std::string, int> regMap;
        std::unordered_map<std::string, std::unordered_set<std::string>> graph;
        int totalRegisters;
        graphPair removeOneNode();
        void insertNode(graphPair g);
    public:
        GraphColoring(int registers): 
            totalRegisters(registers) 
            {};
        
        void createGraph(std::unordered_map<int, CFGNode*>& cfgBlocks);
        void colorGraph();
};