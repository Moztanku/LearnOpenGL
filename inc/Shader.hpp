#pragma once

#include <glm/matrix.hpp>

#include <unordered_map>
#include <filesystem>
#include <string>
#include <sys/types.h>

class Shader
{
    public:
        Shader(const std::filesystem::path& combined_shader_path);
        Shader(const std::filesystem::path& vertex_path, const std::filesystem::path& fragment_path);
        ~Shader();

        void Bind() const;
        void Unbind() const;

        template<typename... Args>
        void SetUniform(const std::string& name, Args... args);

        template<typename Matrix>
        void SetUniformM(const std::string& name, const Matrix& matrix);
    private:
        uint m_id;
        std::filesystem::path m_FilePath;
        std::unordered_map<std::string, uint> m_UniformLocationCache;

        uint GetUniformLocation(const std::string& name);
};
