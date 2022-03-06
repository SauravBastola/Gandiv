#include <iostream>

#include "interpreter.h"


bool ENDFUNCTION = false;
bool ENDLOOP = false;
bool SKIP = false;
Interpreter::object return_value;

void Interpreter::declareFunction(Parser::Node *a, std::map<std::string, object> &scope) {
    std::string fname = a->children[1]->value.attribute;
    object f;
    f.type = FUNCTION;
    f.function_body = a;
    scope[fname] = f;
}

void Interpreter::executeProgram(Parser::Node *a, std::map<std::string, object> &scope) {
    if (a->children.size() == 1) {
        executeStatement(a->children[0], scope);
    } else {
        executeProgram(a->children[0], scope);
        executeStatement(a->children[1], scope);
    }
}

void Interpreter::printSingle(Interpreter::object p) {
    if (p.type == INTS) {
        std::cout << p.int_val;
    } else if (p.type == REAL) {
        std::cout << p.real_val;
    } else if (p.type == BOOLS) {
        if (p.bool_val)std::cout << "true";
        else std::cout << "false";
    } else if (p.type == STRING) {
        std::cout << p.string_val;
    }
}

void Interpreter::printStuff(Parser::Node *a, std::map<std::string, object> &scope) {
    Parser::Node *b = a->children[2];
    while (true) {
        if (b->children[0]->value.name == "endl")std::cout << std::endl;
        else {
            object p = evaluateExpression(b->children[0], scope);
            printSingle(p);
        }
        if (b->children.size() == 1)break;
        b = b->children[2];
    }
}

void Interpreter::executeIf(Parser::Node *a, std::map<std::string, object> &scope) {
    object condition = evaluateExpression(a->children[2], scope);
    if (condition.type == BOOLS) {
        if (condition.bool_val) {
            executeProgram(a->children[5], scope);
        } else if (a->children.size() > 7) {
            executeProgram(a->children[9], scope);
        }
    } else {
        std::cout << "Error in line " << a->children[0]->value.line_number << std::endl;
        std::cout << "Expected boolean expression inside if" << std::endl;
        exit(1);
    }
}

void Interpreter::executeWhile(Parser::Node *a, std::map<std::string, object> &scope) {
    object condition = evaluateExpression(a->children[2], scope);
    if (condition.type == BOOLS) {
        while (condition.bool_val) {
            executeProgram(a->children[5], scope);
            SKIP = false;
            if (ENDLOOP)break;
            condition = evaluateExpression(a->children[2], scope);
        }
        ENDLOOP = false;
    }
}

void Interpreter::returnStuff(Parser::Node *a, std::map<std::string, object> &scope) {
    if (a->children.size() == 1) {
        return_value.type = VOIDS;
        ENDFUNCTION = true;
        return;
    }
    return_value = evaluateExpression(a->children[1], scope);
    ENDFUNCTION = true;
}

void Interpreter::assignVariable(Parser::Node *a, std::map<std::string, object> &scope) {
    scope[a->children[0]->value.attribute] = evaluateExpression(a->children[2], scope);
}

void Interpreter::executeFor(Parser::Node *a, std::map<std::string, object> &scope) {
    assignVariable(a->children[2], scope);
    object condition = evaluateExpression(a->children[4], scope);
    if (condition.type == BOOLS) {
        while (condition.bool_val) {
            executeProgram(a->children[9], scope);
            SKIP = false;
            if (ENDLOOP)break;
            assignVariable(a->children[6], scope);
            condition = evaluateExpression(a->children[4], scope);
        }
        ENDLOOP = false;
    }
}

void Interpreter::executeSimpleFor(Parser::Node *a, std::map<std::string, object> &scope) {
    object left = evaluateExpression(a->children[3], scope);
    object right = evaluateExpression(a->children[5], scope);
    if (left.type != INTS || right.type != INTS)return;

    scope[a->children[1]->value.attribute] = left;
    for (int i = left.int_val; i <= right.int_val; i++) {
        executeProgram(a->children[7], scope);
        SKIP = false;
        if (ENDLOOP)break;
        left.int_val++;
        scope[a->children[1]->value.attribute] = left;
    }
    ENDLOOP = false;
}

