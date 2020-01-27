#include <cxxopts.hpp>
#include <plog/Appenders/ConsoleAppender.h>

#include "clok.hpp"
#include "bridge.hpp"
#include "program.hpp"

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
	cxxopts::Options optParser(argv[0],
#ifdef DEBUG
		"Compiler for the Lok programming language (debug build)"
#else
		"Compiler for the Lok programming language"
#endif
	);

	optParser.add_options()
		("h,help", "This help message")
		("ast-dump", "Dump the syntax trees of all lok input files and quit")
		("silent", "Disable all output")
		("v,verbose", "Enable information output")
		("loquacious", "Enable debug output")
		("garrulous", "Enable scanner debug output (debug builds only)")
		("palaverous", "Enable parser debug output (debug builds only)")
	;

	auto options = optParser.parse(argc, argv);

	if (options.count("help")) {
		std::cout << optParser.help();
		return EXIT_SUCCESS;
	}

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
	} else if (options.count("silent")) {
		plog::get()->setMaxSeverity(plog::Severity::none);
	}

	if (argc == 1) {
		PLOGF << "No input files";
		return EXIT_FAILURE;
	}

	bool astDump = options.count("ast-dump");

	std::queue<bfs::path> lokFiles;
	std::queue<bfs::path> objFiles;
	for (int i = 1; i < argc; i++) {
		bfs::path file = bfs::canonical(argv[i]);
		if (file.extension() == ".lok") {
			lokFiles.push(file);
		} else if (file.extension() == ".o") {
			objFiles.push(file);
		}
	}

	Bridge b;
	std::unordered_map<std::string, std::unique_ptr<Node>> syntaxes;

	while (!lokFiles.empty()) {
		bfs::path path = lokFiles.front();
		std::string filename = path.c_str();
		lokFiles.pop();

		PLOGI << "Parsing file " << filename;
		if (b.parse(filename) != 0) return EXIT_FAILURE;
		syntaxes[filename] = std::move(parseResult);

		if (astDump) {
			std::string astFile = filename.substr(0, filename.find_last_of('.')) += ".ast";
			PLOGI << "Dumping AST to " << astFile;
			dumpAST(syntaxes[filename], astFile);
			continue;
		}

		bfs::path workingDir = path.parent_path();

		for (auto& node : syntaxes[filename]->children) {
			if (node->type == NodeType::LOAD) {
				if (node->children[0]->children[0]->type != NodeType::LIBNAME) {
					std::string newFile = bfs::canonical(parseFilename(node->children[0]->children[0]) + ".lok", workingDir).c_str();
					PLOGD << "File requires parse of " << newFile;
					lokFiles.push(newFile);
				}
			}
		}
	}

	PLOGI << "Finished parsing";

	if (astDump) return EXIT_SUCCESS;

	PLOGI << "Identifying symbols";

	std::unordered_map<std::string, Program> programs;

	for (auto& syntax : syntaxes) {
		PLOGI << "Identifying symbols for " << syntax.first;
		programs[syntax.first] = Program();
		programs[syntax.first].findSymbols(syntax.second);
	}

	PLOGI << "Extrapolating types";

	for (auto& syntax : syntaxes) {
		PLOGI << "Extrapolating types for " << syntax.first;
		programs[syntax.first].extrapolate(syntax.second);
	}
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
