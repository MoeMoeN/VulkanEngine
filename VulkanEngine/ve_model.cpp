#include "ve_model.h"

#include "ve_utils.h"

//libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
// std
#include <cassert>
#include <cstring>
#include <unordered_map>

namespace std {
	template <>
	struct hash<ve::VeModel::Vertex> {
		size_t operator()(ve::VeModel::Vertex const& vertex) const {
			size_t seed = 0;
			ve::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace ve {
	VeModel::VeModel(VeDevice& device, const VeModel::Builder& builder) : veDevice{ device } {
		createVertexBuffers(builder.vertices);
		createIndexBuffers(builder.indices);
	}

	VeModel::~VeModel() {}

	std::unique_ptr<VeModel> VeModel::createModelFromFile(VeDevice& device, const std::string& filepath) {
		Builder builder{};
		builder.loadModel(filepath);
		return std::make_unique<VeModel>(device, builder);
	}

	void VeModel::bind(VkCommandBuffer commandBuffer) {
		VkBuffer buffers[] = { vertexBuffer->getBuffer()};
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (hasIndexBuffer){
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32); //cane be changed to uint16 if model is equal or less than 65535 vertices
		}
	}

	void VeModel::draw(VkCommandBuffer commandBuffer) {
		if (hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
		}
	}

	void VeModel::createVertexBuffers(const std::vector<Vertex>& vertices) {
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		uint32_t vertexSize = sizeof(vertices[0]);

		VeBuffer stagingBuffer{
			veDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());

		vertexBuffer = std::make_unique<VeBuffer>(
			veDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);


		veDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
	}

	void VeModel::createIndexBuffers(const std::vector<uint32_t>& indices) {
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount > 0;
		
		if (!hasIndexBuffer) {
			return;
		}

		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
		uint32_t indexSize = sizeof(indices[0]);

		VeBuffer stagingBuffer{
			veDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());

		indexBuffer = std::make_unique<VeBuffer>(
			veDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);


		veDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
	}

	std::vector<VkVertexInputBindingDescription> VeModel::Vertex::getBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> bindDescriptions(1);
		bindDescriptions[0].binding = 0;
		bindDescriptions[0].stride = sizeof(Vertex);
		bindDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> VeModel::Vertex::getAttributeDescriptions() {
		/* other construction method for VkVertexInputAttributeDesciption
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);
		return attributeDescriptions;
		*/
		//{ location, binding, format, offset }
		return{
			{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
			{1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)},
			{2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)},
			{3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)}
		};
	}

	void VeModel::Builder::loadModel(const std::string& filepath) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				if (index.vertex_index >= 0) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};
					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2]
					};
				}

				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}

				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

}