// http://www.qtcentre.org/wiki/index.php?title=Memory_Leak_Detection_in_VS
// http://www.rawmaterialsoftware.com/viewtopic.php?f=3&t=6475
#ifdef _CRTDBG_MAP_ALLOC
#pragma pop_macro("calloc")
#pragma pop_macro("malloc")
#pragma pop_macro("realloc")
#pragma pop_macro("free")
#endif
