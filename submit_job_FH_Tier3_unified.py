#!/usr/bin/env python3
###############################################################################
# submit_job_FH_Tier3_unified.py
# -----------------------------------------------------------------------------
# Tier3 condor job submitter for the ttHH(4b) FH analyzer.
#
# Modes:
#   main, btagtrig, trigsf  → unchanged behaviour: iterates over `samples:`
#                              once and submits one batch per (sample, file).
#   validation              → NEW: iterates over `validation_scenarios:`
#                              AND `samples:`. Each scenario produces an
#                              independent output_dir suffix and forwards
#                              the per-scenario flags as extra C++ argv.
#
# YAML expectations:
#   - For all modes: `common.year`, `common.analysis_mode`, `samples`.
#   - For validation mode: also `validation_scenarios` (list of dicts).
#
# Output directory naming:
#   non-validation: <path_output_base>/<sample.output_dir>/
#   validation    : <path_output_base>/<scenario.name>/<sample.output_dir>/
###############################################################################

import argparse
import os
import sys
import time
import re
import shlex
import datetime
import subprocess

script_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(script_dir, 'python', 'ttHHmodules'))
import ProxyChecker
import resultChecker  # noqa: F401  (kept for backward compat)


# ============================================================================
# [STEP18] Canonical exit codes (mirror of include/ExitCodes.h, 10-29 band).
#   Numbered so a failed *submit* is identifiable from the log (grep "[FATAL][E").
#   Full table: docs/reference/ERROR_CODES.md (keep in sync with ExitCodes.h).
# ============================================================================
EXIT = {
    "CONFIG_PATH_ENV_MISSING":   12,   # required/empty correction path (no default)
    "CONFIG_PATH_NULL_REQUIRED": 13,   # required correction path set to null
    "XSEC_DB_MISSING":           20,   # sample absent from xsec_db
    "PRESCAN_MISSING":           21,   # sample absent/invalid in prescan_summary
}
NULL_SENTINEL = "__NULL__"             # yml null -> this sentinel reaches the analyzer

def _fatal(code, msg):
    """Print a numbered FATAL line and exit with that code (log-detectable)."""
    sys.stderr.write(f"\n[FATAL][E{code}] {msg}\n")
    sys.exit(code)


