### Future things I'd do, given more time
For the future, if I were to expand the project I would add the following features/functionalities:

## Easier condition adding
A condition-checking class which for every event type can easily register a new condition, perhaps even through the CLI. Alternatively, this could be a config file where the user sets which types of conditions are checked, in the format of a known pattern matching format like regex.

## Dispatcher
An internal dispatcher system, which upon event reception will notify other modules who expressed interest in the incoming event.

## C++20 Modules
Because I'm using a header only library, the compilation time across just one compilation unit is slow. Because I'm including the Crow header file in several libraries, each compilation unit has to go through the same giant header file every time. The solution I would add in the future is C++20 modules. From cppreference:

`(...)Modules are a language feature to share declarations and definitions across translation units. They are an alternative to some use cases of headers(...)`

## Database
Some conditions require persistent storage, like in the case of the repository creation/deletion event checker. The solution: database storage, storing events, event types, time of events, and so on.

## More generalized design
I noticed that the various webhook events have common traits, so I could likely generalize them into the interface class instead of having them in each subclass, e.g the "IsCreated" function. If there exists a class diagram of webhook events, I could use that as a reference.

## Support for both content types
For now I configured the webhook to work only with json, but there are two types, the other being x-www-form-urlencoded

## Store accurate time
I am storing the time of the creation of a repository as it arrives to my server, which may be several seconds after the event transpired. To store an accurate time I'd have to convert the timestamp into a TimePoint type, as opposed to calling the now() function.