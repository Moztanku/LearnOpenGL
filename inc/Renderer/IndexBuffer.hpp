/**
 * @file IndexBuffer.hpp
 * @author Moztanku (mostankpl@gmail.com)
 * @brief Abstraction of OpenGL Index Buffer (EBO), holds indices of vertex data
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include "jac/type_defs.hpp"

class IndexBuffer
{
    public:
        IndexBuffer(const uint* data, const uint count);
        ~IndexBuffer();

        void Bind() const;
        void Unbind() const;

        inline uint GetCount() const { return m_Count; }
    private:
        uint m_id;
        uint m_Count;
};