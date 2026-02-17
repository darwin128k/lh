#include <iostream>
#include <lh/static_cast.h>
#include <lh/void_ptr.h>

using namespace std;

int main() {
    auto a = lh_static_cast(lh_void_ptr, 0);
    return 0;
}