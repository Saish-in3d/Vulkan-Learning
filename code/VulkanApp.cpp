#include "VulkanApp.h"

struct Vertex
{
	float pos[2];
	float color[3];
};

static const std::vector<Vertex> Vertices =
{
	{{0.0f,-0.5f}, {1.0f,0.0f,0.0f,} },
	{{0.5f,0.5f}, {0.0f,1.0f,0.0f,} },
	{{-0.5f,0.5f}, {0.0f,0.0f,1.0f,} },
};



namespace VKFW
{

	void VulkanApp::run()
	{
		initWindow();
		initVulkan();
		createSyncObjects();
		mainLoop();
		cleanup();

	}

	void VulkanApp::initWindow()
	{
		mwindow = MakeSharedRef<Platform::Window>(width, height);

		mwindow->setOnResize(
			[this](int w, int h )
			{
				this->onResize(w, h);

			});

	}

	void VKFW::VulkanApp::initVulkan()
	{

		mInstance = MakeSharedRef<vulkancore::Instance>(bool(true));
		mSurface = MakeSharedRef<vulkancore::WindowSurface>(mInstance,mwindow);

		mDevice = MakeSharedRef<vulkancore::Device>(mInstance,mSurface);
		mCommandPool = MakeSharedRef<vulkancore::CommandPool>(mDevice);

		mSwapChain = MakeSharedRef<vulkancore::SwapChain>(mDevice,mwindow,mSurface);

		mRenderPass = factories::RenderPassFactory::CreateSimpleRenderPass(mDevice,mSwapChain->getSwapChainImageFormat());
		mSwapChain->createFrameBuffers(mRenderPass);

		mUniformManager = MakeSharedRef<renderer::UniformManager>();
		mUniformManager->init(mDevice, mSwapChain->getImageCount());
		mUniformManager->attachCameraUBO();
		mUniformManager->attachObjectUBO();
		mUniformManager->build();



		mVertexBuffer = vulkancore::DataBuffer::createVertexBuffer(mDevice, sizeof(Vertex) * Vertices.size(), (void*)Vertices.data());

		mPipeline = createPipeline();
		recordCommandBuffers();

		

	}

	void VKFW::VulkanApp::mainLoop()
	{
		while (!mwindow->shouldClose())
		{
			mwindow->poolEvents();
			float dt = getFrameTime();
			updateUBO(dt);
			drawFrame();
		}
		vkDeviceWaitIdle(mDevice->getDevice());
	}

	void VKFW::VulkanApp::cleanup()
	{
		mRenderPass.reset();    
		mSwapChain.reset();
		mCommandPool.reset();
		mDevice.reset();
		mSurface.reset();
		mInstance.reset();
		mwindow.reset();
	}


