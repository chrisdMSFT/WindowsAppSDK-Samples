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
#include <winml/Runtime.h>

int wmain(int argc, wchar_t* argv[]) noexcept
{
    std::ignore = argc;
    std::ignore = argv;

    try
    {
        // Initialize WinML runtime first. This will add the necessary package dependencies to the process, and initialize the OnnxRuntime.
        Microsoft::Windows::AI::MachineLearning::WinMLRuntime winmlRuntime;
        if (FAILED(winmlRuntime.GetHResult()))
        {
            std::cerr << "Failed to initialize WinML runtime: " << std::hex << winmlRuntime.GetHResult() << std::endl;
            return -1;
        }

        winrt::init_apartment();
        Ort::Env env(ORT_LOGGING_LEVEL_ERROR, "CppConsoleDesktop");

        // Use WinML to download and register Execution Providers
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

        std::vector<Ort::ConstEpDevice> devices = env.GetEpDevices();
        std::wcout << L"ONNX providers registered: \n";
        for (const Ort::ConstEpDevice& device : devices)
        {
            std::cout << device.EpName() << "\n";
        }
   }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << "\n";
        return -1;
    }

    return 0;
}
