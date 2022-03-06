#pragma once

#include "parser.h"

namespace Interpreter {
    enum dataType {
        REAL,
        INTS,
        BOOLS,
        STRING,
        FUNCTION,
        VOIDS
    };

    struct object {
        dataType type;

        std::string string_val;
        int int_val;
        float real_val;
        bool bool_val;
        Parser::Node *function_body;
    };



    void declareFunction(Parser::Node *a, std::map<std::string, object> &scope);

    void executeProgram(Parser::Node *a, std::map<std::string, object> &scope);

    void printSingle(object p);

    void printStuff(Parser::Node *a, std::map<std::string, object> &scope);

    void executeIf(Parser::Node *a, std::map<std::string, object> &scope);

    void executeWhile(Parser::Node *a, std::map<std::string, object> &scope);


    void returnStuff(Parser::Node *a, std::map<std::string, object> &scope);


    void assignVariable(Parser::Node *a, std::map<std::string, object> &scope);

    void executeFor(Parser::Node *a, std::map<std::string, object> &scope);

    void executeSimpleFor(Parser::Node *a, std::map<std::string, object> &scope);

    void executeStatement(Parser::Node *a, std::map<std::string, object> &scope);

    void displayTypeError(std::string operation, int line_number);

    object evaluateExpression(Parser::Node *a, std::map<std::string, object> &scope);

    object executeFunction(Parser::Node *a, std::map<std::string, object> scope);

}