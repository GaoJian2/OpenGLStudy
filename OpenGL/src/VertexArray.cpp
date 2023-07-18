#include "VertexArray.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"

VertexArray::VertexArray()
{
    GLCALL(glGenVertexArrays(1, &m_RendererID));
    GLCALL(glBindVertexArray(m_RendererID));
}

VertexArray::~VertexArray()
{
    GLCALL(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
    Bind();
	vb.Bind();
    const std::vector<VertexBufferElement>& elements = layout.GetElements();
    unsigned int offset = 0;
    for (int i = 0; i < elements.size(); i++)
    {
        const auto& element = elements[i];
        //参数说明:
        /*(index 0):代表这是顶点的第几个属性。这里只指定了顶点的位置属性且index标为第一个
        * (size 2):代表这是几维的顶点。这里是二维顶点。
        * (type float):属性类型
        * (normalized false):已经被归一化。例如颜色0~255，需要将此参数设置为true，归一化为0~1的浮点数
        * (stride 16):顶点之间（第n个顶点到第n+1个顶点）的偏移量。为了跳转到下一个顶点的内存地址。
        * (*pointer (void*)0):一个顶点中第n(n就是函数对应的这个属性)个属性的偏移量。
        */
        GLCALL(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (void *)offset));
        //index 就是上面函数的第一个参数
        GLCALL(glEnableVertexAttribArray(i));
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }

}

void VertexArray::Bind() const
{
    GLCALL(glBindVertexArray(m_RendererID));
}

void VertexArray::UnBind() const
{
    GLCALL(glBindVertexArray(0));
}
