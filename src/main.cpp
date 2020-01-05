#include <cxxopts.hpp>
#include <plog/Appenders/ConsoleAppender.h>

#include "clok.hpp"
#include "bridge.hpp"

#include <unordered_map>
#include <queue>

#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

#ifdef DEBUG
extern int yy_flex_debug;
int yydebug;
#endif

class Formatter {
public:
	static plog::util::nstring header() {
		return plog::util::nstring();
	}

	static plog::util::nstring format(const plog::Record &record) {
		plog::util::nostringstream ss;
		switch (record.getSeverity()) {
			case plog::Severity::debug:
				ss << "\x1b[36m";
				break;
			case plog::Severity::info:
				ss << "\x1b[32m";
				break;
			case plog::Severity::warning:
				ss << "\x1b[35m";
				break;
			case plog::Severity::fatal:
				ss << "\x1b[1m";
			case plog::Severity::error:
				ss << "\x1b[31m";
				break;
			default:
				break;
		}
		ss << plog::severityToString(record.getSeverity()) << "\x1b[0m: " << record.getMessage() << '\n';
		return ss.str();
	}
};

static plog::ConsoleAppender<Formatter> appender;

std::string parseFilename(NodePtr& node);

int main(int argc, char *argv[]) {
	cxxopts::Options optParser("clok", "Compiler for the Lok programming language");

	optParser.add_options()
		("ast-dump", "Dump the AST's of all lok input files and quit")
		("v,verbose", "Enable information output")
		("loquacious", "Enable debug output")
		("garrulous", "Enable scanner debug output (debug builds only)")
		("palaverous", "Enable parser debug output (debug builds only)")
	;

	auto options = optParser.parse(argc, argv);

#ifdef DEBUG
	yydebug = options.count("palaverous");
	yy_flex_debug = yydebug || options.count("garrulous");
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

	if (argc == 1) {
		PLOGF << "No input files";
		return EXIT_FAILURE;
	}

	bool astDump = options.count("ast-dump");

	Bridge b;
	std::unordered_map<std::string, std::unique_ptr<Node>> syntaxes;

	std::queue<bfs::path> files;
	for (int i = 1; i < argc; i++) {
		files.push(bfs::canonical(argv[i]));
	}

	PLOGI << "Parsing " << argc - 1 << " files";
	while (!files.empty()) {
		std::string filename = files.front().c_str();
		files.pop();

		PLOGI << "Parsing file " << filename;
		if (b.parse(filename) != 0) return EXIT_FAILURE;
		syntaxes[filename] = std::move(parseResult);

		if (astDump) {
			std::string astFile = filename.substr(0, filename.find_last_of('.')) += ".ast";
			PLOGI << "Dumping AST to " << astFile;
			dumpAST(syntaxes[filename], astFile);
			continue;
		}
		bfs::path workingDir = bfs::path(filename).parent_path();

		for (auto& node : syntaxes[filename]->children) {
			if (node->type == NodeType::LOAD) {
				if (node->children[0]->children[0]->type != NodeType::LIBNAME) {
					std::string newFile = bfs::canonical(parseFilename(node->children[0]->children[0]) + ".lok", workingDir).c_str();
					PLOGD << "File requests parse of " << newFile;
					files.push(newFile);
				}
			}
		}
	}

	if (astDump) return EXIT_SUCCESS;
}

std::string parseFilename(NodePtr& node) {
	return node->children.size() > 0 ? strings[node->value.valC] + parseFilename(node->children[0]) : strings[node->value.valC];
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
