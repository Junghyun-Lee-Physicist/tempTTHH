import os

# ==============================================================================
# [설정] 경로
# ==============================================================================
# [STEP17] ntuple production 갱신: ttHH2017UL_fullNano_v20.
#   - config_ttHH2017UL.yaml (NtupleForge) 로 생산된 full-NanoAOD pass-through.
#   - 출력 파일명은 기존과 동일하게 slimmedNtuple_<N>.root (glob 불변).
#   - 새 dataset(렙토닉 V+jets/DY, single t, diboson, tH, 렙토닉 ttV 등) 추가.
SAMPLE_DIR = "/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH2017UL_fullNano_v20"
OUTPUT_DIR = "filelistTier3"

# ==============================================================================
# [샘플 매핑] CRAB on-disk 디렉토리(=DAS primary dataset 이름) -> 출력 short_name
# ------------------------------------------------------------------------------
#   - key   : SAMPLE_DIR 아래 디렉토리 이름. CRAB LFN 의 <primaryDataset> 이며
#             DAS 경로의 첫 컴포넌트와 같다 (config YAML 의 key 가 아님).
#   - value : downstream canonical 식별자(short_name). filelist_<short_name>.txt,
#             analyzer --sample, data/samples_2017UL.json, Config_TtCatGroup.hh,
#             prescan_summary.json, plotter yml 이 모두 이 이름을 쓴다.
#
#   [naming 정책 — STEP17]
#     - 기존 샘플: OLD short_name 유지 (TTbar_Hadronic / ttHH / tt4b / ttbb_* ...).
#       config YAML 이 key 를 rename(TTbar_Hadronic 등) 했지만 그건 ntuplizer 내부
#       라벨일 뿐 on-disk primary dataset 이름은 불변이고, analyzer 의 exact-match
#       categorization/stitching(Config_TtCatGroup.hh) 이 OLD 이름에 묶여 있으므로
#       바꾸지 않는다 (물리 로직 리스크 0).
#     - 신규 샘플: config YAML key 를 그대로 short_name 으로 사용 (legacy 없음).
#
#   [ext 자동 병합]
#     base 와 _ext1/_ext2 는 primary dataset 이름이 동일하므로 SAMPLE_DIR 아래
#     같은 디렉토리에 (publish 태그만 다른) 하위폴더로 들어간다. find_root_files()
#     가 os.walk 로 재귀 수집하므로 base+ext 가 자동으로 한 filelist 에 병합된다
#     (config 주석 "combine BOTH for statistics" 와 일치). → ext 별 항목 불필요.
#
#   - Data(BTagCSV/JetHT/SingleMuon): short_name=None → Run2017 period(B/C/D/E/F)
#     별로 filelist_<PD>_<period>.txt 로 분할 (예: filelist_JetHT_B.txt).
# ==============================================================================
sample_mapping = {
    # ── Data (period 별 분할) ──
    "BTagCSV":     None,
    "JetHT":       None,
    "SingleMuon":  None,

    # ── QCD multijet (HT-binned) ──
    "QCD_HT200to300_TuneCP5_13TeV-madgraphMLM-pythia8":   "QCD_HT200to300",
    "QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8":   "QCD_HT300to500",
    "QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8":   "QCD_HT500to700",
    "QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8":  "QCD_HT700to1000",
    "QCD_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8": "QCD_HT1000to1500",
    "QCD_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8": "QCD_HT1500to2000",
    "QCD_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8":  "QCD_HT2000toInf",

    # ── ttbar inclusive (5FS Powheg) ── [OLD short_name 유지]
    "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8":        "TTbar_DiLep",
    "TTToHadronic_TuneCP5_13TeV-powheg-pythia8":     "TTbar_Hadronic",
    "TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8": "TTbar_SemiLep",

    # ── tt+bb (4FS Powheg-OpenLoops) dedicated ── [OLD short_name 유지]
    "TTbb_4f_TTToHadronic_TuneCP5-Powheg-Openloops-Pythia8":     "TTbb_Hadronic",
    "TTbb_4f_TTToSemiLeptonic_TuneCP5-Powheg-Openloops-Pythia8": "TTbb_SemiLep",
    "TTbb_4f_TTTo2L2Nu_TuneCP5-Powheg-Openloops-Pythia8":        "TTbb_DiLep",

    # ── tt4b dedicated (madgraph LO) ── [OLD short_name 유지]
    "TT4b_TuneCP5_13TeV_madgraph_pythia8": "TT4b",

    # ── signal ttHH / ttH(bb) ── [OLD short_name 유지]
    "TTHHTo4b_TuneCP5_13TeV-madgraph-pythia8":   "TTHHto4b",
    "ttHTobb_M125_TuneCP5_13TeV-powheg-pythia8": "ttHTobb",

    # ── ttH (H->non-bb) ── [신규]
    "ttHToNonbb_M125_TuneCP5_13TeV-powheg-pythia8": "ttHToNonbb",

    # ── tH (single-top + H) ── [신규]
    "THQ_ctcvcp_4f_Hincl_TuneCP5_13TeV_madgraph_pythia8": "tHq",
    "THW_ctcvcp_5f_Hincl_TuneCP5_13TeV_madgraph_pythia8": "tHW",

    # ── ttV, 하드로닉 V decay ──
    "TTZToBB_TuneCP5_13TeV-amcatnlo-pythia8":                 "TTZToBB",  # [OLD 유지]
    "TTWJetsToQQ_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8": "TTWJetsToQQ",

    # ── ttV, 렙토닉 V decay ── [신규]
    "TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8": "TTWJetsToLNu",
    "TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8":         "TTZToLLNuNu",

    # ── ttVV / ttVH / multi-top ── [OLD short_name 유지; ext1 자동 병합]
    "TTZHTo4b_TuneCP5_13TeV-madgraph-pythia8": "TTZHTo4b",
    "TTZZTo4b_TuneCP5_13TeV-madgraph-pythia8": "TTZZTo4b",
    "TTWW_TuneCP5_13TeV-madgraph-pythia8":     "TTWW",
    "TTWH_TuneCP5_13TeV-madgraph-pythia8":     "TTWH",
    "TTWZ_TuneCP5_13TeV-madgraph-pythia8":     "TTWZ",
    "TTTW_TuneCP5_13TeV-madgraph-pythia8":     "TTTW",
    "TTTT_TuneCP5_13TeV-amcatnlo-pythia8":     "TTTT",

    # ── Single top (non-Higgs) ── [신규]
    "ST_t-channel_top_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin-pythia8":     "ST_t_top",
    "ST_t-channel_antitop_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin-pythia8": "ST_t_antitop",
    "ST_tW_top_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8":                    "ST_tW_top",
    "ST_tW_antitop_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8":                "ST_tW_antitop",
    "ST_s-channel_4f_leptonDecays_TuneCP5_13TeV-amcatnlo-pythia8":                  "ST_s_lep",
    "ST_s-channel_4f_hadronicDecays_TuneCP5_13TeV-amcatnlo-pythia8":                "ST_s_had",

    # ── Diboson ── [신규]
    "WW_TuneCP5_13TeV-pythia8": "WW",
    "WZ_TuneCP5_13TeV-pythia8": "WZ",
    "ZZ_TuneCP5_13TeV-pythia8": "ZZ",

    # ── V+jets, V->qq (하드로닉, HT-binned) ── [신규]
    "WJetsToQQ_HT-400to600_TuneCP5_13TeV-madgraphMLM-pythia8": "WJetsToQQ_HT400to600",
    "WJetsToQQ_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8": "WJetsToQQ_HT600to800",
    "WJetsToQQ_HT-800toInf_TuneCP5_13TeV-madgraphMLM-pythia8": "WJetsToQQ_HT800toInf",
    "ZJetsToQQ_HT-400to600_TuneCP5_13TeV-madgraphMLM-pythia8": "ZJetsToQQ_HT400to600",
    "ZJetsToQQ_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8": "ZJetsToQQ_HT600to800",
    "ZJetsToQQ_HT-800toInf_TuneCP5_13TeV-madgraphMLM-pythia8": "ZJetsToQQ_HT800toInf",

    # ── W+jets, W->lnu (렙토닉, gen-HT binned) ── [신규; ext1/ext2 자동 병합]
    "WJetsToLNu_HT-70To100_TuneCP5_13TeV-madgraphMLM-pythia8":    "WJetsToLNu_HT70To100",
    "WJetsToLNu_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8":   "WJetsToLNu_HT100To200",
    "WJetsToLNu_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8":   "WJetsToLNu_HT200To400",
    "WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8":   "WJetsToLNu_HT400To600",
    "WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8":   "WJetsToLNu_HT600To800",
    "WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8":  "WJetsToLNu_HT800To1200",
    "WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8": "WJetsToLNu_HT1200To2500",
    "WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8":  "WJetsToLNu_HT2500ToInf",

    # ── Z+jets / DY, Z->ll (M-50, 렙토닉, gen-HT binned) ── [신규]
    "DYJetsToLL_M-50_HT-70to100_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8":    "DYJetsToLL_M50_HT70to100",
    "DYJetsToLL_M-50_HT-100to200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8":   "DYJetsToLL_M50_HT100to200",
    "DYJetsToLL_M-50_HT-200to400_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8":   "DYJetsToLL_M50_HT200to400",
    "DYJetsToLL_M-50_HT-400to600_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8":   "DYJetsToLL_M50_HT400to600",
    "DYJetsToLL_M-50_HT-600to800_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8":   "DYJetsToLL_M50_HT600to800",
    "DYJetsToLL_M-50_HT-800to1200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8":  "DYJetsToLL_M50_HT800to1200",
    "DYJetsToLL_M-50_HT-1200to2500_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8": "DYJetsToLL_M50_HT1200to2500",
    "DYJetsToLL_M-50_HT-2500toInf_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8":  "DYJetsToLL_M50_HT2500toInf",
}


