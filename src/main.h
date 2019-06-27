#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

void panic(const char* msg);
void my_assert(const char *f, int l, bool x);


#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
