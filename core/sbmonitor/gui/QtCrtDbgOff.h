// http://www.qtcentre.org/wiki/index.php?title=Memory_Leak_Detection_in_VS
// http://www.rawmaterialsoftware.com/viewtopic.php?f=3&t=6475
#ifdef _CRTDBG_MAP_ALLOC
#pragma push_macro("calloc")
#pragma push_macro("malloc")
#pragma push_macro("realloc")
#pragma push_macro("free")
#undef calloc
#undef malloc
#undef realloc
#undef free
#endif
