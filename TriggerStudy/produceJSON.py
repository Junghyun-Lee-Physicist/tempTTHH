import yaml
import gzip
import uproot
import sys
import correctionlib.schemav2 as cs

def fmt_eta(val):
    s = f"{val:.1f}"
    s = s.replace(".", "p").replace("-", "m")
    return s

def main():
    # 1. Load Config
    with open("config.yaml") as f:
        config = yaml.safe_load(f)
    
    use_nb = config['UseNBjets']
    use_eta = config['UseEta']
    nb_bins_cfg = config['NB_Bins']
    eta_bins_cfg = config['Eta_Bins']

    print(f">>> [JSON] Config Loaded.")

    # 2. Open ROOT File
    f_root = uproot.open("ScaleFactors.root")
    sf_dir = f_root["ScaleFactors"]

    # --------------------------------------------------------------------------
    # Helper: Leaf Node Creator (No Patching)
    # --------------------------------------------------------------------------
    def get_sf_node(nb_label, eta_label):
        key = f"SF_{nb_label}_{eta_label}"
        if key not in sf_dir:
            print(f"[Error] Key '{key}' not found in ScaleFactors.root!")
            sys.exit(1)
            
        hist = sf_dir[key].to_hist()
        
        # [수정됨] 보간(Patching) 없이 원본 값 그대로 사용 (0.0은 0.0으로)
        vals = hist.values().flatten().tolist()
        edges_ht = hist.axes[0].edges.tolist()
        edges_pt = hist.axes[1].edges.tolist()

        return cs.MultiBinning(
            nodetype="multibinning",
            inputs=["ht", "pt"],
            edges=[edges_ht, edges_pt],
            content=vals, 
            flow="clamp" # 범위 밖(Edge)만 근처 값 사용
        )

    # --------------------------------------------------------------------------
    # Helper: Eta Node Creator
    # --------------------------------------------------------------------------
    def make_eta_node(nb_label):
        if not use_eta:
            return get_sf_node(nb_label, "Eta_Inc")
        
        eta_edges = eta_bins_cfg
        eta_nodes = []

        # Underflow Copy
        eta_nodes.append(get_sf_node(nb_label, f"Eta_{fmt_eta(eta_edges[0])}_to_{fmt_eta(eta_edges[1])}"))
        
        # Normal Bins
        for i in range(len(eta_edges) - 1):
            eta_nodes.append(get_sf_node(nb_label, f"Eta_{fmt_eta(eta_edges[i])}_to_{fmt_eta(eta_edges[i+1])}"))
            
        # Overflow Copy
        eta_nodes.append(get_sf_node(nb_label, f"Eta_{fmt_eta(eta_edges[-2])}_to_{fmt_eta(eta_edges[-1])}"))

        return cs.Binning(
            nodetype="binning", 
            input="eta", 
            edges=eta_edges,
            content=eta_nodes,
            flow="clamp"
        )

    # --------------------------------------------------------------------------
    # Root Node (nbJets as int)
    # --------------------------------------------------------------------------
    if use_nb:
        nb_edges = [float(x) for x in nb_bins_cfg]
        nb_content = [1.0] # Underflow (nbJets < 3)

        for val in nb_bins_cfg[:-1]:
            nb_content.append(make_eta_node(f"nB{val}"))
            
        last_val = nb_bins_cfg[-1]
        nb_content.append(make_eta_node(f"nB{last_val}p"))

        root = cs.Binning(
            nodetype="binning", 
            input="nbJets", 
            edges=nb_edges,
            content=nb_content,
            flow="clamp"
        )
    else:
        root = make_eta_node("nB_Inc")

    # --------------------------------------------------------------------------
    # Create Correction
    # --------------------------------------------------------------------------
    corr = cs.Correction(
        name="triggerSF", 
        version=1,
        inputs=[
            cs.Variable(name="nbJets", type="int"), # int 타입 확인
            cs.Variable(name="eta", type="real"),
            cs.Variable(name="ht", type="real"),
            cs.Variable(name="pt", type="real")
        ],
        output=cs.Variable(name="weight", type="real"),
        data=root
    )

    cset = cs.CorrectionSet(schema_version=2, corrections=[corr])
    
    with gzip.open("trigger_sf.json.gz", "wt") as fout:
        fout.write(cset.model_dump_json(exclude_unset=True))
        
    print(">>> Created trigger_sf.json.gz (No Patching, Raw Values)")

if __name__ == "__main__": main()
