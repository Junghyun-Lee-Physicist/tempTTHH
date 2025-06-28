import os

# Set the path to your specific directory
directory_path = os.getcwd()

# Find all filelist_*.txt files in the directory
filelists = [f for f in os.listdir(directory_path) if f.startswith('filelist_') and f.endswith('.txt')]

for filelist_name in filelists:
    # Extract directory name from the filelist name (e.g., 'ttHH' or 'QCD_HT100to200')
 
    if 'JetHT' or 'BTagCSV' in filelist_name:
        # For QCD files, include the full range specification in the directory name
        directory_name = '_'.join(filelist_name.split('_')[1:3]).split('.')[0]
    else:
        # For other files, just take the part after 'filelist_' and before '.txt'
        directory_name = filelist_name.split('_')[1].split('.')[0]

    # Construct the path for the resulting directory
    result_directory_path = os.path.join(directory_path, directory_name)

    # Create the directory if it does not exist
    if not os.path.exists(result_directory_path):
        os.makedirs(result_directory_path)

    # Open the filelist file and create separate files for each line
    with open(os.path.join(directory_path, filelist_name), 'r') as filelist:
        for index, line in enumerate(filelist):
            new_filename = f"file_{directory_name}_{index}.txt"
            new_filepath = os.path.join(result_directory_path, new_filename)
            with open(new_filepath, 'w') as new_file:
                new_file.write(line.strip())

print("Process completed: Directories and files have been created for each filelist.")

