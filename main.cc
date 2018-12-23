#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <cstring>
#include <iterator>
#include <fstream>
#include <vector>

#include <libgen.h>
#include <getopt.h>

#include "lbrk.hh"

using namespace std;

int main(int argc, char* argv[])
{
    int c;
    int err;

    lbrk *o = new lbrk();

    // Parse options
    while (( c = getopt(argc, argv, "dehj:lrtuw:")) != EOF)
    {
        switch (c)
        {
            case 'd':
                o->lbrk_set_debug(true);
                break;
            case 'e':
                o->lbrk_set_endings(true);
                break;
            case 'h':
                o->lbrk_usage(argv[0]);
                exit(0);
            case 'j':
                if (strcmp(optarg, "left") != 0 &&
                        strcmp(optarg, "right") != 0 &&
                        strcmp(optarg, "center") != 0)
                {
                    cerr << "Invalid justification mode \"" << optarg << "\"." << endl;
                    o->lbrk_usage(argv[0]);
                    exit(1);
                }
                o->lbrk_set_just(o->lbrk_just_from_string(optarg));
                break;
            case 'l':
                o->lbrk_set_lower(true);
                break;
            case 'r':
                o->lbrk_set_breaks(false);
                break;
            case 't':
                o->lbrk_set_tab(true);
                break;
            case 'u':
                o->lbrk_set_upper(true);
                break;
            case 'w':
                o->lbrk_set_width(atoi(optarg));
                break;
            default:
                o->lbrk_usage(argv[0]);
                exit(1);
        }
    }
    string reason{"Success"};
    if (!o->lbrk_sanity_check(reason))
    {
        cerr << reason << endl;
        exit(1);
    }
    if (optind + 1 == argc)
    {
        err = o->lbrk_from_file( argv[optind] );
    }
    else
    {
        err = o->lbrk_from_stdin();
    }

    // Free up the used object
    delete[] o;

    return err;
}