class CondorJobManager:

    def __init__(self):

        self.time_info = time.strftime("%Y%m%d-%H%M%S")

        # ── [STEP7] CLI 인자 — 코드 내 상수 수정 없이 운영을 제어 ──────────
        #   --mode M            : main/btagtrig/prescan/debug (default main)
        #   --files-per-job N   : job당 입력 파일 수
        #                         (미지정: yml common.files_per_job, 그것도 없으면 1)
        #   --resubmit          : 완료 output은 skip, 미완료 job만 재제출
        #   --resubmit-to SUB   : 재제출 output을 <sample>/<SUB>/ 하위로 분리 저장
        #   --report            : 제출 없이 샘플별 완료/미완료 현황만 출력
        parser = argparse.ArgumentParser(
            description="ttHH FH condor submitter — [STEP7] master filelist 자동 "
                        "분할, files-per-job, 실패 감지/재제출/리포트")
        parser.add_argument("--mode", default="main",
                            choices=["main", "btagtrig", "prescan", "debug"])
        parser.add_argument("--files-per-job", type=int, default=None,
                            help="job당 입력 파일 수 (기본: yml common.files_per_job 또는 1)")
        parser.add_argument("--resubmit", action="store_true",
                            help="완료 output은 skip, 미완료만 재queue")
        parser.add_argument("--resubmit-to", default="",
                            help="재제출 output을 별도 하위 디렉토리에 저장 (--resubmit 전용)")
        parser.add_argument("--report", action="store_true",
                            help="[STEP20] 제출 없이 완료/미완료 요약 테이블만 "
                                 "(sample | jobs | done | miss | done%%). "
                                 "read-only — 디렉토리/파일 생성·수정 없음")
        parser.add_argument("--status", action="store_true",
                            help="[STEP20] 제출 없이 샘플별 상세 현황 "
                                 "(weight, missing idx 목록 포함; 구 --report "
                                 "동작). read-only")
        parser.add_argument("--config", default="",
                            help="yml 경로 명시 (미지정: "
                                 "AnalyzerConfig/Tier3_2017_FH_unified_<mode>.yml)")
        parser.add_argument("--region", default="",
                            choices=["", "muon", "electron"],
                            help="[lepton-CR] QCD 억제 1ℓ+MET 제어영역 "
                                 "(기본: FH lepton veto). yml common.region 도 가능")
        # [SF toggle] production evtWeight 에 각 SF 적용 여부. 기본: trig on, 나머지 off.
        parser.add_argument("--trigsf", default="on",  choices=["on", "off"],
                            help="trigger SF 적용 (기본 on)")
        parser.add_argument("--btagsf", default="off", choices=["on", "off"],
                            help="b-tag shape SF 적용 (기본 off)")
        parser.add_argument("--btagrw", default="off", choices=["on", "off"],
                            help="b-tag norm reweight 적용 (기본 off; 8-group JSON 필요)")
        parser.add_argument("--filelist-dir", default="filelistTier3",
                            help="master filelist 디렉토리 (기본 filelistTier3). "
                                 "연도별로 분리된 경우 지정: 예) --filelist-dir filelistTier3_2018 "
                                 "(make_filelists.py 2018 이 만드는 디렉토리)")
        parser.add_argument("--preflight", action="store_true",
                            help="읽기 전용 사전 점검: yml/xsec_db/prescan_summary/"
                                 "filelist/실행파일/보정 경로/era 추출/output 디렉토리 "
                                 "쓰기권한을 검사하고 로그를 남긴 뒤 종료 "
                                 "(제출·디렉토리 생성 없음, FAIL 있으면 exit 1)")
        args = parser.parse_args()
        self.preflight_only = bool(args.preflight)
        self._cli_config = args.config.strip()
        self._cli_region = args.region.strip()
        self._cli_trigsf  = args.trigsf
        self._cli_btagsf  = args.btagsf
        self._cli_btagrw  = args.btagrw

        # Variables for jobs, please check before running
        self.analyzer_path = f"{script_dir}"
        self.nameofExe = "ttHHanalyzer_unified"
        self.AnalyzerMode = args.mode  # main / btagtrig / prescan / debug ([STEP2] trigsf·validation 제거)

        # ── Resubmit / report control ([STEP7] CLI로 제어) ─────────────────
        self.resubmit_only = bool(args.resubmit)
        self.resubmit_to   = args.resubmit_to.strip().strip("/")
        # [STEP20] 조회 모드 2종 — 내부적으로는 report_only 하나로 묶고
        #   (기존 report_only 분기 전부 재사용: 제출 없음 + read-only),
        #   report_verbose 로 출력 형태만 가른다.
        #     --status : verbose (샘플별 [weight]/[report] 라인 + missing idx)
        #     --report : compact (마지막에 요약 테이블 한 장)
        # 두 모드 모두 마지막에 테이블은 출력한다 (status = 상세 + 테이블).
        self.report_only    = bool(args.report) or bool(args.status)
        self.report_verbose = bool(args.status)
        if bool(args.report) and bool(args.status):
            raise ValueError("[FATAL] --report 와 --status 는 동시 사용 불가")
        if self.report_only and self.resubmit_only:
            raise ValueError("[FATAL] --report/--status 는 --resubmit 과 동시 사용 불가")
        self._report_rows = []   # [STEP20] (sample, jobs, done, miss) 누적 → 테이블
        self.cli_files_per_job = args.files_per_job
        if self.resubmit_to and not self.resubmit_only:
            raise ValueError("[FATAL] --resubmit-to 는 --resubmit 과 함께만 사용 가능")

        # [lepton-CR + SF toggle] output 디렉토리에 region + SF 조합 반영.
        # FH/muon/electron, 그리고 SF on/off 조합이 서로 덮어쓰지 않게 suffix.
        #   region : _muon / _electron (없으면 생략)
        #   SF     : 기본(trig on, btag off, rw off)이면 생략; 벗어난 것만 태그
        #            trig off -> _notrig ; btagsf on -> _btagsf ; btagrw on -> _btagrw
        _region_suffix = f"_{self._cli_region}" if self._cli_region else ""
        _sf_tags = []
        if self._cli_trigsf == "off": _sf_tags.append("notrig")
        if self._cli_btagsf == "on":  _sf_tags.append("btagsf")
        if self._cli_btagrw == "on":  _sf_tags.append("btagrw")
        _sf_suffix = ("_" + "_".join(_sf_tags)) if _sf_tags else ""
        self._dir_suffix = f"{_region_suffix}{_sf_suffix}"
        self.path_output_base = (
            f"/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/"
            f"AnalyzerOutput_{self.AnalyzerMode}{self._dir_suffix}"
        )
        self.os_version = "el9"
        self.memorySize = "12 GB"

        # [TrackC] --config 우선; 미지정 시 모드 이름으로 자동 결정 (기존 동작)
        if getattr(self, "_cli_config", ""):
            self.config_file_path = (self._cli_config if os.path.isabs(self._cli_config)
                                     else os.path.join(self.analyzer_path, self._cli_config))
        else:
            self.config_file_path = os.path.join(
                self.analyzer_path,
                f"AnalyzerConfig/Tier3_2017_FH_unified_{self.AnalyzerMode}.yml")
        self.proxy_path = os.path.join(self.analyzer_path, "proxy.cert")
        self.condor_files_path = os.path.join(
            self.analyzer_path,
            f"condor/filelistTier3_unified_{self.AnalyzerMode}{self._dir_suffix}"
        )
        self.sample_list_path = os.path.join(self.analyzer_path,
                                             args.filelist_dir.strip() or "filelistTier3")

        # ── [--preflight] 읽기 전용 사전 점검 ────────────────────────────────
        # 디렉토리 생성/proxy 체크/제출을 일절 하지 않고, 필요한 것들이 갖춰졌는지만
        # 검사한 뒤 로그 파일을 남기고 종료한다 (FAIL 있으면 exit 1).
        if self.preflight_only:
            sys.exit(self.run_preflight())

        self.make_directory(self.condor_files_path)
        self.make_directory(self.path_output_base, 777)

        # Proxy
        try:
            print(f"Setted Path of proxy : {self.proxy_path}")
            proxy_checker = ProxyChecker.ProxyChecker(self.proxy_path)
            proxy_checker.check()
        except Exception as e:
            print(f"{e}")
            sys.exit(1)

        self.print_memory_status()
        self.process_config_file()

    # -------------------------------------------------------------------------
    def run_preflight(self):
        """[--preflight] 제출 전 읽기 전용 점검. 반환값 = exit code (0/1).

        아무것도 만들지 않고(디렉토리·filelist·condor 파일 없음) 아무것도 제출하지
        않는다. 결과는 화면과 preflight_<mode><suffix>_<timestamp>.log 에 동시 기록
        되므로 그대로 붙여 공유할 수 있다.
        """
        import json          # 이 파일의 기존 관례(지역 import)를 따른다
        rows = []   # (level, check, detail)
        lines = []

        def emit(level, check, detail=""):
            rows.append((level, check, detail))
            line = f"[{level:<4}] {check:<40} {detail}"
            print(line)
            lines.append(line)

        def note(text=""):
            print(text)
            lines.append(text)

        ok   = lambda c, d="": emit("PASS", c, d)
        warn = lambda c, d="": emit("WARN", c, d)
        bad  = lambda c, d="": emit("FAIL", c, d)

        ts = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        log_path = os.path.join(
            self.analyzer_path,
            f"preflight_{self.AnalyzerMode}{self._dir_suffix}_{ts}.log")

        note("=" * 84)
        note("tempTTHH condor PREFLIGHT (read-only)")
        note(f"  mode        : {self.AnalyzerMode}{self._dir_suffix}")
        note(f"  config      : {self.config_file_path}")
        note(f"  analyzer    : {self.analyzer_path}")
        note(f"  SF toggles  : trigsf={self._cli_trigsf} btagsf={self._cli_btagsf} btagrw={self._cli_btagrw}")
        note(f"  time        : {datetime.datetime.now().isoformat(timespec='seconds')}")
        note("=" * 84)

        # ---- 1. config yml --------------------------------------------------
        if not os.path.isfile(self.config_file_path):
            bad("config yml", f"not found: {self.config_file_path}")
            return self._preflight_finish(rows, lines, log_path)
        ok("config yml", self.config_file_path)
        # 주의: 이 저장소는 PyYAML 의존을 피하려고 자체 파서를 쓴다
        # (load_yaml_config). 점검도 실제 제출과 **같은 파서**로 해야 의미가 있다.
        try:
            cfg = self.load_yaml_config(self.config_file_path)
        except Exception as e:
            bad("config yml parse", str(e).replace("\n", " ")[:150])
            return self._preflight_finish(rows, lines, log_path)
        ok("config yml parse", "ok (load_yaml_config, 제출 때와 동일 파서)")

        common  = (cfg or {}).get("common", {}) or {}
        samples = (cfg or {}).get("samples", []) or []

        mode_yml = str(common.get("analysis_mode", "")).strip()
        if mode_yml and mode_yml != self.AnalyzerMode:
            bad("analysis_mode vs --mode",
                f"yml='{mode_yml}' but --mode='{self.AnalyzerMode}' (mismatched config?)")
        else:
            ok("analysis_mode vs --mode", f"{mode_yml or '(unset)'} == {self.AnalyzerMode}")
        is_prescan = (mode_yml == "prescan")

        for key in ("year", "lumi_fb_inv", "xsec_db", "prescan", "files_per_job"):
            if common.get(key) not in (None, ""):
                ok(f"common.{key}", str(common[key]))
            else:
                bad(f"common.{key}", "missing or empty")
        if not samples:
            bad("samples", "empty list -- nothing to submit")
            return self._preflight_finish(rows, lines, log_path)
        ok("samples", f"{len(samples)} entries")

        # ---- 2. executable --------------------------------------------------
        exe = os.path.join(self.analyzer_path, self.nameofExe)
        if os.path.isfile(exe) and os.access(exe, os.X_OK):
            ok("analyzer executable", exe)
        else:
            bad("analyzer executable",
                f"not built/executable: {exe} (run `make` first)")

        # ---- 3. correction paths (null 정책) --------------------------------
        # "" 또는 키 누락 -> E12, null -> 선택 보정 비활성(필수면 E13)
        required_now = {"path_jsonpog"}
        if any(str(s).startswith(("JetHT", "BTagCSV", "SingleMuon")) for s in samples):
            required_now.add("path_goldenjson")
        if self.AnalyzerMode in ("main", "debug"):
            if self._cli_trigsf == "on":
                required_now.add("path_trigsf_dir")
            if self._cli_btagrw == "on":
                required_now.add("path_btag_reweight_json")
        for key in ("path_jsonpog", "path_goldenjson", "path_trigsf_dir",
                    "path_btag_reweight_json", "path_stitch_json",
                    "path_expanded_ttbarid_dir"):
            if key not in common:
                bad(f"{key}", "KEY MISSING -> analyzer exits E12 (no silent default)")
                continue
            val = common[key]
            is_null = (val is None) or (str(val).strip().lower() in ("none", "null", "~"))
            if str(val).strip() == "":
                bad(f"{key}", 'empty string -> E12 (use null to disable explicitly)')
            elif is_null:
                if key in required_now:
                    bad(f"{key}", f"null but REQUIRED for mode={self.AnalyzerMode} "
                                  f"(trigsf={self._cli_trigsf}, btagrw={self._cli_btagrw}) -> E13")
                else:
                    ok(f"{key}", "null -> disabled (ok for this mode/toggles)")
            else:
                p = str(val)
                exists = os.path.exists(p)
                if exists:
                    ok(f"{key}", p)
                elif p.startswith("/cvmfs"):
                    warn(f"{key}", f"not visible here (cvmfs not mounted?): {p}")
                else:
                    bad(f"{key}", f"path does not exist: {p}")

        # ---- 4. xsec_db / prescan_summary ----------------------------------
        db = {}
        db_path = os.path.join(self.analyzer_path, str(common.get("xsec_db", "")))
        if os.path.isfile(db_path):
            try:
                with open(db_path) as f:
                    db = json.load(f)
                meta = db.get("_meta", {})
                ok("xsec_db loaded",
                   f"{db_path} ({len(db) - (1 if '_meta' in db else 0)} samples"
                   + (f", era={meta.get('era')}, lumi={meta.get('lumi_fb_inv')}" if meta else "") + ")")
                if meta.get("lumi_fb_inv") and str(meta["lumi_fb_inv"]) != str(common.get("lumi_fb_inv")):
                    warn("lumi consistency",
                         f"yml lumi_fb_inv={common.get('lumi_fb_inv')} != xsec_db _meta={meta['lumi_fb_inv']}")
                else:
                    ok("lumi consistency", str(common.get("lumi_fb_inv")))
            except Exception as e:
                bad("xsec_db parse", str(e)[:150])
        else:
            (warn if is_prescan else bad)(
                "xsec_db", f"not found: {db_path}"
                + (" (prescan does not need it)" if is_prescan else ""))

        prec = {}
        pre_path = os.path.join(self.analyzer_path, str(common.get("prescan", "")))
        if os.path.isfile(pre_path):
            try:
                with open(pre_path) as f:
                    # 실제 로더(_load_prescan)와 동일하게 top-level "samples" 아래를 본다
                    prec = json.load(f).get("samples", {})
                ok("prescan_summary loaded", f"{pre_path} ({len(prec)} samples)")
            except Exception as e:
                bad("prescan_summary parse", str(e)[:150])
        else:
            (ok if is_prescan else bad)(
                "prescan_summary",
                f"absent: {pre_path}" + (" (expected: prescan CREATES it)" if is_prescan
                                         else " -- run prescan first"))

        # ---- 5. per-sample: filelist / era / xsec / prescan -----------------
        note("-" * 84)
        note(f"Per-sample checks ({len(samples)} samples; filelist dir = {self.sample_list_path})")
        miss_fl, empty_fl, miss_db, miss_pre, era_fail, n_data, n_mc = [], [], [], [], [], 0, 0
        total_files = 0
        for s in samples:
            name = s["sample_name"] if isinstance(s, dict) else str(s)
            fl = os.path.join(self.sample_list_path, f"filelist_{name}.txt")
            if not os.path.isfile(fl):
                miss_fl.append(name)
            else:
                n = sum(1 for l in open(fl) if l.strip())
                total_files += n
                if n == 0:
                    empty_fl.append(name)
            is_data = bool(re.search(r"(JetHT|BTagCSV|SingleMuon|EGamma|MuonEG|DoubleMuon)", name))
            if is_data:
                n_data += 1
                if not (re.search(r"Run\d{4}([A-Z])$", name) or re.search(r"_([A-Z])$", name)):
                    era_fail.append(name)
            else:
                n_mc += 1
                if not is_prescan:
                    rec = db.get(name)
                    if rec is None:
                        miss_db.append(name)
                    elif rec.get("cross_section_fb") in (None, 0):
                        miss_db.append(name + "(xsec null)")
                    if name not in prec:
                        miss_pre.append(name)
                    else:
                        try:
                            if float(prec[name]["runs"]["genEventSumw"]) <= 0:
                                miss_pre.append(name + "(sumw<=0)")
                        except Exception:
                            miss_pre.append(name + "(malformed)")
        ok("MC / Data in config", f"{n_mc} MC, {n_data} Data")
        if miss_fl:
            bad("filelists present", f"{len(miss_fl)} missing (run make_filelists.py): {miss_fl[:6]}")
        else:
            ok("filelists present", f"all {len(samples)} found, {total_files} input files total")
        if empty_fl:
            bad("filelists non-empty", f"{empty_fl[:6]}")
        elif not miss_fl:
            ok("filelists non-empty", "ok")
        fpj = self.cli_files_per_job or int(common.get("files_per_job", 1) or 1)
        if total_files:
            ok("job count estimate",
               f"~{-(-total_files // max(fpj,1))} jobs (files={total_files}, files-per-job={fpj})")
        if era_fail:
            bad("Data era extraction", f"cannot derive era for: {era_fail[:6]} "
                                       f"(need '<PD>_Run<year><ERA>' or '<PD>_<ERA>')")
        elif n_data:
            ok("Data era extraction", f"ok for all {n_data} Data samples")
        if not is_prescan:
            if miss_db:
                bad("xsec_db coverage", f"{len(miss_db)} problem(s): {miss_db[:6]}")
            elif db:
                ok("xsec_db coverage", f"all {n_mc} MC samples present with non-null xsec")
            if miss_pre:
                bad("prescan coverage", f"{len(miss_pre)} problem(s): {miss_pre[:6]}")
            elif prec:
                ok("prescan coverage", f"all {n_mc} MC samples have runs.genEventSumw > 0")
        else:
            ok("xsec_db / prescan coverage", "not required in prescan mode (weight=1.0)")

        # ---- 6. output / condor dirs (생성하지 않고 확인만) -----------------
        note("-" * 84)
        for label, path in (("condor dir", self.condor_files_path),
                            ("output base", self.path_output_base)):
            if os.path.isdir(path):
                if os.access(path, os.W_OK):
                    ok(f"{label} (exists)", path)
                else:
                    bad(f"{label} not writable", path)
            else:
                parent = os.path.dirname(path.rstrip("/"))
                while parent and not os.path.isdir(parent):
                    parent = os.path.dirname(parent)
                if parent and os.access(parent, os.W_OK):
                    ok(f"{label} (will be created)", f"{path}  [parent writable: {parent}]")
                else:
                    bad(f"{label} not creatable", f"{path}  [no writable parent: {parent}]")
        proxy = self.proxy_path
        if os.path.isfile(proxy):
            age_h = (time.time() - os.path.getmtime(proxy)) / 3600.0
            (ok if age_h < 24 else warn)("condor proxy.cert",
                                         f"{proxy} (age {age_h:.1f} h)")
        else:
            bad("condor proxy.cert",
                f"not found: {proxy} -- copy your grid proxy there (worker nodes need it)")

        return self._preflight_finish(rows, lines, log_path)

    # -------------------------------------------------------------------------
    def _preflight_finish(self, rows, lines, log_path):
        n_fail = sum(1 for l, _, _ in rows if l == "FAIL")
        n_warn = sum(1 for l, _, _ in rows if l == "WARN")
        n_pass = sum(1 for l, _, _ in rows if l == "PASS")
        tail = ["-" * 84,
                f"PREFLIGHT SUMMARY: {n_pass} PASS, {n_warn} WARN, {n_fail} FAIL"]
        tail.append("RESULT: NOT READY TO SUBMIT -- fix the FAIL items above." if n_fail
                    else "RESULT: READY TO SUBMIT" + (" (review the WARNs first)" if n_warn else ""))
        for t in tail:
            print(t)
        lines.extend(tail)
        try:
            with open(log_path, "w") as f:
                f.write("\n".join(lines) + "\n")
            print(f"Log written: {log_path}")
        except OSError as e:
            print(f"WARNING: could not write log {log_path}: {e}")
        return 1 if n_fail else 0

    # -------------------------------------------------------------------------
    def make_directory(self, path, permission=755):
        try:
            subprocess.run(['mkdir', '-p', path], check=True)
        except subprocess.CalledProcessError as e:
            print(f"  Error creating directory {path} : {e}")
            sys.exit(1)
        if permission == 777:
            chmod_cmd = ['chmod', '777', path]
            result = subprocess.run(chmod_cmd, stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE, text=True)
            if result.returncode != 0:
                print(f"Error setting 777 on output dir: {result.stderr}")
                sys.exit(1)
            else:
                print(f"Set permissions to 777 for {path}")

    # -------------------------------------------------------------------------
    def _handle_command_log(self):
        """제출 명령어를 condor 디렉토리(region+SF 별로 분리됨)에 기록한다.

        - 실제 제출(report/resubmit 아님): submit_command.txt 에 현재 명령어 +
          타임스탬프를 append. 같은 디렉토리 = 같은 region+SF 조합이므로,
          나중에 report/resubmit 을 어떤 인자로 불러야 하는지 여기서 확인 가능.
        - report/resubmit: 기존 submit_command.txt 를 읽어 '원래 이렇게 제출됨'
          을 출력. 파일이 없으면 안내만.
        """
        log_path = os.path.join(self.condor_files_path, "submit_command.txt")
        # 현재 명령어 재구성 (공백/특수문자 안전)
        cmd = "python3 " + " ".join(shlex.quote(a) for a in sys.argv)
        ts  = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        if self.report_only or self.resubmit_only:
            # 조회 모드 — 원 제출 명령어를 보여준다
            mode = ("status" if (self.report_only and self.report_verbose)
                    else "report" if self.report_only else "resubmit")
            print(f"\n  [cmd-log] ({mode}) condor dir: {self.condor_files_path}")
            if os.path.isfile(log_path):
                print(f"  [cmd-log] 이 디렉토리(region+SF)의 기록된 제출 명령어:")
                with open(log_path) as f:
                    for line in f:
                        line = line.rstrip()
                        if line:
                            print(f"      {line}")
                print(f"  [cmd-log] → report/resubmit 은 위 제출과 같은 "
                      f"--files-per-job / --region / SF 인자로 호출해야 정확합니다.\n")
            else:
                print(f"  [cmd-log][WARN] {log_path} 없음 — 이 조합으로 제출된 "
                      f"기록이 없습니다(경로/인자 확인).\n")
            return

        # 실제 제출 — 명령어 append
        header = ""
        if not os.path.isfile(log_path):
            header = ("# 이 파일은 submit_job_FH_Tier3_unified.py 가 자동 기록.\n"
                      "# 이 condor 디렉토리(region+SF 조합)로 제출된 명령어 이력.\n"
                      "# report/resubmit 시 같은 --files-per-job/--region/SF 로 호출할 것.\n")
        try:
            with open(log_path, "a") as f:
                if header:
                    f.write(header)
                f.write(f"[{ts}] {cmd}\n")
            print(f"  [cmd-log] 제출 명령어 기록: {log_path}")
        except Exception as e:
            print(f"  [cmd-log][WARN] 명령어 기록 실패: {e}")

    # -------------------------------------------------------------------------
    def process_config_file(self):
        config = self.load_yaml_config(self.config_file_path)
        common = config.get("common", {})
        samples = config.get("samples", [])

        # [STEP2] validation 모드(시나리오 외부 루프) 제거 — 단일 패스만 유지.
        # 유효 모드 검증은 C++ analyzer의 parseAnalysisMode가 fatal로 수행하지만,
        # 잘못된 yml로 condor job을 뿌리기 전에 여기서도 조기 차단한다.
        analyzer_mode = common.get("analysis_mode", "main")
        valid_modes = ("main", "btagtrig", "prescan", "debug")
        if analyzer_mode not in valid_modes:
            raise ValueError(
                f"[FATAL] analysis_mode='{analyzer_mode}' is not valid. "
                f"Valid: {valid_modes}. "
                "(trigsf/validation은 2026-06 리팩토링에서 제거 — "
                "docs/changes/STEP_2 참조)")

        # [cmd log] 제출 시 명령어를 condor 디렉토리에 기록 / report·resubmit 시 조회.
        self._handle_command_log()

        for entry in samples:
            try:
                self.parse_config_entry(entry, common)
                # [STEP20] 조회 모드(report/status)는 read-only — output 디렉토리
                # 생성/chmod/출력(prepare_output_directory)을 건너뛴다.
                if not self.report_only:
                    self.prepare_output_directory()
                self.setup_and_submit_job()
            except Exception as e:
                print(f"  Error with sample {entry} : {e}")
                continue

        # [STEP20] 조회 모드 마무리: 요약 테이블 (report = 테이블만,
        # status = 샘플별 상세 출력 뒤 테이블)
        if self.report_only:
            self._print_report_table()

    # -------------------------------------------------------------------------
    # -------------------------------------------------------------------------
    # [TrackC] xsec_db + prescan 로더 (lazy, 1회 캐시)
    # -------------------------------------------------------------------------
    def _load_xsec_db(self, path):
        if getattr(self, "_xsec_db", None) is not None:
            return self._xsec_db
        import json
        with open(path) as f:
            self._xsec_db = json.load(f)
        return self._xsec_db

    def _load_prescan(self, path):
        if getattr(self, "_prescan", None) is not None:
            return self._prescan
        import json
        with open(path) as f:
            self._prescan = json.load(f).get("samples", {})
        return self._prescan

    def _compute_base_weight(self, sample_name, common):
        """[TrackC] base weight 런타임 합성:
              w = lumi * cross_section_pb * br / sumGenW(runs.genEventSumw)
           Data(cross_section_pb=null)는 1.0. yml에 명시 weight가 있으면 그것을
           우선(하위호환/override). xsec_db·prescan 경로는 common에서 받는다."""
        db = self._load_xsec_db(common["xsec_db"])
        rec = db.get(sample_name)
        if rec is None:
            _fatal(EXIT["XSEC_DB_MISSING"],
                   f"sample '{sample_name}' not in xsec_db ({common['xsec_db']}).")
        xsec = rec.get("cross_section_fb")
        if xsec is None:
            return 1.0, "data"          # Data
        br   = rec.get("br", 1.0) or 1.0
        kfac = rec.get("kfactor", 1.0) or 1.0
        meta = db.get("_meta", {})
        lumi = float(common.get("lumi_fb_inv", meta.get("lumi_fb_inv")))
        pre  = self._load_prescan(common["prescan"])
        prec = pre.get(sample_name)
        if prec is None:
            _fatal(EXIT["PRESCAN_MISSING"],
                   f"sample '{sample_name}' not in prescan ({common['prescan']}) "
                   f"— run prescan first.")
        sumw = prec["runs"]["genEventSumw"]              # ★ runs.genEventSumw 사용
        sumw_tree = prec["events"]["sumGenW_total"]      # 비교용
        if sumw <= 0:
            _fatal(EXIT["PRESCAN_MISSING"],
                   f"{sample_name}: sumGenW(runs)={sumw} <= 0 (invalid prescan).")
        # runs vs tree 합 불일치 경고 (사용은 runs)
        if sumw_tree > 0 and abs(sumw - sumw_tree)/sumw > 1e-4:
            print(f"  [warn] {sample_name}: sumGenW runs={sumw:.6e} vs "
                  f"tree={sumw_tree:.6e} differ >0.01% (using runs)")
        w = lumi * xsec * br * kfac / sumw
        return w, (f"xsec_fb={xsec}*br={br:.5f}*k={kfac}*lumi={lumi}"
                   f"/sumGenW={sumw:.4e}")

    def parse_config_entry(self, entry, common):
        self._common = common   # [lepton-CR] generate_argument_list 에서 region 참조용

        # [TrackC] 필수 키 완화: sample_name 만 필수. filelist/output_dir/
        # weight/data_or_mc 는 규칙·xsec_db에서 유도 (yml 명시 시 override).
        if "sample_name" not in entry:
            raise ValueError(f"Invalid entry (missing 'sample_name'): {entry}")
        self.sample_name = entry["sample_name"]

        # filelist: 규칙 'filelist_<sample_name>.txt' (yml override 가능)
        self.file_list_name = entry.get(
            "filelist", f"filelist_{self.sample_name}.txt")
        # output_dir: 기본 = sample_name
        self.sample_output_dir = entry.get("output_dir", self.sample_name)
        self.era = entry.get("era", "")

        # [STEP17] prescan 모드는 normalization 불필요 → xsec_db/prescan_summary 와
        # 완전 분리한다. MC/Data 는 PD 이름으로 판정(Data 는 genWeight 없음), weight 는
        # 1.0 고정(아래). 이로써 새 샘플을 xsec_db/prescan_summary 에 먼저 등록하지
        # 않아도 prescan 이 바로 돈다 (prescan 이 그 둘의 입력을 만드는 단계이므로).
        _is_prescan = (str(common.get("analysis_mode", "")).strip() == "prescan")

        # data_or_mc: override > (prescan: PD 이름) > xsec_db(cross_section_fb null 여부)
        if "data_or_mc" in entry:
            self.data_or_mc = entry["data_or_mc"]
        elif _is_prescan:
            self.data_or_mc = ("Data"
                               if re.search(r"(JetHT|BTagCSV|SingleMuon)", self.sample_name)
                               else "MC")
        else:
            db = self._load_xsec_db(common["xsec_db"])
            rec = db.get(self.sample_name, {})
            self.data_or_mc = "Data" if rec.get("cross_section_fb") is None else "MC"

        # [era] Data 는 analyzer 가 --era 를 필수로 요구한다(트리거 PD 분기 isEraB 등).
        # yml 이 bare-string 샘플이라 era 필드가 없으므로, 샘플명 끝 '_<X>' 에서
        # era 를 자동 추출한다 (예: SingleMuon_C -> 'C', JetHT_E -> 'E').
        # yml 에 명시적 era 가 있으면 그게 우선. MC 는 era 를 두지 않는다.
        if self.data_or_mc == "Data" and not str(self.era).strip():
            # 2026-07-26: Run2017 하드코딩 -> Run<4자리연도> 일반화.
            #   기존: r"Run2017([A-Z])$" 는 2018 키(JetHT_Run2018A)를 못 잡고,
            #   fallback r"_([A-Z])$" 도 'A' 앞 문자가 '8' 이라 실패 -> FATAL.
            #   이제 Run2016/2017/2018/Run3 키 모두 era 를 추출한다.
            m = (re.search(r"Run\d{4}([A-Z])$", self.sample_name)
                 or re.search(r"_([A-Z])$", self.sample_name))
            if m:
                self.era = m.group(1)
                print(f"  [era] {self.sample_name}: Data -> era '{self.era}' "
                      f"(샘플명에서 자동 추출)")
            else:
                raise ValueError(
                    f"[FATAL] Data 샘플 '{self.sample_name}' 의 era 를 결정할 수 "
                    f"없습니다. 샘플명이 '<PD>_<era>' (era=단일 대문자) 규칙이 "
                    f"아니면 yml 에 era 를 명시하세요.")

        # weight: yml 명시 우선 > (prescan: 1.0) > xsec_db+prescan 합성
        if "weight" in entry:
            self.weight = entry["weight"]
        elif _is_prescan:
            self.weight = 1.0   # prescan 미사용 (runPrescan 이 Σgenw 만 누산)
            if not self.report_only or self.report_verbose:
                print(f"  [weight] {self.sample_name}: 1.0  (prescan — normalization 불필요)")
        else:
            self.weight, prov = self._compute_base_weight(self.sample_name, common)
            # [STEP20] compact report 는 테이블만 — [weight] 라인은 --status 에서만
            if not self.report_only or self.report_verbose:
                print(f"  [weight] {self.sample_name}: {self.weight:.10g}  ({prov})")

        self.year = common["year"]
        self.analysis_mode = common["analysis_mode"]

        # [STEP7] job당 파일 수: CLI > yml common.files_per_job > 1
        # (=1 이면 기존 동작과 완전 동일: 한 줄=한 job, output 인덱스 동일)
        fpj = self.cli_files_per_job
        if fpj is None:
            fpj = int(common.get("files_per_job", 1) or 1)
        if fpj < 1:
            raise ValueError(f"[FATAL] files_per_job must be >= 1 (got {fpj})")
        self.files_per_job = fpj

        # [STEP18] 보정 입력 경로 정책 (코드 default 폐기 — ConfigPath.h 와 동일 계약):
        #   - 빈 문자열 / 키 누락  -> FATAL E12 (빈 값은 더 이상 'default 사용' 신호 아님)
        #   - null                 -> 선택 보정이면 비활성(sentinel "__NULL__" 주입),
        #                             필수 보정이면 FATAL E13
        #   - 실제 경로            -> 그대로 주입
        # analyzer 는 항상 env 를 받는다(실제 경로 또는 "__NULL__").
        path_env_map = {
            "path_jsonpog":               "TTHH_JSONPOG_PATH",
            "path_goldenjson":            "TTHH_GOLDENJSON_PATH",
            "path_trigsf_dir":            "TTHH_TRIGSF_DIR",
            "path_btag_reweight_json":    "TTHH_BTAGRW_JSON",
            "path_stitch_json":           "STITCH_FACTORS_JSON",
            "path_expanded_ttbarid_dir":  "EXPANDED_TTBARID_DIR",
        }
        is_data     = (self.data_or_mc == "Data")
        derived_req = self.AnalyzerMode in ("main", "debug")
        # 필수 여부: jsonpog 는 항상(JME/PU); goldenjson 은 Data; trigsf/btagrw 는
        # main·debug 에서 해당 SF 토글이 on 일 때.
        required_keys = {"path_jsonpog"}
        if is_data:
            required_keys.add("path_goldenjson")
        if derived_req and self._cli_trigsf == "on":
            required_keys.add("path_trigsf_dir")
        if derived_req and self._cli_btagrw == "on":
            required_keys.add("path_btag_reweight_json")

        self.env_exports = {}
        for yml_key, env_name in path_env_map.items():
            required = yml_key in required_keys
            v = common.get(yml_key, "")
            if v is None:                                   # yml null
                if required:
                    _fatal(EXIT["CONFIG_PATH_NULL_REQUIRED"],
                           f"{yml_key} is REQUIRED for mode={self.AnalyzerMode} "
                           f"data_or_mc={self.data_or_mc} "
                           f"(trigsf={self._cli_trigsf}, btagrw={self._cli_btagrw}) "
                           f"but is null. Provide a real path, or change mode/toggle.")
                self.env_exports[env_name] = NULL_SENTINEL  # 선택 보정 -> 비활성
                continue
            if isinstance(v, str):
                v = v.strip()
            if v == "":                                     # 빈 문자열 / 키 누락
                _fatal(EXIT["CONFIG_PATH_ENV_MISSING"],
                       f"{yml_key} is empty/missing in the yml common: block. "
                       f"Blank is no longer 'use default'. Give a real path, or "
                       f"null to disable an optional correction. "
                       f"(required here: {sorted(required_keys)})")
            self.env_exports[env_name] = v                  # 실제 경로

        if self.data_or_mc == "MC" and str(self.era).strip():
            raise ValueError(
                "MC samples must not define an eraName. "
                "Please leave era empty."
            )

        # [STEP2] validation 모드 제거 — output_dir 합성 분기 삭제
        self.output_dir = self.sample_output_dir

        self.path_output = os.path.join(self.path_output_base,
                                        self.output_dir + "/")

        # condor file paths — use sanitized name (no slashes)
        sanitized = self.output_dir.replace("/", "_")
        self.script_name = os.path.join(
            self.condor_files_path, f"run_{sanitized}.sh")
        self.condor_submit_name = os.path.join(
            self.condor_files_path, f"{sanitized}_condor.sub")
        self.arg_list_file = os.path.join(
            self.condor_files_path, f"arguments_{sanitized}.txt")
        self.tmp_folder = os.path.join(
            self.condor_files_path,
            f"tmp_{sanitized}_{self.time_info}"
        )
        # [STEP20] tmp 디렉토리는 여기서 만들지 않는다(lazy). 기존에는 모든
        # 호출(report/resubmit 포함)이 샘플마다 무조건 mkdir 해서, 재큐할 job
        # 이 0개인 샘플·조회 모드에서도 빈 tmp_<sample>_<ts>/ 가 쌓였다.
        # 이제 generate_argument_list 가 첫 per-job filelist 를 쓰기 직전에만
        # 생성한다 → 빈 디렉토리 리터 제거. (job .out/.err 도 이 디렉토리에
        # 남으므로, 생성 = "이 invocation 이 이 샘플에 실제 job 을 큐잉했다".)

    # -------------------------------------------------------------------------
    def load_yaml_config(self, path):
        """
        Minimal YAML loader. Supports two top-level lists (`samples:` and
        plus a `common:` mapping. Accepts string,
        int, float, and bool scalars. Comments (#) and blank lines are
        skipped.

        NOTE: This is a hand-rolled parser to avoid an external pyyaml
        dependency. It assumes the YAML follows the convention used in
        Tier3_2017_FH_unified_*.yml.
        """
        def parse_value(raw_value):
            value = raw_value.strip()
            # Strip inline comments (e.g. "true   # comment").
            # Only at the first '#' that is NOT inside quotes.
            if '#' in value:
                in_q = False
                qc = ''
                for i, c in enumerate(value):
                    if c in ('"', "'"):
                        if not in_q:
                            in_q = True; qc = c
                        elif qc == c:
                            in_q = False
                    elif c == '#' and not in_q:
                        value = value[:i].rstrip()
                        break
            if (value.startswith('"') and value.endswith('"')) or \
               (value.startswith("'") and value.endswith("'")):
                return value[1:-1]
            # [STEP18] unquoted null / ~ / none -> None (선택 보정 "비활성" 신호).
            #   주의: 따옴표 "" 는 위에서 빈 문자열로 반환됨 → null 과 구분된다
            #   (빈 문자열 = FATAL E12, null = 선택 비활성/필수 FATAL E13).
            if value.lower() in ("null", "~", "none"):
                return None
            # bool
            if value.lower() == "true":  return True
            if value.lower() == "false": return False
            # int
            try:
                if re.fullmatch(r"-?\d+", value):
                    return int(value)
            except ValueError:
                pass
            # float
            try:
                return float(value)
            except ValueError:
                pass
            return value

        config = {
            "common": {},
            "samples": []
        }   # [STEP2] validation_scenarios 섹션 지원 제거

        section = None        # "common" | "samples"
        current_item = None   # dict for current samples or scenarios entry

        with open(path, "r") as f:
            for raw_line in f:
                line = raw_line.strip()
                if not line or line.startswith("#"):
                    continue

                # ── Section headers ───────────────────────────────────────
                if line == "common:":
                    section = "common"
                    if current_item is not None:
                        # flush previous list-section item
                        config[section_prev_list].append(current_item)
                        current_item = None
                    continue
                if line == "samples:":
                    if current_item is not None:
                        config[section_prev_list].append(current_item)
                        current_item = None
                    section = "samples"
                    section_prev_list = "samples"
                    continue
                # ── List items (start with "- ") ──────────────────────────
                # [TrackC] bare string 항목 지원: "- TTbar_Hadronic" →
                #   {sample_name: TTbar_Hadronic}. 기존 "- key: value" 도 호환.
                if section == "samples" and line.startswith("- "):
                    if current_item is not None:
                        config[section_prev_list].append(current_item)
                    current_item = {}
                    line = line[2:].strip()
                    if line and ":" in line:
                        key, value = line.split(":", 1)
                        current_item[key.strip()] = parse_value(value)
                    elif line:
                        # bare string = sample name only (TrackC 권장 형식)
                        current_item["sample_name"] = parse_value(line)
                    continue

                # ── Plain key:value lines ─────────────────────────────────
                if ":" in line:
                    key, value = line.split(":", 1)
                    if section == "common":
                        config["common"][key.strip()] = parse_value(value)
                    elif section == "samples":
                        if current_item is None:
                            current_item = {}
                        current_item[key.strip()] = parse_value(value)

        # flush final item
        if current_item is not None and section == "samples":
            config[section].append(current_item)

        return config

    # -------------------------------------------------------------------------
    def print_memory_status(self):
        print("\n" + "-" * 74)
        print("  --> Memory Check before submit jobs <--  ")
        print("\nSystem Memory Status:")
        subprocess.run(['free', '-h'])
        print("\n" + "-" * 74)

    # -------------------------------------------------------------------------
    def prepare_output_directory(self):
        check_dir_cmd = ['ls', '-d', self.path_output]
        result = subprocess.run(check_dir_cmd, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE, text=True)
        if result.returncode != 0:
            print(f"Creating output directory: {self.path_output}")
            mkdir_cmd = ['mkdir', '-p', self.path_output]
            mk_result = subprocess.run(mkdir_cmd, stdout=subprocess.PIPE,
                                       stderr=subprocess.PIPE, text=True)
            if mk_result.returncode != 0:
                print(f"Error creating output directory: {mk_result.stderr}")
                sys.exit(1)
        else:
            print(f"Output directory exists: {self.path_output}")
        chmod_cmd = ['chmod', '755', self.path_output]
        result = subprocess.run(chmod_cmd, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE, text=True)
        if result.returncode == 0:
            print(f"Set permissions to 755 for {self.path_output}")

    # -------------------------------------------------------------------------
    def _output_is_complete(self, output_path):
        """Return True iff `output_path` is a finished analyzer output ROOT file.

        Used only when self.resubmit_only is True. Completeness criterion:
          - prescan mode : file opens (non-zombie) and carries a 'prescan'
                           TTree with exactly one entry — the single summary
                           row written by writePrescanTree().
          - other modes  : file opens (non-zombie) and carries at least one
                           non-empty TTree.
        Any failure to verify (missing file, zombie, unreadable, PyROOT not
        available) is treated as 'not complete', so the job is conservatively
        re-queued rather than silently dropped.
        """
        if not os.path.isfile(output_path):
            return False
        try:
            import ROOT
        except Exception as e:
            print(f"  [resubmit][WARN] PyROOT unavailable ({e}); cannot verify "
                  f"outputs — run the submitter inside `cmsenv`. "
                  f"Treating as incomplete.")
            return False

        ROOT.gErrorIgnoreLevel = ROOT.kError
        f = ROOT.TFile.Open(output_path, "READ")
        if not f or f.IsZombie():
            if f:
                f.Close()
            return False
        try:
            if self.analysis_mode == "prescan":
                t = f.Get("prescan")
                return bool(t) and t.InheritsFrom("TTree") and t.GetEntries() == 1
            # [STEP19] 완료판정 = "종료 마커" 검사 (기존: non-empty TTree).
            #
            # analyzer(loop() 끝, ttHHanalyzer_unified.cc)는 writeHistos →
            # writeTree 이후 **마지막으로** cutflow 4종(cutflow / cutflow_w /
            # cutflow_w_btagSF / cutflow_w_full)을 Write 한다. 따라서
            # 'cutflow_w_full' 키의 존재 자체가 "event loop 완주" 마커다.
            #
            # 이 기준은 기존 non-empty TTree 기준의 두 오류를 동시에 고친다:
            #   - 거짓 missing: selection 을 아무 이벤트도 통과 못한 정상
            #     job (Tree entries=0; 저-HT WJets/QCD 에서 구조적으로 발생)
            #     → cutflow 는 있으므로 complete.
            #   - 거짓 complete(half-written, STEP15 §6 의 고위험 케이스):
            #     중간에 죽은 job 은 Tree 에 entry 가 남아도 cutflow 를
            #     끝내 Write 하지 못함 → incomplete 로 정확히 재큐.
            def _has_end_marker(d, depth=0):
                if depth > 4:               # 안전장치 (무한/과도 재귀 방지)
                    return False
                for key in d.GetListOfKeys():
                    if key.GetName() == "cutflow_w_full":
                        return True
                    if key.GetClassName().startswith("TDirectory"):
                        if _has_end_marker(key.ReadObj(), depth + 1):
                            return True
                return False
            return _has_end_marker(f)
        finally:
            f.Close()

    # -------------------------------------------------------------------------
    def generate_argument_list(self):
        """Write the per-job argument file. Returns the number of jobs queued.

        When self.resubmit_only is True, jobs whose output ROOT file is
        already a complete analyzer output are skipped (their per-job
        filelist and argument line are not written). The running `count`
        index still advances for every input file so that the output
        filenames `<sample>_<count>.root` stay aligned with the input
        filelist regardless of how many jobs are skipped.
        """
        # [STEP7] 마스터 filelist(샘플당 한 파일)를 N개씩 chunk로 잘라 job을
        # 만든다. 분할은 결정적(고정 순서·고정 N)이므로 output 이름
        # <sample>_<jobIdx>.root 가 입력 chunk와 1:1로 영구 대응한다 —
        # 재제출/리포트를 같은 N으로 부르면 완료 판정이 정확히 같은 chunk에
        # 매핑된다. files_per_job=1 이면 기존 동작(한 줄=한 job)과 동일.
        sample_list_file_path = os.path.join(
            self.sample_list_path, self.file_list_name)
        with open(sample_list_file_path, "r") as sample_list_in:
            lines = [line.strip() for line in sample_list_in
                     if line.strip() and not line.startswith('#')]
        N = self.files_per_job
        chunks = [lines[i:i + N] for i in range(0, len(lines), N)]
        sanitized = self.output_dir.replace("/", "_")

        # [STEP20] 조회 모드는 완전 read-only 여야 한다. 기존 구현은 report
        # 모드에서도 arguments_<sample>.txt 를 "w" 로 열어 — 아무것도 안 쓰고
        # 닫히며 — 직전 제출의 인자 기록을 **빈 파일로 truncate** 했다(큐잉된
        # job 은 제출 시점에 인자를 이미 읽었으므로 실행엔 무해하나, provenance
        # 소실). 이제 인자 라인을 메모리에 모았다가 제출/재제출일 때만 쓴다.
        n_written = 0
        n_complete = 0
        missing = []
        arg_lines = []
        for job_idx, chunk in enumerate(chunks):
            full_output_path = (
                f"{self.path_output}{self.sample_output_dir}_{job_idx}.root"
            )

            # 완료 판정은 resubmit/report 모드에서만 수행 (일반 제출은 불필요)
            complete = ((self.resubmit_only or self.report_only)
                        and self._output_is_complete(full_output_path))
            if complete:
                n_complete += 1
            else:
                missing.append(job_idx)

            # [STEP7] report 모드: 현황만 집계 — job 생성/제출 없음
            if self.report_only:
                continue
            if self.resubmit_only and complete:
                continue

            # [STEP7] 재제출 분리 저장: --resubmit-to SUB → output을
            # <path_output>/<SUB>/ 아래로 (원본과 비교/검증 용이)
            if self.resubmit_only and self.resubmit_to:
                resub_dir = os.path.join(self.path_output, self.resubmit_to)
                os.makedirs(resub_dir, exist_ok=True)
                full_output_path = os.path.join(
                    resub_dir, f"{self.sample_output_dir}_{job_idx}.root")

            per_job_filelist_name = f"filelist_{sanitized}_{job_idx}.txt"
            per_job_filelist_path = os.path.join(
                self.tmp_folder, per_job_filelist_name)
            # [STEP20] lazy tmp mkdir — 실제로 쓸 filelist 가 생길 때만 생성
            os.makedirs(self.tmp_folder, exist_ok=True)
            with open(per_job_filelist_path, 'w') as per_job_filelist:
                for line in chunk:
                    per_job_filelist.write(line + '\n')

            args = (
                f"--filelist {per_job_filelist_path} "
                f"--output {full_output_path} "
                f"--weight {self.weight} "
                f"--year {self.year} "
                f"--dataOrMC {self.data_or_mc} "
                f"--sample {self.sample_name} "
                f"--mode {self.analysis_mode} "
            )
            if str(self.era).strip():
                args += f" --era {self.era} "
            # [lepton-CR] CLI --region 우선, 없으면 yml common.region
            _region = (self._cli_region
                       or str(self._common.get("region", "")).strip())
            if _region:
                args += f" --region {_region} "
            # [SF toggle] production evtWeight 구성 (analyzer 로 전달)
            args += (f" --trigsf {self._cli_trigsf}"
                     f" --btagsf {self._cli_btagsf}"
                     f" --btagrw {self._cli_btagrw} ")

            arg_lines.append(args)
            n_written += 1

        # 인자 파일은 제출/재제출 경로에서만 기록 (조회 모드는 미접촉)
        if not self.report_only:
            with open(self.arg_list_file, "w") as argout:
                for a in arg_lines:
                    argout.write(a + "\n")

        if self.report_only:
            # [STEP20] 테이블용 누적 (report/status 공통)
            self._report_rows.append(
                (self.sample_name, len(chunks), n_complete, len(missing)))
            # 샘플별 상세 라인은 --status 에서만
            if self.report_verbose:
                print(f"  [report] {self.sample_name}: files={len(lines)} "
                      f"files/job={N} jobs={len(chunks)} "
                      f"complete={n_complete} missing={len(missing)}"
                      + (f" -> idx {missing[:20]}{' ...' if len(missing) > 20 else ''}"
                         if missing else ""))
            return 0
        if self.resubmit_only:
            print(f"  [resubmit] {self.sample_name}: {n_written} job(s) to "
                  f"re-queue out of {len(chunks)} total"
                  + (f" (output -> {self.resubmit_to}/)" if self.resubmit_to else "")
                  + ".")
        return n_written

    # -------------------------------------------------------------------------
    def write_condor_submission_file(self):
        sanitized = self.output_dir.replace("/", "_")
        with open(self.condor_submit_name, 'w') as f:
            f.write(f"x509userproxy           = {self.proxy_path}\n")
            f.write("getenv                  = True\n")
            f.write(f"executable              = {self.script_name}\n")
            f.write("arguments               = $(args)\n")
            f.write(
                f"output                  = "
                f"{os.path.join(self.tmp_folder, f'job_{sanitized}.$(ClusterId).$(ProcId).out')}\n"
            )
            f.write(f"MY.WantOS               = \"{self.os_version}\"\n")
            f.write("MY.XRDCP_CREATE_DIR     = True\n")
            f.write(
                f"error                   = "
                f"{os.path.join(self.tmp_folder, f'error_{sanitized}.$(ClusterId).$(ProcId).err')}\n"
            )
            f.write(
                f"log                     = "
                f"{os.path.join(self.condor_files_path, f'log_{sanitized}.$(ClusterId).log')}\n"
            )
            f.write(f"request_memory          = {self.memorySize}\n")
            f.write(f"queue args from {self.arg_list_file}\n")

    # -------------------------------------------------------------------------
    def create_executable_script(self):
        with open(self.script_name, 'w') as fout:
            fout.write("#!/bin/sh\n")
            fout.write("echo\n")
            fout.write("echo 'START---------------'\n")
            fout.write("echo 'WORKDIR ' ${PWD}\n")
            fout.write("source \"/cvmfs/cms.cern.ch/cmsset_default.sh\"\n")
            fout.write(f"cd \"{self.analyzer_path}\"\n")
            fout.write("cmsenv\n")
            fout.write("echo 'WORKDIR ' ${PWD}\n")
            fout.write(f"source \"{self.analyzer_path}/setup.sh\"\n")
            # [STEP4] yml common.path_* → env 주입 (로그에 남도록 echo 동반)
            for env_name, val in getattr(self, "env_exports", {}).items():
                fout.write(f"export {env_name}=\"{val}\"\n")
                fout.write(f"echo '[paths] {env_name}='\"${{{env_name}}}\"\n")
            fout.write(f"mkdir -p {self.path_output}\n")
            fout.write(f"\"{self.analyzer_path}/{self.nameofExe}\" \"$@\"\n")
        subprocess.call(["chmod", "755", self.script_name])

    # -------------------------------------------------------------------------
    def submit_job(self):
        print(f"Submitting job for sample: {self.sample_name}")
        subprocess.call(["condor_submit", self.condor_submit_name])

    def setup_and_submit_job(self):
        # [STEP20] compact report 는 조용히 집계만 — 배너는 --status/제출에서만
        if not self.report_only or self.report_verbose:
            print(f"\nSetting up job for sample: {self.sample_name}")
        n_jobs = self.generate_argument_list()
        # [report] report 모드는 집계만 하고 항상 0 을 반환한다(제출 안 함).
        # 이때 'All outputs already complete' 는 거짓이므로 건너뛴다.
        if self.report_only:
            return
        if n_jobs == 0:
            print(f"  All outputs already complete for "
                  f"{self.sample_name} — nothing to submit.")
            return
        self.create_executable_script()
        self.write_condor_submission_file()
        self.submit_job()

    # -------------------------------------------------------------------------
    def _print_report_table(self):
        """[STEP20] 완료/미완료 요약 테이블 (NtupleForge crab --report 스타일).

        판정 기준은 STEP19 의 종료 마커(output 에 cutflow_w_full 존재)이므로
        'done' = analyzer 가 event loop 를 완주하고 output 을 닫은 job 이다.
        아직 큐/실행 중인 job 도 output 이 없으니 'miss' 로 집계된다 — 즉
        miss = 실패 + 미실행 + 실행중. 실시간 큐 상태는 condor_q 로 볼 것.
        """
        rows = self._report_rows
        if not rows:
            print("\n[report] no samples processed — nothing to summarize.")
            return
        name_w = max([len("sample")] + [len(r[0]) for r in rows])
        header = (f"{'sample':<{name_w}}  {'jobs':>6}  {'done':>6}  "
                  f"{'miss':>6}  {'done%':>7}")
        bar = "=" * len(header)
        mode_tag = f"mode={self.AnalyzerMode}{self._dir_suffix or ''}"
        print("\n" + bar)
        print(f"Condor output report ({mode_tag})  "
              f"[done = end-marker complete (STEP19)]")
        print(bar)
        print(header)
        print("-" * len(header))
        t_jobs = t_done = t_miss = 0
        for name, jobs, done, miss in rows:
            pct = (100.0 * done / jobs) if jobs else 0.0
            flag = "" if miss == 0 else "  <-- missing"
            print(f"{name:<{name_w}}  {jobs:>6}  {done:>6}  "
                  f"{miss:>6}  {pct:>6.1f}%{flag}")
            t_jobs += jobs; t_done += done; t_miss += miss
        print("-" * len(header))
        t_pct = (100.0 * t_done / t_jobs) if t_jobs else 0.0
        print(f"{'TOTAL':<{name_w}}  {t_jobs:>6}  {t_done:>6}  "
              f"{t_miss:>6}  {t_pct:>6.1f}%")
        print(bar)
        if t_miss:
            print("  -> missing idx 목록: --status  |  재큐: --resubmit "
                  "(같은 --files-per-job/--region/SF 인자로)")
        else:
            print("  -> all outputs complete.")


def main():
    try:
        CondorJobManager()
    except Exception as e:
        print(e)
        sys.exit(1)


if __name__ == "__main__":
    main()
