#include "stdafx.h"

#include <gmock/gmock.h>
#include <gmock-win32.h>

int main(int argc, char* argv[])
{
    const gmock_win32::init_scope gmockWin32{ };

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
