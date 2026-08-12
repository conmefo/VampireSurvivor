#pragma once

#include "AudioIdentifiers.h"
#include <SFML/Graphics/Rect.hpp>
#include <unordered_map>
#include <vector>

// Represents a single pending SFX request within the current frame.
struct PendingSfxRequest
{
    SfxID id;
    PlaySoundOptions options;
};

// Coalesces high-frequency SFX requests within a single frame and applies
// viewport culling to discard off-screen spatial sounds.
class SfxCoalescer
{
public:
    SfxCoalescer();
    ~SfxCoalescer() = default;

    // Submit a sound request for the current frame.
    void Submit(SfxID id, const PlaySoundOptions& options);

    // Set the current viewport bounds for culling (world-space AABB).
    void SetViewportBounds(const sf::FloatRect& bounds);

    // Process all submitted requests for this frame:
    // - Cull off-screen spatial sounds
    // - Coalesce identical SfxIDs into boosted single requests
    // Returns the final list of coalesced, ready-to-play requests.
    std::vector<PendingSfxRequest> Flush();

private:
    // Per-SfxID aggregation bucket for the current frame.
    struct CoalesceBucket
    {
        int count = 0;
        PlaySoundOptions bestOptions;       // Highest priority options
        sf::Vector2f averagePosition{0.f, 0.f};
        bool hasSpatial = false;
    };

    bool IsInsideViewport(const sf::Vector2f& position) const;

    sf::FloatRect m_viewportBounds;
    float m_viewportMargin = 200.0f; // Extra margin around viewport for culling
    std::unordered_map<SfxID, CoalesceBucket> m_frameBuckets;
};
