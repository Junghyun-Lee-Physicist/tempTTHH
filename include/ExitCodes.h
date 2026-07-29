#ifndef TTHH_EXITCODES_H
#define TTHH_EXITCODES_H
// ============================================================================
//  ExitCodes.h  — canonical analyzer exit-code table (single source of truth)
// ----------------------------------------------------------------------------
//  WHY: every essential-logic failure must terminate the process with a
//       *distinct, documented* exit code so a Condor job is flagged and the
//       cause is identifiable from the job log alone (grep "[FATAL][E").
//
//  CONTRACT (see docs/reference/ERROR_CODES.md — keep both in sync):
//    - 0            success
//    - 10-19        configuration / CLI / path-wiring
//    - 20-29        normalization inputs (xsec_db, prescan)        [submitter too]
//    - 30-39        input data (ntuple / TTree)
//    - 40-49        central (POG) corrections (JME/PU/BTag-SF/Golden)
//    - 50-59        derived corrections (trigger SF, b-tag norm reweight)
//    - 60-69        ttbar stitching
//    - 70-79        Expanded_genTtbarId (tt+nb) lookup
//    - 80-89        per-event physics integrity
//
//  Numbers are STABLE: never reassign a meaning. To add a failure, take the
//  next free number in the right band and document it in ERROR_CODES.md.
//  The Python submitter mirrors the 10-29 band (submit_job_FH_Tier3_unified.py).
// ============================================================================

namespace tthh {
enum ExitCode {
    OK                       = 0,

    // 10-19 — configuration / CLI / path wiring
    CONFIG_BAD_MODE          = 10,  // unknown / removed analysis mode
    CONFIG_BAD_RUNINFO       = 11,  // runYear / DataOrMC / sampleName not set
    CONFIG_PATH_ENV_MISSING  = 12,  // required path env unset/empty (no default)
    CONFIG_PATH_NULL_REQUIRED= 13,  // required path explicitly set to null

    // 20-29 — normalization inputs
    XSEC_DB_MISSING          = 20,  // sample absent from xsec_db
    PRESCAN_MISSING          = 21,  // sample absent from prescan_summary

    // 30-39 — input / output data
    INPUT_OPEN_FAIL          = 30,  // cannot read input ntuple / "Events" tree
    OUTPUT_OPEN_FAIL         = 31,  // cannot create the output TFile (--output)

    // 40-49 — central (POG) corrections
    CENTRAL_CORR_LOAD_FAIL   = 40,  // JME/PU/BTag-SF correctionlib load failed
    GOLDENJSON_DATA_MISSING  = 41,  // Data lumi-mask (golden JSON) missing

    // 50-59 — derived corrections
    TRIGSF_LOAD_FAIL         = 50,  // trigger SF JSON missing/corrupt (required)
    BTAGRW_LOAD_FAIL         = 51,  // b-tag norm reweight JSON missing/corrupt (req.)

    // 60-69 — ttbar stitching
    STITCH_JSON_OPEN_FAIL    = 60,  // stitch-factors JSON cannot be opened
    STITCH_JSON_PARSE_FAIL   = 61,  // stitch-factors JSON parse error
    STITCH_JSON_SCHEMA_FAIL  = 62,  // stitch-factors JSON schema/contents invalid
    STITCH_EXPTTID_INACTIVE  = 63,  // sample in stitch plan but tt+nb lookup inactive

    // 70-79 — Expanded_genTtbarId (tt+nb) lookup
    EXPTTID_CHAIN_FAIL       = 70,  // cannot add ttnb_<sample>.root to chain
    EXPTTID_TREE_EMPTY       = 71,  // lookup tree has 0 entries
    EXPTTID_DUP_KEY          = 72,  // conflicting duplicate (run,lumi,event) key
    EXPTTID_SAMPLE_MISMATCH  = 73,  // lookup does not correspond to this sample

    // 80-89 — per-event physics integrity
    PROCESSKEY_EMPTY         = 80,  // MakeProcessKey() returned an empty key
    REWEIGHT_NONFINITE       = 81   // non-finite b-tag normalization reweight
};
}  // namespace tthh

#endif // TTHH_EXITCODES_H
