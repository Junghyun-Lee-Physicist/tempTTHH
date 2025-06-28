#!/usr/bin/env python3

import os
import sys
import time
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
        self.nameofExe = "ttHHanalyzer_trigger" # Name of compiled execution file to run analyzer
        ##self.path_output_base = "/eos/user/t/tom/<eos-storage or anywhere you want to store the outputs>"
        self.path_output_base = "/eos/user/j/junghyun/ttHH/Test"
        self.os_version = "el7"
        self.memorySize = "10 GB"
        self.jobFlavour = "tomorrow"

        self.config_file_path = os.path.join(self.analyzer_path, "AnalyzerConfig/TriggerEffStudy_2017_FH.txt")
        self.proxy_path = os.path.join(self.analyzer_path, "proxy.cert")
        self.condor_files_path = os.path.join(self.analyzer_path, "condor/filePath_Trigger")
        self.sample_list_path = os.path.join(self.analyzer_path, "filelistTrigger")

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
        with open(self.config_file_path, 'r') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue # Skep empty lines or comments

                try:
                    self.parse_config_line(line)
                    self.prepare_output_directory()
                    self.setup_and_submit_job()

                except Exception as e:
                    print("  Error with process of the condor job submition with {line} : {e}")
                    continue


    def parse_config_line(self, line):
        # Parse the configuration line into individual components
        parts = line.strip().split()
        if len(parts) < 6:
            raise ValueError(f"Invalid configuration line (expected at least 6 fields) : {line}")

        self.file_list_name = parts[0]
        self.output_dir = parts[1]
        self.weight = parts[2]
        self.year = parts[3]
        self.data_or_mc = parts[4]
        self.sample_name = parts[5]

        self.path_output = os.path.join(self.path_output_base, self.output_dir + "/")

        self.script_name = os.path.join(self.condor_files_path, f"run_{self.output_dir}.sh")
        self.condor_submit_name = os.path.join(self.condor_files_path, f"{self.output_dir}_condor.sub")
        self.arg_list_file = os.path.join(self.condor_files_path, f"arguments_{self.output_dir}.txt")
        self.tmp_folder = os.path.join(self.condor_files_path, f"tmp_{self.output_dir}_{self.time_info}")
        self.make_directory(self.tmp_folder)


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
        print("\nUser Home Memory Status:")
        subprocess.run(['fs', 'lq'])

        # Check EOS storage space
        print("\nEOS Storage Space:")
        eos_command = ['eos', 'quota']
        try:
            subprocess.run(eos_command)
        except Exception as e:
            print(f"Error checking EOS storage space: {e}")

        print("\n--------------------------------------------------------------------------\n")


    def prepare_output_directory(self):
        # Check if the output directory exists in EOS
        # Since the output directory is in EOS, we need to use EOS commands to check and create it
        eos_base_cmd = ['eos', 'root://eosuser.cern.ch']

        # Check if the directory exists
        check_dir_cmd = eos_base_cmd + ['ls', self.path_output]
        result = subprocess.run(check_dir_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if result.returncode != 0:
            # Directory does not exist, create it
            print(f"Creating EOS output directory: {self.path_output}")
            mkdir_cmd = eos_base_cmd + ['mkdir', '-p', self.path_output]
            result = subprocess.run(mkdir_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            if result.returncode != 0:
                print(f"Error creating output directory: {result.stderr}")
                sys.exit(1)
        else:
            print(f"EOS output directory exists: {self.path_output}")

        # Set permissions to 755
        chmod_cmd = eos_base_cmd + ['chmod', '755', self.path_output]
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
                    argout.write(f"{per_job_filelist_path} {self.output_dir}_{count}.root {self.weight} {self.year} {self.data_or_mc} {self.sample_name}\n")
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
            f.write(f"+JobFlavour             = \"{self.jobFlavour}\"\n")
            f.write(f"queue args from {self.arg_list_file}\n")

    def create_executable_script(self):
        # Create the executable script that Condor will run
        with open(self.script_name, 'w') as fout:
            fout.write("#!/bin/sh\n")
            fout.write("echo\n")
            fout.write("echo\n")
            fout.write("echo 'START---------------'\n")
            fout.write("echo 'WORKDIR ' ${PWD}\n")
            fout.write("source \"/afs/cern.ch/cms/cmsset_default.sh\"\n")
            fout.write(f"cd \"{self.analyzer_path}\"\n")
            fout.write("cmsenv\n")
            fout.write("echo 'WORKDIR ' ${PWD}\n")
            fout.write(f"source \"{self.analyzer_path}/setup.sh\"\n")
            # Prepare output directory in EOS
            fout.write(f"eos root://eosuser.cern.ch mkdir -p {self.path_output}\n")
            ##fout.write(f"eos root://eosuser.cern.ch chmod 777 {self.path_output}\n")
            fout.write(f"\"{self.analyzer_path}/{self.nameofExe}\" \"$1\" \"root://eosuser.cern.ch/{self.path_output}$2\" \"$3\" \"$4\" \"$5\" \"$6\"\n")
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
