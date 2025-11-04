#pragma once
#include <vector>
#include <string>
#include <memory>
#include <cstdint>
#include "API.h"

// File: Buffer.h
// Descrizione: astrazioni per i buffer usati dal renderer (vertex/index/uniform).
// Contiene:
// - definizione dei tipi di dati shader (`ShaderDataType`) e funzione per ottenere
//   la dimensione in byte di ciascun tipo
// - `BufferElement` che rappresenta un singolo elemento nel layout di un vertex buffer
// - `BufferLayout` che organizza una sequenza di `BufferElement` e calcola stride/offset
// - interfacce astratte per `VertexBuffer`, `IndexBuffer` e `UniformBuffer`
//   che vengono concretizzate da implementazioni specifiche dell'API (es. OpenGL)

enum class ShaderDataType
{
    None = 0,
    Float, Float2, Float3, Float4,
    Mat3, Mat4,
    Int, Int2, Int3, Int4,
    Bool
};

inline unsigned int shaderDataTypeSize(ShaderDataType type)
{
    switch (type)
    {
    case ShaderDataType::Float:    return 4;
    case ShaderDataType::Float2:   return 4 * 2;
    case ShaderDataType::Float3:   return 4 * 3;
    case ShaderDataType::Float4:   return 4 * 4;
    case ShaderDataType::Mat3:     return 4 * 3 * 3;
    case ShaderDataType::Mat4:     return 4 * 4 * 4;
    case ShaderDataType::Int:      return 4;
    case ShaderDataType::Int2:     return 4 * 2;
    case ShaderDataType::Int3:     return 4 * 3;
    case ShaderDataType::Int4:     return 4 * 4;
    case ShaderDataType::Bool:     return 1;
    default: return 0;
    }
}

// Un elemento del layout di un vertice
struct BufferElement
{
    std::string name;
    ShaderDataType type = ShaderDataType::None;
    uint32_t size = 0;
    size_t offset = 0;
    bool normalized = false;

    BufferElement() = default;

    BufferElement(ShaderDataType t, const std::string& n, bool norm = false)
        : name(n), type(t), size(shaderDataTypeSize(t)), offset(0), normalized(norm)
    {
    }

    // Numero di componenti scalari (utile per glVertexAttribPointer, ecc.)
    uint32_t GetComponentCount() const
    {
        switch (type)
        {
        case ShaderDataType::Float:   return 1;
        case ShaderDataType::Float2:  return 2;
        case ShaderDataType::Float3:  return 3;
        case ShaderDataType::Float4:  return 4;
        case ShaderDataType::Mat3:    return 3; // 3 vec3
        case ShaderDataType::Mat4:    return 4; // 4 vec4
        case ShaderDataType::Int:     return 1;
        case ShaderDataType::Int2:    return 2;
        case ShaderDataType::Int3:    return 3;
        case ShaderDataType::Int4:    return 4;
        case ShaderDataType::Bool:    return 1;
        default: return 0;
        }
    }
};

// Layout che contiene una sequenza di BufferElement e calcola stride/offset
class BufferLayout
{
public:
    BufferLayout() = default;
    BufferLayout(const std::vector<BufferElement>& elements)
        : m_Elements(elements)
    {
        CalculateOffsetsAndStride();
    }

    uint32_t GetStride() const { return m_Stride; }
    const std::vector<BufferElement>& GetElements() const { return m_Elements; }

    std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
    std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
    std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
    std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

private:
    void CalculateOffsetsAndStride()
    {
        size_t offset = 0;
        m_Stride = 0;
        for (auto& element : m_Elements)
        {
            element.offset = offset;
            offset += element.size;
            m_Stride += element.size;
        }
    }

private:
    std::vector<BufferElement> m_Elements;
    uint32_t m_Stride = 0;
};

enum class BufferUsage
{
    STATIC,
    DYNAMIC,
    STREAM
};

// Interfacce astratte per i buffer GPU
class VertexBuffer
{
public:
    virtual ~VertexBuffer() = default;

    // Binding
    virtual void bind() = 0;
    virtual void unbind() = 0;

    // Dati
    virtual void setData(const void* data, size_t size, BufferUsage usage) = 0;
    virtual void updateData(const void* data, size_t size, size_t offset) = 0;
    virtual void resize(size_t newSize) = 0;

    // Layout
    virtual void setLayout(const BufferLayout& layout) = 0;
    virtual const BufferLayout& getLayout() const = 0;

    // Factory: crea un'istanza concreta in base all'API in uso
    static std::shared_ptr<VertexBuffer> create();

    // informazioni opzionali sulla dimensione
    size_t bufferSize = 0;
    size_t usedSize = 0;
};

class IndexBuffer
{
public:
    virtual ~IndexBuffer() = default;

    virtual void bind() = 0;
    virtual void unbind() = 0;

    virtual void setData(const void* data, size_t size, BufferUsage usage) = 0;
    virtual void updateData(const void* data, size_t size, size_t offset) = 0;
    virtual void resize(size_t newSize) = 0;

    static std::shared_ptr<IndexBuffer> create();

    size_t bufferSize = 0;
    size_t usedSize = 0;
};

class UniformBuffer
{
public:
    virtual ~UniformBuffer() = default;

    virtual void bind() = 0;
    virtual void unbind() = 0;

    virtual void setData(const void* data, size_t size, BufferUsage usage) = 0;
    virtual void updateData(const void* data, size_t size, size_t offset) = 0;
    virtual void resize(size_t newSize) = 0;

    static std::shared_ptr<UniformBuffer> create();

    size_t bufferSize = 0;
    size_t usedSize = 0;
};