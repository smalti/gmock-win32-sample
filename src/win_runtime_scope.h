#pragma once

using OnWrongTID = std::function< void(DWORD) >;

struct WinRuntimeScope final
{
    WinRuntimeScope(const WinRuntimeScope&) = delete;
    WinRuntimeScope& operator = (const WinRuntimeScope&) = delete;

public:
    explicit WinRuntimeScope(
        RO_INIT_TYPE flags      = RO_INIT_SINGLETHREADED,
        OnWrongTID   onWrongTID = { }
    ) noexcept;

    ~WinRuntimeScope();
    operator HRESULT() const noexcept;

private:
    const HRESULT     hr;
    const DWORD       tid;
    const OnWrongTID  onWrongTID;
};
