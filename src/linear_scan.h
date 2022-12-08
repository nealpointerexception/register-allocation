#pragma once
#include "ast.h"
#include "liveout.h"

// traverse ast to create ir
typedef std::unordered_set<std::string> varlist;

// exec lineno, var defines, var usages
typedef std::tuple<int, varlist, varlist> execStepData;
class IRManager{
    private:
        // tracks <defines, usages> line by line for ir
        std::vector<execStepData> irVarData;

        // upwards growing branch number
        int branchNum = 0;
        int lineno = 0;


    public:
        // updates irVarData, also creates ir repr as txt file
        void generateIR(AstNode* root, std::ofstream& file);
        std::vector<execStepData>& getIrVarData() {
            return irVarData;
        }
};

class LinearScan{
    public:
        std::unordered_map<std::string, int> regMap;
        std::unordered_map<std::string, std::pair<int, int>> liveIntervals;
        int maxRegisters;
        int execSteps;

        LinearScan(int registers): maxRegisters(registers) {};
        void computeIntervals(std::vector<execStepData>& irVarData);
        void allocateRegisters();
};