	Sharedref<vulkancore::Pipeline> VulkanApp::createPipeline()
	{
		auto pipeline = MakeSharedRef<vulkancore::Pipeline>(mDevice, mRenderPass);

		// === Shaders ===
		// spv files compiled by CMake and copied to exe/spv/
		// entry point is "main" — Slang renames vertMain/fragMain to main in SPIR-V
		const auto shaderDir = exeDir() / "spv";

		auto vertShader = MakeSharedRef<vulkancore::Shader>(
			mDevice, (shaderDir / "test_vert.spv").string(),
			VK_SHADER_STAGE_VERTEX_BIT, "main");

		auto fragShader = MakeSharedRef<vulkancore::Shader>(
			mDevice, (shaderDir / "test_frag.spv").string(),
			VK_SHADER_STAGE_FRAGMENT_BIT, "main");

		pipeline->setShaderGroup({ vertShader, fragShader });


		VkVertexInputBindingDescription binding{};
		binding.binding = 0;
		binding.stride = sizeof(Vertex);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		
		VkVertexInputAttributeDescription posAttr{};
		posAttr.binding = 0;
		posAttr.location = 0;
		posAttr.format = VK_FORMAT_R32G32_SFLOAT;
		posAttr.offset = 0;

		VkVertexInputAttributeDescription  colirAttr{};
		colirAttr.binding = 0;
		colirAttr.location = 1;
		colirAttr.format = VK_FORMAT_R32G32B32_SFLOAT;
		colirAttr.offset = offsetof(Vertex,color);

		std::vector<VkVertexInputBindingDescription> bindings = { binding };

		std::vector<VkVertexInputAttributeDescription> attributes = { posAttr,colirAttr };


		//VBO
		pipeline->mVertexInputState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		pipeline->mVertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
		pipeline->mVertexInputState.pVertexBindingDescriptions = bindings.data();
		pipeline->mVertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
		pipeline->mVertexInputState.pVertexAttributeDescriptions = attributes.data();

		// Configuring InputAssembly: every 3 vertices form one triangle
		pipeline->mAssemblyState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipeline->mAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipeline->mAssemblyState.primitiveRestartEnable = VK_FALSE;

		// === Chapter 3: Dynamic state + Viewport ===
		// viewport and scissor are dynamic — set per frame in CommandBuffer
		// means we don't bake resolution into the pipeline
		pipeline->mDynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		pipeline->mDynamicState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipeline->mDynamicState.dynamicStateCount =
			(uint32_t)pipeline->mDynamicStates.size();
		pipeline->mDynamicState.pDynamicStates = pipeline->mDynamicStates.data();

		// count = 1, actual values set dynamically in cmd buffer
		pipeline->mViewportState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		pipeline->mViewportState.viewportCount = 1;
		pipeline->mViewportState.scissorCount = 1;

		// === Chapter 4: Rasterizer ===
		pipeline->mRasterState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		pipeline->mRasterState.depthClampEnable = VK_FALSE;
		pipeline->mRasterState.rasterizerDiscardEnable = VK_FALSE;
		pipeline->mRasterState.polygonMode = VK_POLYGON_MODE_FILL;
		pipeline->mRasterState.lineWidth = 1.0f;
		pipeline->mRasterState.cullMode = VK_CULL_MODE_BACK_BIT;
		pipeline->mRasterState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		pipeline->mRasterState.depthBiasEnable = VK_FALSE;

		// === Chapter 5: Multisample — disabled, Part 2 enables MSAA ===
		pipeline->mMultisampleState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipeline->mMultisampleState.sampleShadingEnable = VK_FALSE;
		pipeline->mMultisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		// === Chapter 6: ColorBlend — no blending, output replaces ===
		// TODO L14: enable blending for transparent materials
		VkPipelineColorBlendAttachmentState blendAttachment{};
		blendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		blendAttachment.blendEnable = VK_FALSE;
		pipeline->pushBlendAttachment(blendAttachment);

		pipeline->mBlendState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pipeline->mBlendState.logicOpEnable = VK_FALSE;
		pipeline->mBlendState.attachmentCount =
			(uint32_t)pipeline->mBlendAttachmentStates.size();
		pipeline->mBlendState.pAttachments =
			pipeline->mBlendAttachmentStates.data();

		// === Chapter 7: DepthStencil — disabled, no depth buffer yet ===
		// TODO L13: enable depth test and depth write
		pipeline->mDepthStencilState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		pipeline->mDepthStencilState.depthTestEnable = VK_FALSE;
		pipeline->mDepthStencilState.depthWriteEnable = VK_FALSE;

		// === Chapter 8: PipelineLayout — empty, no descriptor sets, no push constants ===
		// TODO L12: add descriptor set layouts for UBO
		static VkDescriptorSetLayout descriptorlayouts[] = {
			mUniformManager->getLayout()->getLayout()

		};

		pipeline->mPipelineLayoutInfo.sType =
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline->mPipelineLayoutInfo.setLayoutCount = 1;
		pipeline->mPipelineLayoutInfo.pSetLayouts = descriptorlayouts;

		
		/*pipeline->mPipelineLayoutInfo.setLayoutCount = 0;
		pipeline->mPipelineLayoutInfo.pushConstantRangeCount = 0;*/

		pipeline->build();
		return pipeline;
	}

	void VulkanApp::drawFrame()
	{

		mFences[mCurrentFrame]->waitForFence();

		uint32_t imageIndex = 0;

		VkResult Result =  vkAcquireNextImageKHR(
			mDevice->getDevice(),
			mSwapChain->getSwapChain(),
			UINT64_MAX,
			mImageAvailableSemaphores[mCurrentFrame]->getSemaphore(),
			VK_NULL_HANDLE,
			&imageIndex);

		if (Result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to acquire next swapchain image");
		}

		mFences[mCurrentFrame]->resetFence();
		
		VkCommandBuffer cmd = mCommandBuffers[imageIndex]->getCommandBuffer();

		VkSemaphore waitSemaphores[] =  { mImageAvailableSemaphores[mCurrentFrame]->getSemaphore() };

		VkSemaphore signalSemaphores[] =  { mRenderFinishedSemaphores[mCurrentFrame]->getSemaphore() };

		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };


		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmd;
		submitInfo.pWaitDstStageMask = waitStages;

