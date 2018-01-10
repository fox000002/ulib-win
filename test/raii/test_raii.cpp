#include <cstdio>
#include <cstdlib>
#include <stdexcept> // std::runtime_error

namespace test1
{

class file {
public:
    file (const char* filename)
        : file_(std::fopen(filename, "w+"))
	{
        if (!file_)
		{
            throw std::runtime_error("file open failure");
        }
    }
 
    ~file()
	{
        if (std::fclose(file_))
		{
           // failed to flush latest changes.
           // handle it
        }
    }
 
    void write (const char* str)
	{
        if (EOF == std::fputs(str, file_))
		{
            throw std::runtime_error("file write failure");
        }
    }
 
private:
    std::FILE* file_;
 
    // prevent copying and assignment; not implemented
    file (const file &);
    file & operator= (const file &);
};

void go()
{
    file logfile("logfile.txt"); // open file (acquire resource)
    logfile.write("hello logfile!");
    // continue using logfile ...
    // throw exceptions or return without
    //  worrying about closing the log;
    // it is closed automatically when
    // logfile goes out of scope
}

}

namespace test2
{
#ifdef __GNUC__

#define RAII_VARIABLE(vartype,varname,initval,dtor) \
    void _dtor_ ## varname (vartype * v) { dtor(*v); } \
    vartype varname __attribute__((cleanup(_dtor_ ## varname))) = (initval)

	
void _dtor_logfile (FILE* * v) { fclose(*v); }	
	
void go() {
  //RAII_VARIABLE(FILE*, logfile, fopen("logfile2.txt", "w+"), fclose);
  FILE* logfile __attribute__((cleanup(_dtor_logfile))) = (fopen("logfile2.txt", "w+"));
  

  fputs("hello logfile!", logfile);
}

#else

void go()
{

}

#endif

}

int main()
{
	test1::go();
	
	
	test2::go();

    return 0;
}
