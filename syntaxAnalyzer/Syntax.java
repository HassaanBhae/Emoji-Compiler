import java.util.*;
import java.util.regex.*;

public class Implementation {

    enum TokenType {
        Keyword, ID, Datatype,
        IntConst, FloatConst, CharConst, StringConst, BoolConst, 
        If, Else, Elseif,
        Do, While, For,
        Return,
        Class, Interface,Abstract,
        AccessModifier, Static,Final ,
        Extends, Implements,
        This, Super, New,
        Try, Catch, Finally, Except,
        Comma, Colon, SemiColon, DBQoute, Qoute,
        OpenRoundBrkt, CloseRoundBrkt, OpenSqrBrkt, CloseSqrBrkt, OpenCurlyBrkt, CloseCurlyBrkt,
        Dot,
        AddSub, IncDec, MulDivMod,
        RelationOp1, RelationOp2,
        Equal, CompoundAssign,
        AND, OR, NOT,
        InvalidInput,
        $, Break, Continue,
        Void , Main,
        object
    }

    private static final Set<String> KEYWORDS = new HashSet<>(Arrays.asList(
        "publish", "preserve", "personal", "model", "case","concept","final","static", "countercase", "otherwise", "do", "while", "cycle", "carryon",
        "test", "grab", "finally", "new", "this", "super", "int", "float", "string", "char", "true", "false", "throw", "throws",
        "protocol", "extend", "implement", "stop", "return", "bool", "text"
    ));

