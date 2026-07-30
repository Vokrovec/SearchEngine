#define TEST_EXCEPTION(expr, exceptionType) do {    \
                                                \
bool isThrown = false;                          \
try {                                           \
    bool val = expr;                            \
} catch(const exceptionType &) {                \
    isThrown = true;                            \
}                                               \
assert(isThrown);                               \
} while(0)                                      
