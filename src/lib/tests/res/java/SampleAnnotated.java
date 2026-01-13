// Golden sample: Contains Java language features and constructs

package tests; // +1 (package declaration)

import java.util.*; // +1 (import declaration)
import java.io.Serializable; // +1 (import declaration)
import static java.lang.Math.*; // +1 (import declaration)

@FunctionalInterface // +2 (interface declaration, marker annotation)
interface MyFunctional {
    int apply(int x, int y); // +1 (method declaration)
}

@SuppressWarnings({"unchecked", "deprecation"}) // +2 (class declaration, annotation)
public class Sample<T extends Number & Comparable<T>>
    extends AbstractList<T>
    implements Serializable {

    // Fields
    private static final long serialVersionUID = 1L; // +1 (field declaration)
    public int publicField; // +1 (field declaration)
    protected String protectedField = "protected"; // +1 (field declaration)
    private List<T> privateList = new ArrayList<>(); // +1 (field declaration)
    int packagePrivateField; // +1 (field declaration)

    // Enum
    public enum Color { // +1 (enum declaration)
        RED, // +1 (enum constant)
        GREEN, // +1 (enum constant)
        BLUE // +1 (enum constant)
    }

    // Annotation
    @interface MyAnnotation { // +1 (annotation type declaration)
        String value(); // +1 (annotation type element declaration)
        int number() default 42; // +1 (annotation type element declaration)
    }

    // Record
    public record Point(int x, int y) { // +1 (record declaration)
        // Empty
    }

    // Static initializer
    static { // +1 (static initializer)
        System.out.println("Static initializer"); // +1 (expression statement)
    }

    // Instance initializer
    {
        System.out.println("Instance initializer"); // +1 (expression statement)
    }

    // Constructor
    public Sample() { // +1 (constructor declaration)
        super(); // +1 (explicit constructor invocation)
        this.publicField = 0; // +1 (expression statement)
    }

    // Compact constructor for record
    public record CompactRecord(int x, int y) { // +1 (record declaration)
        public CompactRecord { // +1 (compact constructor declaration)
            if (x < 0 || y < 0) { // +1 (if statement)
                throw new IllegalArgumentException(); // +1 (throw statement)
            }
        }
    }

    // Method with generics, varargs, throws, lambda, inner class, and local class
    @MyAnnotation("test") // +2 (method declaration, annotation)
    public <E extends Exception> T exampleMethod(int a, String... args) throws E {
        MyFunctional lambda = (x, y) -> x + y; // +1 (local variable declaration)
        class LocalClass {} // +1 (class declaration)
        new Thread(() -> System.out.println("Lambda")).start(); // +1 (expression statement)
        if (a > 0) { // +1 (if statement)
            for (int i = 0; i < a; i++) { // +1 (for statement)
                try { // +1 (try statement)
                    switch (i) { // +1 (switch expression)
                        case 0 -> System.out.println("Zero"); // +1 (switch label)
                        case 1 -> System.out.print(""); // +1 (switch label)
                        default -> { // +1 (switch label)
                            continue; // +1 (continue statement)
                        }
                    }
                    switch (i) { // +1 (switch expression)
                        case 0: // +1 (switch label)
                            break; // +1 (break statement)
                        case 1: { // +1 (switch label)
                            System.out.println("One"); // +1 (expression statement)
                        }
                        default: // +1 (switch label)
                            continue; // +1 (continue statement)
                    }
                } catch (Exception ex) { // +1 (catch clause)
                    throw (E) ex; // +1 (throw statement)
                } finally { // +1 (finally clause)
                    System.out.println("Finally"); // +1 (expression statement)
                }
            }
        } else if (a == 0) { // +1 (else)
            while (a < 10) { // +1 (while statement)
                a++; // +1 (expression statement)
            }
        } else { // +1 (else)
            do { // +2 (do statement)
                a--; // +1 (expression statement)
            } while (a > -10);
        }
        assert a != 42 : "a should not be 42"; // +1 (assert statement)
        return null; // +1 (return statement)
    }

    // Overridden method
    @Override // +2 (method declaration, marker annotation)
    public T get(int index) {
        return privateList.get(index); // +1 (return statement)
    }

    @Override // +2 (method declaration, marker annotation)
    public int size() {
        return privateList.size(); // +1 (return statement)
    }

    // Static method
    public static void staticMethod() { // +1 (method declaration)
        System.out.println("Static method"); // +1 (expression statement)
    }

    // Synchronized method
    public synchronized void syncMethod() {} // +1 (method declaration)

    // Native method
    public native void nativeMethod(); // +1 (method declaration)

    // Inner class
    public class Inner { // +1 (class declaration)
        public void innerMethod() {} // +1 (method declaration)
    }

    // Static nested class
    public static class Nested { // +1 (class declaration)
        public static void nestedMethod() {} // +1 (method declaration)
    }

    // Anonymous class
    Runnable anon = new Runnable() { // +1 (field declaration)
        @Override // +2 (method declaration, marker annotation)
        public void run() {
            System.out.println("Anonymous class"); // +1 (expression statement)
        }
    };

    // Generic method
    public <U> U genericMethod(U param) { // +1 (method declaration)
        return param; // +1 (return statement)
    }

    // Varargs method
    public void varargsMethod(String... args) { // +1 (method declaration)
        // Empty definition
    }

    // Main method
    public static void main(String[] args) { // +1 (method declaration)
        Sample<Integer> sample = new Sample<>(); // +1 (local variable declaration)
        sample.exampleMethod(1, "a", "b"); // +1 (expression statement)
        sample.anon.run(); // +1 (expression statement)
        synchronized (sample) { // +1 (synchronized statement)
            Nested.nestedMethod(); // +1 (expression statement)
        }
        sample.new Inner().innerMethod(); // +1 (expression statement)
        Point p = new Point(1, 2); // +1 (local variable declaration)
        CompactRecord cr = new CompactRecord(3, 4); // +1 (local variable declaration)
        List<String> list = List.of("a", "b"); // +1 (local variable declaration)
        for (String s : list) { // +1 (enhanced for statement)
            System.out.println(s); // +1 (expression statement)
        }
        try (Scanner sc = new Scanner(System.in)) { // +1 (try with resources statement)
            if (sc.hasNext()) { // +1 (if statement)
                System.out.println(sc.next()); // +1 (expression statement)
            }
        }
        switch (p.x()) { // +1 (switch expression)
            case 1 -> System.out.println("x is 1"); // +1 (switch label)
            default -> System.out.println("x is not 1"); // +1 (switch label)
        }
    }
}
