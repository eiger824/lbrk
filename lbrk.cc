/*
 * Filename:      lbrk.h
 *
 * Author:        Santiago Pagola <santipagola@gmail.com>
 * Brief:         Line-breaker (lbrk) - A simple tool to break text lines in a visually nice way
 *                Implementation file
 * Last modified: Sun Dec 23 10:59:05 CET 2018
*/

#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <typeinfo>
#include <cstring>
#include <iterator>
#include <fstream>
#include <vector>

#include <libgen.h>

#include "lbrk.hh"

lbrk::lbrk() {}

lbrk::lbrk(lbrk const & rhs)
{
    m_width = rhs.width();
    m_dbg = rhs.dbg();
    m_upper = rhs.upper();
    m_lower = rhs.lower();
    m_tab_policy = rhs.tab_policy();
    m_just_policy = rhs.just_policy();
}

inline void lbrk::lbrk_print_vector(vector<string> const & v)
{
    copy(begin(v), end(v), ostream_iterator<string>(cout, NLSTR ));
}

void lbrk::lbrk_usage(char * progname)
{
    progname = basename(progname);
    cout
        << "Usage: " << progname << " [OPTIONS] [<filename>]\n"
        << "\nOPTIONS:\n"
        << " -d       Set debug\n"
        << " -e       Show line endings with a <| delimiter\n"
        << " -h       Show this help and exit\n"
        << " -j mode  Justify text to: left, right, center\n"
        << " -l       Convert text to lowercase\n"
        << " -r       Don't break words, respect them\n"
        << " -t       Convert tabs to spaces\n"
        << " -u       Convert text to uppercase\n"
        << " -w width Width of text after breaking lines\n"
        << endl;
}

// convert string to vector<string>
vector<string> lbrk::lbrk_str2vec(string & str)
{
    stringstream ss(str);
    istream_iterator<string> begin(ss);
    istream_iterator<string> end;
    vector<string> words_in_line(begin, end);
    return words_in_line;
}

// convert vector<string> to string
string lbrk::lbrk_vec2str(vector<string> & vec, char sep)
{
    ostringstream oss;
    string tmp{1, sep};
    copy(vec.begin(), vec.end()-1, ostream_iterator<string>(oss, tmp.data()));
    oss << vec.back();
    return oss.str();
}

// trim from start (in place)
inline std::string& lbrk::lbrk_ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
    return s;
}

// trim from end (in place)
inline std::string& lbrk::lbrk_rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
    return s;
}

string & lbrk::lbrk_pad_line(string & line)
{
    assert(line.length() <= m_width);
    // Pad the line according to width
    if (m_width - line.length() > 0)
    {
        size_t nonspaces = 0;
        vector<string> words_in_line = lbrk_str2vec(line);
        for_each(begin(words_in_line), end(words_in_line), [&](string const & w) {nonspaces += w.length();});

        size_t diff = m_width - nonspaces;
        size_t div, rem;
        
        switch (m_just_policy)
        {
            case LBRK_JUST_CENTER:
                {
                    if (words_in_line.size() > 1)
                    {
                        div = diff / (words_in_line.size() -1);
                        rem = diff % (words_in_line.size() -1);
                        // each will have "div"
                        for (size_t i = 0; i < div; ++i)
                            for_each(words_in_line.begin(), words_in_line.end()-1,
                                    [&](string &w) { w.append(SPACESTR);});

                        // for the remaining spaces, place them uniformly within the sentence
                        int state = 0;
                        int idx;
                        int max_idx =  words_in_line.size() > 0 ? words_in_line.size() - 1 : 0;
                        for (size_t j = 0; j < rem; ++j)
                        {
                            idx = (state == 0) ?
                                (j * max_idx) / rem :
                                ((rem - j) * max_idx) / rem;
                            words_in_line[idx].append(SPACESTR);
                            state = 1 - state;
                        }
                    }
                    else
                    {
                        words_in_line.front().append(string(diff, SPACE));
                    }
                    line.clear();
                    for (auto const &w : words_in_line)
                        line += w;
                }
                break;
            // For left & right justification, just [a|pre]pend lines with spaces
            case LBRK_JUST_LEFT:
                line.append(string(m_width - line.length(), SPACE));
                break;
            case LBRK_JUST_RIGHT:
                line.insert(0, string(m_width - line.length(), SPACE));
                break;
            default:
                DEBUG("Unknown justification policy, returning empty line\n");
                line.clear();
                break;
        }
    }
    return line;
}

