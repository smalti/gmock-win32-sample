#include "stdafx.h"
#include "win_runtime_scope.h"

#include <gmock/gmock.h>
#include <gmock-win32.h>

using namespace std;
using namespace testing;

using testing::Return;
using testing::Invoke;
using testing::_;

MOCK_STDCALL_FUNC(HRESULT, RoInitialize, RO_INIT_TYPE);
MOCK_STDCALL_FUNC(void, RoUninitialize);

struct WinRuntimeScopeFixture : public testing::Test
{
    void TearDown() override
    {
        VERIFY_AND_CLEAR_MODULE_FUNC_EXPECTATIONS(RoInitialize);
        VERIFY_AND_CLEAR_MODULE_FUNC_EXPECTATIONS(RoUninitialize);
    }
};

using WinRuntimeScopeTest    = WinRuntimeScopeFixture;

TEST_F(WinRuntimeScopeTest, SuccessInit)
{
    ON_MODULE_FUNC_CALL(RoInitialize, _).WillByDefault(Return(HRESULT{ S_OK }));
    EXPECT_MODULE_FUNC_CALL(RoUninitialize).Times(1);

    WinRuntimeScope roScope;
    ASSERT_EQ(roScope, S_OK);
}

TEST_F(WinRuntimeScopeTest, SuccessInit_WithInitType)
{
    ON_MODULE_FUNC_CALL(RoInitialize, _).WillByDefault(Return(HRESULT{ S_OK }));
    EXPECT_MODULE_FUNC_CALL(RoUninitialize).Times(1);

    MockFunction< OnWrongTID > wrongTIDMock;
    EXPECT_CALL(wrongTIDMock, Call(_)).Times(0);

    WinRuntimeScope roScope{ RO_INIT_MULTITHREADED, wrongTIDMock.AsStdFunction() };
    ASSERT_EQ(roScope, S_OK);
}

TEST_F(WinRuntimeScopeTest, RpcChangedMode)
{
    ON_MODULE_FUNC_CALL(RoInitialize, _).WillByDefault(Return(HRESULT{ RPC_E_CHANGED_MODE }));
    EXPECT_MODULE_FUNC_CALL(RoUninitialize).Times(0);

    WinRuntimeScope roScope;
    ASSERT_EQ(roScope, RPC_E_CHANGED_MODE);
}

TEST_F(WinRuntimeScopeTest, IfAnotherThread_DoNotUninitialize)
{
    ON_MODULE_FUNC_CALL(RoInitialize, _).WillByDefault(Return(HRESULT{ S_OK }));
    EXPECT_MODULE_FUNC_CALL(RoUninitialize).Times(0);

    MockFunction< OnWrongTID > wrongTIDMock;
    EXPECT_CALL(wrongTIDMock, Call(_)).Times(1);

    std::unique_ptr< WinRuntimeScope > roScope{
        new WinRuntimeScope{ RO_INIT_MULTITHREADED, wrongTIDMock.AsStdFunction() } };

    ASSERT_EQ(*roScope, S_OK);

    std::thread th{ [roScope = std::move(roScope)]{ } };

    if (th.joinable())
        th.join();
}
