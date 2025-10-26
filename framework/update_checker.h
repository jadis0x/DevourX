#pragma once

enum class NotifyType
{
    Info,
    Warning,
    Error,
    Confirm
};

namespace UpdateChecker
{
    void CheckAndNotify();
}