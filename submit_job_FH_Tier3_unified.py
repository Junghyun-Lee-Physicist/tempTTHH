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
        self.AnalyzerMode = "prescan"  # main / btagtrig / trigsf / validation / prescan
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
        scenarios = config.get("validation_scenarios", [])

        analyzer_mode = common.get("analysis_mode", "main")

        # ── Validation mode: outer loop over scenarios ─────────────────────
        if analyzer_mode == "validation":
            if not scenarios:
                raise ValueError(
                    "[ERROR] analysis_mode=validation but no "
                    "`validation_scenarios:` block in YAML."
                )
            for scen in scenarios:
                scen_name = scen.get("name", "default")
                print(f"\n{'='*70}\n[VALIDATION] Submitting scenario: {scen_name}\n{'='*70}")
                self._current_scenario = scen
                for entry in samples:
                    try:
                        self.parse_config_entry(entry, common, scen)
                        self.prepare_output_directory()
                        self.setup_and_submit_job()
                    except Exception as e:
                        print(f"  Error with sample {entry} (scenario "
                              f"{scen_name}) : {e}")
                        continue
        # ── Other modes: single pass over samples ──────────────────────────
        else:
            self._current_scenario = None
            for entry in samples:
                try:
                    self.parse_config_entry(entry, common, None)
                    self.prepare_output_directory()
                    self.setup_and_submit_job()
                except Exception as e:
                    print(f"  Error with sample {entry} : {e}")
                    continue

    # -------------------------------------------------------------------------
    def parse_config_entry(self, entry, common, scenario):

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

        if self.data_or_mc == "MC" and str(self.era).strip():
            raise ValueError(
                "MC samples must not define an eraName. "
                "Please leave era empty."
            )

        # ── Composite output dir for validation mode ───────────────────────
        if scenario is not None:
            scen_name = scenario.get("name", "default")
            self.output_dir = f"{scen_name}/{self.sample_output_dir}"
        else:
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
        `validation_scenarios:`) plus a `common:` mapping. Accepts string,
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
            "samples": [],
            "validation_scenarios": []
        }

        section = None        # "common" | "samples" | "validation_scenarios"
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
                if line == "validation_scenarios:":
                    if current_item is not None:
                        config[section_prev_list].append(current_item)
                        current_item = None
                    section = "validation_scenarios"
                    section_prev_list = "validation_scenarios"
                    continue

                # ── List items (start with "- ") ──────────────────────────
                if section in ("samples", "validation_scenarios") \
                        and line.startswith("- "):
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
                    elif section in ("samples", "validation_scenarios"):
                        if current_item is None:
                            current_item = {}
                        current_item[key.strip()] = parse_value(value)

        # flush final item
        if current_item is not None and section in (
                "samples", "validation_scenarios"):
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
    def _scenario_argv(self):
        """Build the validation-scenario argv suffix.

        Returns the empty string when not in validation mode or when no
        scenario is currently selected.
        """
        scen = getattr(self, "_current_scenario", None)
        if scen is None:
            return ""

        # Order matters only for readability; the C++ side is flag-based.
        parts = [f"--val-scenario {scen.get('name', 'default')}"]

        # Numeric/bool fields: always emit so defaults are explicit on the
        # command line (easier to debug from condor logs).
        bool_fields = [
            ("applyHadWWindow",  "--val-hadWWindow"),
            ("applyHiggsWindow", "--val-higgsWindow"),
            ("tightenJet8",      "--val-tightenJet8"),
            ("applyBtagShapeSF", "--val-btagShape"),
            ("applyBtagNormSF",  "--val-btagNorm"),
            ("applyTriggerSF",   "--val-trig"),
            ("applyTopPtSF",     "--val-topPt"),
            ("ttHVRStyle",       "--val-ttHVR"),
        ]
        int_fields = [
            ("nbJetsCut",              "--val-nbJetsCut"),
            ("forceHiggsRecoMinBjets", "--val-hRecoMin"),
        ]
        for key, flag in int_fields:
            if key in scen:
                parts.append(f"{flag} {int(scen[key])}")
        for key, flag in bool_fields:
            if key in scen:
                v = 1 if bool(scen[key]) else 0
                parts.append(f"{flag} {v}")

        return " ".join(parts)

    # -------------------------------------------------------------------------
    def generate_argument_list(self):
        scen_argv = self._scenario_argv()

        with open(self.arg_list_file, "w") as argout:
            count = 0
            sample_list_file_path = os.path.join(
                self.sample_list_path, self.file_list_name)
            with open(sample_list_file_path, "r") as sample_list_in:
                lines = [line.strip() for line in sample_list_in
                         if line.strip() and not line.startswith('#')]
                for line in lines:
                    sanitized = self.output_dir.replace("/", "_")
                    per_job_filelist_name = f"filelist_{sanitized}_{count}.txt"
                    per_job_filelist_path = os.path.join(
                        self.tmp_folder, per_job_filelist_name)
                    with open(per_job_filelist_path, 'w') as per_job_filelist:
                        per_job_filelist.write(line + '\n')

                    # Output file in the per-scenario per-sample dir
                    full_output_path = (
                        f"{self.path_output}{self.sample_output_dir}_{count}.root"
                    )

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
                    if scen_argv:
                        args += " " + scen_argv

                    argout.write(args + "\n")
                    count += 1

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
            fout.write(f"mkdir -p {self.path_output}\n")
            fout.write(f"\"{self.analyzer_path}/{self.nameofExe}\" \"$@\"\n")
        subprocess.call(["chmod", "755", self.script_name])

    # -------------------------------------------------------------------------
    def submit_job(self):
        scen = getattr(self, "_current_scenario", None)
        scen_label = ("[" + scen["name"] + "] ") if scen else ""
        print(f"Submitting job for sample: {scen_label}{self.sample_name}")
        subprocess.call(["condor_submit", self.condor_submit_name])

    def setup_and_submit_job(self):
        scen = getattr(self, "_current_scenario", None)
        scen_label = ("[" + scen["name"] + "] ") if scen else ""
        print(f"\nSetting up job for sample: {scen_label}{self.sample_name}")
        self.generate_argument_list()
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
