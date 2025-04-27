from collections import defaultdict
import re
import sys

class GrammarAnalyzer:
    def __init__(self):
        self.productions = {}
        self.terminals = set()
        self.non_terminals = set()
        self.first_sets = {}
        self.follow_sets = {}
        self.start_symbol = None
        self.epsilon = "ε"

    def add_production(self, non_terminal, production):
        """Add a production rule to the grammar."""
        if non_terminal not in self.productions:
            self.productions[non_terminal] = []
            self.non_terminals.add(non_terminal)

        # Split production into alternatives (separated by |)
        alternatives = [alt.strip() for alt in production.split('|')]
        
        for alt in alternatives:
            # Parse symbols from the alternative
            symbols = self._parse_symbols(alt)
            
            if symbols:
                self.productions[non_terminal].append(symbols)
            else:
                # Empty production (epsilon)
                self.productions[non_terminal].append([self.epsilon])

    def _parse_symbols(self, text):
        """Parse symbols from text, handling non-terminals in angle brackets."""
        symbols = []
        current_symbol = ""
        in_angle_bracket = False
        
        for char in text:
            if char == '<':
                if current_symbol.strip():
                    for token in current_symbol.strip().split():
                        if token:
                            symbols.append(token)
                            if token != self.epsilon and '<' not in token and '>' not in token:
                                self.terminals.add(token)
                current_symbol = char
                in_angle_bracket = True
            elif char == '>' and in_angle_bracket:
                current_symbol += char
                symbols.append(current_symbol)
                self.non_terminals.add(current_symbol)
                current_symbol = ""
                in_angle_bracket = False
            elif in_angle_bracket:
                current_symbol += char
            else:
                current_symbol += char
        
        # Add any remaining symbols
        if current_symbol.strip():
            for token in current_symbol.strip().split():
                if token:
                    symbols.append(token)
                    if token != self.epsilon and '<' not in token and '>' not in token:
                        self.terminals.add(token)
        
        return symbols

    def parse_grammar_from_text(self, text):
        """Parse grammar from a text representation."""
        lines = text.split('\n')
        for line in lines:
            line = line.strip()
            if not line or line.startswith('//') or line.startswith('**'):
                continue

            # Extract non-terminal and production
            parts = line.split('->')
            if len(parts) == 2:
                non_terminal = parts[0].strip()
                production = parts[1].strip()
                
                # Clean up non-terminal
                non_terminal = non_terminal.replace('**', '').strip()
                if non_terminal.startswith('<') and non_terminal.endswith('>'):
                    self.add_production(non_terminal, production)
                    # Set the first non-terminal as the start symbol if not already set
                    if self.start_symbol is None:
                        self.start_symbol = non_terminal

    def compute_first_sets(self):
        """Compute FIRST sets for all non-terminals."""
        # Initialize FIRST sets
        for nt in self.non_terminals:
            self.first_sets[nt] = set()
        
        for terminal in self.terminals:
            self.first_sets[terminal] = {terminal}
        
        self.first_sets[self.epsilon] = {self.epsilon}
        
        # Repeat until no changes are made
        while True:
            changes = False
            
            for nt, productions in self.productions.items():
                for production in productions:
                    # Handle epsilon production
                    if len(production) == 1 and production[0] == self.epsilon:
                        if self.epsilon not in self.first_sets[nt]:
                            self.first_sets[nt].add(self.epsilon)
                            changes = True
                        continue
                    
                    # Compute FIRST for this production
                    k = 0
                    all_can_derive_epsilon = True
                    
                    while k < len(production) and all_can_derive_epsilon:
                        symbol = production[k]
                        
                        if symbol not in self.first_sets:
                            if '<' not in symbol and '>' not in symbol:
                                self.first_sets[symbol] = {symbol}  # It's a terminal
                        
                        # Add all symbols from FIRST(symbol) except epsilon
                        for term in self.first_sets.get(symbol, set()) - {self.epsilon}:
                            if term not in self.first_sets[nt]:
                                self.first_sets[nt].add(term)
                                changes = True
                        
                        # Check if this symbol can derive epsilon
                        if self.epsilon not in self.first_sets.get(symbol, set()):
                            all_can_derive_epsilon = False
                        
                        k += 1
                    
                    # If all symbols can derive epsilon, add epsilon to FIRST(nt)
                    if all_can_derive_epsilon and self.epsilon not in self.first_sets[nt]:
                        self.first_sets[nt].add(self.epsilon)
                        changes = True
            
            if not changes:
                break

    def compute_follow_sets(self):
        """Compute FOLLOW sets for all non-terminals."""
        # Initialize FOLLOW sets
        for nt in self.non_terminals:
            self.follow_sets[nt] = set()
        
        # Add $ to FOLLOW(start_symbol)
        if self.start_symbol:
            self.follow_sets[self.start_symbol].add('$')
        
        # Repeat until no changes are made
        while True:
            changes = False
            
            for nt, productions in self.productions.items():
                for production in productions:
                    for i, symbol in enumerate(production):
                        if symbol in self.non_terminals:
                            # Case 2: A -> αBβ, add FIRST(β) - {ε} to FOLLOW(B)
                            if i + 1 < len(production):
                                remaining = production[i+1:]
                                first_of_remaining = self.get_first_of_string(remaining)
                                
                                # Add all terminals from FIRST(remaining) to FOLLOW(symbol)
                                for term in first_of_remaining - {self.epsilon}:
                                    if term not in self.follow_sets[symbol]:
                                        self.follow_sets[symbol].add(term)
                                        changes = True
                                
                                # Case 3: A -> αBβ and ε ∈ FIRST(β), add FOLLOW(A) to FOLLOW(B)
                                if self.epsilon in first_of_remaining:
                                    for term in self.follow_sets[nt]:
                                        if term not in self.follow_sets[symbol]:
                                            self.follow_sets[symbol].add(term)
                                            changes = True
                            
                            # Case 3: A -> αB, add FOLLOW(A) to FOLLOW(B)
                            elif i == len(production) - 1:
                                for term in self.follow_sets[nt]:
                                    if term not in self.follow_sets[symbol]:
                                        self.follow_sets[symbol].add(term)
                                        changes = True
            
            if not changes:
                break

    def get_first_of_string(self, symbols):
        """Compute FIRST set for a string of symbols."""
        if not symbols:
            return {self.epsilon}
        
        first_set = set()
        all_derive_epsilon = True
        
        for symbol in symbols:
            # If symbol is not in first_sets, it might be a terminal
            if symbol not in self.first_sets:
                if symbol != self.epsilon:
                    first_set.add(symbol)
                all_derive_epsilon = False
                break
            
            # Add all terminals from FIRST(symbol) except epsilon
            for term in self.first_sets[symbol] - {self.epsilon}:
                first_set.add(term)
            
            # If this symbol cannot derive epsilon, stop here
            if self.epsilon not in self.first_sets[symbol]:
                all_derive_epsilon = False
                break
        
        # If all symbols can derive epsilon, add epsilon to the result
        if all_derive_epsilon:
            first_set.add(self.epsilon)
        
        return first_set

    def check_for_ll1_conflicts(self):
        """Check for LL(1) conflicts in the grammar."""
        conflicts = []
        
        for nt, productions in self.productions.items():
            first_sets_of_productions = []
            
            # Compute FIRST sets for each production
            for production in productions:
                first_set = self.get_first_of_string(production)
                first_sets_of_productions.append(first_set)
            
            # Check for FIRST/FIRST conflicts
            for i in range(len(first_sets_of_productions)):
                for j in range(i + 1, len(first_sets_of_productions)):
                    intersection = first_sets_of_productions[i] & first_sets_of_productions[j]
                    intersection.discard(self.epsilon)
                    
                    if intersection:
                        conflicts.append({
                            'type': 'FIRST/FIRST',
                            'non_terminal': nt,
                            'production1': ' '.join(productions[i]),
                            'production2': ' '.join(productions[j]),
                            'common_terminals': intersection
                        })
            
            # Check for FIRST/FOLLOW conflicts
            for i, prod_first_set in enumerate(first_sets_of_productions):
                if self.epsilon in prod_first_set:
                    follow_set = self.follow_sets[nt]
                    intersection = prod_first_set & follow_set
                    
                    if intersection - {self.epsilon}:
                        conflicts.append({
                            'type': 'FIRST/FOLLOW',
                            'non_terminal': nt,
                            'production': ' '.join(productions[i]),
                            'common_terminals': intersection - {self.epsilon}
                        })
        
        return conflicts

    def is_ll1(self):
        """Check if the grammar is LL(1)."""
        return len(self.check_for_ll1_conflicts()) == 0

    def generate_parsing_table(self):
        """Generate LL(1) parsing table."""
        table = {}
        for nt in self.non_terminals:
            table[nt] = {}
        
        for nt, productions in self.productions.items():
            for i, production in enumerate(productions):
                first_set = self.get_first_of_string(production)
                
                # For each terminal in FIRST(production), add the production to the table
                for terminal in first_set - {self.epsilon}:
                    if terminal in table[nt]:
                        # Conflict detected
                        return None
                    table[nt][terminal] = (production, i)
                
                # If ε is in FIRST(production), add the production to the table for each terminal in FOLLOW(nt)
                if self.epsilon in first_set:
                    for terminal in self.follow_sets[nt]:
                        if terminal in table[nt]:
                            # Conflict detected
                            return None
                        table[nt][terminal] = (production, i)
        
        return table

    def print_first_follow_sets(self):
        """Print FIRST and FOLLOW sets."""
        print("FIRST Sets:")
        for nt in sorted(self.non_terminals):
            first_set = self.first_sets.get(nt, set())
            print(f"FIRST({nt}) = {{{', '.join(sorted(first_set))}}}")
        
        print("\nFOLLOW Sets:")
        for nt in sorted(self.non_terminals):
            follow_set = self.follow_sets.get(nt, set())
            print(f"FOLLOW({nt}) = {{{', '.join(sorted(follow_set))}}}")

    def suggest_ll1_transformations(self):
        """Suggest transformations to make the grammar LL(1)."""
        conflicts = self.check_for_ll1_conflicts()
        
        if not conflicts:
            return "The grammar is already LL(1)."
        
        suggestions = []
        
        for conflict in conflicts:
            if conflict['type'] == 'FIRST/FIRST':
                suggestions.append(f"Left factor the productions for {conflict['non_terminal']}:")
                suggestions.append(f"  Current: {conflict['non_terminal']} -> {conflict['production1']} | {conflict['production2']}")
                suggestions.append(f"  Common terminals: {', '.join(conflict['common_terminals'])}\n")
            else:  # FIRST/FOLLOW
                suggestions.append(f"Eliminate left recursion or resolve FIRST/FOLLOW conflict for {conflict['non_terminal']}:")
                suggestions.append(f"  Production: {conflict['non_terminal']} -> {conflict['production']}")
                suggestions.append(f"  This production can derive epsilon and there's a conflict with FOLLOW({conflict['non_terminal']})")
                suggestions.append(f"  Common terminals: {', '.join(conflict['common_terminals'])}\n")
        
        return "\n".join(suggestions)

    def generate_recursive_descent_parser(self, language="python"):
        """Generate a recursive descent parser for the grammar."""
        if not self.is_ll1():
            conflicts = self.check_for_ll1_conflicts()
            return f"Grammar is not LL(1). Found {len(conflicts)} conflicts. Please fix these before generating parser code."
        
        if language.lower() == "python":
            return self._generate_python_parser()
        elif language.lower() == "java":
            return self._generate_java_parser()
        elif language.lower() == "c++":
            return self._generate_cpp_parser()
        else:
            return f"Language {language} not supported yet."

    def _generate_python_parser(self):
        """Generate Python recursive descent parser code."""
        code = """
import re

class Token:
    def __init__(self, type, value=None, line=0, column=0):
        self.type = type
        self.value = value
        self.line = line
        self.column = column

    def __repr__(self):
        if self.value:
            return f"{self.type}({self.value})"
        return f"{self.type}"

class Lexer:
    def __init__(self, input_text):
        self.input_text = input_text
        self.position = 0
        self.line = 1
        self.column = 1
        self.current_char = self.input_text[0] if input_text else None
        
        # Define token patterns
        self.token_patterns = [
"""
        
        # Generate token patterns from terminals
        for terminal in sorted(self.terminals):
            if terminal != self.epsilon:
                # Escape special regex characters
                escaped_terminal = re.escape(terminal)
                code += f"            (r'{escaped_terminal}', '{terminal}'),\n"
        
        code += """
            # Add patterns for identifiers, constants, etc.
            (r'[a-zA-Z_][a-zA-Z0-9_]*', 'ID'),
            (r'\\d+', 'Const'),
            # Add more token patterns as needed
        ]

    def advance(self):
        if self.current_char == '\\n':
            self.line += 1
            self.column = 1
        else:
            self.column += 1
            
        self.position += 1
        if self.position < len(self.input_text):
            self.current_char = self.input_text[self.position]
        else:
            self.current_char = None

    def skip_whitespace(self):
        while self.current_char and self.current_char.isspace():
            self.advance()

    def get_next_token(self):
        while self.current_char:
            if self.current_char.isspace():
                self.skip_whitespace()
                continue

            # Try to match token patterns
            for pattern, token_type in self.token_patterns:
                regex = re.compile(pattern)
                match = regex.match(self.input_text[self.position:])
                if match:
                    value = match.group(0)
                    line, column = self.line, self.column
                    
                    # Advance position
                    for _ in range(len(value)):
                        self.advance()
                        
                    return Token(token_type, value, line, column)

            # If no match is found, it's an error
            raise Exception(f"Invalid character: {self.current_char} at line {self.line}, column {self.column}")

        # End of input
        return Token('EOF', None, self.line, self.column)

class Parser:
    def __init__(self, lexer):
        self.lexer = lexer
        self.current_token = self.lexer.get_next_token()
        # Symbol table, AST, or other data structures can be added here

    def error(self, expected):
        raise Exception(f"Syntax error at line {self.current_token.line}, column {self.current_token.column}: Expected {expected}, got {self.current_token.type}")

    def eat(self, token_type):
        if self.current_token.type == token_type:
            value = self.current_token.value
            self.current_token = self.lexer.get_next_token()
            return value
        else:
            self.error(token_type)
"""
        
        # Generate parser methods for each non-terminal
        for nt in sorted(self.non_terminals):
            method_name = nt.replace('<', '').replace('>', '').replace('-', '_')
            code += f"\n    def parse_{method_name}(self):\n"
            code += f"        # Parse {nt}\n"
            
            if nt in self.productions:
                # Generate code for each production
                first_prod = True
                for production in self.productions[nt]:
                    first_set = self.get_first_of_string(production)
                    first_terminals = first_set - {self.epsilon}
                    
                    # If production can derive epsilon, also include FOLLOW set
                    if self.epsilon in first_set:
                        first_terminals |= self.follow_sets[nt]
                    
                    if first_terminals:
                        condition = " or ".join([f"self.current_token.type == '{term}'" for term in first_terminals])
                        
                        if first_prod:
                            code += f"        if {condition}:\n"
                            first_prod = False
                        else:
                            code += f"        elif {condition}:\n"
                        
                        # Generate code for the symbols in the production
                        for symbol in production:
                            if symbol == self.epsilon:
                                code += "            pass  # epsilon production\n"
                            elif symbol in self.non_terminals:
                                method = symbol.replace('<', '').replace('>', '').replace('-', '_')
                                code += f"            self.parse_{method}()\n"
                            else:
                                code += f"            self.eat('{symbol}')\n"
                
                # Add else clause for error handling
                if not first_prod:  # Only add else if we had at least one condition
                    code += "        else:\n"
                    expected = ", ".join(sorted(set.union(*[self.get_first_of_string(p) - {self.epsilon} for p in self.productions[nt]])))
                    code += f"            self.error('{expected}')\n"
            
        
        # Generate main parsing method
        start_method = self.start_symbol.replace('<', '').replace('>', '').replace('-', '_')
        code += f"""
    def parse(self):
        self.parse_{start_method}()
        if self.current_token.type != 'EOF':
            self.error('EOF')
        print("Parsing completed successfully!")

def main():
    # Example usage
    input_text = '''
    # Your test code here
    '''
    
    try:
        lexer = Lexer(input_text)
        parser = Parser(lexer)
        parser.parse()
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()
"""
        
        return code

    def _generate_java_parser(self):
        """Generate Java recursive descent parser code."""
        # Java implementation would go here
        return "Java parser generation not implemented yet."

    def _generate_cpp_parser(self):
        """Generate C++ recursive descent parser code."""
        # C++ implementation would go here
        return "C++ parser generation not implemented yet."

