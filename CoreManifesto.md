# Antigravity Engine: Core Manifesto & Development Guidelines

## 1. Introduction
This document serves as the absolute source of truth for the **antigravity** engine. It defines the immutable principles, coding standards, and architectural paradigms that must be strictly enforced across the entire codebase. 

The core philosophy of **antigravity** is built upon three pillars:
1.  **Pristine Code Quality:** Enforcing strict formatting for maximum readability.
2.  **Absolute OOP Compliance:** Ensuring robust, decoupled, and scalable system architecture.
3.  **Data-Driven Design:** The complete eradication of hard-coded values from the source code.

Every module, component, and utility contributed to this engine must pass the constraints outlined in this manifesto.

---

## 2. Strict Coding Standards
To maintain consistency and a highly readable C++ codebase, the following syntax rules are mandatory and leave no room for subjective styling.

### 2.1. Bracing Style (Allman)
The project strictly enforces the **Allman** bracing style. Every opening curly brace `{}` and closing curly brace must reside on its own dedicated line, vertically aligned with the statement that defines it.

### 2.2. Control Flow Spacing (Zero-Space Rule)
There must be **absolutely zero spaces** between any control flow keyword (e.g., `if`, `for`, `while`, `switch`, `catch`) and its corresponding opening parenthesis `(`. 

### 2.3. The Golden Standard (Code Example)
All implementations must mirror the formatting demonstrated below:

```cpp
void EngineCore::InitializeSystems(int retryCount)
{
    if(retryCount <= 0)
    {
        return;
    }

    for(int i = 0; i < retryCount; i++)
    {
        while(SystemNotReady())
        {
            Wait();
        }
    }
}



## 3. Constant Management & Anti-Hard-Coding

The **antigravity** engine operates on a strict Data-Driven Design philosophy. The source code is responsible for defining logic and architecture, not for storing raw data. The inclusion of "magic numbers" (unexplained, hard-coded numbers) or monolithic global constant files is strictly prohibited.

To maintain perfect Encapsulation and prevent Compilation Hell (Include Hell), constant management is divided into three distinct tiers.

### 3.1. The "No Magic Numbers" Rule
Never inject raw numbers or strings directly into logical statements, equations, or method calls, unless they are fundamental algorithmic axioms (e.g., `0`, `1`, `-1`).

* **Violation:** ```cpp
if(currentHealth <= 25) 
{
    WarningState();
}
```
* **Compliant:** ```cpp
if(currentHealth <= LOW_HEALTH_THRESHOLD) 
{
    WarningState();
}
```

### 3.2. Tier 1: Data-Driven Configuration (Externalized)
Values that dictate gameplay balance, visual aesthetics, or UI behavior **must not exist in C++ source files**. 
* **Scope:** Entity health, movement speeds, screen transition durations, UI colors, and text strings.
* **Implementation:** These variables must be externalized into data files (e.g., JSON, XML) and parsed into memory at runtime by a configuration manager.

### 3.3. Tier 2: Class-Scoped Constants (Encapsulated)
If a constant is exclusively relevant to the internal mechanics of a single class, it must remain completely hidden from the rest of the project.
* **Implementation:** Define these as `static constexpr` directly inside the class header, strictly within the `private` scope.
* **Example:**
```cpp
class FaderComponent
{
private:
    static constexpr float DEFAULT_RATIO = 1.0f;
    static constexpr float MIN_ALPHA = 0.0f;
};
```

### 3.4. Tier 3: Domain-Specific Headers (Categorized)
For fundamental constants that genuinely require global access, the "One Giant Global File" anti-pattern is strictly forbidden. 
* **Implementation:** Constants must be decoupled and grouped logically into domain-specific headers (e.g., `MathConstants.h`, `PhysicsConstants.h`).

---

## 4. Strict OOP Compliance (Architecture Guidelines)

**antigravity** relies heavily on pure Object-Oriented Programming (OOP) to ensure systems remain decoupled, testable, and scalable. All code must adhere to the following architectural pillars.

### 4.1. Absolute Encapsulation
Data hiding is mandatory. Classes must protect their internal states from unauthorized external modification.
* **Rule:** No class member variables (properties) may be declared as `public`. 
* **Implementation:** All variables must be `private` or `protected`. External access must be governed exclusively through clearly defined `public` methods (Getters/Setters) or well-controlled interface functions.

### 4.2. Single Responsibility Principle (SRP)
A class must do one thing, and do it perfectly.
* **Rule:** Avoid "God Classes" that manage logic, rendering, and input simultaneously.
* **Example:** The `FaderComponent` is solely responsible for mathematically interpolating an alpha ratio. It **must not** possess a `Draw()` function or interact with the graphics API. The rendering logic remains the responsibility of the UI element that owns the `FaderComponent`.

### 4.3. Composition Over Inheritance
While inheritance is permitted for establishing structural hierarchies (e.g., `Button` inherits from `UIElement`), functionality should be heavily modularized via Composition.
* **Rule:** If a class needs a behavior (e.g., fading, animating, physical movement), it should *own* a component that handles that behavior, rather than inheriting from a massive base class.
* **Example:** Instead of `class FadingButton : public UIElement`, use `class Button : public UIElement` and give it a `FaderComponent fader;` member.

### 4.4. Dependency Injection & Loose Coupling
Classes should not instantiate deep systemic dependencies internally.
* **Rule:** If an object requires a major manager or service (e.g., a `Renderer` or `AssetManager`), that dependency must be passed into its constructor or method via references/pointers. 
* **Example:** A generic `GameEntity` should not call `ResourceManager::GetInstance()->Load(...)` directly. This prevents hard-coupling and allows for easier unit testing.


## 5. Workflow & Communication Protocol

### 5.1. Persistent Planning & Feedback Tracking
To maintain a continuous and trackable history of architectural decisions, all proposed implementation plans, structural reviews, and developer feedback generated by the AI must be explicitly logged into the project's requirement file.

* **Target Document:** `@File:Instruction.md` (or the current primary requirement document).
* **Location:** All feedback and planning steps must be appended under a dedicated `## Feedback` (or `## Implementation Plan`) section at the very end of the file.
* **Enforcement:** The AI is strictly forbidden from outputting critical architectural plans or review notes solely as transient chat messages. The instruction document must always be updated to reflect the proposed path forward and await user approval before proceeding with code generation.

