#include <iostream>
#include <vector>
#include <tuple>
#include <set>
#include <fstream>
#include <sstream>

#include "parser.h"

void Parser::visit(Parser::Node *x, int tabs) {
    for (int i = 0; i < tabs; i++) {
        std::cout << "\t";
    }
    std::cout << x->value.name << ' ';
    if (x->value.name != x->value.attribute)
        std::cout << x->value.attribute;
    std::cout << std::endl;
    for (Node *y: x->children) {
        visit(y, tabs + 1);
    }
}

bool Parser::earley_item::operator<(const Parser::earley_item &e) const {
    return (std::tuple<int, int, int>({production, scanned, origin}) <
            std::tuple<int, int, int>({e.production, e.scanned, e.origin}));
}

bool Parser::earley_item::operator==(const Parser::earley_item &e) const {
    return (std::tuple<int, int, int>({production, scanned, origin}) ==
            std::tuple<int, int, int>({e.production, e.scanned, e.origin}));
}

Parser::Node *Parser::parse(std::vector<Lexer::token> input) {
    std::ifstream g("../dep/grammar.g");
    if(!g) {
        std::cerr << "[Error]!!! Grammar file not found" << std::endl;
        exit(1);
    }
    std::set<Lexer::token> non_terminals;
    std::vector<std::pair<Lexer::token, std::vector<Lexer::token> > > grammar;
    Lexer::token start;
    bool first = true;
    for (std::string line; getline(g, line);) {
        if (line == "")continue;
        std::stringstream words(line);
        std::string word;
        words >> word;
        Lexer::token non_t;
        non_t.name = word;
        non_terminals.insert(non_t);
        if (first) {
            start = non_t;
            first = false;
        }
        std::vector<Lexer::token> production;
        words >> word;
        while (words >> word) {
            Lexer::token t;
            t.name = word;
            production.push_back(t);
        }
        grammar.push_back({non_t, production});
    }

    int n = input.size();
    std::vector <std::set<earley_item>> a(n + 1);
    for (int i = 0; i < grammar.size(); i++) {
        if (grammar[i].first == start) {
            earley_item e = {i, 0, 0, new Node};
            e.tree->value = start;
            a[0].insert(e);
        }
    }
    for (int i = 0; i <= n; i++) {
        if (!a[i].size()) {
            int error_line = input[i].line_number;
            std::cout << "Syntax error in line " << error_line << std::endl;
            exit(1);
        }
        while (true) {
            int prev_size = a[i].size();
            std::set <earley_item> additions;
            for (auto item: a[i]) {
                int p = item.production;
                int s = item.scanned;
                int o = item.origin;
                std::vector <Lexer::token> rule = grammar[p].second;
                Lexer::token nt = grammar[p].first;
                if (rule.size() == s) {
                    for (auto item2: a[o]) {
                        int oldp = item2.production;
                        int olds = item2.scanned;
                        int oldo = item2.origin;
                        std::vector <Lexer::token> oldrule = grammar[oldp].second;
                        if (oldrule.size() != olds && oldrule[olds] == nt) {
                            earley_item e = {oldp, olds + 1, oldo, new Node};
                            e.tree->value = item2.tree->value;
                            e.tree->children = item2.tree->children;
                            e.tree->children.push_back(item.tree);
                            additions.insert(e);
                        }
                    }
                } else if (non_terminals.find(rule[s]) != non_terminals.end()) {
                    for (int k = 0; k < grammar.size(); k++) {
                        Lexer::token n = grammar[k].first;
                        if (n == rule[s]) {
                            Node *x = new Node;
                            x->value = rule[s];
                            additions.insert({k, 0, i, x});
                        }
                    }
                }
            }
            a[i].insert(additions.begin(), additions.end());
            if (a[i].size() == prev_size)break;
        }
        if (i == n)break;
        for (auto item: a[i]) {
            int p = item.production;
            int s = item.scanned;
            int o = item.origin;
            std::vector <Lexer::token> rule = grammar[p].second;
            Lexer::token nt = grammar[p].first;
            if (s != rule.size() && rule[s] == input[i]) {
                earley_item e = {p, s + 1, o, new Node};
                e.tree->value = item.tree->value;
                e.tree->children = item.tree->children;
                Node *x = new Node;
                x->value = input[i];
                e.tree->children.push_back(x);
                a[i + 1].insert(e);
            }
        }
    }
    // cout << grammar.size() << endl;
    // for(token t:input){
    // 	cout << t.name;
    // 	if(t.attribute!=t.name)cout << ' ' << t.attribute;
    // 	cout << endl;
    // }
    for (auto item: a[n]) {
        if (item.origin == 0 && item.scanned == grammar[item.production].second.size() &&
            grammar[item.production].first == start) {
            // visit(item.tree);
            return (item.tree);
        }
    }
    std::cout << "Syntax error in line " << input[n-1].line_number << std::endl;
    exit(1);
}