#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <algorithm>

// OS별 실행 파일 경로 추적을 위한 헤더
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #include <windows.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    #include <limits.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

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

    // OS 독립적으로 현재 가상머신 실행 파일의 폴더 경로를 반환하는 함수
    std::string get_executable_directory() {
        std::string path = "";
#if defined(_WIN32)
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        path = std::string(buffer);
        size_t pos = path.find_last_of("\\/");
        if (pos != std::string::npos) path = path.substr(0, pos + 1);
#elif defined(__APPLE__)
        char buffer[PATH_MAX];
        uint32_t size = sizeof(buffer);
        if (_NSGetExecutablePath(buffer, &size) == 0) {
            path = std::string(buffer);
            size_t pos = path.find_last_of("/");
            if (pos != std::string::npos) path = path.substr(0, pos + 1);
        }
#else
        char buffer[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", buffer, PATH_MAX);
        if (count != -1) {
            path = std::string(buffer, count);
            size_t pos = path.find_last_of("/");
            if (pos != std::string::npos) path = path.substr(0, pos + 1);
        }
#endif
        return path;
    }

    bool run(const std::string& filename) {
        // 실행 파일 폴더 기준 + 타겟 파일명을 합쳐서 진짜 절대 경로 계산
        std::string full_path = get_executable_directory() + filename;
        
        std::ifstream file(full_path);
        if (!file.is_open()) {
            std::cerr << "[에러] 파일 타겟팅 실패! 시도한 경로: " << full_path << "\n";
            return false;
        }

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

        std::cout << "\n[VM] 성공적으로 " << instructions.size() << "개의 명령어 로드 완료. 실행을 시작합니다.\n";
        std::cout << "-------------------------------------\n";

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
                }
            }
            
            else if (op == "PRT") { 
                int addr; ss >> addr; 
                if (string_buffer.find(addr) != string_buffer.end()) {
                    std::cout << string_buffer[addr] << std::endl;
                } else {
                    std::cout << std::endl;
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
    
    // 실행 파일 폴더 밑에 있는 runtime/main.hlnd 를 상대적으로 타격
#if defined(_WIN32)
    std::string target_file = "runtime\\main.hlnd";
#else
    std::string target_file = "runtime/main.hlnd";
#endif

    std::cout << "--- [Holland VM 고정 경로 동기화 모드] ---\n";
    if (!vm.run(target_file)) {
        std::cerr << "[실패] 경로가 올바르지 않거나 파일이 비어있습니다.\n";
    }
    std::cout << "-----------------------------------------\n";
    return 0;
}