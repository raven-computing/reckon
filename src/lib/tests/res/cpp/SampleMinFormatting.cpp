/* Golden sample: Contains C++ language features and constructs */
/**
 * block comment
 */
// line comment
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <functional>
#include <type_traits>
#if defined(__cplusplus)
#  if (__cplusplus >= 201703L)
#    define SAMPLE_CPP_CPP17 1
#  else
#    define SAMPLE_CPP_CPP17 0
#  endif
#else
#  define SAMPLE_CPP_CPP17 0
#endif
/* Pragmas */
#pragma once
/* Preprocessor macros */
#define SAMPLE_VERSION 1
#define STR(x) #x
#define XSTR(x) STR(x)
#define CONCAT(a, b) a##b
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define UNUSED(x) ((void)(x))
/* Forward declarations */
class Shape;
class Circle;
class Rectangle;
/* Type alias */
using StringList = std::vector<std::string>;
using StringMap = std::map<std::string, std::string>;
/* Namespace alias */
namespace fs = std::filesystem;
/* Enum */
enum Color { RED, GREEN, BLUE };
/* Enum class (C++11) */
enum class Direction { NORTH, SOUTH, EAST, WEST };
/* Struct */
struct Point { double x; double y; };
/* Typedef struct */
typedef struct { int width; int height; } Size;
/* Union */
union Data { int i; float f; char c; };
/* Namespace */
namespace geometry {
/* Abstract base class */
class Shape {
public:
    Shape() = default;
    virtual ~Shape() = default;
    virtual double area() const = 0;
    virtual double perimeter() const = 0;
    std::string name() const { return name_; }
protected:
    std::string name_;
};
/* Derived class */
class Circle : public Shape {
public:
    explicit Circle(double radius) : radius_(radius) { name_ = "Circle"; }
    ~Circle() override = default;
    double area() const override { return 3.14159265358979 * radius_ * radius_; }
    double perimeter() const override { return 2.0 * 3.14159265358979 * radius_; }
    double radius() const { return radius_; }
private:
    double radius_;
};
/* Another derived class */
class Rectangle : public Shape {
public:
    Rectangle(double width, double height) : width_(width), height_(height) { name_ = "Rectangle"; }
    ~Rectangle() override = default;
    double area() const override { return width_ * height_; }
    double perimeter() const override { return 2.0 * (width_ + height_); }
private:
    double width_;
    double height_;
};
} // namespace geometry
/* Template class */
template<typename T>
class Stack {
public:
    Stack() = default;
    void push(const T& value) { data_.push_back(value); }
    T pop() {
        if (data_.empty()) { throw std::out_of_range("Stack is empty"); }
        T value = data_.back();
        data_.pop_back();
        return value;
    }
    bool empty() const { return data_.empty(); }
    size_t size() const { return data_.size(); }
private:
    std::vector<T> data_;
};
/* Template function */
template<typename T>
T clamp(T value, T min_val, T max_val) {
    if (value < min_val) { return min_val; }
    if (value > max_val) { return max_val; }
    return value;
}
/* Template specialization */
template<>
const char* clamp<const char*>(const char* value, const char* min_val, const char* max_val) {
    if (std::string(value) < std::string(min_val)) { return min_val; }
    if (std::string(value) > std::string(max_val)) { return max_val; }
    return value;
}
/* Static assert */
static_assert(sizeof(int) >= 4, "int must be at least 4 bytes");
/* Using declaration */
using std::cout;
using std::endl;
using std::string;
/* Free function with control flow */
int processValues(const std::vector<int>& values) {
    if (values.empty()) { return 0; }
    int result = 0;
    for (int v : values) { result += v; }
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] < 0) { continue; }
        result += values[i];
    }
    return result;
}
/* Function with switch statement */
std::string colorName(Color c) {
    switch (c) {
        case RED: return "red";
        case GREEN: return "green";
        case BLUE: return "blue";
        default: return "unknown";
    }
}
/* Function with try/catch */
double safeDivide(double a, double b) {
    if (b == 0.0) { throw std::invalid_argument("Division by zero"); }
    return a / b;
}
double trySafeDivide(double a, double b) {
    try { return safeDivide(a, b); }
    catch (const std::invalid_argument& e) { std::cerr << "Error: " << e.what() << std::endl; return 0.0; }
    catch (...) { std::cerr << "Unknown error" << std::endl; return 0.0; }
}
/* Function with while and do-while loops */
int countDown(int start) {
    int count = 0;
    while (start > 0) { --start; ++count; }
    do { ++count; } while (count < 5);
    return count;
}
/* Lambda expression usage */
void sortAndPrint(std::vector<int>& data) {
    std::sort(data.begin(), data.end(), [](int a, int b) { return a < b; });
    std::for_each(data.begin(), data.end(), [](int v) { std::cout << v << " "; });
}
/* Smart pointer usage */
std::unique_ptr<geometry::Shape> createShape(const std::string& type) {
    if (type == "circle") { return std::make_unique<geometry::Circle>(5.0); }
    else if (type == "rectangle") { return std::make_unique<geometry::Rectangle>(3.0, 4.0); }
    else { return nullptr; }
}
/* extern "C" linkage */
extern "C" {
    int cCompatibleFunc(int x) { return x * 2; }
}
/* Main function */
int main() {
    geometry::Circle circle(5.0);
    geometry::Rectangle rect(3.0, 4.0);
    std::cout << "Circle area: " << circle.area() << std::endl;
    std::cout << "Rect area: " << rect.area() << std::endl;
    Stack<int> stack;
    stack.push(1);
    stack.push(2);
    stack.push(3);
    while (!stack.empty()) { std::cout << stack.pop() << std::endl; }
    std::vector<int> nums = {5, 3, 8, 1, 9, 2, 7, 4, 6};
    sortAndPrint(nums);
    std::cout << std::endl;
    auto shape = createShape("circle");
    if (shape) { std::cout << "Shape: " << shape->name() << std::endl; }
    double result = trySafeDivide(10.0, 2.0);
    std::cout << "Result: " << result << std::endl;
    StringList names = {"Alice", "Bob", "Charlie"};
    for (const auto& name : names) { std::cout << name << std::endl; }
    return 0;
}
