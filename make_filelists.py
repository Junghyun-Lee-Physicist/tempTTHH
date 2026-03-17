import os
import shutil

# ==============================================================================
# [설정] 경로 확인
# ==============================================================================
##SAMPLE_DIR = "/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH2017UL_30thDec2025_v8"
SAMPLE_DIR = "/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH2017UL_09thMar2026_v9_AddttJetsBranches"
OUTPUT_DIR = "filelistTier3" 

# ==============================================================================
# [대상 디렉토리 목록]
# ==============================================================================
directories = [
    "BTagCSV",
    "JetHT",
    "SingleMuon",
    "QCD_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8",
    "QCD_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8",
    "QCD_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8",
    "QCD_HT200to300_TuneCP5_13TeV-madgraphMLM-pythia8",
    "QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8",
    "QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8",
    "QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8",
    "TT4b_TuneCP5_13TeV_madgraph_pythia8",
    "TTHHTo4b_TuneCP5_13TeV-madgraph-pythia8",
    "TTTT_TuneCP5_13TeV-amcatnlo-pythia8",
    "TTTW_TuneCP5_13TeV-madgraph-pythia8",
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8",
    "TTToHadronic_TuneCP5_13TeV-powheg-pythia8",
    "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8",
    "TTWH_TuneCP5_13TeV-madgraph-pythia8",
    "TTWJetsToQQ_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8",
    "TTWW_TuneCP5_13TeV-madgraph-pythia8",
    "TTWZ_TuneCP5_13TeV-madgraph-pythia8",
    "TTZHTo4b_TuneCP5_13TeV-madgraph-pythia8",
    "TTZToBB_TuneCP5_13TeV-amcatnlo-pythia8",
    "TTZZTo4b_TuneCP5_13TeV-madgraph-pythia8",
    "TTbb_4f_TTToHadronic_TuneCP5-Powheg-Openloops-Pythia8",
    "ttHTobb_M125_TuneCP5_13TeV-powheg-pythia8"
]

mc_name_mapping = {
    "TTTo2L2Nu": "TTTo2L2Nu",
    "TTToHadronic": "TTToHadronic",
    "TTToSemiLeptonic": "TTToSemiLeptonic",
    "TTHHTo4b": "ttHH",
    "TT4b": "tt4b",
    "ttHTobb": "ttHtobb",
    "TTTT": "tttt",
    "TTTW": "tttW",
    "TTWH": "ttWH",
    "TTWW": "ttWW",
    "TTWZ": "ttWZ",
    "TTZHTo4b": "ttZHto4b",
    "TTZToBB": "ttZtobb",
    "TTZZTo4b": "ttZZto4b",
    "TTbb_4f": "ttbb",
    "TTWJetsToQQ": "ttWJetsToQQ",
}

def find_root_files(start_path):
    """주어진 경로 아래의 tree*.root 파일의 절대 경로를 리스트로 반환"""
    root_files = []
    for root, dirs, files in os.walk(start_path):
        for file in files:
            # tree*.root 패턴 매칭
            if file.startswith("slimmedNtuple") and file.endswith(".root"):
                absolute_path = os.path.abspath(os.path.join(root, file))
                root_files.append(absolute_path)
    return root_files

def create_split_files(filename, paths):
    """
    1. filelist_NAME.txt -> NAME 디렉토리 생성
    2. 경로 리스트를 하나씩 쪼개서 file_NAME_0.txt 등으로 저장
    """
    # 확장자(.txt) 제거
    base_name = os.path.splitext(filename)[0]
    
    # 접두사(filelist_) 제거하여 코어 이름 추출 (예: QCD_HT2000toInf)
    if base_name.startswith("filelist_"):
        core_name = base_name.replace("filelist_", "")
    else:
        core_name = base_name

    # 분할 파일들을 저장할 하위 디렉토리 경로
    split_dir_path = os.path.join(OUTPUT_DIR, core_name)
    
    # 디렉토리가 없으면 생성
    if not os.path.exists(split_dir_path):
        os.makedirs(split_dir_path)

    # 개별 파일 생성 루프
    for i, path in enumerate(paths):
        # 개별 파일 이름: file_NAME_0.txt
        individual_filename = f"file_{core_name}_{i}.txt"
        individual_filepath = os.path.join(split_dir_path, individual_filename)
        
        with open(individual_filepath, 'w') as f:
            f.write(path + '\n')
            
    return core_name, len(paths)

