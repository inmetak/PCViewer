#include "IsoSurfRenderer.h"

char IsoSurfRenderer::vertPath[]= "shader/isoSurfVert.spv";
char IsoSurfRenderer::fragPath[]= "shader/isoSurfFrag.spv";
char IsoSurfRenderer::computePath[] = "shader/isoSurfComp.spv";
char IsoSurfRenderer::binaryComputePath[] = "shader/isoSurfBinComp.spv";

IsoSurfRenderer::IsoSurfRenderer(uint32_t height, uint32_t width, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue, VkDescriptorPool descriptorPool)
{
	imageHeight = 0;
	imageWidth = 0;
	image3dHeight = 0;
	image3dWidth = 0;
	image3dDepth = 0;
	this->device = device;
	this->physicalDevice = physicalDevice;
	this->commandPool = commandPool;
	this->queue = queue;
	this->descriptorPool = descriptorPool;
	imageMemory = VK_NULL_HANDLE;
	image =	VK_NULL_HANDLE;
	imageView =	VK_NULL_HANDLE;
	sampler = VK_NULL_HANDLE;
	image3dMemory = VK_NULL_HANDLE;
	descriptorSetLayout = VK_NULL_HANDLE,
	descriptorSet = VK_NULL_HANDLE;
	pipeline = VK_NULL_HANDLE;
	renderPass = VK_NULL_HANDLE; 
	pipelineLayout = VK_NULL_HANDLE;
	constantMemory = VK_NULL_HANDLE;
	vertexBuffer = VK_NULL_HANDLE;
	indexBuffer = VK_NULL_HANDLE;
	uniformBuffer = VK_NULL_HANDLE;
	commandBuffer = VK_NULL_HANDLE;
	prepareImageCommand = VK_NULL_HANDLE;
	frameBuffer = VK_NULL_HANDLE;
	imageDescriptorSet = VK_NULL_HANDLE;
	computePipeline = VK_NULL_HANDLE;
	computePipelineLayout = VK_NULL_HANDLE;
	computeDescriptorSetLayout = VK_NULL_HANDLE;
	binaryComputePipeline = VK_NULL_HANDLE;
	binaryComputePipelineLayout = VK_NULL_HANDLE;
	binaryComputeDescriptorSetLayout = VK_NULL_HANDLE;
	brushBuffer = VK_NULL_HANDLE;
	brushMemory = VK_NULL_HANDLE;
	brushByteSize = 0;
	binaryImage = VK_NULL_HANDLE;
	binaryImageView = VK_NULL_HANDLE;
	binaryImageMemory = VK_NULL_HANDLE;

	camPos = glm::vec3(2, 2, 2);
	lightDir = glm::vec3(-1, -1, -1);

	//setting up graphic resources
	
	createBuffer();
	createPipeline();
	createDescriptorSets();
	resize(width, height);

	
	const int w = 100, h = 5, de = 1;
	glm::vec4 d[w * h * de] = {};
	d[0] = glm::vec4(1, 0, 0, 1);
	d[1] = glm::vec4(1, 0, 0, 1);
	d[2] = glm::vec4(1, 0, 0, 1);
	d[3] = glm::vec4(1, 0, 0, 1);
	d[8] = glm::vec4(0, 1, 0, .5f);
	d[26] = glm::vec4(0, 0, 1, .1f);
	/*for (int i = 1; i < 27; i+=3) {
		d[4 * i] = i / 27.0f;
		d[4 * i + 1] = 1 - (i / 27.0f);
		d[4 * i + 2] = 0;
		d[4 * i + 3] = .1f;
	}*/
	//update3dImage(w, h, de, (float*)d);
	resizeBox(1.5f, 1, 1.5f);
}

IsoSurfRenderer::~IsoSurfRenderer()
{
	if (imageMemory) {
		vkFreeMemory(device, imageMemory, nullptr);
	}
	if (image) {
		vkDestroyImage(device, image, nullptr);
	}
	if (imageView) {
		vkDestroyImageView(device, imageView, nullptr);
	}
	if (sampler) {
		vkDestroySampler(device, sampler, nullptr);
	}
	if (image3dMemory) {
		vkFreeMemory(device, image3dMemory, nullptr);
	}
	for (int i = 0; i < image3d.size(); ++i) {
		if (image3d[i]) {
			vkDestroyImage(device, image3d[i], nullptr);
		}
		if (image3dView[i]) {
			vkDestroyImageView(device, image3dView[i], nullptr);
		}
		if (image3dSampler[i]) {
			vkDestroySampler(device, image3dSampler[i], nullptr);
		}
	}
	if (frameBuffer) {
		vkDestroyFramebuffer(device, frameBuffer, nullptr);
	}
	if (descriptorSetLayout) {
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
	}
	if (pipeline) {
		vkDestroyPipeline(device, pipeline, nullptr);
	}
	if (pipelineLayout) {
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	}
	if (renderPass) {
		vkDestroyRenderPass(device, renderPass, nullptr);
	}
	if (constantMemory) {
		vkFreeMemory(device, constantMemory, nullptr);
	}
	if (vertexBuffer) {
		vkDestroyBuffer(device, vertexBuffer, nullptr);
	}
	if (indexBuffer) {
		vkDestroyBuffer(device, indexBuffer, nullptr);
	}
	if (uniformBuffer) {
		vkDestroyBuffer(device, uniformBuffer, nullptr);
	}
	if (computePipeline) {
		vkDestroyPipeline(device, computePipeline, nullptr);
	}
	if (computePipelineLayout) {
		vkDestroyPipelineLayout(device, computePipelineLayout, nullptr);
	}
	if (computeDescriptorSetLayout) {
		vkDestroyDescriptorSetLayout(device, computeDescriptorSetLayout, nullptr);
	}
	if (binaryComputePipeline) {
		vkDestroyPipeline(device, binaryComputePipeline, nullptr);
	}
	if (binaryComputePipelineLayout) {
		vkDestroyPipelineLayout(device, binaryComputePipelineLayout, nullptr);
	}
	if (binaryComputeDescriptorSetLayout) {
		vkDestroyDescriptorSetLayout(device, binaryComputeDescriptorSetLayout, nullptr);
	}
	if (brushBuffer) {
		vkDestroyBuffer(device, brushBuffer, nullptr);
	}
	if (brushMemory) {
		vkFreeMemory(device, brushMemory, nullptr);
	}
	if (binaryImage) {
		vkDestroyImage(device, binaryImage, nullptr);
	}
	if (binaryImageMemory) {
		vkFreeMemory(device, binaryImageMemory, nullptr);
	}
	if (binaryImageView) {
		vkDestroyImageView(device, binaryImageView, nullptr);
	}
	for (auto& col : brushColors) {
		delete[] col.second;
	}
}

