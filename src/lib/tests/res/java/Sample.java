// Golden sample: Contains Java language features and constructs

package tests;

import java.util.*;
import java.io.Serializable;
import static java.lang.Math.*;

@FunctionalInterface
interface MyFunctional {
    int apply(int x, int y);
}

@SuppressWarnings({"unchecked", "deprecation"})
public class Sample<T extends Number & Comparable<T>>
    extends AbstractList<T>
    implements Serializable {

    // Fields
    private static final long serialVersionUID = 1L;
    public int publicField;
    protected String protectedField = "protected";
    private List<T> privateList = new ArrayList<>();
    int packagePrivateField;

    // Enum
    public enum Color {
        RED,
        GREEN,
        BLUE
    }

    // Annotation
    @interface MyAnnotation {
        String value();
        int number() default 42;
    }

    // Record
    public record Point(int x, int y) {
        // Empty
    }

    // Static initializer
    static {
        System.out.println("Static initializer");
    }

    // Instance initializer
    {
        System.out.println("Instance initializer");
    }

    // Constructor
    public Sample() {
        super();
        this.publicField = 0;
    }

    // Compact constructor for record
    public record CompactRecord(int x, int y) {
        public CompactRecord {
            if (x < 0 || y < 0) {
                throw new IllegalArgumentException();
            }
        }
    }

    // Method with generics, varargs, throws, lambda, inner class, and local class
    @MyAnnotation("test")
    public <E extends Exception> T exampleMethod(int a, String... args) throws E {
        MyFunctional lambda = (x, y) -> x + y;
        class LocalClass {}
        new Thread(() -> System.out.println("Lambda")).start();
        if (a > 0) {
            for (int i = 0; i < a; i++) {
                try {
                    switch (i) {
                        case 0 -> System.out.println("Zero");
                        case 1 -> System.out.print("");
                        default -> {
                            continue;
                        }
                    }
                    switch (i) {
                        case 0:
                            break;
                        case 1: {
                            System.out.println("One");
                        }
                        default:
                            continue;
                    }
                } catch (Exception ex) {
                    throw (E) ex;
                } finally {
                    System.out.println("Finally");
                }
            }
        } else if (a == 0) {
            while (a < 10) {
                a++;
            }
        } else {
            do {
                a--;
            } while (a > -10);
        }
        assert a != 42 : "a should not be 42";
        return null;
    }

    // Overridden method
    @Override
    public T get(int index) {
        return privateList.get(index);
    }

    @Override
    public int size() {
        return privateList.size();
    }

    // Static method
    public static void staticMethod() {
        System.out.println("Static method");
    }

    // Synchronized method
    public synchronized void syncMethod() {}

    // Native method
    public native void nativeMethod();

    // Inner class
    public class Inner {
        public void innerMethod() {}
    }

    // Static nested class
    public static class Nested {
        public static void nestedMethod() {}
    }

    // Anonymous class
    Runnable anon = new Runnable() {
        @Override
        public void run() {
            System.out.println("Anonymous class");
        }
    };

    // Generic method
    public <U> U genericMethod(U param) {
        return param;
    }

    // Varargs method
    public void varargsMethod(String... args) {
        // Empty definition
    }

    // Main method
    public static void main(String[] args) {
        Sample<Integer> sample = new Sample<>();
        sample.exampleMethod(1, "a", "b");
        sample.anon.run();
        synchronized (sample) {
            Nested.nestedMethod();
        }
        sample.new Inner().innerMethod();
        Point p = new Point(1, 2);
        CompactRecord cr = new CompactRecord(3, 4);
        List<String> list = List.of("a", "b");
        for (String s : list) {
            System.out.println(s);
        }
        try (Scanner sc = new Scanner(System.in)) {
            if (sc.hasNext()) {
                System.out.println(sc.next());
            }
        }
        switch (p.x()) {
            case 1 -> System.out.println("x is 1");
            default -> System.out.println("x is not 1");
        }
    }
}
