import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Scanner {
    private final String source;
    private final List<Token> tokens = new ArrayList<>();
    private static final Map<String, TokenType> Keywords;
    static {
        Keywords = new HashMap<>();
        Keywords.put("and", TokenType.AND);
        Keywords.put("class", TokenType.CLASS);
        Keywords.put("else", TokenType.ELSE);
        Keywords.put("false", TokenType.FALSE);
        Keywords.put("for", TokenType.FOR);
        Keywords.put("fun", TokenType.FUN);
        Keywords.put("if", TokenType.IF);
        Keywords.put("nil", TokenType.NIL);
        Keywords.put("or", TokenType.OR);
        Keywords.put("print", TokenType.PRINT);
        Keywords.put("return", TokenType.RETURN);
        Keywords.put("super", TokenType.SUPER);
        Keywords.put("this", TokenType.THIS);
        Keywords.put("true", TokenType.TRUE);
        Keywords.put("var", TokenType.VAR);
        Keywords.put("while", TokenType.WHILE);

    }

    private int start = 0;
    private int current = 0;
    private int line = 1;

    Scanner(String source) {
        this.source = source;
    }

    List<Token> scanTokens() {
        while (!isAtEnd()) {
            start = current;
            scanToken();
        }
        tokens.add(new Token(TokenType.EOF, "", null, line));
        return tokens;
    }

    private boolean isAtEnd() {
        return current >= source.length();
    }

    private void scanToken() {
        char c = advance();
        switch (c) {
            case '(':
                addToken(TokenType.LEFT_PAREN);
                break;
            case ')':
                addToken(TokenType.RIGHT_PAREN);
                break;
            case '{':
                addToken(TokenType.LEFT_BRACE);
                break;
            case '}':
                addToken(TokenType.RIGHT_BRACE);
                break;
            case ',':
                addToken(TokenType.COMMA);
                break;
            case '.':
                addToken(TokenType.DOT);
                break;
            case '-':
                addToken(TokenType.MINUS);
                break;
            case '+':
                addToken(TokenType.PLUS);
                break;
            case ';':
                addToken(TokenType.SEMICOLON);
                break;
            case '*':
                addToken(TokenType.STAR);
                break;
            case '!':
                addToken(match('=') ? TokenType.BANG_EQUAL : TokenType.BANG);
                break;
            case '=':
                addToken(match('=') ? TokenType.EQUAL_EQUAL : TokenType.EQUAL);
                break;
            case '<':
                addToken(match('=') ? TokenType.LESS_EQUAL : TokenType.LESS);
                break;
            case '>':
                addToken(match('=') ? TokenType.GREATER_EQUAL : TokenType.GREATER);
                break;
            case '/':
                if (match('/')) {
                    while (peek() != '\n' && !isAtEnd())
                        advance();
                } else if (match('*')) {
                    comment();
                } else {
                    addToken(TokenType.SLASH);
                }
                break;
            case ' ':
            case '\r':
            case '\t':
                break;
            case '\n':
                line++;
                break;
            case '"':
                string();
                break;
            default:
                if (isDigit(c)) {
                    number();
                } else if (isAlpha(c)) {
                    identifier();
                } else {
                    Lox.error(line, "Unexpected character.");
                }
                break;

        }
    }

    private boolean isAlpha(char c) {
        return (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') || c == '_';
    }

    private boolean isAlphaNumric(char c) {
        return isAlpha(c) || isDigit(c);
    }

    private void identifier() {
        while (isAlphaNumric(peek()))
            advance();
        String text = source.substring(start, current);
        TokenType type = Keywords.get(text);
        if (type == null)
            type = TokenType.IDENTIFIER;
        addToken(type);
    }

    private char peekNext() {
        if (current + 1 >= source.length())
            return '\0';
        return source.charAt(current + 1);
    }

    private void number() {
        while (isDigit(peek()))
            advance();
        if (peek() == '.' && isDigit(peekNext())) {
            advance();
            while (isDigit(peek()))
                advance();
        }
        addToken(TokenType.NUMBER, Double.parseDouble(source.substring(start, current)));
    }

    private boolean isDigit(char c) {
        return c >= '0' && c <= '9';
    }
    private void comment(){
        while(peek() !='*' && peekNext() != '/' && !isAtEnd()){
            if(peek() == '\n')
                line++;
            advance();
        }
        if(isAtEnd()){
            Lox.error(line, "Unterminated comment.");
            return;
        }
        advance();
        advance();
    }
    private void string() {
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n')
                line++;
            advance();
        }
        if (isAtEnd()) {
            Lox.error(line, "Unterminated string.");
            return;
        }
        // skip the "
        advance();
        String value = source.substring(start + 1, current - 1);
        addToken(TokenType.STRING, value);

    }

    private char peek() {
        if (isAtEnd())
            return '\0';
        return source.charAt(current);
    }

    private char advance() {
        return source.charAt(current++);
    }

    private void addToken(TokenType type) {
        addToken(type, null);
    }

    private void addToken(TokenType type, Object literal) {
        String text = source.substring(start, current);
        tokens.add(new Token(type, text, literal, line));
    }

    private boolean match(char expected) {
        if (isAtEnd())
            return false;
        if (source.charAt(current) != expected)
            return false;
        current++;
        return true;
    }
}
