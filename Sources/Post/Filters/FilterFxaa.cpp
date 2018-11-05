#include "FilterFxaa.hpp"

namespace acid
{
	FilterFxaa::FilterFxaa(const GraphicsStage &graphicsStage, const float &spanMax) :
		IPostFilter(graphicsStage, {"Shaders/Filters/Default.vert", "Shaders/Filters/Fxaa.frag"}, {}),
		m_uniformScene(UniformHandler()),
		m_spanMax(spanMax)
	{
	}

	void FilterFxaa::Render(const CommandBuffer &commandBuffer, const Vector4 &clipPlane, const ICamera &camera)
	{
		// Updates uniforms.
		m_uniformScene.Push("spanMax", m_spanMax);

		// Updates descriptors.
		m_descriptorSet.Push("UboScene", &m_uniformScene);
	//	m_descriptorSet.Push("writeColour", GetAttachment("writeColour", "resolved"));
	//	m_descriptorSet.Push("samplerColour", GetAttachment("samplerColour", "resolved"));
		PushConditional("writeColour", "samplerColour", "resolved", "diffuse");
		bool updateSuccess = m_descriptorSet.Update(m_pipeline);

		if (!updateSuccess)
		{
			return;
		}

		// Binds the pipeline.
		m_pipeline.BindPipeline(commandBuffer);

		// Draws the object.
		m_descriptorSet.BindDescriptor(commandBuffer);
		m_model->CmdRender(commandBuffer);
	}
}
