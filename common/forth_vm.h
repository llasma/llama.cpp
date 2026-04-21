#pragma once
#include <stack>
#include <string>
#include <unordered_map>
#include <functional>
#include "llama.h"

// Forward declare llama context wrapper
struct llama_context;

class ForthVM {
public:
    // Stack types
    using StringStack = std::stack<std::string>;
    using ValueStack = std::stack<double>;  // MVP: numeric only; extend to variant later
    
    // Command handler signature
    using Handler = std::function<bool(ForthVM&, llama_context*)>;
    
    // std::unordered_map<std::string, void(ForthVM::*)()> handlers_;
    // std::unordered_map<std::string, bool(ForthVM::*)()> handlers_;
    // std::unordered_map<std::string, bool(ForthVM::*)(ForthVM &, llama_context *)()> handlers_;
    std::unordered_map<std::string, std::function<bool(ForthVM&, llama_context*)>> handlers_;

    ForthVM();
    
    // Parse and execute a command string
    // Returns: true if execution succeeded, false on error
    bool execute(const std::string& command, llama_context* ctx = nullptr);
    
    // Get top of string stack (for debugging/output)
    std::string top_string() const;
    bool has_string() const;
    int size() const;
        
    // Get top of value stack
    double top_value() const;
    bool has_value() const;

    // Register custom command handler
    void register_handler(const std::string& name, Handler fn);
    
    // cxxforth(): weird naming but then .... let forkers/forthers decide ....
    int forth(int argc, char** argv) const;
    
private:
    StringStack string_stack_;
    ValueStack value_stack_;
    // std::unordered_map<std::string, Handler> handlers_;
    
    // Built-in handlers
    bool cmd_add(ForthVM& vm, llama_context* ctx);      // +
    bool cmd_sub(ForthVM& vm, llama_context* ctx);      // -
    bool cmd_mul(ForthVM& vm, llama_context* ctx);      // *
    bool cmd_div(ForthVM& vm, llama_context* ctx);      // /
    bool cmd_sym(ForthVM& vm, llama_context* ctx);      // sym: (push symbol name)
    bool cmd_temp(ForthVM& vm, llama_context* ctx);     // temp: (set temperature)
    bool cmd_top_p(ForthVM& vm, llama_context* ctx);    // top-p: (set nucleus sampling)
    bool cmd_print(ForthVM& vm, llama_context* ctx);    // print: (output TOS)
    
    // Helper: tokenize space-delimited input
    static std::vector<std::string> tokenize(const std::string& input);
};
