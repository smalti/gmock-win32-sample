#include "stdafx.h"
#include "win_runtime_scope.h"

WinRuntimeScope::WinRuntimeScope(
    RO_INIT_TYPE flags      /*= RO_INIT_SINGLETHREADED*/,
    OnWrongTID   onWrongTID /*= { }*/
) noexcept :
    hr          { ::RoInitialize(flags) },
    tid         { ::GetCurrentThreadId() },
    onWrongTID  { std::move(onWrongTID) }
{ }

WinRuntimeScope::~WinRuntimeScope()
{
    if (SUCCEEDED(hr))
    {
        const auto currentTid = ::GetCurrentThreadId();
        if (tid == currentTid)
        {
            ::RoUninitialize();
        }
        else
        {
            try
            {
                if (onWrongTID)
                    std::invoke(onWrongTID, currentTid);
            }
            catch (...)
            { }
        }
    }
}

WinRuntimeScope::operator HRESULT() const noexcept
{
    return hr;
}