    public static ArrayList<Object[]> tokenize(String input, int lineNum) {
        ArrayList<Object[]> TOKENS = new ArrayList<>();

        int i = 0;
        while (i < input.length()) {
            char ch = input.charAt(i);

            // Handle string constants
            if (ch == '"') {
                int start = i + 1;
                i++;
                StringBuilder sb = new StringBuilder();
                while (i < input.length()) {
                    if (input.charAt(i) == '\\' && i + 1 < input.length()) {
                        sb.append(input.charAt(i));
                        sb.append(input.charAt(i + 1));
                        i += 2;
                    } else if (input.charAt(i) == '"') {
                        break;
                    } else {
                        sb.append(input.charAt(i));
                        i++;
                    }
                }
                i++; // Skip closing quote
                TOKENS.add(new Object[]{TokenType.StringConst, sb.toString(), lineNum});
                continue;
            }

            // Token boundaries
            if (" \t\n\r(){}[]:;,'\".+-*/%=!&|<>".indexOf(ch) != -1) {
                if (!Character.isWhitespace(ch)) {
                    String tok = String.valueOf(ch);

                    // Try compound tokens
                    if (i + 1 < input.length()) {
                        String nextTok = tok + input.charAt(i + 1);

                        switch (nextTok) {
                            case "++": case "--":
                                TOKENS.add(new Object[]{TokenType.IncDec, nextTok, lineNum});
                                i += 2;
                                continue;
                            case "+=": case "-=": case "*=": case "/=": case "%=":
                                TOKENS.add(new Object[]{TokenType.CompoundAssign, nextTok, lineNum});
                                i += 2;
                                continue;
                            case "==": case "!=":
                                TOKENS.add(new Object[]{TokenType.RelationOp2, nextTok, lineNum});
                                i += 2;
                                continue;
                            case "<=": case ">=":
                                TOKENS.add(new Object[]{TokenType.RelationOp1, nextTok, lineNum});
                                i += 2;
                                continue;
                            case "&&":
                                TOKENS.add(new Object[]{TokenType.AND, nextTok, lineNum});
                                i += 2;
                                continue;
                            case "||":
                                TOKENS.add(new Object[]{TokenType.OR, nextTok, lineNum});
                                i += 2;
                                continue;
                        }
                    }

                    // Single-character tokens
                    TokenType type = switch (tok) {
                        case "+" -> TokenType.AddSub;
                        case "-" -> TokenType.AddSub;
                        case "*" -> TokenType.MulDivMod;
                        case "/" -> TokenType.MulDivMod;
                        case "%" -> TokenType.MulDivMod;
                        case "=" -> TokenType.Equal;
                        case "<", ">" -> TokenType.RelationOp1;
                        case "!" -> TokenType.NOT;
                        case "," -> TokenType.Comma;
                        case ":" -> TokenType.Colon;
                        case ";" -> TokenType.SemiColon;
                        case "'" -> TokenType.Qoute;
                        case "\"" -> TokenType.DBQoute;
                        case "(" -> TokenType.OpenRoundBrkt;
                        case ")" -> TokenType.CloseRoundBrkt;
                        case "[" -> TokenType.OpenSqrBrkt;
                        case "]" -> TokenType.CloseSqrBrkt;
                        case "{" -> TokenType.OpenCurlyBrkt;
                        case "}" -> TokenType.CloseCurlyBrkt;
                        case "." -> TokenType.Dot;
                        default -> TokenType.InvalidInput;
                    };
                    TOKENS.add(new Object[]{type, tok, lineNum});
                }
                i++;
                continue;
            }

            // Handle normal words/identifiers
            StringBuilder sb = new StringBuilder();
            while (i < input.length() && Character.toString(input.charAt(i)).matches("[a-zA-Z0-9_]+")) {
                sb.append(input.charAt(i));
                i++;
            }

            if (!sb.isEmpty()) {
                String match = sb.toString();
                TokenType classPart;

                if (match.equals("$")) classPart = TokenType.$;
                else if (match.matches("^[\\+-]?[0-9]+\\.[0-9]+$")) classPart = TokenType.FloatConst;
                else if (match.matches("^[\\+-]?[0-9]+$")) classPart = TokenType.IntConst;
                else if (KEYWORDS.contains(match)) {
                    classPart = switch (match) {
                        case "publish", "personal", "preserve" -> TokenType.AccessModifier;
                        case "model" -> TokenType.Class;
                        case "protocol" -> TokenType.Interface;
                        case "concept" -> TokenType.Abstract;
                        case "static" -> TokenType.Static;
                        case "final"  -> TokenType.Final;
                        case "extend" -> TokenType.Extends;
                        case "implement" -> TokenType.Implements;
                        case "case" -> TokenType.If;
                        case "countercase" -> TokenType.Elseif;
                        case "otherwise" -> TokenType.Else;
                        case "cycle" -> TokenType.For;
                        case "do" -> TokenType.Do;
                        case "while" -> TokenType.While;
                        case "stop" -> TokenType.Break;
                        case "carryon" -> TokenType.Continue;
                        case "return" -> TokenType.Return;
                        case "test" -> TokenType.Try;
                        case "grab" -> TokenType.Catch;
                        case "finally" -> TokenType.Finally;
                        case "throw"  ->  TokenType.Except;
                        case "true", "false" -> TokenType.BoolConst;
                        case "int", "float", "string", "bool", "char", "text" -> TokenType.Datatype;
                        case "this" -> TokenType.This;
                        case "super" -> TokenType.Super;
                        case "new" -> TokenType.New;
                        case "void"-> TokenType.Void;
                        case "object" -> TokenType.object;
                        default -> TokenType.InvalidInput;
                    };
                } else if (match.matches("^[a-zA-Z_][a-zA-Z0-9_]*$")) {
                    classPart = TokenType.ID;
                } else {
                    classPart = TokenType.InvalidInput;
                }

                TOKENS.add(new Object[]{classPart, match, lineNum});
            }
        }

        return TOKENS;
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        StringBuilder code = new StringBuilder();
        String line;
        System.out.println("Enter your code (Press Enter twice to finish):");

        while (true) {
            line = scanner.nextLine();
            if (line.trim().isEmpty()) break;
            code.append(line).append("\n");
        }

        String cleanedCode = removeComments(code.toString());
        String[] lines = cleanedCode.split("\n");
        ArrayList<Object[]> tokens = new ArrayList<>();

        for (int i = 0; i < lines.length; i++) {
            tokens.addAll(tokenize(lines[i], i + 1));
        }

        System.out.println("\nTOKENS:");
        for (Object[] arr : tokens) {
            System.out.println(Arrays.toString(arr));
        }
    }

    private static String removeComments(String code) {
        // Remove multiline comments
        Pattern multiLine = Pattern.compile("/\\.?\\*/", Pattern.DOTALL);
        Matcher matcher = multiLine.matcher(code);
        code = matcher.replaceAll("");

        // Remove single-line comments
        code = code.replaceAll("//.*", "");
        return code;
    }

    static class SA{
        static int index =0;
        public ArrayList<Object[]> TS;

        SA(ArrayList<Object[]> TS){
            
        
            this.TS = TS; 
            if(ClassDec()){
       
                if(index < TS.size() && TS.get(index)[0].equals(TokenType.$)){
                   
                    System.out.println("syntax is valid");
                    return;
                }
                else{
              
                    System.out.println("end marker is not present");
                    return;
                }
            }
             else{
                System.out.println("syntax is invalid" + TS.get(index)[2]);
            }
        }
        // <Defs> → <OptModList> <PostModDecl>

