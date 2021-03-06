#pragma once

//
// Some useful macros
//

#if defined _CS_LABS_DEBUG && _CS_LABS_DEBUG > 0

    #include <iostream>

    #define LOG(msg) std::cout << " -- " << msg << std::endl

    #define CATCH(type, expr)           \
    try { expr; } catch (type caught)   \
    { LOG("caught at " << __LINE__ << ": " << caught); throw 1; }

#else

    #define LOG(msg)

    #define CATCH(type, expr) expr

#endif
