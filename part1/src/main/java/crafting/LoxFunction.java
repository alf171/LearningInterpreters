package crafting;

import java.util.List;

public class LoxFunction implements LoxCallable {
    private final Stmt.Function decleration;
    private final Enviroment closure;

    public LoxFunction(Stmt.Function decleration, Enviroment closure) {
        this.decleration = decleration;
        this.closure = closure;
    }

    @Override
    public Object call(Interpreter interpreter, List<Object> arguments) {
        Enviroment enviroment = new Enviroment(closure);
        for (int i = 0; i < decleration.params.size(); i++) {
            enviroment.define(decleration.params.get(i).lexeme, arguments.get(i));
        }

        try {
            interpreter.executeBlock(decleration.body, enviroment);
        } catch (Return returnValue) {
            return returnValue.value;
        }
        return null;
    }

    @Override
    public int arity() {
        return decleration.params.size();
    }

    @Override
    public String toString() {
        return "<fn" + decleration.name.lexeme + ">";
    }
}
