#include "pch.h"
#include "CppUnitTest.h"
#include "Application/Application.h"
#include "Render/Renderer.h"
#include "Platform/API.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace OnYuuPerfTests
{
    TEST_CLASS(VulkanUpdateAllDescriptorSetsPerfTest)
    {
    private:
        std::unique_ptr<OnYuu::Application> app_;

    public:
        TEST_METHOD_INITIALIZE(Initialize)
        {
            app_ = std::make_unique<OnYuu::Application>(OnYuu::API::Vulkan);
        }

        TEST_METHOD_CLEANUP(Cleanup)
        {
            if (app_)
            {
                app_->Shutdown();
                app_.reset();
            }
        }

        TEST_METHOD(ProfileUpdateAllDescriptorSetsPath)
        {
            auto* renderer = OnYuu::Render::getInstance();
            Assert::IsNotNull(renderer, L"Renderer non inizializzato");

            for (int i = 0; i < 240; ++i)
            {
                app_->getWindow()->beginFrame();
                renderer->BeginFrame();
                renderer->submit();
                app_->getWindow()->draw();
            }
        }
    };
}
