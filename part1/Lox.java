import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;

public class Lox {

  static boolean hadError = false;

  public static void main(String[] args) throws IOException {
    if (args.length > 1) {
      System.out.print("Usage: jlox [script]");
    } else if (args.length == 1) {
      runFile(args[0]);
    } else {
      runPrompt();
    }
  }

  private static void runFile(String fileName) throws IOException {
    byte[] bytes = Files.readAllBytes(Paths.get(fileName));
    run(new String(bytes, Charset.defaultCharset()));

    if (hadError)
      System.exit(65);
  }

  private static void runPrompt() throws IOException {
    InputStreamReader input = new InputStreamReader(System.in);
    BufferedReader reader = new BufferedReader(input);

    for (;;) {
      System.out.print("> ");
      String line = reader.readLine();
      System.out.println(line);
      if (line == null || line.equals("exit")) {
        break;
      }
      run(line);
      hadError = false;
    }
  }

  private static void run(String source) {
    // Scanner scanner = new Scanner(source);
    // List<Token> tokens = scanner.scanTokens();

    // print for now
    // for (Token token in tokens) {
    // System.out.println(token);
    // }

  }

  /**
   * note that hadError gets set a side effect
   */
  static void error(int line, String message) {
    report(line, "", message);
  }

  private static void report(int line, String where, String message) {
    System.err.println("[line" + line + "] Error" + where + ": " + message);
    hadError = true;
  }
}