### 5.2. Progress Tracking & Milestone Documentation
To maintain an accurate, high-level overview of the engine's development state, all finalized implementations must be persistently logged.

* **Target Document:** `@File:project_summary.md`
* **Trigger Condition:** Immediately after successfully writing, refactoring, or integrating any new feature, class, or architectural system (and receiving user approval).
* **Execution:** The AI must proactively append a concise update to the `project_summary.md` file. This log must detail what was explicitly completed, the core components involved, and how it fits into the current system structure. 
* **Intent:** This ensures the project maintains a clear "source of truth" regarding its progress, preventing duplicate work and providing immediate context for future development sessions.

### 5.3. External AI Specification Verification Rule
All task instructions and micro-task descriptions logged in `Instruction.md` are proposed by an external AI Architect Agent operating without direct visibility into the live repository. Consequently, the implementing AI Developer Agent must perform a strict validation phase prior to generating any source code.

* **Anti-Redundancy & Component Reuse:** The implementing agent must thoroughly inspect the current project state (cross-referencing `project_summary.md` and existing source files) to ensure no existing classes, components, or layout structures are being re-invented. If an existing engine asset or base module (e.g., `UIPanel`, `NineSliceComponent`) can fulfill the requirement, it must be forcefully reused via Composition or permitted Inheritance. Duplication of logic is a direct violation of this manifesto.
* **Architectural Sanity & OOP Review:** The agent must critically evaluate whether the proposed task design perfectly aligns with Section 4 (Strict OOP Compliance). Any specification that threatens encapsulation, introduces tight coupling between decoupled layers (such as mixing view layout with game states), or forces a class to violate the Single Responsibility Principle must be proactively flagged, refined, and corrected before implementation begins.