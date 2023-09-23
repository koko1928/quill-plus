#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cmath>

class Interpreter {
public:
    Interpreter() {
        // Initialize variables
        variables_["pi"] = 3.14159265358979323846;
        variables_["e"] = 2.71828182845904523536;
    }

    void run() {
        std::cout << "C/JavaScript/Go-Like Language Interpreter" << std::endl;
        std::cout << "Type 'quit' to exit." << std::endl;

        while (true) {
            std::string input;
            std::cout << "> ";
            std::getline(std::cin, input);

            if (input == "quit") {
                break;
            }

            try {
                evaluateStatement(input);
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }

private:
    void evaluateStatement(const std::string& statement) {
        std::istringstream iss(statement);
        std::string token;
        iss >> token;

        if (token == "var") {
            // 変数宣言
            handleVariableDeclaration(iss);
        } else if (variables_.find(token) != variables_.end()) {
            // 変数代入
            handleVariableAssignment(iss, token);
        } else if (token == "if") {
            // 条件分岐
            handleConditionalStatement(iss);
        } else if (token == "for") {
            // ループ
            handleForLoop(iss);
        } else if (token == "func") {
            // 関数定義
            handleFunctionDefinition(iss);
        } else if (token == "return") {
            // 関数の戻り値
            handleFunctionReturn(iss);
        } else {
            throw std::runtime_error("Invalid statement: " + statement);
        }
    }

    void handleVariableDeclaration(std::istringstream& iss) {
        std::string varName;
        iss >> varName;

        if (variables_.find(varName) != variables_.end()) {
            throw std::runtime_error("Variable already declared: " + varName);
        }

        if (iss.peek() == '=') {
            iss.ignore();
            double value = evaluateExpression(iss);
            variables_[varName] = value;
        } else {
            variables_[varName] = 0.0;  // デフォルト値
        }
    }

    void handleVariableAssignment(std::istringstream& iss, const std::string& varName) {
        char equalSign;
        iss >> equalSign;

        if (equalSign != '=') {
            throw std::runtime_error("Invalid assignment syntax");
        }

        double value = evaluateExpression(iss);
        variables_[varName] = value;
    }

    void handleConditionalStatement(std::istringstream& iss) {
        double condition = evaluateExpression(iss);
        std::string thenKeyword;
        iss >> thenKeyword;

        if (thenKeyword != "then") {
            throw std::runtime_error("Expected 'then' after condition");
        }

        if (condition != 0.0) {
            std::string trueBranch;
            std::getline(iss, trueBranch);
            evaluateBlock(trueBranch);
        } else {
            std::string falseBranch;
            while (true) {
                std::string line;
                std::getline(iss, line);
                if (line == "end") {
                    break;
                }
                falseBranch += line + '\n';
            }
            evaluateBlock(falseBranch);
        }
    }

    void handleForLoop(std::istringstream& iss) {
        std::string loopHeader;
        std::getline(iss, loopHeader, '{');
        evaluateStatement(loopHeader + "{");

        double condition = evaluateExpression(iss);
        std::string doKeyword;
        iss >> doKeyword;

        if (doKeyword != "do") {
            throw std::runtime_error("Expected 'do' after condition");
        }

        while (condition != 0.0) {
            std::string loopBody;
            while (true) {
                std::string line;
                std::getline(iss, line);
                if (line == "end") {
                    break;
                }
                loopBody += line + '\n';
            }
            evaluateBlock(loopBody);
            condition = evaluateExpression(iss);
        }
    }

    void handleFunctionDefinition(std::istringstream& iss) {
        std::string funcName;
        iss >> funcName;

        if (functions_.find(funcName) != functions_.end()) {
            throw std::runtime_error("Function already defined: " + funcName);
        }

        std::string params;
        std::getline(iss, params, '{');

        std::vector<std::string> paramList;
        std::istringstream paramsIss(params);
        std::string param;
        while (paramsIss >> param) {
            paramList.push_back(param);
        }

        std::string funcBody;
        std::getline(iss, funcBody);
        evaluateBlock(funcBody);

        functions_[funcName] = {paramList, funcBody};
    }

    void handleFunctionReturn(std::istringstream& iss) {
        double result = evaluateExpression(iss);
        if (!currentFunction_.empty()) {
            currentFunctionReturnValue_ = result;
        } else {
            throw std::runtime_error("'return' statement outside of a function");
        }
    }

    void evaluateBlock(const std::string& block) {
        std::istringstream blockStream(block);
        std::string line;
        while (std::getline(blockStream, line)) {
            evaluateStatement(line);
        }
    }

    double evaluateExpression(std::istringstream& iss) {
        // 算術式の評価
        // ここで演算子の優先順位を考慮するなど、より高度な機能を追加できます
        double result = evaluateTerm(iss);
        std::string op;
        while (iss >> op) {
            if (op == "+") {
                result += evaluateTerm(iss);
            } else if (op == "-") {
                result -= evaluateTerm(iss);
            } else if (op == "*") {
                result *= evaluateTerm(iss);
            } else if (op == "/") {
                double divisor = evaluateTerm(iss);
                if (divisor == 0.0) {
                    throw std::runtime_error("Division by zero");
                }
                result /= divisor;
            } else {
                iss.putback(op.back());
                break;
            }
        }
        return result;
    }

    double evaluateTerm(std::istringstream& iss) {
        double result = evaluateFactor(iss);
        std::string op;
        while (iss >> op) {
            if (op == "*") {
                result *= evaluateFactor(iss);
            } else if (op == "/") {
                double divisor = evaluateFactor(iss);
                if (divisor == 0.0) {
                    throw std::runtime_error("Division by zero");
                }
                result /= divisor;
            } else {
                iss.putback(op.back());
                break;
            }
        }
        return result;
    }

    double evaluateFactor(std::istringstream& iss) {
        std::string token;
        iss >> token;

        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
            return std::stod(token);
        } else if (token == "(") {
            double result = evaluateExpression(iss);
            std::string closingParenthesis;
            iss >> closingParenthesis;
            if (closingParenthesis != ")") {
                throw std::runtime_error("Expected ')'");
            }
            return result;
        } else if (variables_.find(token) != variables_.end()) {
            return variables_[token];
        } else if (functions_.find(token) != functions_.end()) {
            // 関数呼び出し
            const auto& [paramList, funcBody] = functions_[token];
            std::map<std::string, double> prevVariables = variables_;

            std::string args;
            iss >> args;

            if (args != "(") {
                throw std::runtime_error("Expected '(' after function name");
            }

            for (const std::string& param : paramList) {
                double argValue = evaluateExpression(iss);
                variables_[param] = argValue;
                if (iss.peek() == ',') {
                    iss.ignore();
                }
            }

            double result = 0.0;

            if (currentFunction_ != token) {
                currentFunction_ = token;
                currentFunctionReturnValue_ = 0.0;
                evaluateBlock(funcBody);
                result = currentFunctionReturnValue_;
            } else {
                evaluateBlock(funcBody);
            }

            variables_ = prevVariables;
            currentFunction_.clear();
            return result;
        } else {
            throw std::runtime_error("Invalid expression: " + token);
        }
    }

    std::map<std::string, double> variables_;
    std::map<std::string, std::pair<std::vector<std::string>, std::string>> functions_;
    std::string currentFunction_;
    double currentFunctionReturnValue_ = 0.0;
};

int main() {
    Interpreter interpreter;
    interpreter.run();

    return 0;
}
