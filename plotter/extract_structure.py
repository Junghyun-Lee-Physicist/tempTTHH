"""
================================================================================
SCRIPT: ROOT Structure Extractor (extract_structure.py)
================================================================================

[ 1. Purpose ]
This script reads ONE representative ROOT file to map its internal structure.
It saves the hierarchy of Directories, Trees, and Histograms to a YAML file.
This structure helps the plotter know "what variables are available to plot".

[ 2. Updates ]
- Removed filename/fullpath metadata from output.
- Added a simple 'description' field.
- Hardcoded input/output paths.

================================================================================
"""

import uproot
import yaml
import os
import sys

# ==========================================
# User Configuration (Hardcoded)
# ==========================================
# Pick ONE representative file to scan structure (e.g., one tttt file)
INPUT_FILE = "/Users/jhlee/ttHH/ntuple/skimmed/gen_tier3_base/TTToHadronic.root"

# Output YAML filename
OUTPUT_FILE = "structure_info.yml"

def get_object_group(classname):
    """
    Simplify ROOT class names into groups.
    """
    if classname.startswith('TH') or classname.startswith('TProfile'):
        return 'Histogram'
    elif classname.startswith('TGraph'):
        return 'Graph'
    elif classname.startswith('TTree') or classname.startswith('TNtuple'):
        return 'Tree'
    elif classname.startswith('TDirectory'):
        return 'Directory'
    else:
        return 'Unknown'

def recursive_scan(directory):
    """
    Recursively scans the directory and returns the dictionary structure.
    """
    structure = {}
    keys = directory.keys(cycle=False) # cycle=False removes ';1' suffix
    
    for key in keys:
        try:
            obj = directory[key]
            classname = obj.classname
            group = get_object_group(classname)
            
            # Basic info
            item_info = {
                'type': group,
                'classname': classname,
                # Key path for uproot access later (e.g. "dir/hist")
                'key_path': f"{directory.path}/{key}".strip("/") 
            }

            # Recursion for Directories
            if group == 'Directory':
                item_info['contents'] = recursive_scan(obj)
            
            # Info for Trees
            elif group == 'Tree':
                item_info['num_entries'] = obj.num_entries
                item_info['branches'] = obj.keys() # List all branches
                
            # Info for Histograms
            elif group == 'Histogram':
                # We can store bin info here if needed in future
                pass

            structure[key] = item_info

        except Exception as e:
            print(f"[Warning] Skipped {key}: {e}")
            continue
            
    return structure

def extract():
    print(f"[-] Inspecting structure of: {INPUT_FILE}")
    
    if not os.path.exists(INPUT_FILE):
        print(f"[Error] File not found: {INPUT_FILE}")
        print("Please check the 'INPUT_FILE' path in the code.")
        sys.exit(1)

    try:
        with uproot.open(INPUT_FILE) as file:
            # Get the full structure
            content_structure = recursive_scan(file)
            
            # Final Output Format
            output_data = {
                'description': 'Internal structure of Analyzer Output files',
                'structure': content_structure
            }

            # Save to YAML
            with open(OUTPUT_FILE, 'w') as f:
                yaml.dump(output_data, f, default_flow_style=False, sort_keys=False)
            
            print(f"[Success] Structure saved to: {OUTPUT_FILE}")

    except Exception as e:
        print(f"[Error] Failed to read file: {e}")

if __name__ == "__main__":
    extract()
