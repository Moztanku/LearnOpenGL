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