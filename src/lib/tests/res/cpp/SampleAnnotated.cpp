/* Golden sample: Contains C++ language features and constructs */

/**
 * block comment
 */

// line comment

#include <iostream> // +1 (preproc include)
#include <vector> // +1 (preproc include)
#include <string> // +1 (preproc include)
#include <map> // +1 (preproc include)
#include <memory> // +1 (preproc include)
#include <algorithm> // +1 (preproc include)
#include <stdexcept> // +1 (preproc include)
#include <utility> // +1 (preproc include)
#include <functional> // +1 (preproc include)
#include <type_traits> // +1 (preproc include)

#if defined(__cplusplus) // +1 (preproc if)
#  if (__cplusplus >= 201703L) // +1 (preproc if)
#    define SAMPLE_CPP_CPP17 1 // +1 (preproc def)
#  else // +1 (preproc else)
#    define SAMPLE_CPP_CPP17 0 // +1 (preproc def)
#  endif
#else // +1 (preproc else)
#  define SAMPLE_CPP_CPP17 0 // +1 (preproc def)
#endif

/* Pragmas */
#pragma once // +1 (preproc directive)

/* Preprocessor macros */
#define SAMPLE_VERSION 1 // +1 (preproc def)
#define STR(x) #x // +1 (preproc function def)
#define XSTR(x) STR(x) // +1 (preproc function def)
#define CONCAT(a, b) a##b // +1 (preproc function def)

#define MAX(a, b) ((a) > (b) ? (a) : (b)) // +1 (preproc function def)
#define MIN(a, b) ((a) < (b) ? (a) : (b)) // +1 (preproc function def)

#define UNUSED(x) ((void)(x)) // +1 (preproc function def)

/* Forward declarations */
class Shape; // +1 (class specifier)
class Circle; // +1 (class specifier)
class Rectangle; // +1 (class specifier)

/* Type alias */
using StringList = std::vector<std::string>; // +1 (alias declaration)
using StringMap = std::map<std::string, std::string>; // +1 (alias declaration)

/* Namespace alias */
namespace fs = std::filesystem; // +1 (namespace alias definition)

/* Enum */
enum Color { // +1 (enum specifier)
    RED, // +1 (enumerator)
    GREEN, // +1 (enumerator)
    BLUE // +1 (enumerator)
};

/* Enum class (C++11) */
enum class Direction { // +1 (enum specifier)
    NORTH, // +1 (enumerator)
    SOUTH, // +1 (enumerator)
    EAST, // +1 (enumerator)
    WEST // +1 (enumerator)
};

/* Struct */
struct Point { // +1 (struct specifier)
    double x; // +1 (field declaration)
    double y; // +1 (field declaration)
};

/* Typedef struct */
typedef struct { // +1 (type definition)
    int width; // +1 (field declaration)
    int height; // +1 (field declaration)
} Size;

/* Union */
union Data { // +1 (union specifier)
    int i; // +1 (field declaration)
    float f; // +1 (field declaration)
    char c; // +1 (field declaration)
};

/* Namespace */
namespace geometry { // +2 (namespace definition, declaration list)

/* Abstract base class */
class Shape { // +1 (class specifier)
public:
    Shape() = default; // +1 (function definition)
    virtual ~Shape() = default; // +1 (function definition)

    virtual double area() const = 0; // +1 (field declaration)
    virtual double perimeter() const = 0; // +1 (field declaration)

    std::string name() const { // +1 (function definition)
        return name_; // +1 (return statement)
    }

protected:
    std::string name_; // +1 (field declaration)
};

/* Derived class */
class Circle : public Shape { // +1 (class specifier)
public:
    explicit Circle(double radius) : radius_(radius) { // +1 (function definition)
        name_ = "Circle"; // +1 (expression statement)
    }

    ~Circle() override = default; // +1 (function definition)

    double area() const override { // +1 (function definition)
        return 3.14159265358979 * radius_ * radius_; // +1 (return statement)
    }

