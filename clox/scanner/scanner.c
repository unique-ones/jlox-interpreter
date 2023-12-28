#include "scanner.h"

#include <ctype.h>
#include <locale.h>
#include <stdatomic.h>
#include <string.h>

#include "../common.h"

typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

Scanner scanner;

void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

bool isAtEnd(void) {
    return *scanner.current == '\0';
}

Token makeToken(TokenType tokenType) {
    Token token;
    token.type = tokenType;
    token.line = scanner.line;
    token.length = scanner.current - scanner.start;
    token.start = scanner.start;
    return token;
}

Token errorToken(char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int) strlen(message);
    token.line = scanner.line;
    return token;
}

char advance(void) {
    return *scanner.current++;
}

bool match(char expected) {
    if (isAtEnd()) return false;
    if (*scanner.current != expected) return false;
    scanner.current++;
    return true;
}

char peek(void) {
    return *scanner.current;
}

char peekNext(void) {
    if (isAtEnd()) return '\0';
    return scanner.current[1];
}

void skipWhitespaces(void) {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;

            case '\n':
                scanner.line++;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if (peekNext() == '*') {
                    int commment_count = 1;
                    while ((!(peek() == '*' && peekNext() == '/') || commment_count > 0) && !isAtEnd()) {
                        advance();
                        if ((peek() == '/' && peekNext() == '*')) commment_count++;
                        if ((peek() == '*' && peekNext() == '/')) commment_count--;
                    }

                    if (isAtEnd()) return;
                    advance();
                    advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}


Token string(void) {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') scanner.line++;
        advance();
    }
    if (isAtEnd()) return errorToken("Unterminated String");
    //if we are not at the end, we are current is pointing to closing quote
    advance();
    return makeToken(TOKEN_STRING);
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

Token Number() {
    while (isDigit(peek())) advance();
    if (peek() == '.') {
        advance();
        while (isDigit(peek()))advance();
    }
    return makeToken(TOKEN_NUMBER);
}

bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_');
}

TokenType checkKeyword(int start, int length, char* rest, TokenType token_type) {
    if (scanner.current - scanner.start == start + length && memcmp(scanner.start + start, rest, length) == 0) {
        return token_type;
    }

    return TOKEN_IDENTIFIER;
}

TokenType identifierType(void) {\
    //tp get star of keyword as current always points one ahead
    switch (scanner.start[0]) {
        case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
        case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
        case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
        case 'f': {
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
                    case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
                    case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
                }
            }
        }
        case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
        case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
        case 'o': return checkKeyword(1, 1, "r", TOKEN_PRINT);
        case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
        case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
        case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
        case 't':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
                    case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
                }
            }
        case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
        case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }
    return TOKEN_IDENTIFIER;
}

Token identifier() {
    while (isDigit(peek()) || isAlpha(peek())) advance();
    return makeToken(identifierType());
}

Token scanToken() {
    skipWhitespaces();
    scanner.start = scanner.current;
    if (isAtEnd()) return makeToken(TOKEN_EOF);
    char c = advance();
    if (isAlpha(c)) return identifier();
    if (isDigit(c)) return Number();
    switch (c) {
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RIGHT_BRACE);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case ',': return makeToken(TOKEN_COMMA);
        case '.': return makeToken(TOKEN_DOT);
        case '-': return makeToken(TOKEN_MINUS);
        case '+': return makeToken(TOKEN_PLUS);
        case '/': return makeToken(TOKEN_SLASH);
        case '*': return makeToken(TOKEN_STAR);
        case '!': return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '>': return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER_EQUAL);
        case '<': return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '=': return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '"': return string();
    }
    return errorToken("Unexpected character");
}
