
import os
import subprocess

class ResultChecker:
    def __init__(self, sample_name, output_dir, arg_list_file):
        self.sample_name = sample_name
        self.output_dir = output_dir  # Should be full path
        self.arg_list_file = arg_list_file
        self.result_check_file = os.path.join(self.output_dir, 'ResultCheck.txt')

    def check_results(self):
        # Read expected output files from the argument list
        expected_files = []
        with open(self.arg_list_file, 'r') as f:
            for line in f:
                args = line.strip().split()
                if len(args) >= 2:
                    output_file = args[1]
                    expected_files.append(output_file)
        # Now check if these files exist in the output directory
        eos_base_cmd = ['eos', 'root://eosuser.cern.ch']
        result_entries = []
        for output_file in expected_files:
            output_file_path = os.path.join(self.output_dir, output_file)
            check_file_cmd = eos_base_cmd + ['ls', output_file_path]
            result = subprocess.run(
                check_file_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
            )
            if result.returncode == 0:
                # File exists
                result_entries.append(f"{self.sample_name} {output_file} O")
            else:
                # File does not exist
                result_entries.append(f"{self.sample_name} {output_file} X")

        # Write the ResultCheck.txt file
        with open(self.result_check_file, 'w') as f:
            for entry in result_entries:
                f.write(entry + '\n')

        print(f"\nResult check completed. Results written to {self.result_check_file}")

