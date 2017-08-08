#include <cstdio>
#include <cstdlib>
#include <cstring>

////////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

////////////////////////////////////////////////////////////////////////////////

#define STR_VERSION         "0.1.0.0"

////////////////////////////////////////////////////////////////////////////////

string str_me;
string opt_srcfile;
string opt_dstfile;
string opt_dstext = ".c";
string opt_symbol;
string opt_structtype = "/* structure type here */";
bool   opt_dataword = false;
bool   opt_convheader = false;

////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    unsigned short address;
    unsigned char  data1;
    unsigned char  data2;
}ssetitem;

vector< ssetitem > ssets;

////////////////////////////////////////////////////////////////////////////////

string trim(string& s,const string& drop)
{
    string r=s.erase(s.find_last_not_of(drop)+1);
    return r.erase(0,r.find_first_not_of(drop));
}

string rtrim(string s,const string& drop)
{
    string r=s.erase(s.find_last_not_of(drop)+1);
    return r;
}

string ltrim(string s,const string& drop)
{
    string r=s.erase(0,s.find_first_not_of(drop));
    return r;
}

int fromhexstr( string &src )
{
    const char* strptr = src.data();

    int retv = 0;

    sscanf( strptr, "0x%x", &retv );

    return retv;
}

int paramcheck( int argc, char** argv )
{
    str_me = argv[0];

    size_t epos = str_me.find_last_of("\\");
    if ( epos == string::npos )
    {
        epos = str_me.find_last_of("/");
    }

    if ( epos != string::npos )
    {
        str_me = str_me.substr( epos + 1 );
    }

    if ( argc > 1 )
    {
        int cnt = 0;

        for( cnt=1; cnt<argc; cnt++ )
        {
            string tmp = argv[ cnt ];

            if ( tmp.size() > 0 )
            {
                if ( ( tmp == "-h" ) || ( tmp == "-H" ) )
                {
                    opt_convheader = true;
                    opt_dstext = ".h";
                }
                else
                if ( ( tmp == "-w" ) || ( tmp == "-W" ) )
                {
                    opt_dataword = true;
                }
                else
                if ( ( tmp.find( "-s=" ) == 0 ) ||
                     ( tmp.find( "-S=" ) == 0 ) )
                {
                    opt_symbol = tmp.substr( 3 );
                }
                else
                if ( ( tmp.find( "-t=" ) == 0 ) ||
                     ( tmp.find( "-T=" ) == 0 ) )
                {
                    opt_structtype = tmp.substr( 3 );
                }
                else
                if ( opt_srcfile.size() == 0 )
                {
                    opt_srcfile = tmp;
                }
                else
                if ( opt_dstfile.size() == 0 )
                {
                    opt_dstfile = tmp;
                }
            }
        }

        if ( opt_srcfile.size() > 0 )
        {
            if ( opt_dstfile.size() == 0 )
            {
                // find ext.
                size_t extpos = opt_srcfile.find_last_of( '.' );
                if ( extpos != string::npos )
                {
                    opt_dstfile = opt_srcfile.substr( 0, extpos );
                }
                opt_dstfile += opt_dstext;
            }
        }

        if ( opt_symbol.size() == 0 )
        {
            if ( opt_convheader == true )
            {
                opt_symbol = "SSET2C_CONVERT_DATA";
            }
            else
            {
                opt_symbol = "sset2c_convert_data";
            }
        }

        return cnt;
    }

    return 0;
}

void printhelp()
{
    printf( "\n" );
    printf( " usages :\n" );
    printf( "\t%s [source file] (options) (output file)\n", str_me.c_str() );
    printf( "\n" );
    printf( " options :\n" );
    printf( "\t-h(-H) : Convert sset to C language header type\n" );
    printf( "\t         Default is C language (.c)\n" );
    printf( "\t-w(-W) : Convert in 2 bytes (word) type instead single byte.\n" );
    printf( "\t         Default is using single byte.\n" );
    printf( "\t-s(-S)=[symbol name] : Using symbol name\n" );
    printf( "\t                       If it ignored, randomly generates.\n" );
    printf( "\t-t(-T)=[structure name] : Using structure name\n" );
    printf( "\t                          Default is a comment : /*struct type here*/\n" );
    printf( "\n" );
}

