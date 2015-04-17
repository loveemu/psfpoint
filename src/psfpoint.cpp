
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

#ifdef WIN32
#include <Windows.h>
#endif

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

bool mbs_to_utf8(std::string & utf8str, const std::string & mbstr)
{
#ifdef WIN32
	int ret;
	int wcs_len;
	wchar_t * wcsbuf;
	int utf8_len;
	char * utf8buf;

	if (mbstr == "") {
		utf8str = "";
		return true;
	}

	wcs_len = MultiByteToWideChar(CP_ACP, 0, &mbstr[0], -1, NULL, 0);
	if (wcs_len == 0) {
		return false;
	}

	wcsbuf = (wchar_t *)malloc(sizeof(wchar_t) * wcs_len);
	if (wcsbuf == NULL) {
		return false;
	}

	ret = MultiByteToWideChar(CP_ACP, 0, &mbstr[0], -1, wcsbuf, wcs_len);
	if (ret == 0) {
		free(wcsbuf);
		return false;
	}

	utf8_len = WideCharToMultiByte(CP_UTF8, 0, wcsbuf, -1, NULL, 0, NULL, NULL);
	if (utf8_len == 0) {
		free(wcsbuf);
		return false;
	}

	utf8buf = (char *)malloc(utf8_len);
	if (utf8buf == NULL) {
		free(wcsbuf);
		return false;
	}

	ret = WideCharToMultiByte(CP_UTF8, 0, wcsbuf, -1, utf8buf, utf8_len, NULL, NULL);
	if (ret == 0) {
		free(utf8buf);
		free(wcsbuf);
		return false;
	}

	utf8str = utf8buf;
	free(utf8buf);
	free(wcsbuf);

	return true;
#else
	// Hopefully, a modern unix OS is using utf-8 and needs no charset conversion.
	utf8str = mbstr;
	return true;
#endif
}

bool utf8_to_mbs(std::string & mbstr, const std::string & utf8str)
{
#ifdef WIN32
	int ret;
	int wcs_len;
	wchar_t * wcsbuf;
	int mbs_len;
	char * mbsbuf;

	if (utf8str == "") {
		mbstr = "";
		return true;
	}

	wcs_len = MultiByteToWideChar(CP_UTF8, 0, &utf8str[0], -1, NULL, 0);
	if (wcs_len == 0) {
		return false;
	}

	wcsbuf = (wchar_t *)malloc(sizeof(wchar_t) * wcs_len);
	if (wcsbuf == NULL) {
		return false;
	}

	ret = MultiByteToWideChar(CP_UTF8, 0, &utf8str[0], -1, wcsbuf, wcs_len);
	if (ret == 0) {
		free(wcsbuf);
		return false;
	}

	mbs_len = WideCharToMultiByte(CP_ACP, 0, wcsbuf, -1, NULL, 0, NULL, NULL);
	if (mbs_len == 0) {
		free(wcsbuf);
		return false;
	}

	mbsbuf = (char *)malloc(mbs_len);
	if (mbsbuf == NULL) {
		free(mbsbuf);
		return false;
	}

	ret = WideCharToMultiByte(CP_ACP, 0, wcsbuf, -1, mbsbuf, mbs_len, NULL, NULL);
	if (ret == 0) {
		free(mbsbuf);
		free(wcsbuf);
		return false;
	}

	mbstr = mbsbuf;
	free(mbsbuf);
	free(wcsbuf);

	return true;
#else
	// Hopefully, a modern unix OS is using utf-8 and needs no charset conversion.
	mbstr = utf8str;
	return true;
#endif
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
			bool charset_error = false;
			for (auto itr = psf_tags.begin(); itr != psf_tags.end(); ++itr) {
				const std::string & name = (*itr).first;
				const std::string & value = (*itr).second;

				std::string value_raw;
				if (!mbs_to_utf8(value_raw, value)) {
					charset_error = true;
					break;
				}
				psf->tags[name] = value_raw;
			}

			if (charset_error) {
				printf("%s: charset conversion error\n", filename.c_str());
				num_errors++;
				delete psf;
				continue;
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
				const std::string & value_raw = (*itr).second;

				std::string value;
				if (!utf8_to_mbs(value, value_raw)) {
					fprintf(stderr, "Warning: Charset conversion error\n");
				}

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