void Interpreter::executeStatement(Parser::Node *a, std::map<std::string, object> &scope) {
    if (ENDFUNCTION || ENDLOOP || SKIP)return;
    Parser::Node *first_child = a->children[0];
    std::string first_child_name = first_child->value.name;

    if (first_child_name == "<print_statement>") {
        printStuff(first_child, scope);
    } else if (first_child_name == "<if_statement>") {
        executeIf(first_child, scope);
    } else if (first_child_name == "<return_statement>") {
        returnStuff(first_child, scope);
    } else if (first_child_name == "<function_decl>") {
        declareFunction(a->children[0], scope);
    } else if (first_child_name == "<while_statement>") {
        executeWhile(a->children[0], scope);
    } else if (first_child_name == "<for_statement>") {
        executeFor(a->children[0], scope);
    } else if (first_child_name == "<assignment>") {
        assignVariable(a->children[0], scope);
    } else if (first_child_name == "<expression>") {
        evaluateExpression(a->children[0], scope);
    } else if (first_child_name == "<simple_for_statement>") {
        executeSimpleFor(a->children[0], scope);
    } else if (first_child_name == "<break_statement>") {
        ENDLOOP = true;
    } else if (first_child_name == "<continue_statement>") {
        SKIP = true;
    }
}

void Interpreter::displayTypeError(std::string operation, int line_number) {
    std::cout << "Invalid operands for operator " << operation << " in line " << line_number << std::endl;
    exit(1);
}

Interpreter::object Interpreter::evaluateExpression(Parser::Node *a, std::map<std::string, object> &scope) {
    Parser::Node *first_child = a->children[0];
    std::string first_child_name = first_child->value.name;
    object x;

    if (first_child_name == "identifier") {
        if (scope.find(first_child->value.attribute) != scope.end())
            x = scope[first_child->value.attribute];
        else {
            std::cout << "Error: variable " << first_child->value.attribute << " is not defined in this scope"
                      << std::endl;
            exit(1);
        }
        return x;
    } else if (first_child_name == "int_literal") {
        x.type = INTS;
        x.int_val = stoi(first_child->value.attribute);
        return x;
    } else if (first_child_name == "real_literal") {
        x.type = REAL;
        x.real_val = stof(first_child->value.attribute);
        return x;
    } else if (first_child_name == "string_literal") {
        x.type = STRING;
        x.string_val = first_child->value.attribute;
        return x;
    } else if (first_child_name == "<function_call>") {
        return executeFunction(first_child, scope);
    } else if (first_child_name == "input") {
        std::string s;
        std::cin >> s;
        x.type = STRING;
        x.string_val = s;
        return x;
    } else if (first_child_name == "true") {
        x.type = BOOLS;
        x.bool_val = true;
        return x;
    } else if (first_child_name == "false") {
        x.type = BOOLS;
        x.bool_val = false;
        return x;
    }


    if (a->children.size() == 1)return evaluateExpression(first_child, scope);
    if (first_child_name == "(")return evaluateExpression(a->children[1], scope);
    x = evaluateExpression(first_child, scope);
    object y = evaluateExpression(a->children[2], scope);
    object result;
    std::string operation = a->children[1]->value.name;
    int line_number = a->children[1]->value.line_number;
    switch (a->children[1]->value.name[0]) {
        case '=': //operation is ==
            if (std::max(x.type, y.type) <= INTS) { //if both are either int or real
                result.type = BOOLS;
                result.bool_val =
                        ((x.type == INTS) ? x.int_val : x.real_val) == ((y.type == INTS) ? y.int_val : y.real_val);
                return result;
            } else displayTypeError(operation, line_number);
        case '!': //operation is !=
            if (std::max(x.type, y.type) <= INTS) { //if both are either int or real
                result.type = BOOLS;
                result.bool_val =
                        ((x.type == INTS) ? x.int_val : x.real_val) != ((y.type == INTS) ? y.int_val : y.real_val);
                return result;
            } else displayTypeError(operation, line_number);
        case '<':
            if (a->children[1]->value.name == "<=") {
                if (std::max(x.type, y.type) <= INTS) { //if both are either int or real
                    result.type = BOOLS;
                    result.bool_val =
                            ((x.type == INTS) ? x.int_val : x.real_val) <= ((y.type == INTS) ? y.int_val : y.real_val);
                    return result;
                } else displayTypeError(operation, line_number);
            }
            if (std::max(x.type, y.type) <= INTS) { //if both are either int or real
                result.type = BOOLS;
                result.bool_val =
                        ((x.type == INTS) ? x.int_val : x.real_val) < ((y.type == INTS) ? y.int_val : y.real_val);
                return result;
            } else displayTypeError(operation, line_number);
        case '>':
            if (a->children[1]->value.name == ">=") {
                if (std::max(x.type, y.type) <= INTS) { //if both are either int or real
                    result.type = BOOLS;
                    result.bool_val =
                            ((x.type == INTS) ? x.int_val : x.real_val) >= ((y.type == INTS) ? y.int_val : y.real_val);
                    return result;
                } else displayTypeError(operation, line_number);
            }
            if (std::max(x.type, y.type) <= INTS) { //if both are either int or real
                result.type = BOOLS;
                result.bool_val =
                        ((x.type == INTS) ? x.int_val : x.real_val) > ((y.type == INTS) ? y.int_val : y.real_val);
                return result;
            } else displayTypeError(operation, line_number);
        case '+':
            if (std::max(x.type, y.type) <= INTS) { //if both are either int or real
                result.type = std::min(x.type, y.type);
                result.real_val =
                        ((x.type == INTS) ? x.int_val : x.real_val) + ((y.type == INTS) ? y.int_val : y.real_val);
                result.int_val = result.real_val;
                return result;
            } else displayTypeError(operation, line_number);
        case '-':
            if (std::max(x.type, y.type) <= INTS) {
                result.type = std::min(x.type, y.type);
                result.real_val =
                        ((x.type == INTS) ? x.int_val : x.real_val) - ((y.type == INTS) ? y.int_val : y.real_val);
                result.int_val = result.real_val;
                return result;
            } else displayTypeError(operation, line_number);
        case '*':
            if (std::max(x.type, y.type) <= INTS) {
                result.type = std::min(x.type, y.type);
                result.real_val =
                        ((x.type == INTS) ? x.int_val : x.real_val) * ((y.type == INTS) ? y.int_val : y.real_val);
                result.int_val = result.real_val;
                return result;
            } else displayTypeError(operation, line_number);
        case '/':
            if (a->children[1]->value.name == "//") {
                if (x.type == INTS && y.type == INTS) {
                    result.type = INTS;
                    result.int_val = x.int_val / y.int_val;
                    return result;
                } else displayTypeError(operation, line_number);
            }
            if (std::max(x.type, y.type) <= INTS) {
                result.type = REAL;
                result.real_val =
                        float((x.type == INTS) ? x.int_val : x.real_val) / ((y.type == INTS) ? y.int_val : y.real_val);
                return result;
            } else displayTypeError(operation, line_number);
        case '%':
            if (x.type == INTS && y.type == INTS) {
                result.type = INTS;
                result.int_val = x.int_val % y.int_val;
                return result;
            } else displayTypeError(operation, line_number);
        case 'a': //and operation
            if (x.type == BOOLS && y.type == BOOLS) {
                result.type = BOOLS;
                result.bool_val = x.bool_val && y.bool_val;
                return result;
            } else displayTypeError(operation, line_number);
        case 'o': // or operation
            if (x.type == BOOLS && y.type == BOOLS) {
                result.type = BOOLS;
                result.bool_val = x.bool_val || y.bool_val;
                return result;
            } else displayTypeError(operation, line_number);
    }

}

