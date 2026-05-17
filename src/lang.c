#include "srcdiff_stdutils.h"
#include "srcdiff.h"

#ifdef _WIN32
#   include <windows.h>
#   define  _sd_load_lib(lib)         LoadLibraryA  (lib)
#   define  _sd_load_libsym(lib, sym) GetProcAddress(lib, sym)
#   define  _sd_free_lib(lib)         FreeLibrary   (lib)
    typedef HMODULE SDTSModule; // genuinely microslop what were you thinking when naming these types
    typedef FARPROC SDTSSym;    // the fuck is a FARPROC
#else
#   include <dlfcn.h>
#   define  _sd_load_lib(lib)         dlopen (lib, RTLD_LAZY | RTLD_LOCAL)
#   define  _sd_load_libsym(lib, sym) dlsym  (lib, sym)
#   define  _sd_free_lib(lib)         dlclose(lib)
    typedef void* SDTSModule;
    typedef void* SDTSSym;
#endif // _WIN32

// the standard function signature for a tree sitter grammar that produces a TSLanguage object
typedef const TSLanguage *(*SDTSLanguageProducer)(void);

typedef struct {
    size_t             count;      // the length of the arrays
    SDTSModule        *ts_modules; // array of handles to tree sitter grammar modules
    const char       **lang_names; // array of pointers to lut keys
    const TSLanguage **ts_langs;   // array of pointers to tree sitter language object
} SDLanguageRegistry;

SDPRIVATE
const SDLanguageRegistry *sd_get_langreg(void) {
    SDPRIVATE SDLanguageRegistry langreg = {0};
    SDPRIVATE bool               available = false;

    if (available)
        goto defer;

    // we decide what languages to load depending on the LUT configuration
    SDLut *lut  = sd_get_lut();
    size_t keyc = sd_lut_key_count(lut);
    langreg = (SDLanguageRegistry) {
        .count      = 0,
        .ts_modules = malloc(sizeof(SDTSModule)  * keyc),
        .lang_names = malloc(sizeof(char*)       * keyc),
        .ts_langs   = malloc(sizeof(TSLanguage*) * keyc)
    };

    sd_set_scope_binary();
    struct dirent *entry;
    DIR *dir = opendir(SRCDIFF_LANGSTORE);
    if (dir == NULL) {
        sd_log_warn("no languages registered - no languages to load");
        goto defer;
    }

    sd_chdir(SRCDIFF_LANGSTORE);
    while ((entry = readdir(dir)) != NULL) {
        char *dot = strrchr(entry->d_name, '.');

        (*dot) = '\0';
        if (!sd_lut_has_key(lut, entry->d_name))
            continue;

        (*dot) = '.';
        SDTSModule module = _sd_load_lib(entry->d_name);
        if (module == NULL) {
            sd_log_warn("unable to load the language parser '%s'", entry->d_name);
            continue;
        }

        (*dot) = '\0';
        const char *lang_name = sd_lut_key_at(lut, sd_lut_key_idx(lut, entry->d_name));

        char symbuf[12+32+1]; // tree_sitter_[32]\0
        snprintf(symbuf, 12+32, "tree_sitter_%s", lang_name);
        SDTSSym fnsym = _sd_load_libsym(module, symbuf);
        if (fnsym == NULL) {
            sd_log_warn(
                "'%s' tree sitter module does not contain the regular tree_sitter_<name> function that is expected "
                "- this parser is invalid",
                lang_name
            );
            _sd_free_lib(module);
            continue;
        }

        size_t regidx = langreg.count++;
        langreg.ts_modules[regidx] = module;
        langreg.lang_names[regidx] = lang_name;
        langreg.ts_langs  [regidx] = ((SDTSLanguageProducer) (uintptr_t) fnsym)();
    }

defer:
    sd_set_scope_user();
    return &langreg;
}

/// Loads the language specified by the `lang_name`
SDPUBLIC
const TSLanguage *sd_load_language(const char *lang_name) {
    assert(lang_name != NULL);

    const SDLanguageRegistry *langreg = sd_get_langreg();
    for (size_t idx = 0; idx < langreg->count; idx++) {
        if (strcmp(lang_name, langreg->lang_names[idx]) != 0)
            continue;

        return langreg->ts_langs[idx];
    }
    return NULL;
}
