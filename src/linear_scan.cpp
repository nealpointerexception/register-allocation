#include "linear_scan.h"

void IRManager::generateIR(AstNode* node, std::ofstream& file){
    if(node->type == NodeType::ROOT || node->type == NodeType::STAT_LIST){
        // loop through children and resolve
        for(auto* child : node->children){
            generateIR(child, file);
        }
    }
    else if(node->type == NodeType::VARDECL){
        varlist defs = {node->children.at(0)->value};
        varlist uses = getUEVar(node->children.at(1));
        file << node->toString() << std::endl;
        irVarData.push_back(std::make_tuple(lineno, defs, uses));
        lineno++;
    }
    else if(node->type == NodeType::IF){
        int myBranchNum = branchNum;
        branchNum += 1;
        file << "if not " << node->children.at(0)->toString();
        file << " goto else_" << myBranchNum << std::endl;

        varlist uses = getUEVar(node->children.at(0));
        irVarData.push_back(std::make_tuple(lineno, varlist(), uses));
        lineno++;

        // true case emit
        generateIR(node->children.at(1), file);

        // emit jump to end
        file << "goto if_" << myBranchNum << "_end" << std::endl;
        lineno++;

        // emit else label
        file << "else_" << myBranchNum << ":" << std::endl;
        lineno++;

        // resolve else case if exists
        if(node->children.size() == 3){
            generateIR(node->children.at(2), file);
        }
        // emit end if statement
        file << "if_" << myBranchNum << "_end:" << std::endl;
        lineno++;
    }
    else if(node->type == NodeType::WHILE){
        int myBranchNum = branchNum;
        branchNum += 1;
        file << "while_" << myBranchNum << ":" << std::endl;
        lineno++;

        file << "if not " << node->children.at(0)->toString();
        file << " goto while_end_" << myBranchNum << std::endl;

        varlist uses = getUEVar(node->children.at(0));
        irVarData.push_back(std::make_tuple(lineno, varlist(), uses));
        lineno++;

        // resolve body
        generateIR(node->children.at(1), file);
        // emit loop statement
        file << "goto while_" << myBranchNum << std::endl;
        lineno++;
        file << "while_end_" << myBranchNum << ":" << std::endl;
        lineno++;

    }
}


void LinearScan::computeIntervals(std::vector<execStepData>& irVarData){
    
    for(auto execElem : irVarData){
        int execNum = std::get<0>(execElem);
        for(auto def : std::get<1>(execElem)){
            if(!liveIntervals.count(def)){
                liveIntervals[def] = std::make_pair(execNum, 0);
            }
            if(liveIntervals[def].second < execNum){
                liveIntervals[def].second = execNum;
            }
        }
        for(auto use : std::get<2>(execElem)){
            if(liveIntervals[use].second < execNum){
                liveIntervals[use].second = execNum;
            }
        }
    }
    std::cout << "Live Intervals:" << std::endl;
    for(auto elem : liveIntervals){
        std::cout << elem.first 
                  << ": [" << elem.second.first
                  << ", " 
                  << elem.second.second 
                  << "]"
                  << std::endl;
        
    }
    execSteps = std::get<0>(irVarData[irVarData.size()-1]);
}

void LinearScan::allocateRegisters(){
    std::unordered_map<std::string, int> inUseRegs;
    std::unordered_set<int> availableRegs;
    for(int i = 0; i < maxRegisters; ++i){
        availableRegs.insert(i);
    }
    // loop through each execution step
    for(int lineno = 0; lineno <= execSteps; ++lineno){
        // check who needs what
        std::vector<std::string> toAssign;
        std::vector<std::string> toExpire;
        for(auto varElem : liveIntervals){
            auto varName = varElem.first;
            auto liveInterval = varElem.second;
            if(liveInterval.second == lineno){
                toExpire.push_back(varName);
            }
            if(liveInterval.first == lineno){
                toAssign.push_back(varName);
            }
        }
        // expire nodes first:
        for(auto v : toExpire){
            inUseRegs.erase(v);
            availableRegs.insert(regMap[v]);
        }
        for(auto v : toAssign){
            // assign register
            if(inUseRegs.size() < maxRegisters){
                int regNum = *availableRegs.begin();
                regMap[v] = regNum;
                inUseRegs[v] = regNum;
                availableRegs.erase(regMap[v]);
            }
            else{
                // no registers available
                // spill a node using a register
                // decided by min total range
                std::string spillVar = "";
                int minRange = execSteps;
                for(auto elem : inUseRegs){
                    int range = liveIntervals[elem.first].second - 
                        liveIntervals[elem.first].first;
                    if(range < minRange){
                        minRange = range;
                        spillVar = elem.first;
                    }
                }
                // mark spilled in overall allocation
                regMap[spillVar] = -1;

                // remove from currently in use and 
                // give up register number
                inUseRegs.erase(spillVar);
                inUseRegs[v] = inUseRegs[spillVar];
            }
        }
    }
    std::cout << "Linear Scan Results:" << std::endl;
    for(auto elem : regMap){
        std::cout << elem.first << ": r" << elem.second << std::endl;
    }
}