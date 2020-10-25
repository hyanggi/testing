// This program does a compile-time check of the language standard.

#ifndef __cplusplus

#if !(__STDC__ == 1 && __STDC_VERSION__ >= 201710L)
#error Not C17 or higher
#endif
int main(void) { return 0; }

#else

#if !(__cplusplus >= 201703L)
#error Not C++17 or higher
#endif
int main() { return 0; }

#endif