vector<string> lbrk::lbrk_adjust_line(string const& line)
{
    size_t len = line.length();
    vector<string> out{};
    if (len <= m_width)
    {
        out.push_back(line);
    }
    else
    {
        string tmp{};
        for_each(begin(line), end(line), [&](char c)
        {
            if (tmp.length() == m_width)
            {
                out.push_back(lbrk_rtrim(tmp));
                tmp.clear();
            }
            tmp.append(string(1, c));
        }
        );
        out.push_back(lbrk_rtrim(tmp));
    }
    return out;
}
vector<string> lbrk::lbrk_fill_lines(string& str)
{
    vector<string> words = lbrk_str2vec(str);
    vector<string> out{};
    string line{};
    for_each(begin(words), end(words), [&](string const &w)
    {
    if (line.length() + w.length() <= m_width)
        {
            line.append(w);
        }
        else
        {
            lbrk_rtrim(line);
            // line can be greater than width (unbreakable long words)
            // then we need to force a break here
            vector<string> this_line = lbrk_adjust_line(line);
            for_each(begin(this_line), end(this_line), [&out](string const& w) {out.push_back(w);});
            line = w;
        }
        if (line.length() < m_width)
        {
            line.append(SPACESTR);
        }
    });
    if (!line.empty())
    {
        // Append what's left on "line"
        out.push_back(line);
    }
    return out;
}

vector<string> lbrk::lbrk_fill_to_width(string& str)
{
    vector<string> out{};
    string line{};
    size_t current = 0;
    for (auto const& c : str)
    {
        if (current == m_width)
        {
            out.push_back(line);
            line.clear();
            current = 0;
        }
        line.append(string(1, c));
        current++;
    }
    if (!line.empty())
    {
        // Append what's left on "line"
        out.push_back(line);
    }
    return out;
}

lbrk::lbrk_just_t lbrk::lbrk_just_from_string(const char* str)
{
    lbrk_just_t pol;
    if (!strcmp(str, "left"))
        pol = LBRK_JUST_LEFT;
    else if (!strcmp(str, "center"))
        pol = LBRK_JUST_CENTER;
    else if (!strcmp(str, "right"))
        pol = LBRK_JUST_RIGHT;
    else
        pol = LBRK_JUST_UNKNOWN;
    return pol;
}

int lbrk::lbrk_process_line(string & line)
{
    // Preprocess the characters:
    // Do the format preprocessing (uppers, lowers, etc)
    transform(begin(line), end(line), begin(line), [&] (char& c) -> char
    {
        if (isspace(c))
        {
            if (c == TAB && m_tab_policy)
                c = SPACE;
        }
        else
        {
            if (m_upper)
                c = toupper(c);
            if (m_lower)
                c = tolower(c);
        }
        return c;
    });

    vector<string> words = lbrk_str2vec(line);
    vector<string> out{};

    out = (!m_break_policy) ?
        lbrk_fill_lines(line) :
        lbrk_fill_to_width(line);

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
        if (!m_break_policy)
        {
            string out{};
            out.reserve(m_width);
            out = line;
            // Pad & print
            cout << lbrk_pad_line(out)
                 << (m_endings ? DELIM : "") 
                 << endl;
        }
        else
        {
            // Just print as it is
            cout << line << endl;
        }
    });
    return 0;
}

int lbrk::lbrk_core(istream & is)
{
    string str{};

    if (typeid(is) == typeid(ifstream))
    {
        ifstream& ifs = reinterpret_cast<ifstream&>(is);
        if (!ifs.is_open())
        {
            cerr << "Could not open file: " << strerror(errno) << endl;
            return 2;
        }

        // Reserve some memory up-front
        ifs.seekg(0, ios::end);
        str.reserve(ifs.tellg());
        ifs.seekg(0, ios::beg);
        str.assign( istreambuf_iterator<char>(ifs), istreambuf_iterator<char>() );

        // Process it
        lbrk_process_line(str);
    }
    else if (typeid(is) == typeid(cin))
    {
        DEBUG("Parsing from stdin\n");
        string line{};
        while (getline(cin, line))
        {
            lbrk_process_line(line);
        }
    }
    return 0;
}

bool lbrk::lbrk_sanity_check(string& reason)
{
    if (m_upper && m_lower)
    {
        reason = "Uppercase and lowercase switches enabled, not possible";
        return false;
    }
    // justification and g_break not compatible...
    if (m_just_policy != lbrk::LBRK_JUST_UNKNOWN && m_break_policy)
    {
        reason = "Justification mode cannot be mixed with breaking lines";
        return false;
    }
    // Set a valid default justification mode if words are to be respected 
    if (!m_break_policy && m_just_policy == lbrk::LBRK_JUST_UNKNOWN)
    {
        m_just_policy = LBRK_JUST_LEFT;
    }
    return true;
}
