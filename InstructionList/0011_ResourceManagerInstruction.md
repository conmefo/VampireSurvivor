# Specification: Resource Management System

## 1. Architectural Overview & Philosophy
The `ResourceManager` is the centralized asset vault for the **antigravity** engine. Its fundamental purpose is to guarantee that heavy assets (Textures, Fonts, Sound Buffers) are loaded from the hard drive exactly once, stored efficiently in RAM, and shared across multiple entities via references.

To maintain strict OOP compliance, this system **strictly forbids the Singleton pattern**. It must be instantiated once at the highest engine level (e.g., the core `Game` or `Engine` class) and passed down to individual States and components using **Dependency Injection** (passing by reference or pointer).

## 2. The Identifier System (Compile-Time Safety)
To eliminate the risks of string-based lookups (typos causing runtime crashes, expensive hashing algorithms), the system mandates the use of strongly typed enumerations (`enum class`).

* **Resource IDs:** Distinct enums must be defined for different asset types (e.g., `TextureID`, `FontID`). 
* **Zero Hard-Coding in States:** UI components, State logic, and Entities must never interact with string-based file paths. They communicate their asset needs to the `ResourceManager` exclusively using these Enum identifiers.
* **Performance:** Enum keys inherently act as integers, ensuring that map lookups execute with blazingly fast $O(1)$ or $O(\log n)$ time complexity, completely bypassing string comparison overhead.

## 3. Internal Storage Mechanics
* **Data Structure:** The internal vault must map the Enum keys to the concrete SFML resource objects (e.g., `sf::Texture`, `sf::Font`) using standard associative containers (like `std::unordered_map` or `std::map`).
* **Encapsulation:** The storage container must be strictly private. External classes cannot add, remove, or iterate over the assets directly.

## 4. Execution Flow: Loading vs. Retrieval

### Phase 1: The Loading Phase (Pre-computation)
* **Intent:** I/O operations (reading from the disk) are notoriously slow and must never occur during active gameplay. 
* **Mechanism:** The system must provide a specific `Load` method that takes an Enum ID and a file path string. This method is called during initialization or loading screens. It reads the file, stores it in the map under the provided Enum ID, and ensures it is ready for immediate use.

### Phase 2: The Retrieval Phase (Active Gameplay)
* **Intent:** Providing lightning-fast access to already loaded assets without redundant memory allocation.
* **Mechanism:** The system provides a `Get` method (e.g., `GetTexture`, `GetFont`) that accepts an Enum ID. It returns a `const` reference to the requested resource.
* **Failsafe:** If a system requests an ID that has not been loaded, the `ResourceManager` must throw a definitive runtime exception or assertion immediately. It must **never** attempt to load the file on the fly during the retrieval phase, as this causes catastrophic frame-drops (stuttering).

## 5. Coding Standards & Engine Manifesto
Implementation must strictly obey the **antigravity** core manifesto:
* **Memory Safety:** Prefer returning references (`&`) rather than raw pointers to avoid accidental null-pointer dereferencing, ensuring the caller knows the asset is guaranteed to exist if the function returns successfully.

## Implementation Plan

The proposed `ResourceManager` architecture is a standard, highly effective, and robust way to handle assets without the pitfalls of the Singleton pattern. 

To make this completely generic and strongly typed, I propose utilizing a C++ template class (e.g., `ResourceManager<Resource, Identifier>`). This ensures we don't have to duplicate the logic for Fonts, Textures, and Sounds.

### 1. Resource Identifiers
- Create `ResourceIdentifiers.h` to define the strong enums:
  - `enum class TextureID { Background, Title, ButtonUI, ... };`
  - `enum class FontID { MainFont, ... };`

### 2. ResourceManager Template
- Create `ResourceManager.h` defining the template class.
- **Internal Storage:** `std::unordered_map<Identifier, std::unique_ptr<Resource>> m_resourceMap;` (using `unique_ptr` guarantees resources aren't accidentally copied in memory).
- **Load Method:** Instantiates the resource, calls `loadFromFile`, and throws an `std::runtime_error` if the I/O fails.
- **Get Method:** Looks up the map and returns a `const Resource&`. Uses an `assert` or `throw` if the resource isn't found to guarantee fail-fast behavior.

### 3. Dependency Injection & Context
- Instantiate the Managers (e.g., `ResourceManager<sf::Texture, TextureID> textureManager;`) at the highest level (in `main.cpp` or the core engine wrapper).
- **Refactoring BaseState:** To prevent method signatures from becoming overwhelmingly large (passing 5 different managers), I propose creating a `struct Context` inside `BaseState.h` or `StateManager.h` containing references to the `StateManager`, `TextureManager`, etc. This `Context` will be injected into all States.
- Refactor `TitleState` to use the injected `Context` rather than loading "background.png" directly.

I am ready to proceed once you confirm this strategy!