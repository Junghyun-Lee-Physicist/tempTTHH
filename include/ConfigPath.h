#ifndef TTHH_CONFIGPATH_H
#define TTHH_CONFIGPATH_H
// ============================================================================
//  ConfigPath.h — correction-input path resolution policy (STEP18)
// ----------------------------------------------------------------------------
//  WHY: the old behaviour silently fell back to a hard-coded default path when
//       a path was blank ("비우면 코드 default"). That hides mis-configuration.
//       New contract (single source of truth; submitter enforces the mirror):
//
//    config value (yml common.path_*)      env passed to analyzer      meaning
//    -----------------------------------   ------------------------    ---------------
//    "<real path>"                          "<real path>"               use it; load-or-FATAL
//    null                                   "__NULL__" (sentinel)       OPTIONAL → disabled (skip)
//                                                                       REQUIRED → FATAL E13
//    "" / missing                           (rejected at submit time)   FATAL E12 if ever seen here
//
//  There is NO code-default fallback. The submitter ALWAYS exports an explicit
//  value (a real path, or the "__NULL__" sentinel for an optional disabled
//  correction); an unset/empty env therefore indicates a wrapper/config bug and
//  is fatal. resolve() returns "" ONLY for the optional+disabled case, which the
//  caller must treat as "do not load this correction".
// ============================================================================
#include <cstdlib>   // std::getenv, std::exit
#include <cstring>
#include <string>
#include <iostream>

#include "ExitCodes.h"

namespace cfgpath {

inline constexpr const char* kNullSentinel = "__NULL__";

// Resolve one correction-input path from its environment variable.
//   envName  : e.g. "TTHH_TRIGSF_DIR"
//   what     : human label for logs, e.g. "trigger SF dir"
//   required : true  -> null/empty/unset is fatal (correction is mandatory)
//              false -> null means "intentionally absent" (returns "")
// Returns the resolved path, or "" iff (optional AND explicitly null).
inline std::string resolve(const char* envName, const char* what, bool required) {
    const char* raw = std::getenv(envName);
    std::string v = raw ? std::string(raw) : std::string();
    // trim surrounding whitespace
    const auto b = v.find_first_not_of(" \t\r\n");
    const auto e = v.find_last_not_of(" \t\r\n");
    v = (b == std::string::npos) ? std::string() : v.substr(b, e - b + 1);

    if (v.empty()) {
        std::cerr << "\n[FATAL][E" << tthh::CONFIG_PATH_ENV_MISSING << "] "
                  << what << ": environment variable " << envName
                  << " is unset/empty.\n"
                  << "  The submitter must export every path explicitly (a real path,\n"
                  << "  or \"" << kNullSentinel << "\" to disable an optional correction).\n"
                  << "  Blank is no longer a 'use default' signal. Aborting (exit "
                  << tthh::CONFIG_PATH_ENV_MISSING << ").\n" << std::endl;
        std::exit(tthh::CONFIG_PATH_ENV_MISSING);
    }

    if (v == kNullSentinel) {
        if (required) {
            std::cerr << "\n[FATAL][E" << tthh::CONFIG_PATH_NULL_REQUIRED << "] "
                      << what << ": this correction is REQUIRED for the current "
                      << "mode/sample but the config set its path to null (" << envName
                      << ").\n  Provide a real path, or change the mode/SF toggle. "
                      << "Aborting (exit " << tthh::CONFIG_PATH_NULL_REQUIRED << ").\n"
                      << std::endl;
            std::exit(tthh::CONFIG_PATH_NULL_REQUIRED);
        }
        std::cout << "[cfgpath] " << what << " = (disabled; config null via " << envName
                  << ")" << std::endl;
        return std::string();  // optional + disabled
    }

    std::cout << "[cfgpath] " << what << " = " << v << "   (env " << envName << ")"
              << std::endl;
    return v;
}

}  // namespace cfgpath

#endif // TTHH_CONFIGPATH_H
