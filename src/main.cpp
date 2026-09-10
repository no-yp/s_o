#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

enum class TokenType {
  // literal
  INT_LIT, FLOAT_LIT, STRING_LIT,
  IDENT, // identifier
  // keywords
  KW_INT, KW_FLOAT, KW_IF, KW_ELSE, KW_WHILE, KW_RETURN,
  // ooerator
  OP_PLUS, OP_MINUS, OP_STAR, OP_SLASH, OP_ASSIGN,
  OP_EQ, OP_NEQ, OP_LT, OP_GT, OP_LTE, OP_GTE,
  // punctuator
  P_LPAREN, P_RPAREN, P_LBRACE, P_RBRACE, P_SEMICOLON, P_COMMA,
  // error
  UNKNOWN, END_OF_FILE
};

struct Token {
  TokenType type; string value;
  int line, column;

  Token(TokenType t, string v, int ln, int col):
    type(t), value(std::move(v)), line(ln), column(col) {}
};

// class implementation of lexical analyzer
class LexicalAnalyzer {
  private:
    string input; size_t position; int line, column;
    static const unordered_map<string, TokenType> keywords;
    
    // helpet of type
    bool isWhitespace(char c) const {
      return c==' '||c=='\t'||c=='\n'||c=='\r';
    }
    bool isAlpha(char c) const { return
      (c>='a'&&c<='z') || (c>='A'&&c<='X') || c=='_';
    }
    bool isDigit(char c) const {return c>='0'&&c<='9';}
    bool isAlphaNumeric(char c) const {return isAlpha(c)||isDigit(c);}
    
    //position movement
    char advance() {
      char c = input[position++];
      if (c=='\n') { line++; column=1; } else column++;
      return c;
    }
    char peek(size_t offset = 0) const {
      size_t i = position + offset;
      return i < input.size() ? input[i] : '\0';
    }
    bool match(char expected) {
      if ( peek()==expected ){ advance(); return true; }
      return false;
    }
    
    // helper of tokenizeer
    Token makeWord() {
      int ln = line, col = column; size_t start = position;
      while (isAlphaNumeric( peek() )) advance();
      string word = input.substr(start, position - start);

      auto it = keywords.find(word);
      if (it != keywords.end()) return
        Token(it->second, word, ln, col);
      return Token(TokenType::IDENT, word, ln, col);
    }
    Token makeNumber() {
      int ln = line, col = column; size_t start = position;
      bool hasDot = false;

      while (isDigit(peek()) || peek() == '.') {
        if (peek() == '.') { if (hasDot) break; hasDot = true; }
        advance();
      }
      string num = input.substr(start, position - start);
      return Token(hasDot? TokenType::FLOAT_LIT : TokenType::INT_LIT,
          num, ln, col);
    }
    Token makeOperator() {
      int ln = line, col = column;
      char c = advance();

      switch (c) {
        case '+': return Token(TokenType::OP_PLUS,  "+", ln, col);
        case '-': return Token(TokenType::OP_MINUS, "-", ln, col);
        case '*': return Token(TokenType::OP_STAR,  "*", ln, col);
        case '/': return Token(TokenType::OP_SLASH, "/", ln, col);
        case '=': if (match('='))
          return Token(TokenType::OP_EQ,    "==", ln, col);
          return Token(TokenType::OP_ASSIGN, "=", ln, col);
        case '<': if (match('='))
          return Token(TokenType::OP_LTE,   "<=", ln, col);
          return Token(TokenType::OP_LT,     "<", ln, col);
        case '>': if (match('='))
          return Token(TokenType::OP_GTE,   ">=", ln, col);
          return Token(TokenType::OP_GT,     ">", ln, col);
        case '!': if (match('='))
          return Token(TokenType::OP_NEQ,   "!=", ln, col); break;
      }
      return Token(TokenType::UNKNOWN, string(1, c), ln, col);
    }
    Token makePunctuator() {
      int ln = line, col = column;
      char c = advance();

      switch (c) {
        case '(': return Token(TokenType::P_LPAREN,    "(", ln, col);
        case ')': return Token(TokenType::P_RPAREN,    ")", ln, col);
        case '{': return Token(TokenType::P_LBRACE,    "{", ln, col);
        case '}': return Token(TokenType::P_RBRACE,    "}", ln, col);
        case ';': return Token(TokenType::P_SEMICOLON, ";", ln, col);
        case ',': return Token(TokenType::P_COMMA,     ",", ln, col);
      }
      return Token(TokenType::UNKNOWN, string(1, c), ln, col);
    }

