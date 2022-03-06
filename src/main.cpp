#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include <sstream>

int main(int argc, char **argv){
    std::map<std::string, Interpreter::object> scope;
    std::string filename;
    if(argc == 1)filename = "main.aj";
    else filename = argv[1];

//    visit(Parser::parse(Lexer::tokenize(filename)));

    std::vector<Lexer::token> tokens = Lexer::tokenize(filename);
    Parser::Node* root_node = Parser::parse(tokens);
    Interpreter::executeProgram(root_node, scope);
}