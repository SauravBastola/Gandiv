#pragma once

#include <vector>
#include <map>
#include <set>
#include <string>

namespace Lexer {

    enum tokenType{
        NONE,
        KEYWORD,
        INTT,
        REALT,
        IDENTIFIER,
        STRINGL
    };

    struct token {
        std::string name;
        std::string attribute;
        int line_number;

        bool operator<(const token&t) const;
        bool operator==(const token& t) const;


    };

    struct State{
        std::map<char, std::vector<State*>> transitions;
        tokenType type;
        bool operator<(const State& x) const;
    };

    std::multiset<State> next_states(std::multiset<State> current_states, char c);

    std::vector<token> create_token(std::multiset<State> start_states, std::string line, int line_number);

    std::vector<token> tokenize(std::string filename);
}