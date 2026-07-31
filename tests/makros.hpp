#define TEST_EXCEPTION(expr, exceptionType) do {\
                                                \
bool isThrown = false;                          \
try {                                           \
    auto _ = expr;                              \
} catch(const exceptionType &) {                \
    isThrown = true;                            \
}                                               \
assert(isThrown);                               \
} while(0)                                      