def write_and_split(filename, paths):
    """Master List 작성 후 바로 Split 수행"""
    if not paths:
        print(f"    [WARNING] '{filename}' 생성을 건너뜁니다. (파일 0개)")
        return
    
    # 1. Master List 작성 (기존 기능)
    output_path = os.path.join(OUTPUT_DIR, filename)
    with open(output_path, 'w') as f:
        for path in paths:
            f.write(path + '\n')
    
    # 2. Split 기능 수행 (추가된 기능)
    core_name, count = create_split_files(filename, paths)
    
    print(f"    [SUCCESS] {filename} ({count} files)")
    print(f"       └─ Split into folder: {OUTPUT_DIR}/{core_name}/ (file_{core_name}_0.txt ...)")

def main():
    print("="*60)
    print(f"Start scanning in: {SAMPLE_DIR}")
    print(f"Output Directory : {OUTPUT_DIR}")
    print("="*60)

    if not os.path.exists(SAMPLE_DIR):
        print(f"[CRITICAL ERROR] '{SAMPLE_DIR}' 경로가 없습니다.")
        return

    if not os.path.exists(OUTPUT_DIR):
        os.makedirs(OUTPUT_DIR)

    try:
        actual_contents = os.listdir(SAMPLE_DIR)
    except Exception as e:
        print(f"Error reading directory: {e}")
        return

    for dirname in directories:
        full_dir_path = os.path.join(SAMPLE_DIR, dirname)
        
        if dirname not in actual_contents:
            print(f"[MISSING] Directory not found: {dirname}")
            continue

        print(f"-> [PROCESSING] {dirname} ...")

        # --- CASE 1: Data (JetHT, BTagCSV) ---
##        if "JetHT" in dirname or "BTagCSV" in dirname or "SingleMuon" in dirname:
##            process_prefix = "JetHT" if "JetHT" in dirname else "BTagCSV"
        if "JetHT" in dirname or "BTagCSV" in dirname or "SingleMuon" in dirname:
            process_prefix = "JetHT" if "JetHT" in dirname else ("SingleMuon" if "SingleMuon" in dirname else "BTagCSV")

            subdirs = []
            try:
                subdirs = [d for d in os.listdir(full_dir_path) if os.path.isdir(os.path.join(full_dir_path, d))]
            except:
                pass
            
            data_found = False
            for subdir in subdirs:
                if "Run2017" in subdir:
                    period = subdir[-1] 
                    output_filename = f"filelist_{process_prefix}_{period}.txt"
                    
                    target_path = os.path.join(full_dir_path, subdir)
                    paths = find_root_files(target_path)
                    
                    if len(paths) > 0:
                        write_and_split(output_filename, paths) # 변경된 함수 호출
                        data_found = True
            
            if not data_found:
                print(f"    [WARNING] {dirname} 내부에서 유효한 Data 폴더를 찾지 못했습니다.")

        # --- CASE 2: QCD Samples ---
        elif "QCD" in dirname:
            short_name = dirname.split("_TuneCP5")[0]
            output_filename = f"filelist_{short_name}.txt"
            
            paths = find_root_files(full_dir_path)
            write_and_split(output_filename, paths) # 변경된 함수 호출

        # --- CASE 3: MC Samples ---
        else:
            short_name = None
            for key, val in mc_name_mapping.items():
                if dirname.startswith(key):
                    short_name = val
                    break
            
            if short_name is None:
                short_name = dirname.split("_TuneCP5")[0]
                print(f"    [NOTICE] 매핑 정보 없음. 임시 이름 사용: {short_name}")

            output_filename = f"filelist_{short_name}.txt"
            paths = find_root_files(full_dir_path)
            write_and_split(output_filename, paths) # 변경된 함수 호출

    print("\n" + "="*60)
    print("All tasks finished. Check the output directory.")
    print("="*60)

if __name__ == "__main__":
    main()
