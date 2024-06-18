For the future, if I were to expand the project I would add the following features/functionalities:

- A condition-checking class which for every event type can easily register a new condition.
- Currently because I'm using a header only library, the compilation time across just one compilation unit is slow. Because I'm including the Crow header file in several libraries, each compilation unit has to go through the same giant header file every time. The solution I would add in the future is c++20 modules. From cppreference:
```(...)Modules are a language feature to share declarations and definitions across translation units. They are an alternative to some use cases of headers(...)```
- Some conditions require persistent storage, like in the case of the repository creation/deletion event checker. The solution: database storage, storing events, event types, time of events, and so on.
