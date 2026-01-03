#!/usr/bin/env python3

import os
import sys
import time
import re
import subprocess
script_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(script_dir, 'python', 'ttHHmodules'))
import ProxyChecker
import resultChecker


class CondorJobManager:

    def __init__(self):

        self.time_info = time.strftime("%Y%m%d-%H%M%S")

        # Variables for jobs, Please check before you run this script
        self.analyzer_path = f"{script_dir}"
        self.nameofExe = "ttHHanalyzer_bTagSF" # Name of compiled execution file to run analyzer
        ##self.path_output_base = "/eos/user/t/tom/<eos-storage or anywhere you want to store the outputs>"
        self.path_output_base = "/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput"
        self.os_version = "el9"
        self.memorySize = "10 GB"
##        self.jobFlavour = "tomorrow"

        self.config_file_path = os.path.join(self.analyzer_path, "AnalyzerConfig/Tier3_2017_FH.yml")
        self.proxy_path = os.path.join(self.analyzer_path, "proxy.cert")
        self.condor_files_path = os.path.join(self.analyzer_path, "condor/filelistTier3")
        self.sample_list_path = os.path.join(self.analyzer_path, "filelistTier3")

        self.make_directory(self.condor_files_path)
        self.make_directory(self.path_output_base, 777) # 2nd argument is for the permissionsn

        try:
            print(f"Setted Path of proxy : {self.proxy_path}")
            proxy_checker = ProxyChecker.ProxyChecker(self.proxy_path)
            proxy_checker.check()
        except Exception as e:
            print(f"{e}") # Error Messages which are defined in ProxyChecker.py
            sys.exit(1)

        self.print_memory_status()
        self.process_config_file()


    def make_directory(self, path, permission=755):
        try:
            subprocess.run(['mkdir', '-p', path], check=True)
        except subprocess.CalledProcessError as e:
            print(f"  Error creating drectory {path} : {e}")
            sys.exit(1)
       
        if permission == 777:
            chmod_cmd = ['chmod', '777', path]
            result = subprocess.run(chmod_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            if result.returncode != 0:
                print(f"Error setting permissions on output directory: {result.stderr}")
                sys.exit(1)
            else:
                print(f"Set permissions to 777 for {path}")


    def process_config_file(self):
        config = self.load_yaml_config(self.config_file_path)
        common = config.get("common", {})
        for entry in config.get("samples", []):
            try:
                self.parse_config_entry(entry, common)
                self.prepare_output_directory()
                self.setup_and_submit_job()

            except Exception as e:
                print(f"  Error with process of the condor job submition with {entry} : {e}")
                continue


    def parse_config_entry(self, entry, common):
        required_keys = [
            "filelist",
            "output_dir",
            "weight",
            "data_or_mc",
            "sample_name",
        ]
        missing_keys = [key for key in required_keys if key not in entry]
        if missing_keys:
            raise ValueError(f"Invalid configuration entry (missing keys: {missing_keys}) : {entry}")

        self.file_list_name = entry["filelist"]
        self.output_dir = entry["output_dir"]
        self.weight = entry["weight"]
        self.year = entry.get("year", common.get("year", ""))
        self.data_or_mc = entry["data_or_mc"]
        self.sample_name = entry["sample_name"]
        self.era = entry.get("era", "")

        if self.data_or_mc == "MC" and str(self.era).strip():
            raise ValueError("MC samples must not define an eraName. Please leave era empty.")

        self.path_output = os.path.join(self.path_output_base, self.output_dir + "/")

        self.script_name = os.path.join(self.condor_files_path, f"run_{self.output_dir}.sh")
        self.condor_submit_name = os.path.join(self.condor_files_path, f"{self.output_dir}_condor.sub")
        self.arg_list_file = os.path.join(self.condor_files_path, f"arguments_{self.output_dir}.txt")
        self.tmp_folder = os.path.join(self.condor_files_path, f"tmp_{self.output_dir}_{self.time_info}")
        self.make_directory(self.tmp_folder)

    def load_yaml_config(self, path):
        def parse_value(raw_value):
            value = raw_value.strip()
            if (value.startswith('"') and value.endswith('"')) or (value.startswith("'") and value.endswith("'")):
                value = value[1:-1]
            if re.fullmatch(r"-?\\d+\\.\\d+", value):
                return float(value)
            return value

        config = {"common": {}, "samples": []}
        section = None
        current_sample = None

        with open(path, "r") as f:
            for raw_line in f:
                line = raw_line.strip()
                if not line or line.startswith("#"):
                    continue

                if line == "common:":
                    section = "common"
                    continue
                if line == "samples:":
                    section = "samples"
                    continue

                if section == "samples" and line.startswith("- "):
                    if current_sample:
                        config["samples"].append(current_sample)
                    current_sample = {}
                    line = line[2:].strip()
                    if line:
                        key, value = line.split(":", 1)
                        current_sample[key.strip()] = parse_value(value)
                    continue

                if ":" in line:
                    key, value = line.split(":", 1)
                    if section == "common":
                        config["common"][key.strip()] = parse_value(value)
                    elif section == "samples":
                        if current_sample is None:
                            current_sample = {}
                        current_sample[key.strip()] = parse_value(value)

        if current_sample:
            config["samples"].append(current_sample)

        return config


    def print_memory_status(self):

        print("\n--------------------------------------------------------------------------\n")
        print("  --> Memory Check before submit jobs <--  ")

        # Print system memory status
        print("\nSystem Memory Status:")
        subprocess.run(['free', '-h'])

        ### Print disk usage status
        ##print("\nDisk Usage Status:")
        ##subprocess.run(['df', '-h'])
        
        # Print User Home directory space
####        print("\nUser Home Memory Status:")
####        subprocess.run(['fs', 'lq'])

####        # Check EOS storage space
####        print("\nEOS Storage Space:")
####        eos_command = ['eos', 'quota']
####        try:
####            subprocess.run(eos_command)
####        except Exception as e:
####            print(f"Error checking EOS storage space: {e}")

        print("\n--------------------------------------------------------------------------\n")


    def prepare_output_directory(self):
        # Check if the output directory exists in EOS
        # Since the output directory is in EOS, we need to use EOS commands to check and create it
####        eos_base_cmd = ['eos', 'root://eosuser.cern.ch']

        # Check if the directory exists
####        check_dir_cmd = eos_base_cmd + ['ls', self.path_output]
        check_dir_cmd = ['ls', '-d', self.path_output]

        result = subprocess.run(check_dir_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if result.returncode != 0:
            # Directory does not exist, create it
            print(f"Creating EOS output directory: {self.path_output}")
####            mkdir_cmd = eos_base_cmd + ['mkdir', '-p', self.path_output]
            mkdir_cmd = ['mkdir', '-p', self.path_output]
            mk_result = subprocess.run(mkdir_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            if mk_result.returncode != 0:
                print(f"Error creating output directory: {result.stderr}")
                sys.exit(1)
        else:
            print(f"EOS output directory exists: {self.path_output}")

        # Set permissions to 755
####        chmod_cmd = eos_base_cmd + ['chmod', '755', self.path_output]
        chmod_cmd = ['chmod', '755', self.path_output]
        result = subprocess.run(chmod_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if result.returncode != 0:
            print(f"Error setting permissions on output directory: {result.stderr}")
            sys.exit(1)
        else:
            print(f"Set permissions to 755 for {self.path_output}")


    def generate_argument_list(self):
        # Create the argument list file for Condor submission
        with open(self.arg_list_file, "w") as argout:
            count = 0
            # Read the sample list file and split it into per-job files
            sample_list_file_path = os.path.join(self.sample_list_path, self.file_list_name)
            with open(sample_list_file_path, "r") as sample_list_in:
                lines = [line.strip() for line in sample_list_in if line.strip() and not line.startswith('#')]
                for line in lines:
                    # For each line, create a per-job filelist in the tmp_folder
                    per_job_filelist_name = f"filelist_{self.output_dir}_{count}.txt"
                    per_job_filelist_path = os.path.join(self.tmp_folder, per_job_filelist_name)
                    with open(per_job_filelist_path, 'w') as per_job_filelist:
                        per_job_filelist.write(line + '\n')

                    # Write the arguments for this job to the argument list file
                    argout.write(
                        f"{per_job_filelist_path} {self.output_dir}_{count}.root {self.weight} {self.year} "
                        f"{self.data_or_mc} {self.sample_name} \"{self.era}\"\n"
                    )
                    count += 1

    
    def write_condor_submission_file(self):
        # Create the Condor submission script
        with open(self.condor_submit_name, 'w') as f:
            f.write(f"x509userproxy           = {self.proxy_path}\n")
            f.write("getenv                  = True\n")
            f.write(f"executable              = {self.script_name}\n")
            f.write("arguments               = $(args)\n")
            f.write(f"output                  = {os.path.join(self.tmp_folder, f'job_{self.sample_name}.$(ClusterId).$(ProcId).out')}\n")
            f.write(f"MY.WantOS               = \"{self.os_version}\"\n")
            f.write("MY.XRDCP_CREATE_DIR     = True\n")
            f.write(f"error                   = {os.path.join(self.tmp_folder, f'error_{self.sample_name}.$(ClusterId).$(ProcId).err')}\n")
            f.write(f"log                     = {os.path.join(self.condor_files_path, f'log_{self.sample_name}.$(ClusterId).log')}\n")
            f.write(f"request_memory          = {self.memorySize}\n")
#            f.write(f"+JobFlavour             = \"{self.jobFlavour}\"\n")
            f.write(f"queue args from {self.arg_list_file}\n")

    def create_executable_script(self):
        # Create the executable script that Condor will run
        with open(self.script_name, 'w') as fout:
            fout.write("#!/bin/sh\n")
            fout.write("echo\n")
            fout.write("echo\n")
            fout.write("echo 'START---------------'\n")
            fout.write("echo 'WORKDIR ' ${PWD}\n")
            fout.write("source \"/cvmfs/cms.cern.ch/cmsset_default.sh\"\n")
            fout.write(f"cd \"{self.analyzer_path}\"\n")
            fout.write("cmsenv\n")
            fout.write("echo 'WORKDIR ' ${PWD}\n")
            fout.write(f"source \"{self.analyzer_path}/setup.sh\"\n")
            # Prepare output directory in EOS
####            fout.write(f"eos root://eosuser.cern.ch mkdir -p {self.path_output}\n")
            fout.write(f"mkdir -p {self.path_output}\n")
            ##fout.write(f"eos root://eosuser.cern.ch chmod 777 {self.path_output}\n")
##            fout.write(f"\"{self.analyzer_path}/{self.nameofExe}\" \"$1\" \"root://eosuser.cern.ch/{self.path_output}$2\" \"$3\" \"$4\" \"$5\" \"$6\" \"$7\"\n")
            fout.write(f"\"{self.analyzer_path}/{self.nameofExe}\" \"$1\" \"{self.path_output}$2\" \"$3\" \"$4\" \"$5\" \"$6\" \"$7\"\n")
        subprocess.call(["chmod", "755", self.script_name])


    def submit_job(self):
        # Submit the job to Condor
        print(f"Submitting job for sample: {self.sample_name}")
        subprocess.call(["condor_submit", self.condor_submit_name])

    def setup_and_submit_job(self):
        # Run all steps to set up and submit the job
        print(f"\nSetting up job for sample: {self.sample_name}")
        self.generate_argument_list()
        self.create_executable_script()
        self.write_condor_submission_file()
        self.submit_job()


def main():
    try:
        # Initialize job manager
        job_manager = CondorJobManager()
        # Job processing happens within the class
    except Exception as e:
        print(e)
        sys.exit(1)

if __name__ == "__main__":
    main()
