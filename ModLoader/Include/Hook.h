#pragma once

#include <string>
#include <tuple>
#include <type_traits>
#include <mutex>

#include <MinHook.h>

#include <core/Scanner.h>
#include <kenshi/Kenshi.h>

struct HookReference
{
    uintptr_t offset;
    void* originalFunction;
    void* hookFunction;

    template <typename Fn>
    Fn* GetOriginal() const {
        return reinterpret_cast<Fn*>(originalFunction);
    }
};

struct HookBase {
    static uintptr_t moduleBase;
    static size_t moduleSize;

    HookReference reference;
    std::string hookName;
    const char* pattern;
    const char* mask;
    bool alwaysLoad;

    virtual ~HookBase() = default;
    virtual void CreateHook() = 0;
};

// Primary template (undefined)
template <typename>
struct function_traits;

// Specialization for function types
template <typename R, typename... Args>
struct function_traits<R(Args...)> {
    using return_type = R;
    using args_tuple = std::tuple<Args...>;
    using fn_type = R(Args...);
};

class MinHookManager {
public:
    static void Initialize() {
        static std::once_flag flag;
        std::call_once(flag, []() {
            MH_Initialize();
            });
    }
};

template <typename TReturn, typename... TArgs>
class Hook : public HookBase {
public:
    Hook(const char* _hookName, uintptr_t _offset, void* _hookFunction, bool _alwaysLoad = false)
    {
        hookName = _hookName;
        alwaysLoad = _alwaysLoad;

        reference.offset = _offset;
        reference.hookFunction = _hookFunction;
        reference.originalFunction = nullptr;
    };

    Hook(const char* _hookName, const char* _pattern, const char* _mask, void* _hookFunction, bool _alwaysLoad = false)
    {
        hookName = _hookName;
        alwaysLoad = _alwaysLoad;
        pattern = _pattern;
        mask = _mask;

        reference.offset = 0;
        reference.hookFunction = _hookFunction;
        reference.originalFunction = nullptr;
    };

    uintptr_t PatternScan(uintptr_t base, size_t size, const char* pattern, const char* mask)
    {
        size_t patternLen = strlen(mask);

        for (size_t i = 0; i < size - patternLen; i++) {
            bool found = true;
            for (size_t j = 0; j < patternLen; j++) {
                if (mask[j] == 'x' && pattern[j] != *(char*)(base + i + j)) {
                    found = false;
                    break;
                }
            }
            if (found) {
                return i;
            }
        }
        return 0;
    }

    void CreateHook()
    {
        if (moduleBase == 0)
        {
            GetModuleCodeRegion(Kenshi::GetKenshiVersion().GetBinaryName().c_str(), &moduleBase, &moduleSize);
        }

        if (reference.offset == 0 && pattern)
        {
            reference.offset = PatternScan(moduleBase, moduleSize, pattern, mask);
        }

        if (!reference.offset)
            return;

        void* targetAddr = (void*)(moduleBase + reference.offset);

        MinHookManager::Initialize();

        if (MH_CreateHook(targetAddr, reference.hookFunction, &reference.originalFunction) != MH_OK)
            return;

        MH_EnableHook(targetAddr);
    };
};
