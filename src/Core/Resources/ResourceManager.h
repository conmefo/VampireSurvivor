#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <stdexcept>
#include <cassert>

template<typename Resource, typename Identifier>
class ResourceManager
{
public:
    void Load(Identifier id, const std::string& filename)
    {
        std::unique_ptr<Resource> resource(new Resource());
        
        if(!resource->loadFromFile(filename))
        {
            throw std::runtime_error("ResourceManager::Load - Failed to load " + filename);
        }
        
        InsertResource(id, std::move(resource));
    }

    template<typename Parameter>
    void Load(Identifier id, const std::string& filename, const Parameter& secondParam)
    {
        std::unique_ptr<Resource> resource(new Resource());
        
        if(!resource->loadFromFile(filename, secondParam))
        {
            throw std::runtime_error("ResourceManager::Load - Failed to load " + filename);
        }
        
        InsertResource(id, std::move(resource));
    }

    void LoadFromImage(Identifier id, const sf::Image& image)
    {
        std::unique_ptr<Resource> resource(new Resource());
        
        if(!resource->loadFromImage(image))
        {
            throw std::runtime_error("ResourceManager::LoadFromImage - Failed to load from image");
        }
        
        InsertResource(id, std::move(resource));
    }

    Resource& Get(Identifier id)
    {
        auto found = m_resourceMap.find(id);
        assert(found != m_resourceMap.end() && "ResourceManager::Get - Resource ID not found!");
        return *found->second;
    }

    const Resource& Get(Identifier id) const
    {
        auto found = m_resourceMap.find(id);
        assert(found != m_resourceMap.end() && "ResourceManager::Get - Resource ID not found!");
        return *found->second;
    }

    Resource* GetPtr(Identifier id)
    {
        auto found = m_resourceMap.find(id);
        if (found != m_resourceMap.end())
        {
            return found->second.get();
        }
        return nullptr;
    }

    const Resource* GetPtr(Identifier id) const
    {
        auto found = m_resourceMap.find(id);
        if (found != m_resourceMap.end())
        {
            return found->second.get();
        }
        return nullptr;
    }

private:
    void InsertResource(Identifier id, std::unique_ptr<Resource> resource)
    {
        auto inserted = m_resourceMap.insert(std::make_pair(id, std::move(resource)));
        assert(inserted.second && "ResourceManager::InsertResource - Resource ID already exists!");
    }

    std::unordered_map<Identifier, std::unique_ptr<Resource>> m_resourceMap;
};
