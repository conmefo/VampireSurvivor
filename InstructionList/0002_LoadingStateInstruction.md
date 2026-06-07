Act as a Senior C++ Game Developer. We are building a Vampire Survivors clone using SFML and an OOP-based State Pattern architecture.

**Task Overview:** We have just established the core backbone of our State Machine. Now, it is time to implement the **LoadingState** — the very first screen our game encounters.

**1. File Requirements:** Please implement these two files in the `src/States/Intro/` directory:

- `LoadingState.h`
- `LoadingState.cpp`

**2. Functional Requirements:**

- `LoadingState` must publicly inherit from `BaseState`.
- **`Init()`**: Initialize a simple `sf::Text` object displaying "LOADING... 0%" in the center of the screen.
- **`HandleInput()`**: Since this is a passive loading screen, this method can be empty, but must be defined.
- **`Update(float dt)`**:
  - Increment a local float `loadingProgress` (0.0 to 100.0) based on `dt`.
  - Update the `sf::Text` string to reflect the percentage (e.g., "LOADING... 45%").
  - If `loadingProgress >= 100.0f`, call `stateManager.AddState(std::make_unique<WarningState>())` (Assume `WarningState` is defined later, or just provide a placeholder comment for the transition logic).
- **`Draw(sf::RenderWindow& window)`**: Simply draw the `sf::Text` object onto the window.

**3. Strict Coding Style Rules:**

- **Allman Style:** Braces on new lines.
- **No Space after Keywords:** `if(condition)`, `while(running)`, `for(int i=0;...)`. Never use spaces like `if (condition)`.
- **Modern C++:** Use `std::unique_ptr`, `override` keyword for virtual methods, and `#pragma once`.

**4. Context:**

- `BaseState.h` provides the interface: `virtual void Init() = 0;`, `virtual void HandleInput(sf::Event& event) = 0;`, `virtual void Update(float dt) = 0;`, and `virtual void Draw(sf::RenderWindow& window) = 0;`.

Please provide the complete implementations. Ensure the code is production-ready, clean, and follows the specified naming conventions.