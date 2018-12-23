/*
 * Filename:      lbrk.h
 *
 * Author:        Santiago Pagola <santipagola@gmail.com>
 * Brief:         Line-breaker (lbrk) - A simple tool to break text lines in a visually nice way
 *                Header file
 * Last modified: Sun Dec 23 10:59:05 CET 2018
 */

#ifndef LBRK_H_
#define LBRK_H_

using namespace std;

/********************** Macros *************************/
#define     NL       '\n'
#define     NLSTR    "\n"
#define     TAB      '\t'
#define     SPACE    ' '
#define     SPACESTR " "
#define     ZERO     '\0'
#define     DELIM    "<|"
#define     DEBUG(...) if (m_dbg) fprintf(stderr, __VA_ARGS__)
#define     LBRK_DEFAULT_WIDTH  80
/*******************************************************/

class lbrk
{
    public:
        /******************** Definitions **********************/
        enum justification_policy 
        {
            LBRK_JUST_LEFT,
            LBRK_JUST_CENTER,
            LBRK_JUST_RIGHT,
            LBRK_JUST_UNKNOWN
        };
        using lbrk_just_t = justification_policy;

        /*******************************************************/
        /********************* Constructors ********************/
        lbrk();
        lbrk(lbrk const & rhs);
        lbrk(lbrk &&rhs) = default;
        /*******************************************************/
        /********************** Functions **********************/
        /*
         * Function: lbrk_print_vector
         * Brief:    Given a vector, it prints its elements
         * @param v: The vector to print
         * Returns:  Nothing
         */
        inline void lbrk_print_vector(vector<string> const & v);

        /*
         * Function:        lbrk_usage
         * Brief:           Prints the usage of the program
         * @param progname: argv[0], i.e., the name of the program
         * Returns:         Nothing
         */
        void lbrk_usage(char * progname);

        /*
         * Function:   lbrk_str2vec
         * Brief:      Given a string, returns a string vector.
         *             The split character is always whitespace
         * @param str: The string to convert
         * Returns:    A vector<string> object
         */
        vector<string> lbrk_str2vec(string & str);

        /*
         * Function:   lbrk_vec2str
         * Brief:      Given a vector of strings and a separator, it composes a string
         * @param vec: The vector string to transform
         * @param sep: The separator character to use
         * Returns:    A string object
         */
        string lbrk_vec2str(vector<string> & vec, char sep);

        /*
         * Functions: lbrk_[rl]trim
         * Brief:     Given a string, these functions trim leading (ltrim) or trailing
         *            (rtrim) whitespaces from it
         * @param s:  Reference to the string to transform
         * Returns:   A reference to the transformed string
         */
        inline string& lbrk_ltrim(string &s);
        inline string& lbrk_rtrim(string &s);

        /*
         * Function:     lbrk_pad_line
         * Brief:        Given a string and a width, it adjusts this string to fit width
         *               characters depending on the justification policy given.
         * @param line:  The string line to pad
         * Returns:      A reference to the modified string
         */
        string & lbrk_pad_line(string & line);


        vector<string> lbrk_adjust_line(string const& line, size_t width);
        vector<string> lbrk_fill_lines(string& str, size_t width);
        vector<string> lbrk_fill_to_width(string& str, size_t width);

        int lbrk_from_file(const char* path);
        int lbrk_from_stdin();
        lbrk_just_t lbrk_just_from_string(const char* str);

        bool lbrk_sanity_check(string &reason);

        inline void lbrk_set_debug(bool d) { m_dbg = d; }
        inline void lbrk_set_tab(bool t) {m_tab_policy = t;}
        inline void lbrk_set_upper(bool u) {m_upper = u;}
        inline void lbrk_set_lower(bool l) {m_lower = l;}
        inline void lbrk_set_just(lbrk::lbrk_just_t pol) { m_just_policy = pol; }
        inline void lbrk_set_width(size_t w) { m_width = w; }
        inline void lbrk_set_endings(bool e) { m_endings = e; }
        inline void lbrk_set_breaks(bool b) { m_break_policy = b; }

        inline bool dbg() const { return m_dbg; }
        inline size_t width() const { return m_width; }
        inline bool upper() const { return m_upper; }
        inline bool lower() const { return m_lower; }
        inline bool tab_policy() const { return m_tab_policy; }
        inline lbrk::lbrk_just_t just_policy() const { return m_just_policy; }
        inline bool endings() const { return m_endings; }
        inline bool break_policy() const { return m_break_policy; }

        /*******************************************************/
    private:
        lbrk_just_t m_just_policy {LBRK_JUST_UNKNOWN};
        size_t m_width      {LBRK_DEFAULT_WIDTH};
        bool m_dbg          {false};
        bool m_upper        {false};
        bool m_lower        {false};
        bool m_tab_policy   {false};
        bool m_endings      {false};
        bool m_break_policy {true};
};
#endif /* LBRK_H_ */
