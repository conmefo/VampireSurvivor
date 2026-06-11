# Detailed Specification: Dynamic Pixel-Perfect Corner Upscaling for 9-Slice Components

This document describes the architectural upgrade for the 9-Slice Sprite Component. The goal is to dynamically calculate a non-distorted, pixel-perfect corner scale based on the target object's dimensions, utilizing an **upward rounding (Ceil)** approach to maintain strong pixel art aesthetics while eliminating pixel bleeding.

---

## 1. Architectural Concept

In pixel art games, standard 9-slice components fail when a small source texture (e.g., $16 \times 16$ pixels with a $3 \times 3$ corner) is upscaled onto a large UI container. The corners remain tiny and unnoticeable, losing their stylized appearance.

To fix this, the component must dynamically upscale the corner geometry. By using **Ceil Rounding**, we guarantee that the corners adapt to the scale of the object and lean towards a chunkier, more defined pixel look, rather than shrinking down.

---

## 2. The Golden Rule of Ceil Rounding

When rounding up the scale factor, the component must satisfy the **Spatial Sufficiency Condition**:
> The combined size of the upscaled corners must never exceed the total target dimensions of the object.

If the object is too small but the corner scale is rounded up, the center width or center height will become negative ($Center\_Size < 0$). This causes the mesh coordinates to invert, leading to severe visual overlap and glitching. Therefore, the algorithm must include a fallback validation check.

---

## 3. Step-by-Step Execution Flow

The component upgrade modifies the layout recalculation phase (`Update9Slice`) into four distinct logical phases:

### Phase 1: Raw Ratio Calculation
First, determine how many times larger the target object is compared to the original sprite texture. Calculate the raw floating-point ratios for both axes independently:
* $Ratio\_X = Target\_Width \div Original\_Sprite\_Width$
* $Ratio\_Y = Target\_Height \div Original\_Sprite\_Height$

### Phase 2: Uniform Ceil Rounding
To prevent corner distortion (stretching on one axis), both axes must share an identical, integer-based scale factor. 
1. Select the smaller of the two raw ratios ($Ratio\_X$ and $Ratio\_Y$). This ensures the corner scale is bounded by the most restricted dimension of the object.
2. Apply a ceiling function ($\lceil \dots \rceil$) to this selected ratio to force it to the next highest integer. This integer is your **Target Corner Scale**.

### Phase 3: Spatial Sufficiency Validation (The Fallback Check)
Before committing to the rounded-up scale, verify that the object can actually fit these larger corners:
1. Calculate the hypothetical corner size: $Rendered\_Corner\_Size = Original\_Corner\_Size \times Target\_Corner\_Scale$.
2. Check if twice the $Rendered\_Corner\_Size$ is greater than or equal to either the $Target\_Width$ or $Target\_Height$.
3. **The Fallback:** If the corners overlap, the ceiling choice was invalid. The component must immediately fall back to a lower integer scale (using a floor function $\lfloor \dots \rfloor$ or subtracting $1$ from the target scale) to protect the UI's structural integrity.

### Phase 4: Geometry Partitioning
Once the integer scale factor is locked in, allocate the actual pixel spacing for the 9 regions:
* **The 4 Corners:** Fixed strictly to the calculated integer size. Because they use a uniform integer multiplier, the pixels remain completely square and sharp—**zero distortion, zero pixel bleeding**.
* **The Edges and Center:** They absorb all remaining sub-pixel fractions and empty space. Since the center and edges are designed to loop (Tile) or stretch, they handle fractional values gracefully without compromising the overall art style.

---

## 4. Summary of Benefits

* **Visual Consistency:** The corner pixels dynamically grow to match the scale of the UI, preserving the chunky, nostalgic pixel art proportions.
* **Pixel Perfect Precision:** By forcing the corner multiplier to a strict integer, texture coordinates mapping (UVs) align perfectly with screen pixels, completely eliminating fuzzy edges and pixel bleeding.
* **Automation:** Developers no longer need to manually tweak a scale variable for every single UI element; the component automatically delivers the optimal aesthetic based on size.