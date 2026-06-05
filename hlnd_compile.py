import os

class PerfectHollandCompiler:
    def __init__(self):
        self.symbol_table = {}       
        self.next_mem_addr = 500     
        self.temp_str_addr = 100     
        
        self.label_stack = []        
        self.label_counter = 0       
        self.current_function = None 

    def parse_components(self, content):
        content = content.strip()
        if content.endswith("//"):
            content = content[:-2]
        return [sub.strip() for sub in content.split("\\\\") if sub.strip()]

    def clean_quotes(self, text):
        text = text.strip()
        if text.startswith(('\'', '"', '‘', '’', '`')) and text.endswith(('\'', '"', '‘', '’', '`')):
            return text[1:-1]
        return text

    def is_literal_string(self, text):
        text = text.strip()
        return text.startswith(('\'', '"', '‘', '’', '`')) and text.endswith(('\'', '"', '‘', '’', '`'))

    def compile(self, source_code):
        compiled_lines = []
        # \r 유령 공백 차단 로직 유지
        source_code = source_code.replace('\r', '')
        lines = source_code.splitlines()

        for idx, line in enumerate(lines, 1):
            line = line.strip()
            if not line or line.startswith("#"):
                continue

            # 1. 변수 관리 명령어 (//alloc) 처리
            if line.startswith("//alloc"):
                content = line[7:].strip()
                tokens = self.parse_components(content)
                if not tokens: continue
                
                var_part = tokens[0]
                if "**" in var_part:
                    _, var_name = var_part.split("**")
                    var_name = var_name.strip()
                    if var_name not in self.symbol_table:
                        self.symbol_table[var_name] = self.next_mem_addr
                        self.next_mem_addr += 1
                    var_addr = self.symbol_table[var_name]

                    if len(tokens) >= 3:
                        param = tokens[1].strip()    
                        command = tokens[2].strip()  
                        if command == "save":
                            if "+" in param:
                                p1, p2 = param.split("+")
                                p1, p2 = p1.strip(), p2.strip()
                                compiled_lines.append(f"LOAD {self.symbol_table[p1]}" if p1 in self.symbol_table else f"PUSH {p1}")
                                compiled_lines.append(f"LOAD {self.symbol_table[p2]}" if p2 in self.symbol_table else f"PUSH {p2}")
                                compiled_lines.append("ADD")
                            else:
                                compiled_lines.append(f"LOAD {self.symbol_table[param]}" if param in self.symbol_table else f"PUSH {param}")
                            compiled_lines.append(f"STORE {var_addr}")
                continue

            # 2. 함수 및 제어 블록 (//def) 처리
            if line.startswith("//def"):
                content = line[5:].strip()
                
                if "close" in content:
                    if self.label_stack:
                        lbl_info = self.label_stack.pop()
                        lbl_id = lbl_info['id']
                        if lbl_info['action'] in ['if', 'else']:
                            compiled_lines.append(f":IF_END_{lbl_id}")
                        elif lbl_info['action'] == 'while':
                            compiled_lines.append(f"JUMP WHILE_START_{lbl_id}")
                            compiled_lines.append(f":WHILE_END_{lbl_id}")
                    else:
                        if self.current_function == "main":
                            compiled_lines.append("HALT")
                        self.current_function = None
                    continue

                tokens = self.parse_components(content)
                if not tokens: continue

                if tokens[0] in ["main", "num", "cha"]:
                    self.current_function = tokens[1] if len(tokens) > 1 else tokens[0]
                    continue

                if tokens[0] == "logic" and tokens[1] == "if":
                    cond = tokens[2] if len(tokens) > 2 else ""
                    if "==" in cond:
                        var_name, val = cond.split("==")
                        var_name, val = var_name.strip(), val.strip()
                        var_addr = self.symbol_table.get(var_name, 500)
                        lbl_id = self.label_counter
                        self.label_counter += 1
                        compiled_lines.append(f"LOAD {var_addr}")
                        compiled_lines.append(f"PUSH {val}")
                        compiled_lines.append("CMP")
                        compiled_lines.append(f"JE IF_TRUE_{lbl_id}")
                        compiled_lines.append(f"JUMP IF_ELSE_{lbl_id}")
                        compiled_lines.append(f":IF_TRUE_{lbl_id}")
                        self.label_stack.append({'id': lbl_id, 'action': 'if'})
                    continue

                if tokens[0] == "else":
                    if self.label_stack and self.label_stack[-1]['action'] == 'if':
                        lbl_id = self.label_stack.pop()['id']
                        compiled_lines.append(f"JUMP IF_END_{lbl_id}")
                        compiled_lines.append(f":IF_ELSE_{lbl_id}")
                        self.label_stack.append({'id': lbl_id, 'action': 'else'})
                    continue

            # 3. 일반 입출력 실행 스트림 (//open) 처리
            if line.startswith("//open"):
                content = line[6:].strip()
                tokens = self.parse_components(content)
                if not tokens: continue

                command = tokens[0]
                params = tokens[1:]

                if command == "write":
                    for param in params:
                        param = param.strip()
                        if self.is_literal_string(param):
                            compiled_lines.append(f'WRT {self.temp_str_addr} "{self.clean_quotes(param)}"')
                            compiled_lines.append(f'PRT {self.temp_str_addr}')
                        elif param in self.symbol_table:
                            compiled_lines.append(f'LOAD {self.symbol_table[param]}')
                            compiled_lines.append('PRINT')

                elif command == "read":
                    for param in params:
                        param = param.strip()
                        if param.startswith("sep="): continue
                        if param not in self.symbol_table:
                            self.symbol_table[param] = self.next_mem_addr
                            self.next_mem_addr += 1
                        compiled_lines.append(f'INP_NUM {self.symbol_table[param]}')
                continue

            if "//drop//" in line:
                compiled_lines.append(f'WRT {self.temp_str_addr} ""')
                compiled_lines.append(f'PRT {self.temp_str_addr}')
                continue

        # 강제 종료 안전장치
        has_halt = False
        for cl in compiled_lines:
            if "HALT" in cl:
                has_halt = True
                break
        if not has_halt:
            compiled_lines.append("HALT")

        return "\n".join(compiled_lines)

def main():
    source_path = os.path.join("code", "main.holland")
    output_path = os.path.join("runtime", "main.hlnd")

    # ★ 파일 강제 재생성 로직 삭제! 파일이 없을 때만 최소한의 예외 처리
    if not os.path.exists(source_path):
        print(f"[에러] {source_path} 파일이 존재하지 않습니다. 파일을 생성하고 코드를 작성해 주세요.")
        return

    with open(source_path, "r", encoding="utf-8") as f:
        source_code = f.read()

    compiler = PerfectHollandCompiler()
    compiled_result = compiler.compile(source_code)

    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(compiled_result + "\n")

    print(f"[컴파일 완료] -> {output_path} (기계어 생성 성공)")

if __name__ == "__main__":
    main()