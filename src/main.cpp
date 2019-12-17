#include <cxxopts.hpp>
#include <plog/Appenders/ConsoleAppender.h>

#include "clok.hpp"

#ifdef DEBUG
extern int yy_flex_debug, yydebug;
#endif

class Formatter {
public:
    static plog::util::nstring header() {
        return plog::util::nstring();
    }

    static plog::util::nstring format(const plog::Record &record) {
        plog::util::nostringstream ss;
        ss << plog::severityToString(record.getSeverity()) << ": " << record.getMessage() << '\n';
        return ss.str();
    }
};

static plog::ConsoleAppender<Formatter> appender;

int main(int argc, char *argv[]) {
    cxxopts::Options optParser("clok", "Compiler for the Lok programming language");

    optParser.add_options()
        ("v,verbose", "Enable information output")
        ("loquacious", "Enable debug output")
        ("garrulous", "Enable scanner debug output (debug builds only)")
        ("palaverous", "Enable parser debug output (debug builds only)")
    ;

    auto options = optParser.parse(argc, argv);

#ifdef DEBUG
    yydebug = options.count("palaverous");
    yy_flex_debug = yydebug || options.count("garrulous")
#endif

    plog::init(plog::warning, &appender);

    if (
#ifdef DEBUG
        yy_flex_debug ||
#endif
        options.count("loquacious")) {
        plog::get()->setMaxSeverity(plog::Severity::debug);
    } else if (options.count("verbose")) {
        plog::get()->setMaxSeverity(plog::Severity::info);
    }

    PLOGI << "Initialised Logger";
}

std::vector<std::string> strings;

strings_t getString(std::string string) {
    for (strings_t i = 0; i < strings.size(); i++) {
        if (string.compare(strings[i]) == 0) {
            return i;
        }
    }

    strings.push_back(string);
    return strings.size() - 1;
}
