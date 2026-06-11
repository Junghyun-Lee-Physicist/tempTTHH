"""
================================================================================
SCRIPT: Sample Configuration Generator for Merged Files (create_sample_config.py)
================================================================================

[ 1. Purpose ]
This script scans a directory containing ALREADY MERGED (hadd) ROOT files.
Each .root file is treated as a separate process/sample.
It generates a 'samples_config.yml' file compatible with the stack plotter.

[ 2. Assumption ]
You have already run 'hadd' so that you have files like:
  - /path/to/merged/ttHH.root
  - /path/to/merged/tttt.root
  - /path/to/merged/Data.root
  (Instead of folders full of split files)

================================================================================
"""

import os
import yaml
import glob
from itertools import cycle

# ==========================================
# User Configuration
# ==========================================
# Directory where your merged .root files are located
TARGET_DIR = "/Users/jhlee/ttHH/ntuple/skimmed/gen_tier3"

# Output YAML filename
OUTPUT_FILENAME = "samples_config.yml"

# Color Palette (Paul Tol's "Muted" Scheme)
COLOR_PALETTE = [
    '#332288', # Indigo
    '#88CCEE', # Cyan
    '#44AA99', # Teal
    '#117733', # Green
    '#999933', # Olive
    '#DDCC77', # Sand
    '#CC6677', # Rose
    '#882255', # Wine
    '#AA4499', # Purple
    '#DDDDDD', # Grey
]

def classify_process(filename):
    """
    Classify if a file is DATA or MC based on its name.
    """
    # Adjust these keywords based on your merged filenames
    ##data_keywords = ['Data', 'SingleMuon', 'DoubleMuon', 'JetHT', 'BTagCSV', 'EGamma', 'MuonEG']
    data_keywords = ['Data', 'JetHT', 'BTagCSV']

    name_clean = filename.replace(".root", "")
    for kw in data_keywords:
        if kw in name_clean:
            return 'DATA'
    return 'MC'

def generate_config():
    print(f"[-] Scanning merged files in: {TARGET_DIR}")
    
    if not os.path.exists(TARGET_DIR):
        print(f"[Error] Directory not found: {TARGET_DIR}")
        print("Please create the directory or update 'TARGET_DIR' in the script.")
        return

    # Basic YAML structure
    sample_config = {
        'description': 'Configuration for HADD-merged samples',
        'samples': {}
    }

    # Find all .root files in the top level of TARGET_DIR
    # (No recursive search anymore)
    root_files = sorted(glob.glob(os.path.join(TARGET_DIR, "*.root")))
    
    if not root_files:
        print(f"[Warning] No .root files found in {TARGET_DIR}")
        return

    # Color iterator
    color_cycle = cycle(COLOR_PALETTE)

    count = 0
    for file_path in root_files:
        # Get filename without extension for the Label/ID
        filename = os.path.basename(file_path)
        process_name = os.path.splitext(filename)[0] # e.g., "ttHH"
        
        proc_type = classify_process(filename)
        
        # Prepare entry
        sample_entry = {
            'type': proc_type,
            'path': os.path.dirname(file_path), # Directory path
            # Even if it's a single file, we keep it as a list for compatibility
            'files': [file_path], 
            'label': process_name,
        }

        # Assign Color
        if proc_type == 'DATA':
            sample_entry['color'] = '#000000'
            sample_entry['label'] = 'Data'
        else:
            sample_entry['color'] = next(color_cycle)

        # Add to config
        sample_config['samples'][process_name] = sample_entry
        count += 1
        print(f"    [+] Added: {process_name} ({proc_type})")

    # Save to YAML
    with open(OUTPUT_FILENAME, 'w') as f:
        yaml.dump(sample_config, f, default_flow_style=False, sort_keys=False)
    
    print(f"\n[Success] Found {count} files. Configuration saved to: {os.path.abspath(OUTPUT_FILENAME)}")

if __name__ == "__main__":
    generate_config()
