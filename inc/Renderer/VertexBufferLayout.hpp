/**
 * @file VertexBufferLayout.hpp
 * @author Moztanku (mostankpl@gmail.com)
 * @brief Class that describes single vertice (element) in VertexBuffer
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include <glad/gl.h>

#include <vector>
#include <cassert>

#include "jac/type_defs.hpp"

struct VertexBufferElement 
{
    uint count;
    uint type;
    uchar normalized;

    static uint GetSizeOfType(uint type)
    {
        switch (type)
        {
            case GL_FLOAT: return sizeof(GLfloat);
            case GL_UNSIGNED_INT: return sizeof(GLuint);
            case GL_UNSIGNED_BYTE: return sizeof(GLbyte);
        }
        return 0;
    }
};

class VertexBufferLayout
{
    public:
        VertexBufferLayout() : m_Stride(0) {};

        template<typename T>
        void Push(uint count)
        {
            assert(false);
        }

        inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
        inline uint GetStride() const { return m_Stride; }

    private:
        std::vector<VertexBufferElement> m_Elements;
        uint m_Stride;
};
