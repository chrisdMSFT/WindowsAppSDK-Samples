// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include "WindowsMLShared.h"

using namespace winrt::Windows::Foundation;
using namespace WindowsML::Shared;

int wmain()
{
    winrt::init_apartment();

    try
    {
        try
        {
            std::cout << "Getting available providers..." << std::endl;
            auto catalog = winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderCatalog::GetDefault();
            auto providers = catalog.FindAllProviders();
            for (const auto& provider : providers)
            {
                std::wcout << L"Provider: " << provider.Name().c_str() << std::endl;
                try
                {
                    auto readyState = provider.ReadyState();
                    std::wcout << L"  Ready state: " << static_cast<int>(readyState) << std::endl;

                    // Only call EnsureReadyAsync if we allow downloads or if the provider is already ready
                    provider.EnsureReadyAsync().get();

                    provider.TryRegister();
                }
                catch (...)
                {
                    // Continue if provider fails to initialize
                }
            }
        }
        catch (std::exception const& ex)
        {
            std::wcout << L"Error: " << std::wstring(ex.what(), ex.what() + strlen(ex.what())).c_str() << std::endl;
        }
    }
    catch (...)
    {
        std::wcout << L"An unexpected error occurred." << std::endl;
        return -1;
    }

    return 0;
}
