#pragma once

#include "lexer.h"

namespace Parser {
    struct Node {
        Lexer::token value;
        std::vector<Node *> children;
    };

    void visit(Node *x, int tabs = 0);

    struct earley_item {
        int production;
        int scanned;
        int origin;
        Node *tree;

        bool operator<(const earley_item &e) const;

        bool operator==(const earley_item &e) const;
    };

    Node *parse(std::vector<Lexer::token> input);
}