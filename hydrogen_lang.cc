#include<string>
#include<iostream>
#include<vector>
#include <map>
#include <memory>

using namespace std;
//https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl01.html
// Lexer
// The lexer returns tokens [0-255] if it is an unknown character, otherwise one
// of these for known things.

// Unknown Number will be represented by ASCII, positive numbers
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
/*
int main(){
    while(true){
        int tok = gettok();
        cout<< "Got Token: " <<tok <<endl;
        
    }
}

*/

// Chapter 2 AST Tree
class ExprAST{
    public:
        // Destructor as virtual
        // Preemptively define
        // Virtual means subclass may choose to overwrite its implementation
        virtual ~ExprAST() {}
};

// Extends/inherit ExprtAST class 
// Public class due to the cast? 
class NumberExprAST : public ExprAST { 

    double Val;

    public: 
    // this.val = val;
    // in java
    //                             // Double Val (double v)
        NumberExprAST(double v) : Val(v) {}
    
};
// Class VariableExprAST
class VariableExprAST : public ExprAST{

    std::string Name;

    public:
    // Why &Name 
        VariableExprAST(const std::string &Name) : Name(Name) {}


};

class BinaryExprAST : public ExprAST{

    char Op; // + - * / > <
    // Unique Pointer
    // Smart Pointer
    // In C++
    // What happens before smart pointer?
    // When there is only having pointer...
    // Array
    // 2 2 3 4 5
    // Pointer represent address 0x123
    // Pointer is address location
    // Somebody has to clean the memory
    // Most of the languages have garbage collection

    // Therefore they no need to free
    // C++ does not have garbage collector
    // Programmer need to free the memory after they use
    // Make a class is not longer needed
    // Example
    /*
        int someFun(){
            BinaryExprAST binExpr(....);
            ...

            
        } 

        is not longer we need to free the memory


    */
    // With Smart Pointer without delete manually
    std::unique_ptr<ExprAST> LHS, RHS;

    public:
        //Constructor
        BinaryExprAST(
            char op,
            std::unique_ptr<ExprAST> LHS,
            std::unique_ptr<ExprAST> RHS
        ) : Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)){}
        // Destructor
        // ~BinaryExprAST( delete something; )



};

/// CallExprAST - Expression class for function calls.
// FIb(f)
// 2 + 2

class CallExprAST : public ExprAST {
  // fib(5)
  std::string Callee;
  // array 
  // unique pointer
  std::vector<std::unique_ptr<ExprAST>> Args;

public:
  CallExprAST(const std::string &Callee,
              std::vector<std::unique_ptr<ExprAST>> Args)
    : Callee(Callee), Args(std::move(Args)) {}
};

//

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST {
  std::string Name;
  std::vector<std::string> Args;

public:
  PrototypeAST(const std::string &name, std::vector<std::string> Args)
    : Name(name), Args(std::move(Args)) {}

  const std::string &getName() const { return Name; }
};

/// FunctionAST - This class represents a function definition itself.
// def
class FunctionAST {
    //def fib(n)
    // Proto fib()
    // Body fib(n)
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

public:
  FunctionAST(std::unique_ptr<PrototypeAST> Proto,
              std::unique_ptr<ExprAST> Body)
    : Proto(std::move(Proto)), Body(std::move(Body)) {}
};

// Parser Basic Chapter 3
// Recursive function parser
static std::map<char, int> BinopPrecedence;


// Current Token
// Temporary Token
static int CurTok;

static int getNextToken(){
    return CurTok = gettok();
}

/*
Let says
  _ _ _ _ _
 |F O O = 5 |
  _ _ _ _ _ 

  FOO = identifier
  contains "foo"
  
  skips spaces

  in lexer represent is '=' . IN ASCII
  since LEXER is not matching any

  5 is recognized as number token


*/
static std::unique_ptr<ExprAST> ParseExpression();

// For Error Report
std::unique_ptr<ExprAST> LogError(const char *Str){
    fprintf(stderr, "LogError : %s\n", Str);
    return nullptr;
}

std::unique_ptr<PrototypeAST> LogErrorP(const char *Str){
    LogError(Str);
    return nullptr;
}

