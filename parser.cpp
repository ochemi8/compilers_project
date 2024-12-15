#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdexcept>
using namespace std;


typedef enum {
    SEMICOLON, 
    IF, 
    THEN, 
    END, 
    REPEAT, 
    UNTIL, 
    READ, 
    WRITE,
    ASSIGN, 
    LESSTHAN, 
    EQUAL, 
    PLUS, 
    MINUS, 
    MULT, 
    DIV,
    OPENBRACKET, 
    CLOSEBRACKET, 
    IDENTIFIER, 
    NUMBER,
    UNKNOWN,
    ELSE
} TokenType;

typedef struct {
    string value;
    TokenType type;
} Token;

const char *keywords[] = {
    "if",
    "then",
    "end",
    "repeat",
    "until",
    "read",
    "write"
    };
const TokenType keywordsTokens[] = {
    IF,
    THEN,
    END,
    REPEAT,
    UNTIL,
    READ,
    WRITE
    };
     string tokenTypeToString(TokenType type) {
        switch (type) {
            case SEMICOLON: return "SEMICOLON";
            case IF: return "IF";
            case THEN: return "THEN";
            case END: return "END";
            case REPEAT: return "REPEAT";
            case UNTIL: return "UNTIL";
            case READ: return "READ";
            case WRITE: return "WRITE";
            case ASSIGN: return "ASSIGN";
            case LESSTHAN: return "LESSTHAN";
            case EQUAL: return "EQUAL";
            case PLUS: return "PLUS";
            case MINUS: return "MINUS";
            case MULT: return "MULT";
            case DIV: return "DIV";
            case OPENBRACKET: return "OPENBRACKET";
            case CLOSEBRACKET: return "CLOSEBRACKET";
            case IDENTIFIER: return "IDENTIFIER";
            case NUMBER: return "NUMBER";
            case UNKNOWN: return "UNKNOWN";
            default: return "UNKNOWN";
        }
    }
 
string filetoss(const string& filename) {
    ifstream file(filename); // Open the file
    if (!file.is_open()) {
        fprintf(stderr,"can't open file");
    }

    stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();  
}
void comment_handling(string code,int* i){
    int braces_stack=1;
     //cout<<*i;
    (*i)++;
   
    while(braces_stack){
        if(code[*i]=='{'){braces_stack++;}
        else if(code[*i]=='}'){braces_stack--;}
        (*i)++;
        //cout<<"testcomment "<<i<<endl;
    }

}
vector <pair <string,TokenType>> tokenize(string code){
    int vc_index=0;
    vector <pair <string,TokenType>> output ;
    //cout<<"test1\n";
    for(int i =0 ; i<code.length() ;i++){
        //cout<<"test2 "<<i<<endl;
       
        string buf="";
        Token token; 
        if(token.type==UNKNOWN) {fprintf(stderr,"Unknown character \"%c\"",code[i-1]); exit(1);}
        if(code[i]=='{') comment_handling(code,&i);
        
        if(isdigit(code[i])){
        while (isdigit(code[i])){
            buf+=code[i];
           // cout<<"test3 "<<i<<" "<<buf<<endl;
            i++;
        }
        token.value=buf;
        token.type=NUMBER;
        output.push_back(make_pair(token.value,token.type));
        i--;
        continue;}

        if(!isalnum(code[i]) && !isspace(code[i]) ){
            buf=code[i];
            switch(code[i]){
                case '=' : token.type=EQUAL;
                break;
                case ';' : token.type=SEMICOLON;
                break;
                case '+' : token.type=PLUS;
                break;
                case '-' : token.type=MINUS;
                break;
                case '*' : token.type=MULT;
                break;
                case '/' : token.type=DIV;
                break;
                case '(' : token.type=OPENBRACKET;
                break;
                case ')' : token.type=CLOSEBRACKET;
                break;
                case '<' : token.type=LESSTHAN;
                break;
                case ':' : if(code[i+1]=='=') {token.type=ASSIGN; i++; buf+=code[i];}
                            else token.type=UNKNOWN;
                break;
                default: token.type=UNKNOWN;
                
            }
            token.value=buf;
            output.push_back(make_pair(token.value,token.type));
        continue;
        }
        if(isalpha(code[i])){
            token.type=UNKNOWN;

            while(isalnum(code[i])){
                
                buf+=code[i];
                //if(isdigit(code[i])) token.type=IDENTIFIER;
                i++;
            }
           // if(token.type!=IDENTIFIER){
                int  j=0;
                while(j<7){
                    if(!(buf.compare(keywords[j]))) token.type=TokenType(j+1);
                    //else token.type=IDENTIFIER;
                    j++;
                }
                if(!((int)token.type < 8 && (int)token.type > 0)) token.type=IDENTIFIER;
            //}
            token.value=buf;
            output.push_back(make_pair(token.value,token.type));
            i--;
            
        continue;

        }




    }
    return output;

}
class Parser {
private:
    vector<pair<string, TokenType>> tokens;
    size_t current; // Track the current position in the token list



    // Get the current token
    pair<string, TokenType> peek() const {
        if (current < tokens.size()) {
            return tokens[current];
        }
        return {"", UNKNOWN}; // Return an UNKNOWN token at EOF
    }

