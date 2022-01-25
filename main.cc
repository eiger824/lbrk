#include <iostream>
#include <string>
#include <fstream>
#include <cstring>

#include <getopt.h>

#include "lbrk.hh"

using namespace std;

int main(int argc, char* argv[])
{
    int c;
    int err;

    lbrk *o = new lbrk();
    lbrk::lbrk_just_t policy;

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
                policy = o->lbrk_just_from_string(optarg);
                if (policy == lbrk::LBRK_JUST_UNKNOWN)
                {
                    cerr << "Invalid justification mode \"" << optarg << "\"." << endl;
                    o->lbrk_usage(argv[0]);
                    exit(1);
                }
                o->lbrk_set_just(policy);
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
    if (optind < argc)
    {
        for (int i = optind; i < argc; ++i)
        {
            if (!(optind + 1 == argc))
                cout << "File: " << argv[i] << endl;
            ifstream ifs { argv[i] };
            err = o->lbrk_core( ifs );
        }
    }
    else
    {
        err = o->lbrk_core( cin );
    }

    // Free up the used object
    delete o;

    return err;
}

