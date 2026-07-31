#include "ParticleManager.h"
#include <cmath>

namespace
{
    const char* PARTICLE_FRAG = R"(
        uniform sampler2D texture;
        uniform bool overrideColor;

        void main()
        {
            vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
            if (pixel.a > 0.0)
            {
                if (overrideColor)
                    gl_FragColor = vec4(gl_Color.rgb, pixel.a * gl_Color.a);
                else
                    gl_FragColor = pixel * gl_Color;
            }
            else
            {
                gl_FragColor = vec4(0.0);
            }
        }
    )";

    sf::Shader* GetParticleShader()
    {
        static sf::Shader shader;
        static bool loaded = false;
        if (!loaded)
        {
            if(sf::Shader::isAvailable())
            {
                shader.loadFromMemory(PARTICLE_FRAG, sf::Shader::Fragment);
            }
            loaded = true;
        }
        return &shader;
    }
}

namespace vs
{

ParticleManager::ParticleManager()
{
    m_vertices.setPrimitiveType(sf::Quads);
}

void ParticleManager::Initialize(const TextureAtlas* atlas, const ParticleDataManager* dataManager)
{
    m_atlas = atlas;
    m_dataManager = dataManager;
}

const ParticleEmitterConfig& ParticleManager::GetConfig(const std::string& id) const
{
    if (m_dataManager)
    {
        return m_dataManager->GetConfig(id);
    }
    static ParticleEmitterConfig emptyConfig;
    return emptyConfig;
}

ParticleEmitter* ParticleManager::SpawnEmitter(const ParticleEmitterConfig& config, const sf::Vector2f& position)
{
    sf::IntRect texRect;
    if (m_atlas && !config.textureId.empty())
    {
        AssetTextureData data = m_atlas->GetTextureData(config.textureId);
        texRect = data.rect;
    }
    else
    {
        // Fallback or solid color
        texRect = sf::IntRect(0, 0, 8, 8);
    }

    auto emitter = std::make_unique<ParticleEmitter>(config, position, texRect);
    ParticleEmitter* ptr = emitter.get();
    m_emitters.push_back(std::move(emitter));
    return ptr;
}

void ParticleManager::Update(float dt)
{
    for (auto it = m_emitters.begin(); it != m_emitters.end(); )
    {
        (*it)->Update(dt);
        if ((*it)->IsFinished())
        {
            it = m_emitters.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void ParticleManager::Draw(sf::RenderTarget& target) const
{
    if (!m_atlas) return;

    for (const auto& emitter : m_emitters)
    {
        const auto& particles = emitter->GetParticles();
        if (particles.empty()) continue;

        m_vertices.resize(particles.size() * 4);
        size_t vertexIndex = 0;

        for (const auto& p : particles)
        {
            float halfWidth = (p.textureRect.width / 2.0f) * p.size;
            float halfHeight = (p.textureRect.height / 2.0f) * p.size;

            sf::Vertex* quad = &m_vertices[vertexIndex];

            if (p.rotation != 0.0f)
            {
                float angle = p.rotation * 3.14159265f / 180.0f;
                float c = std::cos(angle);
                float s = std::sin(angle);
                
                // Top-left
                float tx1 = -halfWidth; float ty1 = -halfHeight;
                quad[0].position = sf::Vector2f(p.position.x + tx1 * c - ty1 * s, p.position.y + tx1 * s + ty1 * c);
                // Top-right
                float tx2 = halfWidth; float ty2 = -halfHeight;
                quad[1].position = sf::Vector2f(p.position.x + tx2 * c - ty2 * s, p.position.y + tx2 * s + ty2 * c);
                // Bottom-right
                float tx3 = halfWidth; float ty3 = halfHeight;
                quad[2].position = sf::Vector2f(p.position.x + tx3 * c - ty3 * s, p.position.y + tx3 * s + ty3 * c);
                // Bottom-left
                float tx4 = -halfWidth; float ty4 = halfHeight;
                quad[3].position = sf::Vector2f(p.position.x + tx4 * c - ty4 * s, p.position.y + tx4 * s + ty4 * c);
            }
            else
            {
                quad[0].position = sf::Vector2f(p.position.x - halfWidth, p.position.y - halfHeight);
                quad[1].position = sf::Vector2f(p.position.x + halfWidth, p.position.y - halfHeight);
                quad[2].position = sf::Vector2f(p.position.x + halfWidth, p.position.y + halfHeight);
                quad[3].position = sf::Vector2f(p.position.x - halfWidth, p.position.y + halfHeight);
            }

            // Colors
            quad[0].color = p.color;
            quad[1].color = p.color;
            quad[2].color = p.color;
            quad[3].color = p.color;

            // TexCoords
            quad[0].texCoords = sf::Vector2f(static_cast<float>(p.textureRect.left), static_cast<float>(p.textureRect.top));
            quad[1].texCoords = sf::Vector2f(static_cast<float>(p.textureRect.left + p.textureRect.width), static_cast<float>(p.textureRect.top));
            quad[2].texCoords = sf::Vector2f(static_cast<float>(p.textureRect.left + p.textureRect.width), static_cast<float>(p.textureRect.top + p.textureRect.height));
            quad[3].texCoords = sf::Vector2f(static_cast<float>(p.textureRect.left), static_cast<float>(p.textureRect.top + p.textureRect.height));

            vertexIndex += 4;
        }

        sf::RenderStates states;
        float bType = emitter->GetConfig().blendModeType;
        if (bType < 0.5f) {
            states.blendMode = sf::BlendAlpha;
        } else if (bType < 1.5f) {
            states.blendMode = sf::BlendAdd;
        } else {
            states.blendMode = sf::BlendMultiply;
        }
        if (!emitter->GetConfig().textureId.empty()) {
            states.texture = m_atlas->GetTextureData(emitter->GetConfig().textureId).texture;
        } else {
            states.texture = nullptr;
        }

        if (emitter->GetConfig().overrideColor && sf::Shader::isAvailable()) {
            sf::Shader* shader = GetParticleShader();
            shader->setUniform("texture", sf::Shader::CurrentTexture);
            shader->setUniform("overrideColor", true);
            states.shader = shader;
        }

        target.draw(m_vertices, states);
    }
}

void ParticleManager::Clear()
{
    m_emitters.clear();
}

} // namespace vs