            public boolean Defs(){
                if(TS.get(index)[0].equals(TokenType.AccessModifier)|| TS.get(index)[0].equals(TokenType.Static) || TS.get(index)[0].equals(TokenType.Final)|| TS.get(index)[0].equals(TokenType.Abstract)){
                    if(OptModList()){
                        if(PostModDecl()){
                            return true;
                         }
                    }
                }
               return false;
            }


            // <MainMethodDecl> → public static void main ( ) { <OptMST> } 
            public boolean MainMethodDecl(){
                if(TS.get(index)[0].equals(TokenType.AccessModifier)){
                    if(TS.get(index)[0].equals(TokenType.AccessModifier)){
                        index++;
                        if(TS.get(index)[0].equals(TokenType.Static)){
                            index++;
                            if(TS.get(index)[0].equals(TokenType.Void)){
                                index++;
                                if(TS.get(index)[0].equals(TokenType.Main)){
                                    index++;
                                     if(TS.get(index)[0].equals(TokenType.OpenRoundBrkt)){
                                        index++;
                                        if(TS.get(index)[0].equals(TokenType.CloseRoundBrkt)){
                                            index++;
                                            if(TS.get(index)[0].equals(TokenType.OpenCurlyBrkt)){
                                                index++;
                                                if(OptMST()){
                                                    if(TS.get(index)[0].equals(TokenType.CloseCurlyBrkt)){
                                                        index++;
                                                        return true;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                return false ;
            }

            // <MethodDecl> → <OptModList>  DT ID  ( <OptParaList> ) {<OptMST>}
            public boolean ProtocolMember(){
                if(TS.get(index)[0].equals(TokenType.AccessModifier)|| (TS.get(index)[0].equals(TokenType.Static)) || (TS.get(index)[0].equals(TokenType.Final))|| (TS.get(index)[0].equals(TokenType.Abstract))){
                    if(OptModList()){
                        if(TS.get(index)[0].equals(TokenType.Datatype)){
                            index++;
                            if(TS.get(index)[0].equals(TokenType.ID)){
                                index++;
                                if(TS.get(index)[0].equals(TokenType.OpenRoundBrkt)){
                                    index++;
                                    if(OptParaList()){
                                        if(TS.get(index)[0].equals(TokenType.CloseCurlyBrkt)){
                                            index++;
                                            if(TS.get(index)[0].equals(TokenType.OpenCurlyBrkt)){
                                                index++;
                                                if(OptMST()){
                                                    if(TS.get(index)[0].equals(TokenType.CloseCurlyBrkt)){
                                                        index++;
                                                        return true;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
            
                            }
                        }
                    }
                }
                return false;
            }

            // <ClassMembers> → <ConceptMember> <ClassMembers> | ε
            public boolean ClassMembers() {
                if(TS.get(index)[0].equals(TokenType.AccessModifier)|| (TS.get(index)[0].equals(TokenType.Static)) || (TS.get(index)[0].equals(TokenType.Final))|| (TS.get(index)[0].equals(TokenType.Abstract))){
                    if (ConceptMember()) {
                        if (ClassMembers()) {
                            return true;
                        } 
                    }  
                }  
                else if(TS.get(index)[0].equals(TokenType.CloseCurlyBrkt)){//folloe set
                    return true;

                }
                 return false;
               }

            // <ConceptMember> →<OptModList> DT ID <ConceptTail>
            public boolean ConceptMember() {
                if(TS.get(index)[0].equals(TokenType.AccessModifier)|| (TS.get(index)[0].equals(TokenType.Static)) || (TS.get(index)[0].equals(TokenType.Final))|| (TS.get(index)[0].equals(TokenType.Abstract))){
                    if (OptModList()) {
                        if (TS.get(index)[0] == TokenType.Datatype) {
                            index++;
                            if (TS.get(index)[0] == TokenType.ID) {
                                index++;
                                if (ConceptTail()) {
                                    return true;
                                }
                            }
                        }
                    }
                }
                return false;
            }


            // <ConceptTail> → ( <OptParaList> ) <TailAfterParams> | ;

            public boolean ConceptTail() {
                if (TS.get(index)[0] == TokenType.OpenRoundBrkt) {
                    if (TS.get(index)[0] == TokenType.OpenRoundBrkt) {
                        index++;
                        if (OptParaList()) {
                            if (TS.get(index)[0] == TokenType.CloseRoundBrkt) {
                                 index++;
                                if (TailAfterParams()) {
                                    return true;
                                }
                            }
                        }
                    }
                }
                else if (TS.get(index)[0] == TokenType.SemiColon) {
                    if (TS.get(index)[0] == TokenType.SemiColon){
                        index++;
                        return true;
                    }
                }
                return false;
            }
            // <TailAfterParams> → ; | { <OptMST> }

            public boolean TailAfterParams() {
                if (TS.get(index)[0] == TokenType.SemiColon) {
                    index++;
                    return true;
                }
                else if (TS.get(index)[0] == TokenType.OpenCurlyBrkt) {
                    if(TS.get(index)[0] == TokenType.OpenCurlyBrkt){
                        index++;
                         if (OptMST()) {
                             if (TS.get(index)[0] == TokenType.CloseCurlyBrkt) {
                                index++;
                                return true;
                             }
                        }
                    }
                }
                return false;
            }

            //<ConstructorDec> → <OptModList> ID ( <OptParaList> ) { <OptTS> <OptMST> }
            public boolean ConstructorDec() {
                if (TS.get(index)[0].equals(TokenType.AccessModifier)||(TS.get(index)[0].equals(TokenType.Static))||(TS.get(index)[0].equals(TokenType.Final))||(TS.get(index)[0].equals(TokenType.Abstract))) {
                    
                    if (OptModList()) {
                        if (TS.get(index)[0].equals(TokenType.ID)) { // ID
                            index++;
                            if (TS.get(index)[0].equals(TokenType.OpenRoundBrkt)) { // (
                                index++;
                                if (OptParaList()) { 
                                    if (TS.get(index)[0].equals(TokenType.CloseRoundBrkt)) { // )
                                        index++;
                                        if (TS.get(index)[0].equals(TokenType.OpenCurlyBrkt)) { // {
                                            index++;
                                            if (OptTS()) { 
                                                if (OptMST()) {
                                                    if (TS.get(index)[0].equals(TokenType.CloseCurlyBrkt)) { // }
                                                        index++;
                                                        return true;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                return false;
                
            }
            
            //<OptTS> → <TSCall> ; | ε
            public boolean OptTS() {
                if (TSCall()) {
                    if (TS.get(index)[0].equals(TokenType.SemiColon)) { // ;
                        index++;
                        return true;
                    }
                }
                // ε (epsilon)
                else if(TS.get(index)[0].equals(TokenType.For)||TS.get(index)[0].equals(TokenType.While)||TS.get(index)[0].equals(TokenType.Do)||TS.get(index)[0].equals(TokenType.Try)||TS.get(index)[0].equals(TokenType.Except)||TS.get(index)[0].equals(TokenType.If)||TS.get(index)[0].equals(TokenType.Return)||TS.get(index)[0].equals(TokenType.Break)||TS.get(index)[0].equals(TokenType.Continue)||TS.get(index)[0].equals(TokenType.This)||TS.get(index)[0].equals(TokenType.Super)||TS.get(index)[0].equals(TokenType.OpenRoundBrkt)||TS.get(index)[0].equals(TokenType.ID)||TS.get(index)[0].equals(TokenType.Datatype)||TS.get(index)[0].equals(TokenType.object)){
                    return true;
                }
                return false;
            }
            
            //<TSCall> → this ( <OptParaList> ) | super ( <OptParaList> )| ID ID ( <OptParaList> )
            public boolean TSCall() {
                if(TS.get(index)[0].equals(TokenType.This)){
                    if (TS.get(index)[0].equals(TokenType.This)) { // this
                        index++;
                        if (TS.get(index)[0].equals(TokenType.OpenRoundBrkt)) {
                            index++;
                            if (OptParaList()) {
                                if (TS.get(index)[0].equals(TokenType.CloseRoundBrkt)) {
                                 index++;
                                    return true;
                                }
                             }
                         }
                    }    
                } 
                else if (TS.get(index)[0].equals(TokenType.Super)) { 
                    if(TS.get(index)[0].equals(TokenType.Super)){
                        index++;
                        if (TS.get(index)[0].equals(TokenType.OpenRoundBrkt)) {
                             index++;
                            if (OptParaList()) {
                                if (TS.get(index)[0].equals(TokenType.CloseRoundBrkt)) {
                                    index++;
                                    return true;
                                }
                            }
                        }
                    }
                } 
                else if (TS.get(index)[0].equals(TokenType.ID)) {
                    if(TS.get(index)[0].equals(TokenType.ID)){
                        index++;
                        if (TS.get(index)[0].equals(TokenType.ID)) {
                            index++;
                            if (TS.get(index)[0].equals(TokenType.OpenRoundBrkt)) {
                                index++;
                                if (OptParaList()) {
                                    if (TS.get(index)[0].equals(TokenType.CloseRoundBrkt)) {
                                        index++;
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
                return false;
            