void IsoSurfRenderer::resize(uint32_t width, uint32_t height)
{
	if (imageWidth == width && imageHeight == height) {
		return;
	}
	imageWidth = width;
	imageHeight = height;
	
	check_vk_result(vkDeviceWaitIdle(device));

	if (image) {
		vkDestroyImage(device, image, nullptr);
	}
	if (imageView) {
		vkDestroyImageView(device, imageView, nullptr);
	}
	if (frameBuffer) {
		vkDestroyFramebuffer(device, frameBuffer, nullptr);
	}
	if (sampler) {
		vkDestroySampler(device, sampler, nullptr);
	}

	createImageResources();

	//transforming the image to the right format
	createPrepareImageCommandBuffer();
	VkResult err;

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &prepareImageCommand;

	err = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	check_vk_result(err);

	if (image3dSampler.size()) {
		updateCommandBuffer();
	}
}

void IsoSurfRenderer::resizeBox(float width, float height, float depth)
{
	boxWidth = width;
	boxHeight = height;
	boxDepth = depth;
	render();
}

void IsoSurfRenderer::update3dBinaryVolume(uint32_t width, uint32_t height, uint32_t depth, uint32_t amtOfAttributes, const std::vector<uint32_t>& densityAttributes, const std::vector<std::pair<float, float>>& densityAttributesMinMax, const glm::uvec3& positionIndices, std::vector<float*>& data, std::vector<uint32_t>& indices, std::vector<std::vector<std::pair<float,float>>>& brush)
{
	VkResult err;

	uint32_t required3dImages = densityAttributes.size();

	//TODO: change to non constant
	int w = SpacialData::rlatSize;
	int d = SpacialData::rlonSize;
	int h = SpacialData::altitudeSize + 22;	//the top 22 layer of the dataset are twice the size of the rest
	width = w;
	height = h;
	depth = d;

	image3dWidth = width;
	image3dHeight = height;
	image3dDepth = depth;

	//destroying old resources
	if (image3dMemory) {
		vkFreeMemory(device, image3dMemory, nullptr);
	}
	for (int i = 0; i < image3d.size(); ++i) {
		if (image3d[i]) {
			vkDestroyImage(device, image3d[i], nullptr);
			image3d[i] = VK_NULL_HANDLE;
		}
		if (image3dView[i]) {
			vkDestroyImageView(device, image3dView[i], nullptr);
			image3dView[i] = VK_NULL_HANDLE;
		}
	}
	image3d.clear();
	image3dView.clear();
	image3dOffsets.clear();
	for (int i = image3dSampler.size(); i < required3dImages; ++i) {
		image3dSampler.push_back({});
		VkUtil::createImageSampler(device,VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,VK_FILTER_NEAREST,1,1,&image3dSampler.back());
	}

	//creating new resources
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	uint32_t memoryTypeBits = 0;
	VkMemoryRequirements memRequirements;
	for (int i = 0; i < required3dImages; ++i) {
		image3d.push_back({});
		image3dView.push_back({});
		image3dOffsets.push_back(0);

		image3dOffsets[i] = allocInfo.allocationSize;
		VkUtil::create3dImage(device, image3dWidth, image3dHeight, image3dDepth, VK_FORMAT_R32_SFLOAT, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, &image3d[i]);

		vkGetImageMemoryRequirements(device, image3d[i], &memRequirements);

		allocInfo.allocationSize += memRequirements.size;
		memoryTypeBits |= memRequirements.memoryTypeBits;
	}

	allocInfo.memoryTypeIndex = VkUtil::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, 0);
	err = vkAllocateMemory(device, &allocInfo, nullptr, &image3dMemory);
	check_vk_result(err);
	VkCommandBuffer imageCommands;
	VkUtil::createCommandBuffer(device, commandPool, &imageCommands);
	VkClearColorValue clear = { 0,0,0,0 };
	VkImageSubresourceRange range = { VK_IMAGE_ASPECT_COLOR_BIT,0,1,0,1 };
	for (int i = 0; i < required3dImages; ++i) {
		vkBindImageMemory(device, image3d[i], image3dMemory, image3dOffsets[i]);

		VkUtil::create3dImageView(device, image3d[i], VK_FORMAT_R32_SFLOAT, 1, &image3dView[i]);

		VkUtil::transitionImageLayout(imageCommands, image3d[i], VK_FORMAT_R32_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		vkCmdClearColorImage(imageCommands, image3d[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear, 1, &range);
		VkUtil::transitionImageLayout(imageCommands, image3d[i], VK_FORMAT_R32_SFLOAT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
	}
	VkUtil::commitCommandBuffer(queue, imageCommands);
	err = vkQueueWaitIdle(queue);
	check_vk_result(err);
	vkFreeCommandBuffers(device, commandPool, 1, &imageCommands);

	//checking values in the first 3d image
	//float* im = new float[width * depth * height];
	//This was checked and is working!!!!!!!
	//VkUtil::downloadImageData(device, physicalDevice, commandPool, queue, image3d[4], width, height, depth, im, width * depth * height * sizeof(float));
	//uint32_t zeroCount = 0;
	//for (int i = 0; i < width * depth * height; ++i) {
	//	if (im[i] != 0) zeroCount++;
	//}

	//std::vector<VkDescriptorSetLayout> layouts;
	//layouts.push_back(descriptorSetLayout);
	//if (!descriptorSet) {
	//	VkUtil::createDescriptorSets(device, layouts, descriptorPool, &descriptorSet);
	//}
	//
	////creating the density images via the compute pipeline ----------------------------------------
	//VkBuffer infos;
	//VkDeviceMemory infosMem;
	//uint32_t infosByteSize = sizeof(ComputeInfos) + densityAttributes.size() * sizeof(float);
	//ComputeInfos* infoBytes = (ComputeInfos*)new char[infosByteSize];
	//VkUtil::createBuffer(device, infosByteSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, &infos);
	//allocInfo = {};
	//allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	//VkMemoryRequirements memReq = {};
	//vkGetBufferMemoryRequirements(device, infos, &memReq);
	//allocInfo.allocationSize = memReq.size;
	//allocInfo.memoryTypeIndex = VkUtil::findMemoryType(physicalDevice, memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	//vkAllocateMemory(device, &allocInfo, nullptr, &infosMem);
	//vkBindBufferMemory(device, infos, infosMem, 0);
	//
	////fill infoBytes and upload it
	//infoBytes->amtOfAttributes = amtOfAttributes;
	//infoBytes->amtOfDensityAttributes = densityAttributes.size();
	//infoBytes->amtOfIndices = amtOfIndices;
	//infoBytes->dimX = width;
	//infoBytes->dimY = height;
	//infoBytes->dimZ = depth;
	//infoBytes->xInd = positionIndices.x;
	//infoBytes->yInd = positionIndices.y;
	//infoBytes->zInd = positionIndices.z;
	//infoBytes->xMin = densityAttributesMinMax[positionIndices.x].first;
	//infoBytes->xMax = densityAttributesMinMax[positionIndices.x].second;
	//infoBytes->yMin = densityAttributesMinMax[positionIndices.y].first;
	//infoBytes->yMax = densityAttributesMinMax[positionIndices.y].second;
	//infoBytes->zMin = densityAttributesMinMax[positionIndices.z].first;
	//infoBytes->zMax = densityAttributesMinMax[positionIndices.z].second;
	//int* inf = (int*)(infoBytes + 1);
	//for (int i = 0; i < densityAttributes.size(); ++i) {
	//	inf[i] = densityAttributes[i];
	//	//inf[3 * i + 1] = densityAttributesMinMax[i].first;
	//	//inf[3 * i + 2] = densityAttributesMinMax[i].second;
	//}
	//PCUtil::numdump((int*)(infoBytes), densityAttributes.size() + 16);
	//VkUtil::uploadData(device, infosMem, 0, infosByteSize, infoBytes);
	//
	////create descriptor set and update all need things
	//VkDescriptorSet descSet;
	//std::vector<VkDescriptorSetLayout> sets;
	//sets.push_back(computeDescriptorSetLayout);
	//VkUtil::createDescriptorSets(device, sets, descriptorPool, &descSet);
	//VkUtil::updateDescriptorSet(device, infos, infosByteSize, 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descSet);
	//
	//std::vector<VkImageLayout> imageLayouts(required3dImages, VK_IMAGE_LAYOUT_GENERAL);
	//VkUtil::updateStorageImageArrayDescriptorSet(device, image3dSampler, image3dView, imageLayouts, 1, descSet);
	//VkUtil::updateDescriptorSet(device, indices, amtOfIndices * sizeof(uint32_t), 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descSet);
	//VkUtil::updateDescriptorSet(device, data, amtOfData * sizeof(float), 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descSet);
	//
	////creating the command buffer, binding all the needed things and dispatching it to update the density images
	//VkCommandBuffer computeCommands;
	//VkUtil::createCommandBuffer(device, commandPool, &computeCommands);
	//
	//vkCmdBindPipeline(computeCommands, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
	//vkCmdBindDescriptorSets(computeCommands, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 1, &descSet, 0, { 0 });
	//uint32_t patchAmount = amtOfIndices / LOCALSIZE;
	//patchAmount += (amtOfIndices % LOCALSIZE) ? 1 : 0;
	//vkCmdDispatch(computeCommands, patchAmount, 1, 1);
	//VkUtil::commitCommandBuffer(queue, computeCommands);
	//err = vkQueueWaitIdle(queue);
	//check_vk_result(err);
	//
	//vkFreeCommandBuffers(device, commandPool, 1, &computeCommands);
	//vkFreeDescriptorSets(device, descriptorPool, 1, &descSet);
	//vkFreeMemory(device, infosMem, nullptr);
	//vkDestroyBuffer(device, infos, nullptr);
	//delete[] infoBytes;

	//uploading the density values currently manual, as there is an error in the compute pipeline ----------------------------------------------------------

	float** densityImages = new float*[required3dImages];
	for (int i = 0; i < required3dImages; ++i) {
		densityImages[i] = new float[w * d * h];
		memset(densityImages[i], 0, w * d * h * sizeof(float));
	}
	for (int i : indices) {
		int x = SpacialData::getRlatIndex(data[i][positionIndices.x]);
		int y = SpacialData::getAltitudeIndex(data[i][positionIndices.y]);
		if (y > h - 44)
			y = (y - h + 44) * 2 + (h - 44);
		int z = SpacialData::getRlonIndex(data[i][positionIndices.z]);
		assert(x >= 0);
		assert(y >= 0);
		assert(z >= 0);

		for (int j = 0; j < required3dImages; ++j) {
			densityImages[j][IDX3D(x, y, z, w, h)] = data[i][densityAttributes[j]];
			if (y >= h - 44) densityImages[j][IDX3D(x, y + 1, z, w, h)] = data[i][densityAttributes[j]];
		}
	}

	for (int i = 0; i < required3dImages; ++i) {
		VkUtil::uploadImageData(device, image3d[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_FORMAT_R32_SFLOAT, w, h, d, densityImages[i], w * h * d * sizeof(float));
		delete[] densityImages[i];
	}
	delete[] densityImages;

	VkUtil::createCommandBuffer(device, commandPool, &imageCommands);
	for (int i = 0; i < required3dImages; ++i) {
		VkUtil::transitionImageLayout(imageCommands, image3d[i], VK_FORMAT_R32_SFLOAT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
	VkUtil::commitCommandBuffer(queue, imageCommands);
	err = vkQueueWaitIdle(queue);
	check_vk_result(err);
	vkFreeCommandBuffers(device, commandPool, 1, &imageCommands);

	//end of uploading the density values ------------------------------------------------------------------------------------------------------------------

	//setting up all resources for the compute pipeline to create the binary volume

	//TODO: change this to be able to be more than just one image
	if (!binaryImage) {
		VkUtil::create3dImage(device, w, h, d, VK_FORMAT_R8_UNORM, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, &binaryImage);
		VkMemoryRequirements memReq;
		vkGetImageMemoryRequirements(device, binaryImage, &memReq);
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memReq.size;
		allocInfo.memoryTypeIndex = VkUtil::findMemoryType(physicalDevice, memReq.memoryTypeBits, 0);
		vkBindImageMemory(device, binaryImage, binaryImageMemory, 0);

		VkUtil::create3dImageView(device, binaryImage, VK_FORMAT_R8_UNORM, 1, &binaryImageView);
	}

	VkCommandBuffer binaryCommands;
	VkUtil::createCommandBuffer(device, commandPool, &binaryCommands);

	uint32_t binaryComputeInfosSize = sizeof(BinaryComputeInfos) + 2 * amtOfAttributes * sizeof(float);
	for (auto axis : brush) {
		binaryComputeInfosSize += axis.size() * 2 * sizeof(float);
	}
	BinaryComputeInfos* binaryComputeInfos = (BinaryComputeInfos*)new char[binaryComputeInfosSize];
	binaryComputeInfos->amtOfAxis = amtOfAttributes;
	binaryComputeInfos->maxX = w;
	binaryComputeInfos->maxY = h;
	binaryComputeInfos->maxZ = d;
	float* brushI = (float*)(binaryComputeInfos + 1);

	uint32_t curOffset = amtOfAttributes;		//the first offset is for axis 1, which is the size of the axis
	for (int axis = 0; axis < amtOfAttributes; ++axis) {
		brushI[axis] = curOffset;
		brushI[curOffset++] = brush[axis].size();
		for (int b = 0; b < brush[axis].size(); ++b) {
			brushI[curOffset++] = brush[axis][b].first;
			brushI[curOffset++] = brush[axis][b].second;
		}
	}

#ifdef _DEBUG
	assert(curOffset == binaryComputeInfosSize);
#endif

	VkBuffer binaryComputeBuffer;
	VkDeviceMemory binaryComputeMemory;
	VkUtil::createBuffer(device, binaryComputeInfosSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, &binaryComputeBuffer);
	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(device, binaryComputeBuffer, &memReq);
	allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex = VkUtil::findMemoryType(physicalDevice, memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	vkAllocateMemory(device, &allocInfo, nullptr, &binaryComputeMemory);
	vkBindBufferMemory(device, binaryComputeBuffer, binaryComputeMemory, 0);
	VkUtil::uploadData(device, binaryComputeMemory, 0, binaryComputeInfosSize, binaryComputeInfos);

	VkDescriptorSet binaryDescriptorSet;
	std::vector<VkDescriptorSetLayout> layouts;
	layouts.push_back(binaryComputeDescriptorSetLayout);
	VkUtil::createDescriptorSets(device, layouts, descriptorPool, &binaryDescriptorSet);

	VkUtil::updateDescriptorSet(device, binaryComputeBuffer, binaryComputeInfosSize, 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, binaryDescriptorSet);
	std::vector<VkImageLayout> imageLayouts(required3dImages, VK_IMAGE_LAYOUT_GENERAL);
	VkUtil::updateImageArrayDescriptorSet(device, image3dSampler, image3dView, imageLayouts, 1, binaryDescriptorSet);
	VkUtil::updateStorageImageDescriptorSet(device, binaryImageView, VK_IMAGE_LAYOUT_GENERAL, 2, binaryDescriptorSet);

	VkUtil::transitionImageLayout(binaryCommands, binaryImage, VK_FORMAT_R8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	vkCmdClearColorImage(binaryCommands, binaryImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear, 1, &range);
	VkUtil::transitionImageLayout(imageCommands, binaryImage, VK_FORMAT_R8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

	vkCmdBindPipeline(binaryCommands, VK_PIPELINE_BIND_POINT_COMPUTE, binaryComputePipeline);
	vkCmdBindDescriptorSets(binaryCommands, VK_PIPELINE_BIND_POINT_COMPUTE, binaryComputePipelineLayout, 0, 1, &binaryDescriptorSet, 0, nullptr);
	uint32_t patchAmtX = w / LOCALSIZE3D + ((w % LOCALSIZE) ? 1 : 0);
	uint32_t patchAmtY = h / LOCALSIZE3D + ((h % LOCALSIZE) ? 1 : 0);
	uint32_t patchAmtZ = d / LOCALSIZE3D + ((d % LOCALSIZE) ? 1 : 0);
	vkCmdDispatch(binaryCommands, patchAmtX, patchAmtY, patchAmtZ);
	VkUtil::transitionImageLayout(binaryCommands, binaryImage, VK_FORMAT_R8_UNORM, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	VkUtil::commitCommandBuffer(queue, binaryCommands);
	check_vk_result(vkQueueWaitIdle(queue));

	vkDestroyBuffer(device, binaryComputeBuffer, nullptr);
	vkFreeMemory(device, binaryComputeMemory, nullptr);
	vkFreeCommandBuffers(device, commandPool, 1, &binaryCommands);
	//checking change
	//VkUtil::downloadImageData(device, physicalDevice, commandPool
	//for (int i = 0; i < width * depth * height; ++i) {
	//	if (im[i] != 0) {
	//		float ok = im[i];
	//		zeroCount2++;
	//	}
	//}
	//std::cout << "Difference in zeros: " << zeroCount2<< std::endl;
	//delete[] im;

	if (!descriptorSet) {
		resize(1, 1);
		return;
	}

	updateCommandBuffer();
	render();
}

void IsoSurfRenderer::updateCameraPos(float* mouseMovement)
{
	//rotation matrix for height adjustment
	glm::mat4 vertical;
	vertical = glm::rotate(glm::mat4(1.0f), mouseMovement[1] * VERTICALPANSPEED, glm::normalize(glm::cross(camPos, glm::vec3(0, 1, 0))));
	glm::vec3 temp = vertical * glm::vec4(camPos, 1);
	if (dot(temp, glm::vec3(1, 0, 0)) * dot(camPos, glm::vec3(1, 0, 0)) < 0 || dot(temp, glm::vec3(0, 0, 1)) * dot(camPos, glm::vec3(0, 0, 1)) < 0)
		vertical = glm::mat4(1.0f);
	//rotation matrix for horizontal adjustment
	glm::mat4 horizontal = glm::rotate(glm::mat4(1.0f), mouseMovement[0] * HORIZONTALPANSPEED, glm::vec3(0, 1, 0));
	camPos = horizontal * vertical * glm::vec4(camPos,1);

	//adding zooming
	glm::vec3 zoomDir = -camPos;
	camPos += ZOOMSPEED * zoomDir * mouseMovement[2];
}

void IsoSurfRenderer::addBrush(std::string& name, std::vector<std::vector<std::pair<float, float>>> minMax)
{
	brushes[name] = minMax;
	brushColors[name] = new float[4];
	brushColors[name][0] = 1;
	brushColors[name][1] = 1;
	brushColors[name][2] = 1;
	brushColors[name][3] = 1;
	updateBrushBuffer();
	updateDescriptorSet();
	updateCommandBuffer();
	render();
}

bool IsoSurfRenderer::updateBrush(std::string& name, std::vector<std::vector<std::pair<float, float>>> minMax)
{
	if (brushes.find(name) == brushes.end()) return false;

	brushes[name] = minMax;
	
	updateBrushBuffer();
	updateDescriptorSet();
	render();

	return true;
}

bool IsoSurfRenderer::deleteBrush(std::string& name)
{
	return brushes.erase(name) > 0;
}

void IsoSurfRenderer::render()
{
	if (!image3dWidth || !image3dHeight || !image3dDepth || !brushes.size())
		return;

	VkResult err;

	//uploading the uniformBuffer
	UniformBuffer ubo;
	ubo.mvp = glm::perspective(glm::radians(45.0f), (float)imageWidth / (float)imageHeight, 0.1f, 100.0f);;
	ubo.mvp[1][1] *= -1;
	glm::mat4 look = glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	float max = glm::max(glm::max(image3dWidth, image3dHeight), image3dDepth);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f),glm::vec3(boxWidth,boxHeight,boxDepth));
	ubo.mvp = ubo.mvp * look *scale;
	ubo.camPos = glm::inverse(scale) * glm::vec4(camPos,1);

	ubo.faces.x = float(ubo.camPos.x > 0) - .5f;
	ubo.faces.y = float(ubo.camPos.y > 0) - .5f;
	ubo.faces.z = float(ubo.camPos.z > 0) - .5f;

	ubo.lightDir = lightDir;
	void* d;
	vkMapMemory(device, constantMemory, uniformBufferOffset, sizeof(UniformBuffer), 0, &d);
	memcpy(d, &ubo, sizeof(UniformBuffer));
	vkUnmapMemory(device, constantMemory);

	//uploading the storage buffer with bruhs infos -----------------------------------
	//converting the map of brushes to the graphics data structure
	std::vector<std::vector<Brush>> gpuData;
	uint32_t bId = 0;
	for (auto& brush : brushes) {
		for (int axis = 0; axis < brush.second.size(); ++axis) {
			if (gpuData.size() <= axis) gpuData.push_back({});
			if (brush.second[axis].size()) gpuData[axis].push_back({bId});
			for (auto& minMax : brush.second[axis]) {
				gpuData[axis].back().minMax.push_back(minMax);
			}
		}
		++bId;
	}
	std::vector<float*> colors;
	for (auto& col : brushColors) {
		colors.push_back(col.second);
	}

	BrushInfos* brushInfos = (BrushInfos*)new char[brushByteSize];
	brushInfos->amtOfAxis = gpuData.size();
	float* brushI = (float*)(brushInfos + 1);
	uint32_t curOffset = gpuData.size();		//the first offset is for axis 1, which is the size of the axis
	for (int axis = 0; axis < gpuData.size(); ++axis) {
		brushI[axis] = curOffset;
		brushI[curOffset++] = gpuData[axis].size();
		int brushOffset = curOffset;
		curOffset += gpuData[axis].size();
		for (int brush = 0; brush < gpuData[axis].size(); ++brush) {
			brushI[brushOffset + brush] = curOffset;
			brushI[curOffset++] = gpuData[axis][brush].bIndex;
			brushI[curOffset++] = gpuData[axis][brush].minMax.size();
			brushI[curOffset++] = colors[gpuData[axis][brush].bIndex][0];
			brushI[curOffset++] = colors[gpuData[axis][brush].bIndex][1];
			brushI[curOffset++] = colors[gpuData[axis][brush].bIndex][2];
			brushI[curOffset++] = colors[gpuData[axis][brush].bIndex][3];
			for (int minMax = 0; minMax < gpuData[axis][brush].minMax.size(); ++minMax) {
				brushI[curOffset++] = gpuData[axis][brush].minMax[minMax].first;
				brushI[curOffset++] = gpuData[axis][brush].minMax[minMax].second;
			}
		}
	}

	VkUtil::uploadData(device, brushMemory, 0, brushByteSize, brushInfos);

	//checking the brush infos
	//for (int axis = 0; axis < brushInfos->amtOfAxis; ++axis) {
	//	int axisOffset = int(brushI[axis]);
	//	//check if there exists a brush on this axis
	//	if (bool(brushI[axisOffset])) {		//amtOfBrushes > 0
	//		//as there exist brushes we get the density for this attribute
	//		//float density = texture(texSampler[axis], startPoint).x;
	//		//for every brush
	//		for (int brush = 0; brush < brushI[axisOffset]; ++brush) {
	//			int brushOffset = int(brushI[axisOffset + 1 + brush]);
	//			//for every MinMax
	//			for (int minMax = 0; minMax < brushI[brushOffset + 1]; ++minMax) {
	//				int minMaxOffset = brushOffset + 6 + 2 * minMax;			//+6 as after 1 the brush index lies, then the amtount of Minmax lies and then the color comes in a vec4
	//				int brushIndex = int(brushI[brushOffset]);
	//				float mi = brushI[minMaxOffset];
	//				float ma = brushI[minMaxOffset + 1];
	//				std::cout << "Axis: " << axis << ", brush index: " << brushIndex << ", Min: " << mi << ", Max: " << ma << "Color: " << brushI[brushOffset + 2]<< " " << brushI[brushOffset + 3] << " " << brushI[brushOffset + 4] << " " << brushI[brushOffset + 5] << std::endl;
	//				//bool stepInOut = prevDensity[axis] < mi && density >= mi ||
	//				//	prevDensity[axis] > mi&& density <= mi ||
	//				//	prevDensity[axis] > ma&& density <= ma ||
	//				//	prevDensity[axis] < ma && density >= ma;
	//				//
	//				////this are all the things i have to set to test if a surface has to be drawn
	//				//brushBorder[brushIndex] = brushBorder[brushIndex] || stepInOut;
	//				//brushBits[brushIndex] &= (uint((density<mi || density>ma) && !brushBorder[brushIndex]) << axis) ^ 0xffffffff;
	//				//brushColor[brushIndex] = vec4(bInfo.brushes[brushOffset + 2, brushOffset + 3, brushOffset + 4, brushOffset + 5]);
	//
	//				//the surface calculation is moved to the end of the for loop, as we have to check for every attribute of the brush if it is inside it
	//				//if(stepInBot^^stepOutBot || stepInTop^^stepOutTop){			//if we stepped in or out of the min max range blend surface color to total color
	//				//	vec4 surfColor = vec4(bInfo.brushes[brushOffset + 1,brushOffset + 2,brushOffset + 3,brushOffset + 4]);
	//				//	outColor.xyz += (1-outColor.w) * surfColor.w * surfColor.xyz;
	//				//	outColor.w += (1-outColor.w) * surfColor.w;
	//				//	//check for alphaStop
	//				//	if(outColor.w > alphaStop) br = true;
	//				//}
	//			}
	//		}
	//		//prevDensity[axis] = density;
	//	}
	//}

	delete[] brushInfos;

	//submitting the command buffer
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	err = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	check_vk_result(err);
}

void IsoSurfRenderer::setImageDescriptorSet(VkDescriptorSet descriptor)
{
	imageDescriptorSet = descriptor;
}

VkDescriptorSet IsoSurfRenderer::getImageDescriptorSet()
{
	return imageDescriptorSet;
}

VkSampler IsoSurfRenderer::getImageSampler()
{
	return sampler;
}

VkImageView IsoSurfRenderer::getImageView()
{
	return imageView;
}

void IsoSurfRenderer::createPrepareImageCommandBuffer()
{
	VkUtil::createCommandBuffer(device, commandPool, &prepareImageCommand);
	VkUtil::transitionImageLayout(prepareImageCommand, image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	vkEndCommandBuffer(prepareImageCommand);
}

void IsoSurfRenderer::createImageResources()
{
	VkResult err;
	
	VkUtil::createImage(device, imageWidth, imageHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, &image);

	VkMemoryRequirements memReq = {};
	vkGetImageMemoryRequirements(device, image, &memReq);
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex = VkUtil::findMemoryType(physicalDevice, memReq.memoryTypeBits, 0);
	err = vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory);
	check_vk_result(err);

	vkBindImageMemory(device, image, imageMemory, 0);

	VkUtil::createImageView(device, image, VK_FORMAT_R8G8B8A8_UNORM, 1, VK_IMAGE_ASPECT_COLOR_BIT, &imageView);
	std::vector<VkImageView> views;
	views.push_back(imageView);
	VkUtil::createFrameBuffer(device, renderPass, views, imageWidth, imageHeight, &frameBuffer);
	VkUtil::createImageSampler(device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_FILTER_LINEAR, 16, 1, &sampler);
}

void IsoSurfRenderer::createBuffer()
{
	VkUtil::createBuffer(device, 8 * sizeof(glm::vec3), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &vertexBuffer);
	VkUtil::createBuffer(device, 12 * 3 * sizeof(uint16_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, &indexBuffer);
	VkUtil::createBuffer(device, sizeof(UniformBuffer), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, &uniformBuffer);

	VkResult err;

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(device, vertexBuffer, &memReq);

	uint32_t memoryTypeBits = memReq.memoryTypeBits;
	VkMemoryAllocateInfo memAlloc = {};
	memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAlloc.allocationSize = memReq.size;
	uint32_t indexBufferOffset = memReq.size;

	vkGetBufferMemoryRequirements(device, indexBuffer, &memReq);
	memAlloc.allocationSize += memReq.size;
	memoryTypeBits |= memReq.memoryTypeBits;
	uniformBufferOffset = memAlloc.allocationSize;

	vkGetBufferMemoryRequirements(device, uniformBuffer, &memReq);
	memAlloc.allocationSize += memReq.size;
	memoryTypeBits |= memReq.memoryTypeBits;

	memAlloc.memoryTypeIndex = VkUtil::findMemoryType(physicalDevice, memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	err = vkAllocateMemory(device, &memAlloc, nullptr, &constantMemory);
	check_vk_result(err);

	vkBindBufferMemory(device, vertexBuffer, constantMemory, 0);
	vkBindBufferMemory(device, indexBuffer, constantMemory, indexBufferOffset);
	vkBindBufferMemory(device, uniformBuffer, constantMemory, uniformBufferOffset);

	//creating the data for the buffers
	glm::vec3 vB[8];
	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			for (int z = 0; z < 2; z++) {
				vB[(x << 2) | (y << 1) | z] = glm::vec3(x - .5f, y - .5f, z - .5f);
			}
		}
	}

	uint16_t iB[12 * 3] = { 0,1,2, 1,3,2, 0,4,1, 1,4,5, 0,2,4, 2,6,4, 2,3,6, 3,7,6, 4,6,5, 5,6,7, 1,5,7, 1,7,3 };

	void* d;
	vkMapMemory(device, constantMemory, 0, sizeof(vB), 0, &d);
	memcpy(d, vB, sizeof(vB));
	vkUnmapMemory(device, constantMemory);
	vkMapMemory(device, constantMemory, indexBufferOffset, sizeof(iB), 0, &d);
	memcpy(d, iB, sizeof(iB));
	vkUnmapMemory(device, constantMemory);
}

void IsoSurfRenderer::createPipeline()
{
	VkShaderModule shaderModules[5] = {};
	//the vertex shader for the pipeline
	std::vector<char> vertexBytes = PCUtil::readByteFile(vertPath);
	shaderModules[0] = VkUtil::createShaderModule(device, vertexBytes);
	//the fragment shader for the pipeline
	std::vector<char> fragmentBytes = PCUtil::readByteFile(fragPath);
	shaderModules[4] = VkUtil::createShaderModule(device, fragmentBytes);


	//Description for the incoming vertex attributes
	VkVertexInputBindingDescription bindingDescripiton = {};		//describes how big the vertex data is and how to read the data
	bindingDescripiton.binding = 0;
	bindingDescripiton.stride = sizeof(glm::vec3);
	bindingDescripiton.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription attributeDescription = {};	//describes the attribute of the vertex. If more than 1 attribute is used this has to be an array
	attributeDescription.binding = 0;
	attributeDescription.location = 0;
	attributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescription.offset = offsetof(glm::vec3, x);

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescripiton;
	vertexInputInfo.vertexAttributeDescriptionCount = 1;
	vertexInputInfo.pVertexAttributeDescriptions = &attributeDescription;

	//vector with the dynamic states
	std::vector<VkDynamicState> dynamicStates;
	dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

	//Rasterizer Info
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	//multisampling info
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	//blendInfo
	VkUtil::BlendInfo blendInfo;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	blendInfo.blendAttachment = colorBlendAttachment;
	blendInfo.createInfo = colorBlending;

	//creating the descriptor set layout
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	bindings.push_back(uboLayoutBinding);

	uboLayoutBinding.binding = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	uboLayoutBinding.descriptorCount = MAXAMTOF3DTEXTURES;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	bindings.push_back(uboLayoutBinding);

	uboLayoutBinding.binding = 2;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	bindings.push_back(uboLayoutBinding);

	std::vector<bool> valid{ true,false,true };
	VkUtil::createDescriptorSetLayoutPartiallyBound(device, bindings, valid, &descriptorSetLayout);
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
	descriptorSetLayouts.push_back(descriptorSetLayout);

	VkUtil::createRenderPass(device, VkUtil::PASS_TYPE_COLOR_OFFLINE, &renderPass);

	VkUtil::createPipeline(device, &vertexInputInfo, imageWidth, imageHeight, dynamicStates, shaderModules, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, &rasterizer, &multisampling, nullptr, &blendInfo, descriptorSetLayouts, &renderPass, &pipelineLayout, &pipeline);

	// creating the compute pipeline to fill the density images --------------------------------------------------
	VkShaderModule computeModule = VkUtil::createShaderModule(device, PCUtil::readByteFile(computePath));

	bindings.clear();
	VkDescriptorSetLayoutBinding binding = {};
	binding.descriptorCount = 1;						
	binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	binding.binding = 0;								//compute infos
	binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	bindings.push_back(binding);

	binding.binding = 1;								//density pictures
	binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	binding.descriptorCount = MAXAMTOF3DTEXTURES;
	bindings.push_back(binding);

	binding.binding = 2;								//indices buffer
	binding.descriptorCount = 1;
	binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	bindings.push_back(binding);

	binding.binding = 3;								//data buffer
	bindings.push_back(binding);

	std::vector<bool> validd{ true,false,true,true };
	VkUtil::createDescriptorSetLayoutPartiallyBound(device, bindings, validd, &computeDescriptorSetLayout);
	std::vector<VkDescriptorSetLayout>layouts;
	layouts.push_back(computeDescriptorSetLayout);

	VkUtil::createComputePipeline(device, computeModule, layouts, &computePipelineLayout, &computePipeline);

	//creating the compute pipeline to fill the binary images -----------------------------------------------------
	VkShaderModule binaryModule = VkUtil::createShaderModule(device, PCUtil::readByteFile(binaryComputePath));

	bindings.clear();
	binding.binding = 0;								//binary compute infos (eg. brush infos)
	binding.descriptorCount = 1;
	binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	bindings.push_back(binding);

	binding.binding = 1;								//density images
	binding.descriptorCount = MAXAMTOF3DTEXTURES;
	binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	bindings.push_back(binding);

	binding.binding = 2;								//binary image
	binding.descriptorCount = 1;
	bindings.push_back(binding);

	validd = { true,false,true };
	VkUtil::createDescriptorSetLayoutPartiallyBound(device, bindings, validd, &binaryComputeDescriptorSetLayout);
	layouts.clear();
	layouts.push_back(binaryComputeDescriptorSetLayout);

	VkUtil::createComputePipeline(device, binaryModule, layouts, &binaryComputePipelineLayout, &binaryComputePipeline);
}

void IsoSurfRenderer::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts;
	layouts.push_back(descriptorSetLayout);
	if (!descriptorSet) {
		VkUtil::createDescriptorSets(device, layouts, descriptorPool, &descriptorSet);
	}
}

void IsoSurfRenderer::updateDescriptorSet()
{
	VkUtil::updateDescriptorSet(device, uniformBuffer, sizeof(UniformBuffer), 0, descriptorSet);
	std::vector<VkImageLayout> layouts;
	for (int i = 0; i < image3d.size(); ++i) {
		layouts.push_back(VK_IMAGE_LAYOUT_GENERAL);
	}
	VkUtil::updateImageArrayDescriptorSet(device, image3dSampler, image3dView, layouts, 1, descriptorSet);
	VkUtil::updateDescriptorSet(device, brushBuffer, brushByteSize, 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descriptorSet);
}

void IsoSurfRenderer::updateBrushBuffer()
{
	if (brushes.empty()) return;

	//converting the map of brushes to the graphics data structure
	std::vector<std::vector<std::vector<std::pair<float, float>>>> gpuData;
	for (auto& brush : brushes) {
		for (int axis = 0; axis < brush.second.size(); ++axis) {
			if (gpuData.size() <= axis) gpuData.push_back({});
			if(brush.second[axis].size()) gpuData[axis].push_back({});
			for (auto& minMax : brush.second[axis]) {
				gpuData[axis].back().push_back(minMax);
			}
		}
	}

	//get the size for the new buffer
	uint32_t byteSize = 4 * sizeof(float);		//Standard information + padding
	byteSize += gpuData.size() * sizeof(float);		//offsets for the axes(offset a1, offset a2, ..., offset an)
	for (int axis = 0; axis < gpuData.size(); ++axis) {
		byteSize += (1 + gpuData[axis].size()) * sizeof(float);		//amtOfBrushes + offsets of the brushes
		for (int brush = 0; brush < gpuData[axis].size(); ++brush) {
			byteSize += (6 + 2 * gpuData[axis][brush].size()) * sizeof(float);		//brush index(1) + amtOfMinMax(1) + color(4) + space for minMax
		}
	}

	if (brushByteSize >= byteSize) return;		//if the current brush byte size is bigger or equal to the requred byte size simply return. No new allocastion needed

	brushByteSize = byteSize;

	//deallocate too small buffer
	if (brushBuffer) vkDestroyBuffer(device, brushBuffer, nullptr);
	if (brushMemory) vkFreeMemory(device, brushMemory, nullptr);

	VkUtil::createBuffer(device, byteSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, &brushBuffer);
	VkMemoryRequirements memReq = {};
	VkMemoryAllocateInfo allocInfo = {};
	vkGetBufferMemoryRequirements(device, brushBuffer, &memReq);
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex = VkUtil::findMemoryType(physicalDevice, memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	vkAllocateMemory(device, &allocInfo, nullptr, &brushMemory);
	vkBindBufferMemory(device, brushBuffer, brushMemory, 0);
}

void IsoSurfRenderer::updateCommandBuffer()
{
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

	VkResult err;
	VkUtil::createCommandBuffer(device, commandPool, &commandBuffer);
	VkUtil::transitionImageLayout(commandBuffer, image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	std::vector<VkClearValue> clearValues;
	clearValues.push_back({ .1f,.1f,.1f,1 });
	VkUtil::beginRenderPass(commandBuffer, clearValues, renderPass, frameBuffer, { imageWidth,imageHeight });

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
	VkDescriptorSet sets[1] = { descriptorSet };
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, sets, 0, nullptr);

	VkViewport viewport = {};					//description for our viewport for transformation operation after rasterization
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = imageWidth;
	viewport.height = imageHeight;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor = {};						//description for cutting the rendered result if wanted
	scissor.offset = { 0, 0 };
	scissor.extent = { (uint32_t)imageWidth,(uint32_t)imageHeight };
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDrawIndexed(commandBuffer, 3 * 6 * 2, 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	VkUtil::transitionImageLayout(commandBuffer, image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	err = vkEndCommandBuffer(commandBuffer);
	check_vk_result(err);

	err = vkDeviceWaitIdle(device);
	check_vk_result(err);
}
