#include "ResNetModelHelper.hpp"
#include "winml/onnxruntime_c_api.h"

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <utility>
#include <vector>
#include <winrt/base.h>
#include <winrt/Microsoft.Windows.AI.MachineLearning.h>

#include <winml/onnxruntime_cxx_api.h>

int wmain(int argc, wchar_t* argv[]) noexcept
{
    std::ignore = argc;
    std::ignore = argv;

    try
    {
        winrt::init_apartment();
        Ort::Env env(ORT_LOGGING_LEVEL_ERROR, "CppConsoleDesktop");

        winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderCatalog catalog = 
            winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderCatalog::GetDefault();

            auto providers = catalog.FindAllProviders();
        for (const auto& provider : providers)
        {
            std::wcout << L"Provider    : " << std::wstring_view{provider.Name()} << L'\n';
            std::wcout << L" ReadyState : " << std::to_underlying(provider.ReadyState()) << L'\n';
            winrt::Windows::Foundation::IAsyncOperationWithProgress action = provider.EnsureReadyAsync();

            action.Progress([](const auto& sender, double progress) {
                std::wcout << L"  Progress  : " << progress << L"%\n";
            });

            action.get();
            provider.TryRegister();
        }

    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << "\n";
        return -1;
    }

    return 0;
}
