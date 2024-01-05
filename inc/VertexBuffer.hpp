#pragma once

#include "jac/type_defs.hpp"

class VertexBuffer
{
    public:
        VertexBuffer(const void* data,  uint size);
        ~VertexBuffer();

        void Bind() const;
        void Unbind() const;
    private:
        uint m_id;
};