    double perimeter() const override { // +1 (function definition)
        return 2.0 * 3.14159265358979 * radius_; // +1 (return statement)
    }

    double radius() const { // +1 (function definition)
        return radius_; // +1 (return statement)
    }

private:
    double radius_; // +1 (field declaration)
};

/* Another derived class */
class Rectangle : public Shape { // +1 (class specifier)
public:
    Rectangle(double width, double height) // +1 (function definition)
        : width_(width), height_(height) {
        name_ = "Rectangle"; // +1 (expression statement)
    }

    ~Rectangle() override = default; // +1 (function definition)

    double area() const override { // +1 (function definition)
        return width_ * height_; // +1 (return statement)
    }

    double perimeter() const override { // +1 (function definition)
        return 2.0 * (width_ + height_); // +1 (return statement)
    }

private:
    double width_; // +1 (field declaration)
    double height_; // +1 (field declaration)
};

} // namespace geometry

/* Template class */
template<typename T> // +1 (template declaration)
class Stack { // +1 (class specifier)
public:
    Stack() = default; // +1 (function definition)

    void push(const T& value) { // +1 (function definition)
        data_.push_back(value); // +1 (expression statement)
    }

    T pop() { // +1 (function definition)
        if (data_.empty()) { // +1 (if statement)
            throw std::out_of_range("Stack is empty"); // +1 (throw statement)
        }
        T value = data_.back(); // +1 (declaration)
        data_.pop_back(); // +1 (expression statement)
        return value; // +1 (return statement)
    }

    bool empty() const { // +1 (function definition)
        return data_.empty(); // +1 (return statement)
    }

    size_t size() const { // +1 (function definition)
        return data_.size(); // +1 (return statement)
    }

private:
    std::vector<T> data_; // +1 (field declaration)
};

/* Template function */
template<typename T> // +1 (template declaration)
T clamp(T value, T min_val, T max_val) { // +1 (function definition)
    if (value < min_val) { // +1 (if statement)
        return min_val; // +1 (return statement)
    }
    if (value > max_val) { // +1 (if statement)
        return max_val; // +1 (return statement)
    }
    return value; // +1 (return statement)
}

/* Template specialization */
template<> // +1 (template declaration)
const char* clamp<const char*>( // +1 (function definition)
    const char* value,
    const char* min_val,
    const char* max_val
) {
    if (std::string(value) < std::string(min_val)) { // +1 (if statement)
        return min_val; // +1 (return statement)
    }
    if (std::string(value) > std::string(max_val)) { // +1 (if statement)
        return max_val; // +1 (return statement)
    }
    return value; // +1 (return statement)
}

/* Static assert */
static_assert(sizeof(int) >= 4, "int must be at least 4 bytes"); // +1 (static assert declaration)

/* Using declaration */
using std::cout; // +1 (using declaration)
using std::endl; // +1 (using declaration)
using std::string; // +1 (using declaration)

/* Free function with control flow */
int processValues(const std::vector<int>& values) { // +1 (function definition)
    if (values.empty()) { // +1 (if statement)
        return 0; // +1 (return statement)
    }

    int result = 0; // +1 (declaration)

    for (int v : values) { // +1 (for range loop)
        result += v; // +1 (expression statement)
    }

    for (size_t i = 0; i < values.size(); ++i) { // +1 (for statement)
        if (values[i] < 0) { // +1 (if statement)
            continue; // +1 (continue statement)
        }
        result += values[i]; // +1 (expression statement)
    }

    return result; // +1 (return statement)
}

/* Function with switch statement */
std::string colorName(Color c) { // +1 (function definition)
    switch (c) { // +1 (switch statement)
        case RED: // +1 (case statement)
            return "red"; // +1 (return statement)
        case GREEN: // +1 (case statement)
            return "green"; // +1 (return statement)
        case BLUE: // +1 (case statement)
            return "blue"; // +1 (return statement)
        default: // +1 (case statement)
            return "unknown"; // +1 (return statement)
    }
}

