#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <algorithm>

class HollandVirtualMachine {
private:
    std::vector<int> memory;               
    std::vector<int> op_stack;             
    std::map<int, std::string> string_buffer; 
    int flag;                              

    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

public:
    HollandVirtualMachine() : flag(0) {
        memory.resize(1024, 0);
    }

    bool run(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;

        std::vector<std::string> instructions;
        std::map<std::string, int> labels;
        std::string line;

        while (std::getline(file, line)) {
            std::string cleaned = trim(line);
            if (cleaned.empty() || cleaned[0] == '#') continue;
            
            if (cleaned[0] == ':') {
                labels[cleaned.substr(1)] = instructions.size();
            } else {
                instructions.push_back(cleaned);
            }
        }
        file.close();

        if (instructions.empty()) return false;

        // ★ [초강력 디버깅] 가상머신이 읽은 실시간 기계어 내용을 강제로 콘솔에 덤프합니다.
        std::cout << "\n[VM] 성공적으로 " << instructions.size() << "개의 명령어 로드 완료.\n";
        std::cout << "=== [기계어 추적 로그] ===\n";
        for (size_t i = 0; i < instructions.size(); ++i) {
            std::cout << " Line " << i + 1 << ": " << instructions[i] << "\n";
        }
        std::cout << "==========================\n\n";
        std::cout << "--- [실행 스트림 시작] ---\n";

        size_t pc = 0;
        while (pc < instructions.size()) {
            std::string raw_line = instructions[pc]; 
            std::stringstream ss(raw_line);
            std::string op;
            ss >> op;

            if (op == "PUSH") { int val; ss >> val; op_stack.push_back(val); }
            else if (op == "STORE") { int addr; ss >> addr; if (!op_stack.empty()) { memory[addr] = op_stack.back(); op_stack.pop_back(); } }
            else if (op == "LOAD") { int addr; ss >> addr; op_stack.push_back(memory[addr]); }
            else if (op == "ADD") { if (op_stack.size() >= 2) { int b = op_stack.back(); op_stack.pop_back(); int a = op_stack.back(); op_stack.pop_back(); op_stack.push_back(a + b); } }
            else if (op == "SUB") { if (op_stack.size() >= 2) { int b = op_stack.back(); op_stack.pop_back(); int a = op_stack.back(); op_stack.pop_back(); op_stack.push_back(a - b); } }
            
            else if (op == "INP_NUM") { 
                int addr; ss >> addr; 
                int user_input = 0;
                std::cin >> user_input; 
                memory[addr] = user_input; 
            }
            
            else if (op == "WRT") {
                int addr; ss >> addr;
                size_t first_quote = raw_line.find('"');
                size_t last_quote = raw_line.rfind('"');
                
                if (first_quote != std::string::npos && last_quote != std::string::npos && first_quote < last_quote) {
                    string_buffer[addr] = raw_line.substr(first_quote + 1, last_quote - first_quote - 1);
                } else {
                    string_buffer[addr] = ""; 
                }
            }
            
            else if (op == "PRT") { 
                int addr; ss >> addr; 
                if (string_buffer.find(addr) != string_buffer.end()) {
                    std::cout << string_buffer[addr] << std::endl;
                } else {
                    std::cout << "[VM 경고] " << addr << "번지에 출력할 문자열이 비어있음" << std::endl;
                }
            }
            
            else if (op == "PRINT") { if (!op_stack.empty()) { std::cout << op_stack.back() << std::endl; op_stack.pop_back(); } }
            else if (op == "CMP") { if (op_stack.size() >= 2) { int b = op_stack.back(); op_stack.pop_back(); int a = op_stack.back(); op_stack.pop_back(); if (a == b) flag = 0; else if (a > b) flag = 1; else flag = -1; } }
            else if (op == "JE") { std::string target; ss >> target; if (flag == 0) { pc = labels[target]; continue; } }
            else if (op == "JL") { std::string target; ss >> target; if (flag == -1) { pc = labels[target]; continue; } }
            else if (op == "JUMP") { std::string target; ss >> target; if (labels.find(target) != labels.end()) { pc = labels[target]; continue; } }
            else if (op == "HALT") { break; }

            pc++;
        }
        return true;
    }
};

int main() {
    HollandVirtualMachine vm;
    std::string default_path = "runtime/main.hlnd";

    if (!vm.run(default_path)) {
        std::cerr << "[에러] 파일을 로드할 수 없습니다.\n";
    }
    std::cout << "-------------------------\n";
    return 0;
}