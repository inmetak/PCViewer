#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <string>
#include <map>
#include "VkUtil.h"
#include "PCUtil.h"

#define SHADERPATH "shader/histComp.spv"
#define LOCALSIZE 256

class HistogramManager {
public:
	struct Histogram {
		float maxGlobalCount;							//maximung value accross all attributes
		std::vector<float> maxCount;					//maximum histogramm value for each attribute
		std::vector<std::pair<float, float>> ranges;	//the value ranges for each attribute
		std::vector<std::vector<float>> bins;			//histogramm values for each attribute
	};


	HistogramManager(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue, VkDescriptorPool descriptorPool, uint32_t binsAmount);
	~HistogramManager();

	void computeHistogramm(std::string& name, std::vector<std::pair<float,float>>& minMax, VkBuffer data, uint32_t amtOfData, VkBuffer indices, uint32_t amtOfIndices, VkBufferView indicesActivations);
	Histogram& getHistogram(std::string name);
	bool containsHistogram(std::string& name);
	void setNumberOfBins(uint32_t n);

private:
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkCommandPool commandPool;
	VkQueue queue;
	VkDescriptorPool descriptorPool;

	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	VkDescriptorSetLayout descriptorSetLayout;
	// 3 ubo buffers for
	// informations:
	// numOfBins numOfAttributes float[min,max]
	// bins:
	// array for all bins
	VkBuffer uboBuffers[2];
	uint32_t uboOffsets[2];
	VkDeviceMemory uboMemory;

	uint32_t numOfBins;
	std::map<std::string, Histogram> histograms;
};