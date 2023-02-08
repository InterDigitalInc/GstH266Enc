#include "common.h"
#include "log.h"

#define LUMA_PADDING 8
#define CHROMA_PADDING LUMA_PADDING >> 1

#define UVG_LIBRARY_EXPORT

#if defined(UVG_LIBRARY_EXPORT) // inside DLL
#   define UVG_LIBRARY_API   __declspec(dllexport)
#else // outside DLL
#   define UVG_LIBRARY_API   __declspec(dllimport)
#endif  // XYZLIBRARY_EXPORT

#ifdef __cplusplus
extern "C"
{
#endif

    UVG_LIBRARY_API int uvg_init(H266Config *config);
    UVG_LIBRARY_API int uvg_start();
    UVG_LIBRARY_API int uvg_handle(H266Frame *frame);
    UVG_LIBRARY_API int uvg_stop();
    UVG_LIBRARY_API int uvg_flush();
    UVG_LIBRARY_API int uvg_close();

#ifdef __cplusplus 
}
#endif