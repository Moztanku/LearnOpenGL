/**
 * @file VertexArray.hpp
 * @author Moztanku (mostankpl@gmail.com)
 * @brief Abstraction of OpenGL Vertex Array Object (VAO), describes how to interpret vertex data
 * @version 0.1
 * @date 2024-01-05
 * @see VertexBuffer.hpp VertexBufferLayout.hpp
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"

class VertexArray
{
    public:
        VertexArray();
        ~VertexArray();

        void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

        void Bind() const;
        void Unbind() const;
    private:
        uint m_id;
};