#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <malloc.h>
#include "main.h"

typedef int __guard;

extern "C" {
int __cxa_guard_acquire(__guard*);
void __cxa_guard_release(__guard*);


char* __cxa_demangle(const char* mangled_name,
                     char* output_buffer,
                     size_t* length,
                     int* status)
{
    (void)mangled_name;
    (void)output_buffer;
    (void)length;
    (void)status;
    panic(__FUNCTION__);
    return NULL;
}

void __cxa_throw(void *thrown_exception, void *pvtinfo, void (*dest)(void *))
{
    (void)thrown_exception;
    (void)pvtinfo;
    (void)dest;
    panic(__FUNCTION__);
}

/* Called by __libc_init_array() */
void _init(void)
{
    /* empty */
}

/* Called by __libc_fini_array() */
void _fini(void)
{
    /* empty */
}

}

void* operator new(size_t s)
{
    void* p = malloc(s);
    if (p == NULL) {
        panic(__FUNCTION__);
    }
    return p;
}

void* operator new[](size_t s)
{
    void* p = malloc(s);
    if (p == NULL) {
        panic(__FUNCTION__);
    }
    return p;
}

void operator delete(void* p)
{
    free(p);
}

void operator delete[](void* p)
{
    free(p);
}

void __assert_func(const char *_file, int _line, const char *_func, const char *_expr )
{
    (void)_file;
    (void)_line;
    (void)_func;
    (void)_expr;
    panic(__FUNCTION__);
}

void abort(void)
{
    panic(__FUNCTION__);
    while (1) {
        ;
    }
}

void _exit(int code)
{
    (void)code;
    panic(__FUNCTION__);
    while(1);
}

namespace std {

void __throw_out_of_range(char const*)
{
    panic(__FUNCTION__);
}

void __throw_bad_alloc()
{
    panic(__FUNCTION__);
}

void __throw_bad_function_call(void)
{
    panic(__FUNCTION__);
}
} // namespace std