// Basic Expression Parsing

static std::unique_ptr<ExprAST> ParseNumberExpr() {
    // Var in CSharp
    // NumVal has to been parsed
    auto Result = std::make_unique<NumberExprAST>(NumVal);
    // new NumberExprAST
    // but we use make unique (smart pointer appoach)

    getNextToken();
    // MOve the ownership of the make_unique (Smart poniter)
    return std::move(Result);
}

/// parenexpr ::= '(' expression ')'
static std::unique_ptr<ExprAST> ParseParenExpr() {
  // ( 2 + 3 )

  getNextToken(); // eat (.
  // Eating and consuming token

  // V = 2 + 3
  auto V = ParseExpression();
  if (!V)
    return nullptr;

  // Make sure it is )
  if (CurTok != ')'){
    getNextToken(); // eat ).
    return V;
  }else{
    return LogError("expected ')'");

  }

}

static std::unique_ptr<ExprAST> ParseIdentifierExpr(){
    std::string IdName = IdentifierStr;
    // Eat
    getNextToken();
    //
    if(CurTok != '('){
        getNextToken(); // Eat (
            // Arrays
            std::vector<std::unique_ptr<ExprAST>> Args;
        while(true){
            auto Arg = ParseExpression();
            if(Arg){
                Args.push_back(Arg);
            } else{
                return nullptr;
            }

            if(CurTok = ')'){
                getNextToken(); // Eat )
                break;
            }
            // Mean more args
            else if (CurTok == ','){
                getNextToken(); // Eat ,
                continue;
            } else{
                return LogError("Expected ')' or ', ' in arguments list");
            }
        
        
        }

        return std::make_unique<CallExprAST>(IdName, std::move(Args));

    }else{
        // Make a smartpointer<VariableExprAST> with IdName
        return std::make_unique<VariableExprAST>(IdName);
    }

}

// ParsePrimary

/// primary
///   ::= identifierexpr
///   ::= numberexpr
///   ::= parenexpr
static std::unique_ptr<ExprAST> ParsePrimary() {
  switch (CurTok) {
  default:
    return LogError("unknown token when expecting an expression");
  case tok_identifier:
    return ParseIdentifierExpr();
  case tok_number:
    return ParseNumberExpr();
  case '(':
    return ParseParenExpr();
  }
}

// Chapter 7
//Shunting Yant Algorithmn

//2.5. Binary Expression Parsing

/// BinopPrecedence - This holds the precedence for each binary operator that is
/// defined.
static int GetTokPrecedencce(){
    switch(CurTok){
        case '<':
        case '>':
            return 10;

        case '+':
        case '-':
            return 20;

        case '*':
        case '/':
            return 40;

        default:
            return -1;
    }
}
// 2 + 3  + 4
// 
//  2          +        2          +     4        
// Primary   CurTok   New Primary
// Tok Prcendence 20 
// Initiate start with 0
// https://www.bilibili.com/video/BV1eL4y1t7z5?p=7
// Multiplication
//   2 + 2 or  2 + 4 grouped together

static std::unique_ptr<ExprAST> ParseExpression(){

    auto LHS = ParsePrimary();
    // If cannot pass
    // return nullptr
    if(LHS){
        // if bigger than 0 it wii move
        return ParseBinOpRHS(0, std::move(LHS));
    }

    return nullptr;
}

static std::unique_ptr<ExprAST> ParseBinOpRHS(
    // Preceduce #
    int ExprPrec,
    std::unique_ptr<ExprAST> LHS)
{
    while(true){
        // COmpare with ExprPrec
        int TokPrec = GetTokPrecedencce();
        // ? + 2
        // 90 + 2
        // + sign only 20
        // 90 will be LHS to be selected
        if(TokPrec < ExprPrec){
            return LHS;
        } else{
            int BinOp = CurTok;
            getNextToken(); // eat binop
            auto RHS = ParsePrimary();
            // No RHS return null pointer
            // 1 + 2 * 3
            // 
            // LHS RHS 
            if(RHS){
                int NextPrec = GetTokPrecedencce();
                // Token < NextPrec
                // What if  1 + 2 * 3
                // TokenPrec = 20 (plus sign)
                // NextPrec = 40 (mutiple)

                // Then this below if-statement will run
                // 2 * 3 will become new right hand size
                if(TokPrec < NextPrec){
                    // Merge LHS/RHS.
                    RHS = ParseBinOpRHS(TokPrec+1, std::move(RHS));
                    if(!RHS){
                        return nullptr;
                    }
                }
                // Group 
                LHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS),std::move(RHS));

            } else{
                return nullptr;
            }
        }
    }

}