    // statement validator
    bool isOperator(char c) const { return
      c=='+'||c=='-'||c=='*'||c=='/'||c=='='||c=='!'||c=='<'||c=='>';
    }
    bool isPunctuator(char c) const { return
      c=='('||c==')'||c=='{'||c=='}'||c==';'||c==',';
    }

  public:
    LexicalAnalyzer(const string& source):
      input(source), position(0), line(1), column(1) {}

    vector<Token> tokenize() {
      vector<Token> tokens;

      while (position < input.size()) {
        char c = peek();
        if (isWhitespace(c) ) { advance(); continue; }

        if (isAlpha(c)      ) tokens.push_back(makeWord()); else
        if (isDigit(c)      ) tokens.push_back(makeNumber()); else
        if (isOperator(c)   ) tokens.push_back(makeOperator()); else
        if (isPunctuator(c) ) tokens.push_back(makePunctuator());
        else {
          int ln = line, col = column; advance();
          tokens.push_back(Token(TokenType::UNKNOWN, string(1, c), ln, col));
        }
      }
      tokens.push_back(Token(TokenType::END_OF_FILE, "", line, column));
			return tokens;
    }
};

const unordered_map<string, TokenType> LexicalAnalyzer::keywords = {
  { "int",     TokenType::KW_INT    },
  { "float",   TokenType::KW_FLOAT  },
  { "if",      TokenType::KW_IF     },
  { "else",    TokenType::KW_ELSE   },
  { "while",   TokenType::KW_WHILE   },
  { "return",  TokenType::KW_RETURN }
};

const char* tokenTypeName(TokenType t) {
  switch (t) {
    case TokenType::INT_LIT:     return "INT_LIT";
    case TokenType::FLOAT_LIT:   return "FLOAT_LIT";
    case TokenType::STRING_LIT:  return "STRING_LIT";
    case TokenType::IDENT:       return "IDENT";

    case TokenType::KW_INT:      return "KW_INT";
    case TokenType::KW_FLOAT:    return "KW_FLOAT";
    case TokenType::KW_IF:       return "KW_IF";
    case TokenType::KW_ELSE:     return "KW_ELSE";
    case TokenType::KW_WHILE:    return "KW_WHILE";
    case TokenType::KW_RETURN:   return "KW_RETURN";

    case TokenType::OP_PLUS:     return "OP_PLUS";
    case TokenType::OP_MINUS:    return "OP_MINUS";
    case TokenType::OP_STAR:     return "OP_STAR";
    case TokenType::OP_SLASH:    return "OP_SLASH";
    case TokenType::OP_ASSIGN:   return "OP_ASSIGN";
    case TokenType::OP_EQ:       return "OP_EQ";
    case TokenType::OP_NEQ:      return "OP_NEQ";
    case TokenType::OP_LT:       return "OP_LT";
    case TokenType::OP_GT:       return "OP_GT";
    case TokenType::OP_LTE:      return "OP_LTE";
    case TokenType::OP_GTE:      return "OP_GTE";

    case TokenType::P_LPAREN:    return "P_LPAREN";
    case TokenType::P_RPAREN:    return "P_RPAREN";
    case TokenType::P_LBRACE:    return "P_LBRACE";
    case TokenType::P_RBRACE:    return "P_RBRACE";
    case TokenType::P_SEMICOLON: return "P_SEMICOLON";
    case TokenType::P_COMMA:     return "P_COMMA";

    case TokenType::UNKNOWN:     return "UNKNOWN";
    case TokenType::END_OF_FILE: return "END_OF_FILE";
  }
  return "?";
}

void printTokens(const vector<Token>& tokens) {
  for (const auto& t : tokens) {
    cout << t.line << ":" << t.column << "  " << tokenTypeName(t.type);
    if (!t.value.empty()) cout << "  \"" << t.value << "\"";
    cout << '\n';
  }
}

int main() {
  string src =
    "\nint main() {\n\tconst float pi = 3.14;\n\tint y = 5;\n"
    "\tfloat z = x + y;\n\nwhile (z != 0) {\n\tif (z <= 0)"
    " break;\n\tz--;\n\n"
    "\tretrun 0\n}\n"
    "\n\nconstif,\n<>!whilereturn"
  ;

  LexicalAnalyzer lexer(src);
  auto tokens = lexer.tokenize();

  cout << "\nSource:\n" << src << "\n\nTokens:\n";
  printTokens(tokens);

  cout << endl;
  return 0;
}
