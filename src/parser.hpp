#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include "ast.hpp"


enum class TokenType {
  // literal
  INT_LIT, FLOAT_LIT, STRING_LIT,
  IDENT, // identifier
  // keywords
  KW_LET, KW_INT, KW_FLOAT, KW_IF, KW_ELSE, KW_WHILE, KW_RETURN,
  // ooerator
  OP_PLUS, OP_MINUS, OP_STAR, OP_SLASH, OP_ASSIGN,
  OP_EQ, OP_NEQ, OP_LT, OP_GT, OP_LTE, OP_GTE,
  // punctuator
  P_LPAREN, P_RPAREN, P_LBRACE, P_RBRACE, P_SEMICOLON, P_COMMA,
  // error
  UNKNOWN, END_OF_FILE
};

struct Token {
  TokenType type; std::string value;
  int line, column;

  Token(TokenType t, std::string v, int ln, int col):
    type(t), value(std::move(v)), line(ln), column(col) {}
};


class Parser {
private:
  const std::vector<Token>& tokens; size_t pos;

  const Token& peek() const { return tokens[pos]; }
  const Token& previous() const { return tokens[pos - 1]; }
  bool isAtEnd() const { return peek().type == TokenType::END_OF_FILE; }
  const Token& advance() { if (!isAtEnd()) pos++; return previous(); }
  bool check(TokenType t) const {
    if (isAtEnd()) return false; return peek().type == t;
  }
  bool match(TokenType t) { if (check(t)) {advance(); return true;} return false;}
  
  [[noreturn]] void error(const std::string& msg) const {
    const Token& t = peek();
    throw std::runtime_error(
      "Parse error at " + std::to_string(t.line) + ":"
      + std::to_string(t.column) + " — " + msg
      + " (found: \""+t.value+"\")"
    );
  }

  // --- grammar
  ExprPtr parseExpression() {
    ExprPtr left = parseTerm();

    while (match(TokenType::OP_PLUS) || match(TokenType::OP_MINUS)) {
      std::string op = previous().value;
      ExprPtr right = parseTerm();
      left = std::make_unique<BinaryOperation>(op, std::move(left), std::move(right));
    }
    return left;
  }

  ExprPtr parseTerm(){
    ExprPtr left = parseFactor();

    while (match(TokenType::OP_STAR) || match(TokenType::OP_SLASH)) {
      std::string op = previous().value; ExprPtr right = parseFactor();
      left = std::make_unique<BinaryOperation>(op, std::move(left), std::move(right));
    }
    return left;
  }

  ExprPtr parseFactor() {
    if (match(TokenType::INT_LIT))
    {return std::make_unique<NumberLiteral>(previous().value, false);}
    if (match(TokenType::FLOAT_LIT))
    {return std::make_unique<NumberLiteral>(previous().value, false);}
    if (match(TokenType::IDENT))
    { return std::make_unique<Identifier>(previous().value); }

    if (match(TokenType::P_LPAREN)) {
      ExprPtr inner = parseExpression();
      if (!match(TokenType::P_RPAREN)) error("expected ')'");
      return inner;
    }
    error("expected number, identifier or '('");
  }

  StmtPtr parsrStatement() {
    if (match(TokenType::KW_LET)) {
      if (!match(TokenType::IDENT)) {
        error("expected variable name after 'let'");
      }
      std::string name = previous().value;

      if (!match(TokenType::OP_ASSIGN)) {
        error("expected '=' after variable name");
      }

      ExprPtr init = parseExpression();
      return std::make_unique<VariableDeclaration>(std::move(name), std::move(init));
    }
    ExprPtr expr = parseExpression();
    return std::make_unique<ExpressionStatement>(std::move(expr));
  }

public:
  Parser(const std::vector<Token>& toks): tokens(toks), pos(0) {}

  std::vector<StmtPtr> parseProgram() {
    std::vector<StmtPtr> program;
    while (!isAtEnd())
    { program.push_back(parsrStatement()); }
    return program;
  }
};
