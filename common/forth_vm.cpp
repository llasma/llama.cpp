#include "forth_vm.h"
#include "common.h"  // llama.cpp common utilities
#include <sstream>
#include <iostream>
#include <cmath>
#include "cxxforth.h"


ForthVM::ForthVM() {
/*
    // Register built-in arithmetic handlers
    handlers_["+"] = ForthVM::cmd_add;
    handlers_["-"] = &ForthVM::cmd_sub;
    handlers_["*"] = &ForthVM::cmd_mul;
    handlers_["/"] = &ForthVM::cmd_div;
    
    // Register LLM control handlers
    handlers_["sym:"] = &ForthVM::cmd_sym;
    handlers_["temp:"] = &ForthVM::cmd_temp;
    handlers_["top-p:"] = &ForthVM::cmd_top_p;
    handlers_["print:"] = &ForthVM::cmd_print;
*/
}

std::vector<std::string> ForthVM::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (iss >> token) {  // space-delimited, skips whitespace
        tokens.push_back(token);
    }
    return tokens;
}

bool ForthVM::execute(const std::string& command, llama_context* ctx) {
    auto tokens = tokenize(command);
    
    if (tokens[0]=="FORTH") {
      std::cout << "  is FORTH !!";
      
/*
# Source - https://stackoverflow.com/a/19082779
# Posted by Dietmar Kühl, modified by community. See post 'Timeline' for change history
# Retrieved 2026-04-20, License - CC BY-SA 3.0
*/

      // std::string array[] = { "s1", "s2" };
      std::vector<char*> vec;
      std::transform(std::begin(tokens), std::end(tokens),
                     std::back_inserter(vec),
                     [](std::string& s){ s.push_back(0); return &s[0]; });
      vec.push_back(nullptr);
      char** carray = vec.data();
           
      // forth(tokens.size(), command.c_str());
      forth(tokens.size(), carray);
    }
    
    for (const auto& token : tokens) {
        // Check if token is a registered command
        auto it = handlers_.find(token);
        if (it != handlers_.end()) {
            // Execute handler; abort on failure
            if (!it->second(*this, ctx)) {
                std::cerr << "[ForthVM] Error executing command: " << token << std::endl;
                return false;
            }
        } else {
            // Push unknown token as string literal
            string_stack_.push(token);
            // Also try to push as numeric value if possible
            try {
                value_stack_.push(std::stod(token));
            } catch (...) {
                // Not a number; string stack only
            }
        }
    }
    return true;
}

// ============ Built-in Handler Implementations ============

bool ForthVM::cmd_add(ForthVM& vm, llama_context*) {
    if (vm.value_stack_.size() < 2) return false;
    double b = vm.value_stack_.top(); vm.value_stack_.pop();
    double a = vm.value_stack_.top(); vm.value_stack_.pop();
    vm.value_stack_.push(a + b);
    return true;
}

bool ForthVM::cmd_sub(ForthVM& vm, llama_context*) {
    if (vm.value_stack_.size() < 2) return false;
    double b = vm.value_stack_.top(); vm.value_stack_.pop();
    double a = vm.value_stack_.top(); vm.value_stack_.pop();
    vm.value_stack_.push(a - b);
    return true;
}

bool ForthVM::cmd_mul(ForthVM& vm, llama_context*) {
    if (vm.value_stack_.size() < 2) return false;
    double b = vm.value_stack_.top(); vm.value_stack_.pop();
    double a = vm.value_stack_.top(); vm.value_stack_.pop();
    vm.value_stack_.push(a * b);
    return true;
}

bool ForthVM::cmd_div(ForthVM& vm, llama_context*) {
    if (vm.value_stack_.size() < 2) return false;
    double b = vm.value_stack_.top(); vm.value_stack_.pop();
    double a = vm.value_stack_.top(); vm.value_stack_.pop();
    if (std::abs(b) < 1e-10) return false;  // avoid division by zero
    vm.value_stack_.push(a / b);
    return true;
}

bool ForthVM::cmd_sym(ForthVM& vm, llama_context*) {
    // sym: expects a symbol name on string stack TOS
    if (vm.string_stack_.empty()) return false;
    std::string sym_name = vm.string_stack_.top(); vm.string_stack_.pop();
    // MVP: just echo; future: integrate with grammar/sampling
    std::cout << "[ForthVM] Registered symbol: " << sym_name << std::endl;
    return true;
}

bool ForthVM::cmd_temp(ForthVM& vm, llama_context* ctx) {
    if (vm.value_stack_.empty()) return false;
    double temp = vm.value_stack_.top(); vm.value_stack_.pop();
    if (ctx && temp >= 0.0 && temp <= 2.0) {
        // Note: llama.cpp doesn't expose per-call temp setter in public API
        // This is a placeholder; real implementation would use sampler chain
        std::cout << "[ForthVM] Temperature set to: " << temp << " (requires sampler re-init)" << std::endl;
        return true;
    }
    return false;
}

bool ForthVM::cmd_top_p(ForthVM& vm, llama_context* ctx) {
    if (vm.value_stack_.empty()) return false;
    double top_p = vm.value_stack_.top(); vm.value_stack_.pop();
    if (ctx && top_p >= 0.0 && top_p <= 1.0) {
        std::cout << "[ForthVM] Top-p set to: " << top_p << " (requires sampler re-init)" << std::endl;
        return true;
    }
    return false;
}

bool ForthVM::cmd_print(ForthVM& vm, llama_context*) {
    if (!vm.value_stack_.empty()) {
        std::cout << "[ForthVM] Value TOS: " << vm.value_stack_.top() << std::endl;
    } else if (!vm.string_stack_.empty()) {
        std::cout << "[ForthVM] String TOS: " << vm.string_stack_.top() << std::endl;
    } else {
        std::cout << "[ForthVM] Stacks empty" << std::endl;
    }
    return true;
}

// ============ Public Getters ============
std::string ForthVM::top_string() const {
    return string_stack_.empty() ? "" : string_stack_.top();
}

int ForthVM::size() const {
    return string_stack_.empty() ? 0 : string_stack_.size();
}

bool ForthVM::has_string() const { return !string_stack_.empty(); }
double ForthVM::top_value() const {
    return value_stack_.empty() ? 0.0 : value_stack_.top();
}
bool ForthVM::has_value() const { return !value_stack_.empty(); }

// int cxxforth_main(int argc, const char** argv);
int ForthVM::forth(int argc, char** argv) const {
    cxxforth_main(argc,argv);
    return string_stack_.empty() ? 0 : string_stack_.size();
}
