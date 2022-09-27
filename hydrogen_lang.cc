#include<string>
#include<iostream>

using namespace std;
//https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl01.html
// Lexer
// The lexer returns tokens [0-255] if it is an unknown character, otherwise one
// of these for known things.

enum Token{
  tok_eof = -1,

  // commands
  tok_def = -2,
  tok_extern = -3,

  // primary
  tok_identifier = -4,
  tok_number = -5,

};
// Globbal Variable
static std::string IdentifierStr; // Filled in if tok_identifier
static double NumVal;             // Filled in if tok_number

static int gettok(){
    static int LastChar = ' ';

    // isspace is checking the space
    // Skip spaces
    while(isspace(LastChar)){
        // Read one character
        LastChar = getchar();
        // read until no space left
    }

    // C & JavaScript Standard Identifier (Common) 
    // name name1 name2

    // Check Alphabet
    // Get identifier
    if(isalpha(LastChar)){
        // Convert Char into String 
        // IdentifierStr is declared as string above
        // C++ & JavaScript could handle this scenario
        IdentifierStr = LastChar;
        // Check next 
        while(isalnum(LastChar = getchar())){
            IdentifierStr += LastChar;
        }
        // Check the keyword Def
        if(IdentifierStr == "def"){
            // Return is token def
            return tok_def;
        }

        if(IdentifierStr == "extern"){
            return tok_extern;
        }

        return tok_identifier;

    }

    // Match Number
    if(isdigit(LastChar) || LastChar == '.' ){
        std::string NumStr;
        do{
            NumStr += LastChar;
            LastChar = getchar();
        } while(isdigit(LastChar) || LastChar == '.');

        NumVal = strtod(NumStr.c_str(),0);
        return tok_number;
    }
    // Some problem here when there is . number . number
    // Unreal practical to real world

    // Now we need cope with comment 
    if(LastChar == '#'){
        do{
            LastChar = getchar();
        }while(LastChar != EOF && LastChar != '\n');

        if(LastChar != EOF){
            // Recursive Call itself
            // We skip the comment not as token 
            // That is the reason why we call itself
            return gettok();
        }
    
    }

    if(LastChar == EOF){
        return tok_eof;
    }

    int ThisChar = LastChar;
    LastChar = getchar();
    return ThisChar;

    // End of Chapter 1 Lexer

}

int main(){
    while(true){
        int tok = gettok();
        cout<< "Got Token: " <<tok <<endl;
        
    }
}