def find_root_files(start_path):
    """주어진 경로 아래의 slimmedNtuple*.root 파일의 절대 경로를 리스트로 반환.
    base+ext 가 같은 primary dataset 디렉토리에 있으면 os.walk 로 함께 수집됨."""
    root_files = []
    for root, dirs, files in os.walk(start_path):
        for file in files:
            if file.startswith("slimmedNtuple") and file.endswith(".root"):
                absolute_path = os.path.abspath(os.path.join(root, file))
                root_files.append(absolute_path)
    return root_files


def create_split_files(filename, paths):
    """
    1. filelist_NAME.txt -> NAME 디렉토리 생성
    2. 경로 리스트를 하나씩 쪼개서 file_NAME_0.txt 등으로 저장
    """
    base_name = os.path.splitext(filename)[0]

    if base_name.startswith("filelist_"):
        core_name = base_name.replace("filelist_", "")
    else:
        core_name = base_name

    split_dir_path = os.path.join(OUTPUT_DIR, core_name)

    if not os.path.exists(split_dir_path):
        os.makedirs(split_dir_path)

    for i, path in enumerate(paths):
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

    # 1. Master List 작성
    output_path = os.path.join(OUTPUT_DIR, filename)
    with open(output_path, 'w') as f:
        for path in paths:
            f.write(path + '\n')

    # 2. Split 수행
    core_name, count = create_split_files(filename, paths)

    print(f"    [SUCCESS] {filename} ({count} files)")
    print(f"       └─ Split into folder: {OUTPUT_DIR}/{core_name}/ (file_{core_name}_0.txt ...)")


