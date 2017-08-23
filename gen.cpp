#include <fstream>
#include <iostream>

constexpr size_t test_power = 100;

struct recursion;
struct folding;
struct folding2;

template<class T>
struct counter{};

template<class T>
struct is_unique{};

template<class T>
struct index_of{};

//#define CURRENT_TAG counter<recursion>

void generate(std::ostream& os, counter<recursion>)
{
    os << "template<class T, class H=empty, class... Args>\n\
struct counter {\n\
    static constexpr size_t value = std::is_same_v<T, H> + counter<T, Args...>::value;\n\
};\n\
template<class T>\n\
struct counter<T, empty> {\n\
    static constexpr size_t value = 0;\n\
};\n";
};

void generate(std::ostream& os, counter<folding>)
{
    os << "template<class T, class... Args>\n\
struct counter {\n\
    static constexpr size_t value = (0 + ... + std::is_same_v<T, Args>);\n\
};\n";
}

template<class T>
void generate_asserts(std::ostream& os, size_t k, counter<T>)
{
    for (size_t i = 0; i < test_power; ++i) {
        os << "static_assert( n" << i << "::counter<void";
        for (size_t j = 0; j < k; ++j)
            os << ", c" << j;
        os << ">::value == 0 );\n";
    }
}

void generate(std::ostream& os, is_unique<recursion>)
{
    generate(os, counter<recursion>{});
    os << "template<class H=empty, class... Args>\n\
struct is_unique {\n\
    static constexpr size_t value = counter<H, Args...>::value == 0 && is_unique<Args...>::value;\n\
};\n\
template<>\n\
struct is_unique<empty> {\n\
    static constexpr size_t value = true;\n\
};\n";
}

void generate(std::ostream& os, is_unique<folding>)
{
    generate(os, counter<folding>{});
    os << "template<class... Args>\n\
struct is_unique {\n\
    static constexpr size_t value = (... && (counter<Args, Args...>::value == 1));\n\
};\n";
}

template<class T>
void generate_asserts(std::ostream& os, size_t k, is_unique<T>)
{
    for (size_t i = 0; i < test_power; ++i) {
        os << "static_assert( n" << i << "::is_unique<";
        bool comma = false;
        for (size_t j = 0; j < k; ++j) {
            if (comma)
                os << ", ";
            os << "c" << j;
            comma = true;
        }
        os << ">::value );\n";
    }
}

void generate(std::ostream& os, index_of<recursion>)
{
    os << "template<class T, size_t I, class H=empty, class... Args>\n\
struct index_of_impl {\n\
    static constexpr size_t value = index_of_impl<T, I + 1, Args...>::value;\n\
};\n\
template<class T, size_t I, class... Args>\n\
struct index_of_impl<T, I, T, Args...> {\n\
    static constexpr size_t value = I;\n\
};\n\
template<class T, size_t I>\n\
struct index_of_impl<T, I, empty> {\n\
    static constexpr size_t value = size_t(-1);\n\
};\n\
template<class T, class... Args>\n\
struct index_of {\n\
    static constexpr size_t value = index_of_impl<T, 0, Args...>::value;\n\
};\n";
}

void generate(std::ostream& os, index_of<folding>)
{
    os << "enum index_t: size_t {};\n\
constexpr index_t operator << (index_t lhs, index_t rhs) {\n\
    return lhs < rhs ? lhs : rhs;\n\
}\n\
template<class T, class U, class... Args>\n\
struct index_of_impl;\n\
template<class T, size_t... I, class... Args>\n\
struct index_of_impl<T, std::index_sequence<I...>, Args...> {\n\
    static constexpr size_t value = (index_t(-1) << ... << index_t(std::is_same_v<T, Args> ? I : -1));\n\
};\n\
template<class T, class... Args>\n\
struct index_of {\n\
    static constexpr size_t value = index_of_impl<T, std::index_sequence_for<Args...>, Args...>::value;\n\
};\n";
}

void generate(std::ostream& os, index_of<folding2>)
{
    os << "struct iholder {\n\
    size_t index = 0;\n\
    bool found = false;\n\
};\n\
constexpr iholder operator << (iholder h, bool is_same) {\n\
    if (h.found)\n\
        return h;\n\
    if (is_same)\n\
        return iholder{h.index, true };\n\
    return iholder{h.index + 1, false};\n\
}\n\
constexpr size_t normalize(iholder h) {\n\
    return h.found ? h.index : -1;\n\
}\n\
template<class T, class... Args>\n\
struct index_of {\n\
    static constexpr size_t value = normalize((iholder{} << ... << (std::is_same_v<T, Args>)));\n\
};\n";
}

template<class T>
void generate_asserts(std::ostream& os, size_t k, index_of<T>)
{
    for (size_t i = 0; i < test_power; ++i) {
        os << "static_assert( n" << i << "::index_of<void";
        for (size_t j = 0; j < k; ++j)
            os << ", c" << j;
        os << ", void";
        for (size_t j = k; j < test_power; ++j)
            os << ", c" << j;
        os << ">::value == " << k << " );\n";
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Not enough parameters" << std::endl;
        return 1;
    }

    size_t const k = atoi(argv[1]);
    if (k > test_power) {
        std::cerr << "Parameter too large" << std::endl;
        return 1;
    }

    std::ofstream of{"gen_main.cpp"};

    of << "#include <utility>\n\
#include <cstddef>\n\
#include <type_traits>\n";

    of << "struct empty;\n\
namespace std {\n\
template<class T, class U>\n\
constexpr bool is_same_v = std::is_same<T, U>::value;\n\
}\n";

    for (size_t i = 0; i < test_power; ++i) {
        of << "class c" << i << ";\n";
    }

    for (size_t i = 0; i < test_power; ++i) {
        of << "namespace n" << i << "{\n";
        generate(of, CURRENT_TAG{});
        of << "}\n";
    }

    of << "int main() {\n";
    generate_asserts(of, k, CURRENT_TAG{});
    of << "}\n";
}
