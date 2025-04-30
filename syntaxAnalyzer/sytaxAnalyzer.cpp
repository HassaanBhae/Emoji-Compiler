#include <iostream>
#include <windows.h>
#include <vector>
#include <regex>
#include <unordered_set>
#include <string>
#include <fstream>

using namespace std;
    
// Token types
enum class myTokenType { KEYWORD, IDENTIFIER, LITERAL, OPERATOR, SEPARATOR, COMMENT, WHITESPACE ,ACCESSMODIFIER ,DT ,ID };
int index = 0; // Current token index

bool program() {
    if (tokens[index].type == myTokenType::ACCESSMODIFIER || tokens[index].type == myTokenType::ACCESSMODIFIER ||
        tokens[index].type == myTokenType::DT || tokens[index].type == myTokenType::ID) {
        
        if (dec()) {
            if (mainFunc()) {
                if (dec()) {
                    return true;
                }
            }
        }
    }
    return false;
}

// <dec> ->  <struct_dec> | <struct_children> | <interface> | ε
bool dec() {
    if (tokens[index].type == myTokenType::Public || tokens[index].type == myTokenType::Private) {
        if (struct_dec()) {
            return true;
        }
    }
    else if (tokens[index].type == myTokenType::DT || tokens[index].type == myTokenType::ID) {
        if (struct_children()) {
            return true;
        }
    }
    else if (tokens[index].value == "interface") { // Checking token value
        if (interfaceFunc()) {
            return true;
        }
    }
    else if (/* ε case: check if current token is FIRST of next rule */) {
        return true;
    }

    return false;
}
