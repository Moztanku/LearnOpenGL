/**
 * @file VertexBufferLayout.cpp
 * @author Moztanku (mostankpl@gmail.com)
 * @brief Implementation of VertexBufferLayout class
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "VertexBufferLayout.hpp"

template<>
void VertexBufferLayout::Push<float>(uint count)
{
    m_Elements.push_back({count, GL_FLOAT, GL_FALSE});
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
}

template<>
void VertexBufferLayout::Push<uint>(uint count)
{
    m_Elements.push_back({count, GL_UNSIGNED_INT, GL_TRUE});
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
}

template<>
void VertexBufferLayout::Push<u_char>(uint count)
{
    m_Elements.push_back({count, GL_UNSIGNED_BYTE, GL_TRUE});
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
}