/* Function with try/catch */
double safeDivide(double a, double b) { // +1 (function definition)
    if (b == 0.0) { // +1 (if statement)
        throw std::invalid_argument("Division by zero"); // +1 (throw statement)
    }
    return a / b; // +1 (return statement)
}

double trySafeDivide(double a, double b) { // +1 (function definition)
    try { // +1 (try statement)
        return safeDivide(a, b); // +1 (return statement)
    } catch (const std::invalid_argument& e) { // +1 (catch clause)
        std::cerr << "Error: " << e.what() << std::endl; // +1 (expression statement)
        return 0.0; // +1 (return statement)
    } catch (...) { // +1 (catch clause)
        std::cerr << "Unknown error" << std::endl; // +1 (expression statement)
        return 0.0; // +1 (return statement)
    }
}

/* Function with while and do-while loops */
int countDown(int start) { // +1 (function definition)
    int count = 0; // +1 (declaration)
    while (start > 0) { // +1 (while statement)
        --start; // +1 (expression statement)
        ++count; // +1 (expression statement)
    }
    do { // +2 (do statement)
        ++count; // +1 (expression statement)
    } while (count < 5);
    return count; // +1 (return statement)
}

/* Lambda expression usage */
void sortAndPrint(std::vector<int>& data) { // +1 (function definition)
    std::sort(data.begin(), data.end(), [](int a, int b) { // +1 (expression statement)
        return a < b; // +1 (return statement)
    });
    std::for_each(data.begin(), data.end(), [](int v) { // +1 (expression statement)
        std::cout << v << " "; // +1 (expression statement)
    });
}

/* Smart pointer usage */
std::unique_ptr<geometry::Shape> createShape(const std::string& type) { // +1 (function definition)
    if (type == "circle") { // +1 (if statement)
        return std::make_unique<geometry::Circle>(5.0); // +1 (return statement)
    } else if (type == "rectangle") { // +1 (else clause)
        return std::make_unique<geometry::Rectangle>(3.0, 4.0); // +1 (return statement)
    } else { // +1 (else clause)
        return nullptr; // +1 (return statement)
    }
}

/* extern "C" linkage */
extern "C" { // +2 (linkage specification, declaration list)
    int cCompatibleFunc(int x) { // +1 (function definition)
        return x * 2; // +1 (return statement)
    }
}

/* Main function */
int main() { // +1 (function definition)
    geometry::Circle circle(5.0); // +1 (declaration)
    geometry::Rectangle rect(3.0, 4.0); // +1 (declaration)

    std::cout << "Circle area: " << circle.area() << std::endl; // +1 (expression statement)
    std::cout << "Rect area: " << rect.area() << std::endl; // +1 (expression statement)

    Stack<int> stack; // +1 (declaration)
    stack.push(1); // +1 (expression statement)
    stack.push(2); // +1 (expression statement)
    stack.push(3); // +1 (expression statement)

    while (!stack.empty()) { // +1 (while statement)
        std::cout << stack.pop() << std::endl; // +1 (expression statement)
    }

    std::vector<int> nums = {5, 3, 8, 1, 9, 2, 7, 4, 6}; // +1 (declaration)
    sortAndPrint(nums); // +1 (expression statement)
    std::cout << std::endl; // +1 (expression statement)

    auto shape = createShape("circle"); // +1 (declaration)
    if (shape) { // +1 (if statement)
        std::cout << "Shape: " << shape->name() << std::endl; // +1 (expression statement)
    }

    double result = trySafeDivide(10.0, 2.0); // +1 (declaration)
    std::cout << "Result: " << result << std::endl; // +1 (expression statement)

    StringList names = {"Alice", "Bob", "Charlie"}; // +1 (declaration)
    for (const auto& name : names) { // +1 (for range loop)
        std::cout << name << std::endl; // +1 (expression statement)
    }

    return 0; // +1 (return statement)
}
