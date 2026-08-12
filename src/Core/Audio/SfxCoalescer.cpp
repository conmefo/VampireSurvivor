#include "SfxCoalescer.h"
#include <cmath>
#include <algorithm>

SfxCoalescer::SfxCoalescer() = default;

void SfxCoalescer::Submit(SfxID id, const PlaySoundOptions& options)
{
    // Viewport culling: immediately discard spatial sounds outside the viewport
    if (options.isSpatial && !IsInsideViewport(options.position))
    {
        return;
    }

    auto& bucket = m_frameBuckets[id];
    bucket.count++;

    if (bucket.count == 1)
    {
        // First request: initialize the bucket
        bucket.bestOptions = options;
        if (options.isSpatial)
        {
            bucket.averagePosition = options.position;
            bucket.hasSpatial = true;
        }
    }
    else
    {
        // Subsequent requests: keep the highest priority option and average the position
        if (static_cast<int>(options.priority) > static_cast<int>(bucket.bestOptions.priority))
        {
            bucket.bestOptions.priority = options.priority;
        }

        if (options.isSpatial && bucket.hasSpatial)
        {
            float n = static_cast<float>(bucket.count);
            bucket.averagePosition.x = bucket.averagePosition.x * ((n - 1.0f) / n) + options.position.x / n;
            bucket.averagePosition.y = bucket.averagePosition.y * ((n - 1.0f) / n) + options.position.y / n;
        }
    }
}

void SfxCoalescer::SetViewportBounds(const sf::FloatRect& bounds)
{
    m_viewportBounds = bounds;
}

std::vector<PendingSfxRequest> SfxCoalescer::Flush()
{
    std::vector<PendingSfxRequest> result;
    result.reserve(m_frameBuckets.size());

    for (auto& [sfxId, bucket] : m_frameBuckets)
    {
        PendingSfxRequest req;
        req.id = sfxId;
        req.options = bucket.bestOptions;

        // Coalesce volume boost: multiple identical sounds in one frame
        // get a single boosted playback instead of N separate plays.
        // Formula: boost = 1.0 + 0.15 * log2(count), capped at 1.5x
        if (bucket.count > 1)
        {
            float boost = 1.0f + 0.15f * std::log2(static_cast<float>(bucket.count));
            req.options.volumeScale = std::min(req.options.volumeScale * boost, 1.5f);
        }

        // Use averaged position for spatial coalesced sounds
        if (bucket.hasSpatial)
        {
            req.options.position = bucket.averagePosition;
            req.options.isSpatial = true;
        }

        result.push_back(req);
    }

    m_frameBuckets.clear();
    return result;
}

bool SfxCoalescer::IsInsideViewport(const sf::Vector2f& position) const
{
    // Expand viewport by margin for graceful culling at edges
    sf::FloatRect expanded(
        m_viewportBounds.left - m_viewportMargin,
        m_viewportBounds.top - m_viewportMargin,
        m_viewportBounds.width + m_viewportMargin * 2.0f,
        m_viewportBounds.height + m_viewportMargin * 2.0f
    );

    return expanded.contains(position);
}
