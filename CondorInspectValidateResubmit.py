#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
CondorInspectValidateResubmit.py

Inspects failed Condor jobs and optionally resubmits them.

Usage:
    ./CondorInspectValidateResubmit.py [--resubmit]
    # Redirect stdout to a log file: ./CondorInspectValidateResubmit.py > log.txt

Options:
    --resubmit   Actually resubmit failed jobs. Without this flag, runs in dry-run mode.
"""
import os
import sys
import glob
import subprocess
import argparse
import logging
import signal

# --- Argument parsing ---------------------------------------------------
parser = argparse.ArgumentParser(description="Inspect and optionally resubmit failed Condor jobs.")
parser.add_argument('--resubmit', action='store_true', help='Actually resubmit failed jobs')
args = parser.parse_args()
RESUBMIT_MODE = args.resubmit

# --- Configuration variables -------------------------------------------
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CONDOR_FILES_PATH = os.path.join(SCRIPT_DIR, 'condor', 'filePath_Btag')
EOS_BASE_POSIX = '/eos/user/j/junghyun/ttHH/Btag_el9'
RESUB_LOG_BASE = os.path.join(SCRIPT_DIR, 'condor', 'filePath_Btag', 'ResubmittedJobV4')
RESUB_EOS_OUTPUT_BASE = f"{EOS_BASE_POSIX}/ResubOutputV4"
ERROR_KEYWORDS = ['ERROR']
SAMPLES = [
    'QCD_HT1000to1500', 'QCD_HT1500to2000', 'QCD_HT2000toInf',
    'QCD_HT200to300', 'QCD_HT300to500', 'QCD_HT500to700', 'QCD_HT700to1000',
    'SingleMuon_B', 'SingleMuon_C', 'SingleMuon_D', 'SingleMuon_E', 'SingleMuon_F',
    'BTagCSV_B', 'BTagCSV_C', 'BTagCSV_D', 'BTagCSV_E', 'BTagCSV_F',
    'JetHT_B', 'JetHT_C', 'JetHT_D', 'JetHT_E', 'JetHT_F',
    'tt4b', 'ttbb', 'ttHH', 'ttHtobb', 'ttJets', 'ttTohadronic',
    'tttt', 'tttW', 'ttWH', 'ttWW', 'ttWZ', 'ttZHto4b', 'ttZtobb', 'ttZZto4b',
]

# --- Logging setup ------------------------------------------------------
logging.basicConfig(stream=sys.stdout, level=logging.INFO, format='[%(levelname)s] %(message)s')

# --- Timeout handler ----------------------------------------------------
class TimeoutException(Exception): pass

def _on_timeout(signum, frame):
    raise TimeoutException
signal.signal(signal.SIGALRM, _on_timeout)

# --- Setup confirmation ------------------------------------------------
def confirm_setup(timeout=10):
    print('========== Setup Confirmation ==========')
    print(f"RESUBMIT_MODE         : {'ON' if RESUBMIT_MODE else 'OFF'}")
    print(f"RESUB_LOG_BASE        : {RESUB_LOG_BASE}")
    print(f"RESUB_EOS_OUTPUT_BASE : {RESUB_EOS_OUTPUT_BASE}")
    print(f"EOS_BASE_POSIX        : {EOS_BASE_POSIX}")
    print(f"CONDOR_FILES_PATH     : {CONDOR_FILES_PATH}")
    print('Samples               : ' + ', '.join(SAMPLES))
    sys.stdout.write(f"Proceed? Enter 'y' to continue (auto in {timeout}s): ")
    sys.stdout.flush()
    signal.alarm(timeout)
    try:
        choice = sys.stdin.readline().strip()
        signal.alarm(0)
        if choice.lower() != 'y':
            print('Configuration not confirmed. Exiting.')
            sys.exit(0)
    except TimeoutException:
        print(f'\nTimeout reached ({timeout}s). Proceeding automatically.')
    print('========================================')

# --- Utility functions -------------------------------------------------
def find_tmp_dirs_for_sample(sample):
    return [d for d in glob.glob(os.path.join(CONDOR_FILES_PATH, f'tmp_{sample}_*')) if os.path.isdir(d)]

def parse_sample_timestamp(dirname):
    # dirname: 'tmp_<sample>_<timestamp>'
    if not dirname.startswith('tmp_'): return None, None
    base = dirname[len('tmp_'):]
    idx = base.rfind('_')
    if idx < 0: return None, None
    sample = base[:idx]
    timestamp = base[idx+1:]
    return sample, timestamp

def list_error_files(tmpdir, sample):
    return sorted(glob.glob(os.path.join(tmpdir, f'error_{sample}.*.err')))

def read_error_content(path):
    try: return open(path).read()
    except: return ''

def eos_file_exists(posix_path):
    return subprocess.run(['eos','root://eosuser.cern.ch','ls',posix_path],
                          stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode==0

# --- Resubmit preparation ------------------------------------------------
def prepare_and_submit_resubmit(sample, timestamp, failed_set):
    resub_dir = os.path.join(RESUB_LOG_BASE, f'{sample}_{timestamp}')
    os.makedirs(resub_dir, exist_ok=True)

    # original arguments
    arg_path = os.path.join(CONDOR_FILES_PATH, f'arguments_{sample}.txt')
    orig_args = open(arg_path).read().splitlines() if os.path.isfile(arg_path) else []

    # write new arguments
    new_args = []
    for pid in sorted(failed_set, key=int):
        idx = int(pid)
        if idx < len(orig_args): new_args.append(orig_args[idx])
        else: logging.warning(f'proc_id {pid} out of range')
    args_file = os.path.join(resub_dir, f'arguments_failed_{sample}_{timestamp}.txt')
    with open(args_file,'w') as f: f.write('\n'.join(new_args)+'\n')
    logging.info(f'Resubmit arguments written: {args_file}')

    # write modified JDL
    orig_jdl = os.path.join(CONDOR_FILES_PATH, f'{sample}_condor.sub')
    new_jdl = os.path.join(resub_dir, f'{sample}_resubmit_{timestamp}.sub')
    with open(orig_jdl) as fin, open(new_jdl,'w') as fout:
        for line in fin:
            s = line.strip()
            if s.startswith('queue args from'):
                fout.write(f'queue args from {args_file}\n')
            elif s.startswith('output'):
                fout.write(f'output = {resub_dir}/job_{sample}.$(ClusterId).$(ProcId).out\n')
            elif s.startswith('error'):
                fout.write(f'error = {resub_dir}/error_{sample}.$(ClusterId).$(ProcId).err\n')
            elif s.startswith('log'):
                fout.write(f'log = {resub_dir}/log_{sample}.$(ClusterId).log\n')
            else:
                fout.write(line)
    logging.info(f'Resubmit JDL written: {new_jdl}')

    if RESUBMIT_MODE:
        ret = subprocess.run(['condor_submit', new_jdl])
        if ret.returncode==0:
            logging.info(f'Submitted resubmission for {sample} at {timestamp}')
        else:
            logging.error(f'condor_submit failed for {new_jdl}')
    else:
        logging.info('Dry-run mode: use --resubmit to actually submit jobs')

# --- Main workflow ------------------------------------------------------
def main():
    confirm_setup()
    if not os.path.isdir(CONDOR_FILES_PATH):
        logging.error(f'Condor files path not found: {CONDOR_FILES_PATH}')
        sys.exit(1)

    inspections = {}
    mismatches = []

    # Phase 1: inspect
    for sample in SAMPLES:
        print()  # blank line between samples
        logging.info(f'Inspecting sample: {sample}')
        tmpdirs = find_tmp_dirs_for_sample(sample)
        if not tmpdirs:
            logging.warning(f'No tmp dirs for {sample}')
            continue
        for tmp in tmpdirs:
            dirname = os.path.basename(tmp)
            _, ts = parse_sample_timestamp(dirname)
            logging.info(f'  Timestamp: {ts}')
            err_files = list_error_files(tmp, sample)
            pids = set(); failed_err = []
            for ef in err_files:
                parts = os.path.basename(ef).split('.')
                if len(parts)<4: continue
                pid = parts[-2]; pids.add(pid)
                if any(kw in read_error_content(ef) for kw in ERROR_KEYWORDS):
                    failed_err.append(pid)
            all_ids = sorted(pids, key=int)
            failed_err = sorted(set(failed_err), key=int)
            logging.info(f'    Detected IDs     : {all_ids or ["None"]}')
            logging.info(f'    Failed by error : {failed_err or ["None"]}')
            missing = [pid for pid in all_ids if not eos_file_exists(f'{EOS_BASE_POSIX}/{sample}/{sample}_{pid}.root')]
            logging.info(f'    Missing on EOS  : {missing or ["None"]}')
            inspections[(sample,ts)] = {'failed_err': set(failed_err), 'missing': set(missing)}
            if set(failed_err) != set(missing):
                mismatches.append((sample,ts))

    print()
    if mismatches:
        for s,ts in mismatches:
            logging.error(f'Mismatch for {s} at {ts}')
        sys.exit(1)

    # Phase 2: resubmit
    for (sample,ts), data in inspections.items():
        failed = data['failed_err'] | data['missing']
        if failed:
            prepare_and_submit_resubmit(sample, ts, failed)

    logging.info('====== Inspection & Resubmit Complete ======')

if __name__=='__main__':
    main()

