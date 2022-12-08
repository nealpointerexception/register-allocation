#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// ANTLR4 Runtime
#include "antlr4-runtime.h"

// Generated lexer and parser
#include "simpleLexer.h"
#include "simpleParser.h"

// Error Policy Include
#include "ExceptionErrorListener.h"

#include "ast.h"
#include "cfg.h"
#include "liveout.h"
#include "graph_coloring.h"
#include "linear_scan.h"

using namespace antlrcpp;
using namespace antlr4;

std::string readf(const std::string& src_file);
std::string readf(const std::string& src_file) 
{
	// Find the file extension name
	std::size_t delimiter_pos = src_file.find_last_of(".");
	if (src_file.substr(delimiter_pos + 1).compare("simp") != 0)
	{	
		// Looks like I should use boost:program_options, maybe fix this before submission
		std::cerr << "Incorrect arguments\nfile must be of extension .simp" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::ifstream read(src_file);
	std::stringstream buffer;
	buffer << read.rdbuf();
	return buffer.str();
}

int main(int argc, char *argv[])
{	
	if (argc != 2) {
		std::cerr << "Incorrect arguments\nusage: ./reg_alloc <src_file>" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::string src_code = readf(argv[1]);
	std::string inFileName = std::string(argv[1]);
	ANTLRInputStream input(src_code);
	
	// Lexer`
	simpleLexer lexer(&input);
	CommonTokenStream tokens(&lexer);

	// tokens.fill();
	// for (auto token : tokens.getTokens()) {
	// 	std::cout << token->toString() << std::endl;
	// }

	// Parser
	simpleParser parser(&tokens);
	tree::ParseTree *tree = nullptr;

	// We can get parsing errors here from syntax
	try {
		tree = parser.program();	
	}
	catch (ParseCancellationException &e) {
		std::cout << "ParserError: ";
		std::cout << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	// slepl interpreter
	SimpleAst simpleAst;
	// Interpret the source code
	tree::ParseTreeWalker walker;
	walker.walk(&simpleAst, tree);

	AstNode* root = simpleAst.getAst();
	std::ofstream astFile;
	astFile.open(inFileName + "_ast.mmd", std::ios::trunc);
	outputTree(root, 0, astFile);
	astFile.close();
	
	CFGCreator cfgCreator;
	cfgCreator.genCFG(root);
	cfgCreator.outputCFG(inFileName + "_cfg.mmd");

	LiveOut liveout(cfgCreator.getCFGBlocks());
	liveout.prepCFG();
	liveout.computeLiveOut();

	GraphColoring graphColoring(4);
	graphColoring.createGraph(cfgCreator.getCFGBlocks());
	graphColoring.colorGraph();

	IRManager irman;
	std::ofstream irFile;
	irFile.open(inFileName + "_ir.txt");
	irman.generateIR(root, irFile);

	std::cout << std::endl;
	LinearScan linearScan(4);
	linearScan.computeIntervals(irman.getIrVarData());
	linearScan.allocateRegisters();

}



