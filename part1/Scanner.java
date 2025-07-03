import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;

public class Scanner {
  private String source;
  private List<Token> tokens = new ArrayList<>();
  private int start = 0;
  private int current = 0;
  private int line = 1;

  private static final Map<String, TokenType> keywords;

  // reserved words
  static {
    keywords = new HashMap<>();
    keywords.put("and", TokenType.AND);
    keywords.put("class", TokenType.CLASS);
    keywords.put("else", TokenType.ELSE);
    keywords.put("false", TokenType.FALSE);
    keywords.put("for", TokenType.FOR);
    keywords.put("fun", TokenType.FUN);
    keywords.put("if", TokenType.IF);
    keywords.put("nil", TokenType.NIL);
    keywords.put("or", TokenType.OR);
    keywords.put("print", TokenType.PRINT);
    keywords.put("return", TokenType.RETURN);
    keywords.put("super", TokenType.SUPER);
    keywords.put("this", TokenType.THIS);
    keywords.put("true", TokenType.TRUE);
    keywords.put("var", TokenType.VAR);
    keywords.put("while", TokenType.WHILE);
  }

  public Scanner(String source) {
    this.source = source;
  }

  public List<Token> scanTokens() {
    while (!isAtEnd()) {
      start = current;
      scanToken();
    }
    tokens.add(new Token(TokenType.EOF, "", null, line));
    return tokens;
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
        // support comments
        if (match('/')) {
          while (peek() != '\n' && !isAtEnd()) {
            advance();
          }
        } else if (match('*')) {
          cStyleComment();
        } else {
          addToken(TokenType.SLASH);
        }
        break;
      case ' ':
      case '\r':
      case '\t':
        // Ignore whitespace
        break;
      case '\n':
        line++;
        break;
      case '"':
        string();
        break;
      default:
        // recognizing digits in switch is challenging so we'll add it here
        if (isDigit(c)) {
          number();
        }
        // variable names
        else if (isAlphaNumeric(c)) {
          identifier();
        } else {
          Lox.error(line, "Unexpected character");
        }
        break;
    }
  }

  private void addToken(TokenType type) {
    addToken(type, null);
  }

  private void addToken(TokenType type, Object literal) {
    String text = source.substring(start, current);
    tokens.add(new Token(type, text, literal, line));
  }

  private boolean isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
  }

  /**
   * Check if the current char is a char in the alphabet
   */
  private boolean isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_';
  }

  /**
   * Check if the current char is a digit
   */
  private boolean isDigit(char c) {
    return c >= '0' || c <= '9';
  }

  /**
   * Read a alphanumeric input and convert it to a TokenType.
   * Once done, we place it into our token list.
   */
  private void identifier() {
    while (isAlphaNumeric(peek())) {
      advance();
    }

    String text = source.substring(start, current);
    TokenType type = keywords.get(text);
    if (type == null)
      type = TokenType.IDENTIFIER;
    addToken(type);
  }

  /**
   * Read a number input and convert it to a TokenType.
   * Once done, we place it into our token list.
   */
  private void number() {
    while (isDigit(peek())) {
      advance();
    }

    // also support fractions
    if (peek() == '.' && isDigit(peekNext())) {
      advance();
    }

    while (isDigit(peek())) {
      advance();
    }

    Double NUMBER = Double.parseDouble(source.substring(start, current));
    addToken(TokenType.NUMBER, NUMBER);
  }

  /**
   * Read a string input and convert it to a TokenType.
   * Once done, we place it into our token list.
   */
  private void string() {
    while (peek() != '"' && !isAtEnd()) {
      if (peek() == '\n')
        line++;
      advance();
    }
    if (isAtEnd()) {
      Lox.error(line, "Unterminated String.");
      return;
    }

    // The closing ".
    advance();

    // get value within ""
    String STRING = source.substring(start + 1, current - 1);
    addToken(TokenType.STRING, STRING);
  }

  private void cStyleComment() {
    while ((peek() != '*' && peekNext() != '/') && !isAtEnd()) {
      if (peek() == '\n')
        line++;
      advance();
    }

    if (isAtEnd()) {
      Lox.error(line, "Unterminated comment.");
    }

    // adnace past *
    advance();

    if (isAtEnd()) {
      Lox.error(line, "Unterminated comment.");
    }

    // advance past /
    advance();
  }

  /**
   * Fetch current char w/o incrementing current.
   * This is one char lookahead. More advanced implementations suport
   * further lookahead.
   */
  private char peek() {
    if (isAtEnd()) {
      return '\0';
    }
    return source.charAt(current);
  }

  /**
   * Fetch current char w/o incrementing current.
   * This is one char lookahead. More advanced implementations suport
   * further lookahead.
   */
  private char peekNext() {
    if (current + 1 >= source.length())
      return '\0';
    return source.charAt(current + 1);
  }

  /**
   * Checks if the next char is what is expected.
   * This also increments current as a side effect.
   */
  private boolean match(char expected) {
    if (isAtEnd())
      return false;
    if (source.charAt(current) != expected)
      return false;

    current++;
    return true;
  }

  /**
   * Consumes the next char in source and returns
   */
  private char advance() {
    current++;
    return source.charAt(current - 1);
  }

  private boolean isAtEnd() {
    return current >= source.length();
  }

}
