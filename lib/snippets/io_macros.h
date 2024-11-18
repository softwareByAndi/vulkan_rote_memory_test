#include <iostream>
#include "ascii_colors.h"

#define LOG_SUCCESS std::cout << GREEN << "  SUCCESS" << RESET << std::endl << std::endl
#define LOG_FAILURE std::cerr << RED << "  FAILURE" << std::endl << RESET << std::endl
#define FAIL(x) LOG_FAILURE; throw std::runtime_error(x)
#define print(x) std::cout << x << std::endl
#define list(x) std::cout << "  - " << x << std::endl
#define list_blue(x) std::cout << BLUE << "  - " << x << RESET << std::endl
#define list_gray(x) std::cout << GRAY << "  - " << x << RESET << std::endl
#define label(x, y) std::cout << "  - " << x << " : " << std::boolalpha << y << std::endl
#define label_blue(x, y) std::cout << BLUE << "  - " << x << " : " << std::boolalpha << y << RESET << std::endl
#define NEWLINE std::cout << std::endl
#define DNL std::endl << std::endl;

#define SECTION(name) print(name); /*<editor-fold desc="name">*/
#define END_SECTION /*</editor-fold>*/
