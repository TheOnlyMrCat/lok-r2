#include <cxxopts.hpp>

#include "clok.hpp"

INITIALIZE_EASYLOGGINGPP

#ifdef DEBUG
extern int yy_flex_debug, yydebug;
#endif

int main() {
    cxxopts::Options options("clok", "Compiler for the Lok programming language");

    options.add_options()
        ("v,verbose", "Enable information output")
        ("loquacious", "Enable debug output")
        ("garrulous", "Enable scanner debug output")
        ("palaverous", "Enable parser debug output")
    ;

#ifdef DEBUG

#endif

}