		Result = vkQueueSubmit(mDevice->getGraphicQueue(), 1, &submitInfo, mFences[mCurrentFrame]->getFence());
		if (Result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to vkQueueSubmit");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.swapchainCount = 1;
		VkSwapchainKHR sc = mSwapChain->getSwapChain();
		presentInfo.pSwapchains = &sc;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.pImageIndices = &imageIndex;
		
		Result = vkQueuePresentKHR(mDevice->getPresentQueue(), &presentInfo);
		if (Result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to vkQueuePresentKHR");
		}

		mCurrentFrame = (mCurrentFrame + 1) % mSwapChain->getImageCount();

		
	}

	void VulkanApp::recordCommandBuffers()
	{

		auto framebuffers = mSwapChain->getSwapChainFramebuffers();
		auto extent = mSwapChain->getSwapChainExtent();
		auto count = mSwapChain->getImageCount();

		mCommandBuffers.resize(count);
		for (uint32_t i = 0; i < count; i++)
		{
			mCommandBuffers[i] = MakeSharedRef<vulkancore::CommandBuffer>(mDevice, mCommandPool);

			//COMMANDbuffer recording starts here
			mCommandBuffers[i]->beginCommandBuffer();
			VkClearValue clearColor{};
			clearColor.color = { 0.0f, 0.2f,0.4f,1.0f };

			VkRenderPassBeginInfo rpInfo{};
			rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			rpInfo.renderPass = mRenderPass->getRenderPass();
			rpInfo.framebuffer = mSwapChain->getSwapChainFramebuffers()[i];
			rpInfo.renderArea.offset = { 0,0 };
			rpInfo.renderArea.extent = mSwapChain->getSwapChainExtent();
			rpInfo.clearValueCount = 1;
			rpInfo.pClearValues = &clearColor;

			
			mCommandBuffers[i]->beginRenderPass(rpInfo);

			//Binding graphics pipeline
			mCommandBuffers[i]->bindGraphicPipeline(mPipeline);

			mCommandBuffers[i]->bindDescriptorSets(mPipeline, mUniformManager->getDescriptorSet(i));
			

			//Binding vertiex buffer
			VkBuffer vertexBuffers[] = { mVertexBuffer->getBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(mCommandBuffers[i]->getCommandBuffer(), 0, 1, vertexBuffers,offsets);


			VkViewport viewport{};
			viewport.x = 0.f;
			viewport.y = static_cast<float>(extent.height); //(fLIPING THE VIEWPORT as per vulkan)
			viewport.width = static_cast<float>(extent.width);
			viewport.height = -static_cast<float>(extent.height);
			viewport.minDepth = 0.f;
			viewport.maxDepth = 0.f;
			vkCmdSetViewport(mCommandBuffers[i]->getCommandBuffer(), 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset = { 0,0 };
			scissor.extent = extent;

			vkCmdSetScissor(mCommandBuffers[i]->getCommandBuffer(), 0, 1, &scissor);

			mCommandBuffers[i]->draw(Vertices.size());
			mCommandBuffers[i]->endRenderPass();
			mCommandBuffers[i]->endCommandBuffer();
		}
		

	}


	void VulkanApp::onResize(int w, int h)
	{

		width = w;
		height = h;
		mFrameBufferResized = true;
		std::cout << "Resized" << w << "x" << h << "y"<<std::endl;
		//Vulkan implementation on resize

	}
	float VulkanApp::getFrameTime()
	{
		
		double currentTime = glfwGetTime();
		float deltaTime = static_cast<float>(currentTime - lastTime);
		//lastTime = currentTime;
		return deltaTime;

	}
	void VulkanApp::updateUBO(float dt)
	{
		mRotatiom = dt * glm::radians(45.f);
		ObjectUBO obj{};
		obj.model = glm::rotate(glm::mat4(1.f), mRotatiom, glm::vec3(0.f,0.f,1.f));

		mUniformManager->updateObjectUBO(obj,mCurrentFrame);

		CameraUBO cam{};
		cam.view = glm::lookAt(
			glm::vec3(0.0f, 0.0f, 2.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		float aspect = static_cast<float>(width) / height;
		cam.proj = glm::perspective(glm::radians(45.f),aspect,0.1f,100.f);

		mUniformManager->updateCameraUBO(cam, mCurrentFrame);

	}
	void VulkanApp::createSyncObjects()
	{
		const uint32_t count = mSwapChain->getImageCount();
		mImageAvailableSemaphores.clear();
		mRenderFinishedSemaphores.clear();
		mFences.clear();

		for (uint32_t i = 0; i < count; i++) {
			mImageAvailableSemaphores.push_back(
				MakeSharedRef<vulkancore::Semaphore>(mDevice));
			mRenderFinishedSemaphores.push_back(
				MakeSharedRef<vulkancore::Semaphore>(mDevice));
			mFences.push_back(
				MakeSharedRef<vulkancore::Fence>(mDevice, true));  // signaled=true to avoid initial deadlock
		}


	}
}
