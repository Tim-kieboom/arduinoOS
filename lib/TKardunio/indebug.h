// To enable assertions (debug mode), leave the line below uncommented.
// To disable assertions (release mode), comment out the line below.
#define ENABLE_TK_DEBUGGING

#ifdef ENABLE_TK_DEBUGGING
#define IF_DEBUG(body) body
#else
#define IF_DEBUG(body) body
#endif