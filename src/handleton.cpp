#define __HANDLETON_OWNER__

#include "handleton.hpp"

#include "logger.hpp"          // ilrd::Logger
#include "cli.hpp"             // CLI
#include "event_handler.hpp"   // EventHandler

namespace ilrd
{

template class Handleton<Logger>;

template class Handleton<EventHandler>;

template class Handleton<CLI>;

} // ilrd