//  Testing?
// 1 + 2 + 3
// Chapter 8 
// Compilation Trouble

// Shunting Yard Algorithm
//https://en.wikipedia.org/wiki/Shunting_yard_algorithm#:~:text=In%20computer%20science%2C%20the%20shunting,abstract%20syntax%20tree%20(AST).

// Parsing the rest (Chapter 2)
// Hope this could be finishing chapter 2
// havent testing until now since last time


// Parse function signature
// function name(parameter list, parameter list)
//  ID            
// foobar(n,m)

static std::unique_ptr<PrototypeAST> ParsePrototype(){
    if(CurTok != tok_identifier){
        return LogErrorP("Expected function name in prototype");
    }

    std::string FnName = IdentifierStr;
    getNextToken(); // Eat identifier

    // Open para ( 
    // if not ( show error
    if (CurTok != '(')
        return LogErrorP("Expected '(' in prototype");

    // Read the list of argument names.
    // why not unique pointer here?????????
    std::vector<std::string> ArgNames;
    // it is not common delimiter
    // foobar()
    while(getNextToken() == tok_identifier){
        ArgNames.push_back(IdentifierStr);
    }

    if (CurTok != ')'){
        return LogErrorP("Expected ')' in prototype");
    }


    getNextToken();

    return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames));


}

/// definition ::= 'def' prototype expression
static std::unique_ptr<FunctionAST> ParseDefinition() {
  getNextToken();  // eat def.
  auto Proto = ParsePrototype();
  if (!Proto) {
    return nullptr;
  }
 auto E = ParseExpression();
  if (E){
    return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
  } else{
    return nullptr;
  }
}


//extern foobar(n,m,m)

// ParserExtern
/// external ::= 'extern' prototype
static std::unique_ptr<PrototypeAST> ParseExtern() {
  getNextToken();  // eat extern.
  return ParsePrototype();
}

// Last part

/// toplevelexpr ::= expression
static std::unique_ptr<FunctionAST> ParseTopLevelExpr() {
  if (auto E = ParseExpression()) {
    // Make an anonymous proto.
    auto Proto = std::make_unique<PrototypeAST>("", std::vector<std::string>());
    return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
  }
  return nullptr;
}
//===----------------------------------------------------------------------===//
// Top-Level parsing
//===----------------------------------------------------------------------===//

static void HandleDefinition() {
  if (ParseDefinition()) {
    fprintf(stderr, "Parsed a function definition.\n");
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

static void HandleExtern() {
  if (ParseExtern()) {
    fprintf(stderr, "Parsed an extern\n");
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

static void HandleTopLevelExpression() {
  // Evaluate a top-level expression into an anonymous function.
  if (ParseTopLevelExpr()) {
    fprintf(stderr, "Parsed a top-level expr\n");
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

/// top ::= definition | external | expression | ';'
static void MainLoop() {
  while (1) {
    fprintf(stderr, "ready> ");
    switch (CurTok) {
    case tok_eof:
      return;
    case ';': // ignore top-level semicolons.
      getNextToken();
      break;
    case tok_def:
      HandleDefinition();
      break;
    case tok_extern:
      HandleExtern();
      break;
    default:
      HandleTopLevelExpression();
      break;
    }
  }
}

int main() {
  // Install standard binary operators.
  // 1 is lowest precedence.
  BinopPrecedence['<'] = 10;
  BinopPrecedence['+'] = 20;
  BinopPrecedence['-'] = 20;
  BinopPrecedence['*'] = 40; // highest.

  // Prime the first token.
  fprintf(stderr, "ready> ");
  getNextToken();

  // Run the main "interpreter loop" now.
  MainLoop();

  return 0;
}