Interpreter::object Interpreter::executeFunction(Parser::Node *a, std::map<std::string, object> scope) {
    return_value.type = VOIDS;
    Interpreter::object b;
    if (scope.find(a->children[0]->value.attribute) != scope.end())
        b = scope[a->children[0]->value.attribute];
    else if (a->children[0]->value.attribute == "int") {
        object in = evaluateExpression(a->children[2], scope);
        object r;
        r.type = INTS;
        r.int_val = stoi(in.string_val);
        return r;
    } else if (a->children[0]->value.attribute == "real") {
        object in = evaluateExpression(a->children[2], scope);
        object r;
        r.type = REAL;
        r.real_val = stof(in.string_val);
        return r;
    } else {
        std::cout << "Error in line " << a->children[0]->value.line_number << std::endl;
        std::cout << "Could not find definition of function " << a->children[0]->value.attribute << " in current scope"
                  << std::endl;
        exit(1);
    }
    if (b.type != FUNCTION) {
        std::cout << "Error in line " << a->children[0]->value.line_number << std::endl;
        std::cout << a->children[0]->value.attribute << " is not last defined as a function in current scope"
                  << std::endl;
        exit(1);
    }
    Parser::Node *x = b.function_body;
    Parser::Node *expected = x->children[3];
    Parser::Node *found = a->children[2];
    int s1, s2;
    while (true) {
        s1 = expected->children.size();
        s2 = found->children.size();
        if (s1 == s2) {
            if (s1 == 0)break; //function with no arguments (child is ')')
            int last = s1 - 1;
            std::string var_name = expected->children[last]->value.attribute;
            object d = evaluateExpression(found->children[last], scope);
            scope[var_name] = d;
            if (s1 == 1)break;
            expected = expected->children[0];
            found = found->children[0];
        } else {
            std::cout << "Error in line " << a->children[0]->value.line_number << std::endl;
            std::cout << "Number of arguments don't match in function " << a->children[0]->value.attribute << std::endl;
            exit(1);
        }
    }
    if (s1 == 0) executeProgram(x->children[5], scope);
    else executeProgram(x->children[6], scope);
    ENDFUNCTION = false;
    object r = return_value;
    return_value.type = VOIDS;
    return r;
}
