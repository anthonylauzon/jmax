
#if defined(DECLARE_VEC_FUN)
#undef DECLARE_VEC_FUN
#endif

#define DECLARE_VEC_FUN(class, args)\
 DECLARE_FTL_VEC_ ## class ## args\
 DECLARE_FTL_VECX_ ## class ## args