    // Advance to the next token
    pair<string, TokenType> advance() {
        if (current < tokens.size()) {
            return tokens[current++];
        }
        return {"", UNKNOWN}; // Return an UNKNOWN token at EOF
    }

    // Match the current token with the expected type
    void match(TokenType expectedType) {
        if (peek().second == expectedType) {
            advance();
        } else {
            fprintf(stderr, "user used unexpected type");
        }
    }
    void program(){
        cout<<"program"<<endl;
        stmt_sequence();
    }
    void stmt_sequence(){
        cout<<"stmtseq"<<endl;
        statement();
        while(peek().second == SEMICOLON)
        {
            match(SEMICOLON);
            statement();
        }
    }
    void statement(){
        cout<<"stmt" <<endl;
        switch(peek().second){
            case IF : if_stmt(); break;
            case REPEAT : repeat_stmt(); break;
            case IDENTIFIER : assign_stmt(); break;
            case READ : read_stmt(); break;
            case WRITE : write_stmt(); break;
            default : throw runtime_error("Syntax Error: Invalid statement.");
        }
    }
    void if_stmt()
    {
        cout<<"ifstmt"<<endl;
        match(IF);
        exp();
        match(THEN);
        stmt_sequence();
        if(peek().second== ELSE) 
        {
            match(ELSE);
            stmt_sequence();
        }
        match(END);
    }

    void repeat_stmt()
    {
        cout<<"repeat"<<endl;
        match(REPEAT);
        stmt_sequence();
        match(UNTIL);
        exp();

    }

    void assign_stmt()
    {
        cout<<"assn"<<endl;
        match(IDENTIFIER);
        match(ASSIGN);
        exp();
    }

    void read_stmt()
    {
        cout<<"read"<<endl;
        match(READ);
        match(IDENTIFIER);
    }

    void write_stmt()
    {
        cout<<"write"<<endl;
        match(WRITE);
        exp();    
    }

    void exp()
    {
        cout<<"exp"<<endl;
        simple_exp();
        if(peek().second == LESSTHAN | peek().second == EQUAL){
            match(peek().second);
            simple_exp();
        }

    }

    void simple_exp()
    {
        cout<<"simpleexp\n";
        term();
        while(peek().second == PLUS | peek().second == MINUS){
            match(peek().second);
            term();
        }

    }

    void term()
    {
        cout<<"term\n";
        factor();
        while(peek().second == MULT | peek().second == DIV){
            match(peek().second);
            factor();
        }
    }

    void factor()
    {
        cout<<"factor\n";
        if(peek().second == OPENBRACKET){
            match(OPENBRACKET);
            exp();
            match(CLOSEBRACKET);
        }
        else if(peek().second == NUMBER) match(peek().second);
        else if(peek().second == IDENTIFIER) match(peek().second);
        else throw runtime_error("Syntax Error: Invalid statement.");

    }
public:
    // Constructor to initialize the parser with tokens
    Parser(const vector<pair<string, TokenType>>& tokens) : tokens(tokens), current(0) {}
     void parse() {
        program();
    }


};





void print_vector(vector <pair <string,TokenType>> vc){
    ofstream file("output.txt");
    for(int i =0 ; i<vc.size();i++){
        string strtypetoken;
        
        switch(vc[i].second){
            case 0:  strtypetoken = "SEMICOLON"; break;
            case 1:  strtypetoken = "IF"; break;
            case 2:  strtypetoken = "THEN"; break;
            case 3:  strtypetoken = "END"; break;
            case 4:  strtypetoken = "REPEAT"; break;
            case 5:  strtypetoken = "UNTIL"; break;
            case 6:  strtypetoken = "READ"; break;
            case 7:  strtypetoken = "WRITE"; break;
            case 8:  strtypetoken = "ASSIGN"; break;
            case 9:  strtypetoken = "LESSTHAN"; break;
            case 10: strtypetoken = "EQUAL"; break;
            case 11: strtypetoken = "PLUS"; break;
            case 12: strtypetoken = "MINUS"; break;
            case 13: strtypetoken = "MULT"; break;
            case 14: strtypetoken = "DIV"; break;
            case 15: strtypetoken = "OPENBRACKET"; break;
            case 16: strtypetoken = "CLOSEDBRACKET"; break;
            case 17: strtypetoken = "IDENTIFIER"; break;
            case 18: strtypetoken = "NUMBER"; break;
            case 19: strtypetoken = "UNKNOWN"; break;
        }
        
        

    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
        exit(1);
    }

   
    file<<vc[i].first<<" , "<<strtypetoken<<endl;
   
    }file.close();
}

int main(int argc , char* argv[]){
    
    if(argc!=2){
        fprintf(stderr,"Incorrect format ... Try : tiny_scanner <input.tn>\n");
        exit(1);
    }
    

    string codeSnippet = filetoss(argv[1]);
    
    vector<pair<string, TokenType>> tokens = tokenize(codeSnippet);
    for (const auto &token : tokens) {
    cout << "Token: " << token.first << ", Type: " << tokenTypeToString(token.second) << endl;
}

        Parser parser(tokens);
        cout<<"test2"<<endl;
        parser.parse();
        cout<<"test3"<<endl;
        cout << "Parsing succeeded!" << endl;
    
/*
TODO: 
handle unknown like ':'
comments 3aaaa3
clean code?! never heard of that!

handle else as token

*/
    return 0;
}
