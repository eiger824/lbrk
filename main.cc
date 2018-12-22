#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <iterator>
#include <fstream>
#include <vector>

#include <libgen.h>
#include <getopt.h>

#define     NL     '\n'
#define     TAB    '\t'
#define     SPACE  ' '
#define     ZERO   '\0'
#define     DELIM  "<|"
#define     DEBUG(...) if (g_debug == 1) fprintf(stderr, __VA_ARGS__)

using namespace std;

static int g_debug     = 0;
static int g_break     = 1;
static int g_spaces    = 0;
static int g_lowercase = 0;
static int g_uppercase = 0;
static int g_endings   = 0;

/* Functors to use whether uppercase/lowercase modes are set */
static char& lbrk_modify(char& c)
{
    if (isspace(c))
    {
        if (c == TAB && g_spaces == 1)
            c = SPACE;
    }
    else
    {
        if (g_uppercase)
            c = toupper(c);
        if (g_lowercase)
            c = tolower(c);
    }
    return c;
}

static inline void lbrk_print_vector(vector<string> const & v)
{
    for (auto const & w : v)
    {
        cout << w << endl;
    }
}

static void lbrk_usage(char * progname)
{
    progname = basename(progname);
    cout
        << "Usage: " << progname << " [OPTIONS] [<filename>]\n"
        << "\nOPTIONS:\n"
        << " -d       Set debug\n"
        << " -e       Show line endings with a <| delimiter\n"
        << " -h       Show this help and exit\n"
        << " -l       Convert text to lowercase\n"
        << " -r       Respect words, don't break them [IN PROGRESS]\n"
        << " -t       Convert tabs to spaces\n"
        << " -u       Convert text to uppercase\n"
        << " -w width Width of text after breaking lines\n"
        << endl;
}

// convert string to vector<string>
static vector<string> str2vec(string & str)
{
    stringstream ss(str);
    istream_iterator<string> begin(ss);
    istream_iterator<string> end;
    vector<string> words_in_line(begin, end);
    return words_in_line;
}

// convert vector<string> to string
static string vec2str(vector<string> & vec, char sep)
{
    ostringstream oss;
    string tmp{1, sep};
    copy(vec.begin(), vec.end()-1, ostream_iterator<string>(oss, tmp.data()));
    oss << vec.back();
    return oss.str();
}

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

static string & lbrk_pad_line(string & line, size_t width)
{
    assert(line.length() <= width);
    // Pad the line according to width
    if (width - line.length() > 0)
    {
        size_t nonspaces = 0;
        vector<string> words_in_line = str2vec(line);
        for_each(begin(words_in_line), end(words_in_line), [&](string const & w) {nonspaces += w.length();});

        size_t diff = width - nonspaces;
        size_t div, rem;

        if (words_in_line.size() > 1)
        {
            div = diff / (words_in_line.size() -1);
            rem = diff % (words_in_line.size() -1);
            // each will have "div"
            for (size_t i = 0; i < div; ++i)
                for_each(words_in_line.begin(), words_in_line.end()-1, [&](string &w) { w.append(" ");});
            int state = 0; 
            int idx;
            int max_idx =  words_in_line.size() > 0 ? words_in_line.size() - 1 : 0;
            for (size_t j = 0; j < rem; ++j)
            {
                if (state == 0)
                {
                    idx = (j * max_idx) / rem;
                    state = 1;
                }
                else
                {
                    idx = ((rem - j) * max_idx) / rem;
                    state = 0;
                }
                words_in_line[idx].append(" ");
            }
        }
        else
        {
            div = diff;
            rem = 0;
            for (size_t i = 0; i < div; ++i)
                words_in_line[0].append(" ");
        }

        line.clear();
        for (auto const &w : words_in_line)
            line += w;

    }
    return line;
}

static vector<string> lbrk_fill_lines(string& str, size_t width)
{
    vector<string> words = str2vec(str);
    vector<string> out{};
    size_t current = 0;
    string line{};
    for_each(begin(words), end(words), [&](string const &w)
    {
        if  (current + w.length() <= width)
        {
            line.append(w);
            current += w.length();
        }
        else
        {
            rtrim(line);
            out.push_back(line);
            line.clear();
            line.append(w);
            current = w.length();
        }
        if (current < width)
        {
            line.append(" ");
            current++;
        }
    });
    // Append what's left on "line"
    out.push_back(line);
    return out;
}

int main(int argc, char* argv[])
{
    int c;
    size_t width = 80; // default
    char *file = NULL;

    // Parse options
    while (( c = getopt(argc, argv, "dehlrtuw:")) != EOF)
    {
        switch (c)
        {
            case 'd':
                g_debug = 1;
                break;
            case 'e':
                g_endings = 1;
                break;
            case 'h':
                lbrk_usage(argv[0]);
                exit(0);
            case 'l':
                g_lowercase = 1;
                break;
            case 'r':
                g_break = 0;
                break;
            case 't':
                g_spaces = 1;
                break;
            case 'u':
                g_uppercase = 1;
                break;
            case 'w':
                width = atoi(optarg);
                break;
            default:
                lbrk_usage(argv[0]);
                exit(1);
        }
    }
    // Upper and lower not compatible...
    if (g_uppercase == 1 && g_lowercase == 1)
    {
        cerr << "Uppercase and lowercase switches enabled, not possible" << endl;
        exit(1);
    }
    if (optind + 1 == argc)
    {
        file = argv[optind];
    }
    else
    {
        DEBUG("Parsing from stdin\n");
        string line{};
        while (getline(cin, line))
        {
            // Preprocess the characters
            transform(begin(line), end(line), begin(line), lbrk_modify);
            cout << lbrk_pad_line(line, width) << endl;
        }
        exit(0);
    }

    ifstream ifs{file};
    string str{};

    // Open the file
    if (!ifs.is_open()) return -1;

    // Reserve some memory up-front
    ifs.seekg(0, ios::end);
    str.reserve(ifs.tellg());
    ifs.seekg(0, ios::beg);
    str.assign( istreambuf_iterator<char>(ifs), istreambuf_iterator<char>() );

    // Preprocess the characters:
    // Do the format preprocessing (uppers, lowers, etc)
    transform(begin(str), end(str), begin(str), lbrk_modify);

    vector<string> words = str2vec(str);
    vector<string> out = lbrk_fill_lines(str, width);

    /* At this point:
     *
     * => "words" contains all the input words
     * => "out"   contains the broken lines at the
     *            specified width
     */

    // Do the processing (if needed)
    for_each(begin(out), end(out), [&](string& line)
    {
        // The resulting output string shall be of exactly
        // length "width". This will be the printed line
        if (g_break == 0)
        {
            string out{line};
            out.reserve(width);
            // Pad & print
            cout << lbrk_pad_line(out, width) << endl;
        }
        else
        {
            // Just print as it is
            cout << line << endl;
        }
    });
    return 0;
}