def main():
    print("Grammar Analysis Tool")
    print("=====================\n")
    
    # Get input method
    print("How would you like to input the grammar?")
    print("1. From a file")
    print("2. Enter manually")
    choice = input("Enter your choice (1 or 2): ")
    
    grammar_text = ""
    
    if choice == "1":
        filename = input("Enter the filename: ")
        try:
            with open(filename, 'r') as file:
                grammar_text = file.read()
        except Exception as e:
            print(f"Error reading file: {e}")
            sys.exit(1)
    else:
        print("Enter the grammar rules (empty line to finish):")
        while True:
            line = input()
            if not line:
                break
            grammar_text += line + "\n"
    
    # Create analyzer and process grammar
    analyzer = GrammarAnalyzer()
    analyzer.parse_grammar_from_text(grammar_text)
    
    # Compute FIRST and FOLLOW sets
    analyzer.compute_first_sets()
    analyzer.compute_follow_sets()
    
    # Display results
    analyzer.print_first_follow_sets()
    
    # Check if grammar is LL(1)
    conflicts = analyzer.check_for_ll1_conflicts()
    if not conflicts:
        print("\nThe grammar is LL(1)!\n")
        
        # Ask if user wants to generate parser code
        generate_code = input("Would you like to generate a recursive descent parser? (y/n): ")
        if generate_code.lower() == 'y':
            language = input("What language? (python/java/c++): ")
            parser_code = analyzer.generate_recursive_descent_parser(language)
            
            print("\nGenerated Parser Code:")
            print(parser_code)
            
            # Ask if user wants to save the code
            save_code = input("Would you like to save this code to a file? (y/n): ")
            if save_code.lower() == 'y':
                filename = input("Enter the filename: ")
                try:
                    with open(filename, 'w') as file:
                        file.write(parser_code)
                    print(f"Code saved to {filename}")
                except Exception as e:
                    print(f"Error saving file: {e}")
    else:
        print(f"\nThe grammar is NOT LL(1). Found {len(conflicts)} conflicts:")
        
        for i, conflict in enumerate(conflicts):
            print(f"\nConflict {i+1} ({conflict['type']}):")
            print(f"  Non-terminal: {conflict['non_terminal']}")
            
            if conflict['type'] == 'FIRST/FIRST':
                print(f"  Production 1: {conflict['production1']}")
                print(f"  Production 2: {conflict['production2']}")
            else:  # FIRST/FOLLOW
                print(f"  Production: {conflict['production']}")
            
            print(f"  Common terminals: {', '.join(conflict['common_terminals'])}")
        
        # Suggest transformations
        print("\nSuggested transformations:")
        print(analyzer.suggest_ll1_transformations())

if __name__ == "__main__":
    main()