def main():
    print("=" * 60)
    print(f"Start scanning in: {SAMPLE_DIR}")
    print(f"Output Directory : {OUTPUT_DIR}")
    print("=" * 60)

    if not os.path.exists(SAMPLE_DIR):
        print(f"[CRITICAL ERROR] '{SAMPLE_DIR}' 경로가 없습니다.")
        return

    if not os.path.exists(OUTPUT_DIR):
        os.makedirs(OUTPUT_DIR)

    for dirname, short_name in sample_mapping.items():
        full_dir_path = os.path.join(SAMPLE_DIR, dirname)

        if not os.path.exists(full_dir_path):
            print(f"[MISSING] Directory not found: {dirname}")
            continue

        print(f"-> [PROCESSING] {dirname} ...")

        # Data: Run2017 period(B/C/D/E/F)별로 분할
        if short_name is None:
            process_prefix = dirname
            try:
                subdirs = [d for d in os.listdir(full_dir_path)
                           if os.path.isdir(os.path.join(full_dir_path, d))]
            except Exception:
                subdirs = []

            data_found = False
            for subdir in subdirs:
                if "Run2017" in subdir:
                    period = subdir[-1]
                    output_filename = f"filelist_{process_prefix}_Run2017{period}.txt"
                    paths = find_root_files(os.path.join(full_dir_path, subdir))
                    if paths:
                        write_and_split(output_filename, paths)
                        data_found = True

            if not data_found:
                print(f"    [WARNING] {dirname} 내부에서 유효한 Data 폴더를 찾지 못했습니다.")

        # MC: 단일 filelist (base+ext 는 os.walk 로 자동 병합)
        else:
            output_filename = f"filelist_{short_name}.txt"
            paths = find_root_files(full_dir_path)
            write_and_split(output_filename, paths)

    print("\n" + "=" * 60)
    print("All tasks finished. Check the output directory.")
    print("=" * 60)


if __name__ == "__main__":
    main()
