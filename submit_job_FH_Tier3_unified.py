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

import os
import sys
import time
import re
import subprocess

script_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(script_dir, 'python', 'ttHHmodules'))
import ProxyChecker
import resultChecker  # noqa: F401  (kept for backward compat)


class CondorJobManager:

    def __init__(self):

        self.time_info = time.strftime("%Y%m%d-%H%M%S")

        # Variables for jobs, please check before running
        self.analyzer_path = f"{script_dir}"
        self.nameofExe = "ttHHanalyzer_unified"
        self.AnalyzerMode = "main"  # main / btagtrig / trigsf (x) / validation / prescan

        # ── Resubmit control ──────────────────────────────────────────────
        # False → normal run: queue every job (default; behaviour unchanged).
        # True  → resubmit pass: skip jobs whose output ROOT is already a
        #         complete analyzer output, re-queue only missing/incomplete
        #         ones. Flip to True manually for a resubmission run.
        self.resubmit_only = False

        self.path_output_base = (
            f"/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/"
            f"AnalyzerOutput_{self.AnalyzerMode}"
        )
        self.os_version = "el9"
        self.memorySize = "12 GB"

        self.config_file_path = os.path.join(
            self.analyzer_path,
            f"AnalyzerConfig/Tier3_2017_FH_unified_{self.AnalyzerMode}.yml"
        )
        self.proxy_path = os.path.join(self.analyzer_path, "proxy.cert")
        self.condor_files_path = os.path.join(
            self.analyzer_path,
            f"condor/filelistTier3_unified_{self.AnalyzerMode}"
        )
        self.sample_list_path = os.path.join(self.analyzer_path, "filelistTier3")

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

        for entry in samples:
            try:
                self.parse_config_entry(entry, common)
                self.prepare_output_directory()
                self.setup_and_submit_job()
            except Exception as e:
                print(f"  Error with sample {entry} : {e}")
                continue

    # -------------------------------------------------------------------------
    def parse_config_entry(self, entry, common):

        required_keys_inEntry = [
            "filelist", "output_dir", "weight",
            "data_or_mc", "sample_name"
        ]
        missing = [k for k in required_keys_inEntry if k not in entry]
        if missing:
            raise ValueError(
                f"Invalid entry (missing keys: {missing}): {entry}"
            )

        self.file_list_name = entry["filelist"]
        self.sample_output_dir = entry["output_dir"]
        self.weight = entry["weight"]
        self.data_or_mc = entry["data_or_mc"]
        self.sample_name = entry["sample_name"]
        self.era = entry.get("era", "")

        self.year = common["year"]
        self.analysis_mode = common["analysis_mode"]

        # [STEP4] 보정 입력 경로 — yml common.path_* 를 condor 실행 sh의
        # export로 주입한다. 비어 있거나 없으면 export하지 않음 → analyzer가
        # 코드 내 default(Tier3)를 사용 (하위호환).
        path_env_map = {
            "path_jsonpog":               "TTHH_JSONPOG_PATH",
            "path_goldenjson":            "TTHH_GOLDENJSON_PATH",
            "path_trigsf_dir":            "TTHH_TRIGSF_DIR",
            "path_btag_reweight_json":    "TTHH_BTAGRW_JSON",
            "path_stitch_json":           "STITCH_FACTORS_JSON",
            "path_expanded_ttbarid_dir":  "EXPANDED_TTBARID_DIR",
        }
        self.env_exports = {}
        for yml_key, env_name in path_env_map.items():
            v = common.get(yml_key, "")
            if isinstance(v, str) and v.strip():
                self.env_exports[env_name] = v.strip()

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
        self.make_directory(self.tmp_folder)

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
                if section == "samples" and line.startswith("- "):
                    if current_item is not None:
                        config[section_prev_list].append(current_item)
                    current_item = {}
                    line = line[2:].strip()
                    if line and ":" in line:
                        key, value = line.split(":", 1)
                        current_item[key.strip()] = parse_value(value)
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
            for key in f.GetListOfKeys():
                obj = key.ReadObj()
                if obj.InheritsFrom("TTree") and obj.GetEntries() > 0:
                    return True
            return False
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
        n_written = 0
        with open(self.arg_list_file, "w") as argout:
            count = 0
            sample_list_file_path = os.path.join(
                self.sample_list_path, self.file_list_name)
            with open(sample_list_file_path, "r") as sample_list_in:
                lines = [line.strip() for line in sample_list_in
                         if line.strip() and not line.startswith('#')]
                for line in lines:
                    sanitized = self.output_dir.replace("/", "_")

                    # Output file in the per-scenario per-sample dir
                    full_output_path = (
                        f"{self.path_output}{self.sample_output_dir}_{count}.root"
                    )

                    # [resubmit-only] skip jobs already finished. `count` must
                    # still advance to keep the <sample>_<count>.root mapping.
                    if self.resubmit_only and self._output_is_complete(full_output_path):
                        count += 1
                        continue

                    per_job_filelist_name = f"filelist_{sanitized}_{count}.txt"
                    per_job_filelist_path = os.path.join(
                        self.tmp_folder, per_job_filelist_name)
                    with open(per_job_filelist_path, 'w') as per_job_filelist:
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

                    argout.write(args + "\n")
                    n_written += 1
                    count += 1

        if self.resubmit_only:
            print(f"  [resubmit] {self.sample_name}: {n_written} job(s) to "
                  f"re-queue out of {count} total.")
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
        print(f"\nSetting up job for sample: {self.sample_name}")
        n_jobs = self.generate_argument_list()
        if n_jobs == 0:
            print(f"  All outputs already complete for "
                  f"{self.sample_name} — nothing to submit.")
            return
        self.create_executable_script()
        self.write_condor_submission_file()
        self.submit_job()


def main():
    try:
        CondorJobManager()
    except Exception as e:
        print(e)
        sys.exit(1)


if __name__ == "__main__":
    main()
