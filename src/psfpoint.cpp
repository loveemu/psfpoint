
#define NOMINMAX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <limits>
#include <algorithm>

#include "PSFFile.h"

#define APP_NAME    "psfpoint"
#define APP_VER     "[2015-04-17]"
#define APP_URL     "http://github.com/loveemu/psfpoint"

bool both_are_spaces(char lhs, char rhs)
{
	return (lhs == rhs) && (lhs == ' ');
}

void replace_all(std::string& str, const std::string& from, const std::string& to)
{
	std::string::size_type pos = 0;
	while (pos = str.find(from, pos), pos != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length();
	}
}

static void usage(const char * progname)
{
	printf("%s %s\n", APP_NAME, APP_VER);
	printf("<%s>\n", APP_URL);
	printf("\n");
	printf("Usage: `%s [-tf] [-variable=value ...] psf-file(s)`\n", progname);
	printf("\n");
}

int main(int argc, char *argv[])
{
	if (argc == 1) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	std::map<std::string, std::string> opt_tags;
	bool title_from_filename = false;

	int argi = 1;
	while (argi < argc && argv[argi][0] == '-')
	{
		char * p_equal = strchr(argv[argi], '=');

		if (p_equal != NULL) {
			// tag option
			std::string name(argv[argi], 1, p_equal - argv[argi] - 1);
			std::string value(p_equal + 1);
			opt_tags[name] = value;
		}
		else {
			// regular option
			if (strcmp(argv[argi], "--help") == 0) {
				usage(argv[0]);
				return EXIT_FAILURE;
			}
			else if (strcmp(argv[argi], "-tf") == 0) {
				title_from_filename = true;
			}
			else {
				fprintf(stderr, "Error: Unknown option \"%s\"\n", argv[argi]);
				return EXIT_FAILURE;
			}
		}

		argi++;
	}

	int argnum = argc - argi;
	if (argnum == 0) {
		fprintf(stderr, "Error: No input files\n");
		return EXIT_FAILURE;
	}

	if (opt_tags.size() != 0) {
		printf("-----replacing variables-----\n");

		if (title_from_filename) {
			printf("title=[from filename]\n");
		}

		for (auto itr = opt_tags.begin(); itr != opt_tags.end(); ++itr) {
			const std::string & name = (*itr).first;
			const std::string & value = (*itr).second;

			if (!title_from_filename || name != "title") {
				printf("%s=%s\n", name.c_str(), value.c_str());
			}
		}

		printf("-----------------------------\n");
	}

	int num_errors = 0;
	for (; argi < argc; argi++) {
		std::string filename(argv[argi]);

		PSFFile * psf = PSFFile::load(filename);
		if (psf == NULL) {
			printf("%s: load error\n", filename.c_str());
			num_errors++;
			continue;
		}

		std::map<std::string, std::string> psf_tags(opt_tags);
		if (title_from_filename) {
			std::string title(filename);

			// remove extension
			std::string::size_type offset_dot = title.find_last_of('.');
			if (offset_dot != std::string::npos) {
				title = title.substr(0, offset_dot);
			}

			// trim some beginning characters
			std::string::size_type offset_start = title.find_first_not_of(" _%0123456789-");
			if (offset_start != std::string::npos) {
				title = title.substr(offset_start);
			}

			// replace some other stuff
			replace_all(title, "%20", " ");
			replace_all(title, "_", " ");

			// replace multiple spaces with one space
			std::string::iterator new_end = std::unique(title.begin(), title.end(), both_are_spaces);
			title.erase(new_end, title.end());

			psf_tags["title"] = title;
		}

		if (psf_tags.size() != 0) {
			for (auto itr = psf_tags.begin(); itr != psf_tags.end(); ++itr) {
				const std::string & name = (*itr).first;
				const std::string & value = (*itr).second;
				psf->tags[name] = value;
			}

			if (!PSFFile::save(filename, psf->version, &psf->reserved[0], psf->reserved.size(), psf->compressed_exe.compressed_data(), psf->compressed_exe.compressed_size(), psf->tags)) {
				printf("%s: save error\n", filename.c_str());
				num_errors++;
				delete psf;
				continue;
			}

			printf("%s: ok\n", filename.c_str());
		}
		else {
			// Put tag variables for another PSF tagging
			const std::map<std::string, std::string> & current_tags = psf->tags;

			printf("psfpoint");
			for (auto itr = current_tags.begin(); itr != current_tags.end(); ++itr) {
				const std::string & name = (*itr).first;
				const std::string & value = (*itr).second;

				if (name.find_first_of(" ") != std::string::npos || value.find_first_of(" ") != std::string::npos || value.find_first_of("\n") != std::string::npos) {
					printf(" \"-%s=%s\"", name.c_str(), value.c_str());
				}
				else {
					printf(" -%s=%s", name.c_str(), value.c_str());
				}
			}

			if (filename.find_first_of(" ") != std::string::npos) {
				printf(" \"%s\"", filename.c_str());
			}
			else {
				printf(" %s", filename.c_str());
			}
			printf("\n");
		}

		delete psf;
	}

	return (num_errors == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
