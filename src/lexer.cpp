#include <iostream>
#include <fstream>

#include "lexer.h"

bool Lexer::token::operator<(const Lexer::token &t) const {
    return name < t.name;
}

bool Lexer::token::operator==(const Lexer::token &t) const {
    return name == t.name;
}

bool Lexer::State::operator<(const Lexer::State &x) const {
    return type < x.type;
}

std::multiset<Lexer::State> Lexer::next_states(std::multiset<Lexer::State> current_states, char c) {
    std::multiset<Lexer::State> result;
    for (Lexer::State s: current_states) {
        for (Lexer::State *nxt: s.transitions[c]) {
            result.insert(*nxt);
        }
    }
    return result;
}

std::vector<Lexer::token>
Lexer::create_token(std::multiset<Lexer::State> start_states, std::string line, int line_number) {
    std::vector<token> result;
    if (!line.size())return result;
    std::multiset<State> current = start_states;
    int last_seen = -1;
    Lexer::token t;
    for (int i = 0; i < line.size(); i++) {
        current = next_states(current, line[i]);
        if (!current.size())break;
        for (State s: current) {
            if (!s.type)continue;
            last_seen = i + 1;
            // cout << s.type << endl;
            t.line_number = line_number;
            t.attribute = line.substr(0, i + 1);
            switch (s.type) {
                case KEYWORD:
                    t.name = line.substr(0, i + 1);
                    break;
                case INTT:
                    t.name = "int_literal";
                    break;
                case REALT:
                    t.name = "real_literal";
                    break;
                case IDENTIFIER:
                    t.name = "identifier";
                    break;
                case STRINGL:
                    t.name = "string_literal";
                    t.attribute = line.substr(1, i - 1);
                    break;
            }
            break;
        }
    }
    if (last_seen == -1) {
        if (line[0] == ' ' || line[0] == '\t')
            return (create_token(start_states, line.substr(1, line.size() - 1), line_number));
        std::cout << "Error: could not recognize " << line << std::endl;
        exit(1);
    }
    result.push_back(t);
    std::vector<token> additions = create_token(start_states, line.substr(last_seen, line.size() - last_seen),
                                                line_number);
    result.insert(result.end(), additions.begin(), additions.end());
    return result;
}

std::vector<Lexer::token> Lexer::tokenize(std::string filename) {

    std::string filepath = "../programs/" + filename;

    Lexer::State integerealStart;
    Lexer::State realEnd;
    Lexer::State identifierStart;
    Lexer::State identifierEnd;
    Lexer::State keywordStart;
    Lexer::State stringStart;
    Lexer::State stringMid;
    Lexer::State stringEnd;

    stringStart.transitions[char(34)].push_back(&stringMid); // for character "
    stringMid.transitions[char(34)].push_back(&stringEnd); // for character "
    stringEnd.type = STRINGL;
    for (char c = ' '; c <= '~'; c++) {
        if (c != char(34))
            stringMid.transitions[c].push_back(&stringMid);
    }

    for (char c = '0'; c <= '9'; c++) {
        integerealStart.transitions[c].push_back(&integerealStart);
        realEnd.transitions[c].push_back(&realEnd);
        identifierEnd.transitions[c].push_back(&identifierEnd);
    }
    integerealStart.transitions['.'].push_back(&realEnd);
    integerealStart.type = INTT;
    realEnd.type = REALT;

    for (char c = 'a'; c <= 'z'; c++) {
        identifierStart.transitions[c].push_back(&identifierEnd);
        identifierEnd.transitions[c].push_back(&identifierEnd);
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        identifierStart.transitions[c].push_back(&identifierEnd);
        identifierEnd.transitions[c].push_back(&identifierEnd);
    }
    identifierEnd.type = IDENTIFIER;

    std::ifstream k("../dep/keywords.k");
    if (!k) {
        std::cerr << "[Error]!!! Keywords file not found" << std::endl;
        exit(1);
    }
    std::string kwd;
    while (k >> kwd) {
        State *current = &keywordStart;
        for (char c: kwd) {
            State *nextState = new State;
            current->transitions[c].push_back(nextState);
            current = nextState;
        }
        current->type = KEYWORD;
    }

    std::multiset<State> start_states = {integerealStart, identifierStart, keywordStart, stringStart};

    std::ifstream i(filepath);
    if (!i) {
        std::cerr << "[Error]!!! Program file " << filename << " not found." << std::endl;
        exit(1);
    }
    std::vector<token> tokens;
    int line_number = 1;
    for (std::string line; getline(i, line);) {
        std::vector<token> additions = create_token(start_states, line, line_number);
        tokens.insert(tokens.end(), additions.begin(), additions.end());
        line_number++;
    }
    return tokens;
}