package crafting;

import java.util.List;

public class LoxFunction implements LoxCallable {
    private final Stmt.Function decleration;
    private final Enviroment closure;
    private final boolean isInitializer;

    public LoxFunction(Stmt.Function decleration, Enviroment closure, boolean isInitializer) {
        this.decleration = decleration;
        this.closure = closure;
        this.isInitializer = isInitializer;
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
            if (isInitializer) {
                return closure.getAt(0, "this");
            }
            return returnValue.value;
        }
        // no return; init return
        if (isInitializer) {
            return closure.getAt(0, "this");
        }
        return null;
    }

    @Override
    public int arity() {
        return decleration.params.size();
    }

    LoxFunction bind(LoxInstance instance) {
        Enviroment enviroment = new Enviroment(closure);
        enviroment.define("this", instance);
        return new LoxFunction(decleration, enviroment, isInitializer);
    }

    @Override
    public String toString() {
        return "<fn" + decleration.name.lexeme + ">";
    }
}