int convertingwork()
{
    ifstream sfile( opt_srcfile.c_str() );

    if (  sfile.is_open() == true )
    {
        printf( "Reading ...\n" );

        string aline;

        while( getline( sfile, aline ) )
        {
            if ( aline.size() > 5 )
            {
                string tmpline = trim( aline, " \t\r\n" );

                // check comment ....
                size_t fpos = tmpline.find( "//" );
                if ( fpos != 0 )
                {
                    const char* strptr = tmpline.data();
                    // check starts with 's' or 'S'.
                    if ( ( strptr[0] == 's' ) || ( strptr[0] == 'S' ) )
                    {
                        ssetitem sitem = {0};
                        string s_addr = "0x" + tmpline.substr(1,4);
                        string s_dat1 = "0x" + tmpline.substr(5,2);
                        string s_dat2 = "0x";

                        if ( aline.size() > 7 )
                        {
                            s_dat2 += tmpline.substr(7,2);
                        }

                        if ( s_addr.size() < 3 ) s_addr = "0x0000";
                        if ( s_dat1.size() < 3 ) s_dat1 = "0x00";
                        if ( s_dat2.size() < 3 ) s_dat2 = "0x00";

                        sitem.address = (unsigned short)fromhexstr( s_addr );
                        sitem.data1   = (unsigned char)fromhexstr( s_dat1 );
                        sitem.data2   = (unsigned char)fromhexstr( s_dat2 );

                        ssets.push_back( sitem );
                    }
                }
            }
        }

        sfile.close();

        printf( "Total counts = %d\n", (int)ssets.size() );

        ofstream dfile( opt_dstfile.c_str() );

        if( dfile.is_open() == true )
        {
            printf( "Writing file to %s ...\n",
                    opt_dstfile.c_str() );

            string codehdr;
            string codeend;

            if ( opt_convheader == true )
            {
                string opthdr;

                opthdr  = "__";
                opthdr += opt_symbol;
                opthdr += "_H__";

                codehdr  = "#ifndef ";
                codehdr += opthdr;
                codehdr += "\n";
                codehdr += "#define ";
                codehdr += opthdr;
                codehdr += "\n\n";
                codehdr += "#define ";
                codehdr += opt_symbol;
                codehdr += " \\\n";
                codehdr += "{ \\\n";

                codeend  = "}\n\n";
                codeend += "#endif";
                codeend += "  ////// of ";
                codeend += opthdr;
            }
            else
            {
                codehdr  = "static struct /*struct type here*/ ";
                codehdr += opt_symbol;
                codehdr += "[] = \n";
                codehdr += "{\n";

                codeend  = "};\n";
            }

            dfile.write( codehdr.c_str(), codehdr.size() );

            char tmpbuff[512] = {0};

            for( size_t cnt=0; cnt<ssets.size(); cnt++ )
            {
                if ( opt_dataword == true )
                {
                    sprintf( tmpbuff,
                            "\t{ 0x%04X, 0x%02X, 0x%02X }, ",
                            ssets[cnt].address,
                            ssets[cnt].data1,
                            ssets[cnt].data2 );
                }
                else
                {
                    sprintf( tmpbuff,
                            "\t{ 0x%04X, 0x%02X }, ",
                            ssets[cnt].address,
                            ssets[cnt].data1 );
                }

                if ( opt_convheader == true )
                {
                    strcat( tmpbuff, " \\\n" );
                }
                else
                {
                    strcat( tmpbuff, "\n" );
                }

                dfile.write( tmpbuff, strlen( tmpbuff ) );
            }

            dfile.write( codeend.c_str(), codeend.size() );
            dfile.close();

            printf( "Completed.\n" );
        }
        else
        {
            printf( "Error: cannot create new file : %s\n",
                    opt_dstfile.c_str() );

        }
    }
    else
    {
        printf( "Error : file open failure = %s\n", opt_srcfile.c_str() );

        return -1;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main( int argc, char** argv )
{
    int retparams = paramcheck( argc, argv );

    printf( "%s, sset to c/h converter, rageworx@gmail.com, version %s\n",
            str_me.c_str(),
            STR_VERSION );

    if ( retparams == 0 )
    {
        printhelp();
        return 0;
    }

    printf( "\n" );
    printf( " - source file : %s\n", opt_srcfile.c_str() );
    printf( " - output file : %s\n", opt_dstfile.c_str() );
    printf( " - symbol name : %s\n", opt_symbol.c_str() );

    if ( opt_convheader == true )
    {
        printf( " - converting type : clang Header\n" );
    }
    else
    {
        printf( " - converting type : clang code\n" );
    }

    if ( opt_dataword == true )
    {
        printf( " - converting byte : 2 \n" );
    }
    else
    {
        printf( " - converting byte : 1 \n" );
    }

    printf( "\n" );

    convertingwork();

    return 0;
}
