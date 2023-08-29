#pragma once

struct WinRuntimeScope final
{
    using WrongTID = std::function< void(DWORD) >;

public:
    explicit WinRuntimeScope(
        RO_INIT_TYPE flags      = RO_INIT_SINGLETHREADED,
        WrongTID     onWrongTID = { }
    ) noexcept :
        hr          { ::RoInitialize(flags) },
        tid         { ::GetCurrentThreadId() },
        onWrongTID  { std::move(onWrongTID) }
    { }

    ~WinRuntimeScope()
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
                try {
                    if (onWrongTID)
                        std::invoke(onWrongTID, currentTid);
                }
                catch (...)
                { }
            }
        }
    }

    operator HRESULT() const noexcept { return hr; }

    WinRuntimeScope(const WinRuntimeScope&) = delete;
    WinRuntimeScope& operator = (const WinRuntimeScope&) = delete;

private:
    const HRESULT   hr;
    const DWORD     tid;
    const WrongTID  onWrongTID;
};
