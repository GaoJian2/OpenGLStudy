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
        //����˵��:
        /*(index 0):�������Ƕ���ĵڼ������ԡ�����ָֻ���˶����λ��������index��Ϊ��һ��
        * (size 2):�������Ǽ�ά�Ķ��㡣�����Ƕ�ά���㡣
        * (type float):��������
        * (normalized false):�Ѿ�����һ����������ɫ0~255����Ҫ���˲�������Ϊtrue����һ��Ϊ0~1�ĸ�����
        * (stride 16):����֮�䣨��n�����㵽��n+1�����㣩��ƫ������Ϊ����ת����һ��������ڴ��ַ��
        * (*pointer (void*)0):һ�������е�n(n���Ǻ�����Ӧ���������)�����Ե�ƫ������
        */
        GLCALL(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (void *)offset));
        //index �������溯���ĵ�һ������
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
