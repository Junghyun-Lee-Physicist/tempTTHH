import ROOT
import yaml
import correctionlib
import numpy as np

# Style Setup
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetPalette(ROOT.kRainBow)
ROOT.gStyle.SetPaintTextFormat(".3f")

def load_config(cfg_path="config.yaml"):
    with open(cfg_path, 'r') as f:
        return yaml.safe_load(f)

# Plotting Helper (동일함)
def make_pretty_plot(matrix_data, x_edges, y_edges, out_name, title, z_title="Scale Factor"):
    n_bins_x = len(x_edges) - 1
    n_bins_y = len(y_edges) - 1
    h_uniform = ROOT.TH2D(f"h_uni_{out_name}", title, n_bins_x, 0, float(n_bins_x), n_bins_y, 0, float(n_bins_y))

    for i in range(n_bins_x):
        for j in range(n_bins_y):
            h_uniform.SetBinContent(i + 1, j + 1, matrix_data[i][j])

    for i in range(n_bins_x): h_uniform.GetXaxis().SetBinLabel(i + 1, f"{x_edges[i]:.0f}-{x_edges[i+1]:.0f}")
    for j in range(n_bins_y): h_uniform.GetYaxis().SetBinLabel(j + 1, f"{y_edges[j]:.0f}-{y_edges[j+1]:.0f}")

    c = ROOT.TCanvas(f"c_{out_name}", title, 1200, 900)
    c.SetRightMargin(0.15); c.SetLeftMargin(0.12); c.SetBottomMargin(0.12)
    h_uniform.GetZaxis().SetRangeUser(0.5, 1.5)
    h_uniform.Draw("COLZ")

    latex = ROOT.TLatex()
    latex.SetTextSize(0.035); latex.SetTextAlign(22)
    for i in range(n_bins_x):
        for j in range(n_bins_y):
            val = h_uniform.GetBinContent(i + 1, j + 1)
            latex.DrawLatex(h_uniform.GetXaxis().GetBinCenter(i + 1), h_uniform.GetYaxis().GetBinCenter(j + 1), f"{val:.3f}")

    c.SaveAs(f"{out_name}.pdf")
    print(f">>> Created {out_name}.pdf")

def main():
    config = load_config()
    cset = correctionlib.CorrectionSet.from_file("trigger_sf.json.gz")
    sf_tool = cset["triggerSF"]

    ht_bins = config['HT_Bins']
    pt_bins = config['PT_Bins']
    use_nb = config.get('UseNBjets', False)
    use_eta = config.get('UseEta', False)
    
    test_cases = []
    if not use_nb and not use_eta:
        # [FIX] nbJets는 int 타입으로 (0)
        test_cases.append( ("nB_Inc_Eta_Inc", 0, 0.0) ) 
    else:
        # [FIX] Config에서 가져온 nb bin 값을 int로 변환
        test_nb = int(config['NB_Bins'][0]) if use_nb else 0
        test_eta = 0.0
        test_cases.append( ("Test_Config_Case", test_nb, test_eta) )

    for label, t_nb, t_eta in test_cases:
        print(f"\n[Validation] Processing {label} (nb={t_nb}, eta={t_eta})...")

        # 1. Standard Plot
        data_matrix = []
        for i in range(len(ht_bins) - 1):
            row = []
            ht_center = (ht_bins[i] + ht_bins[i+1]) / 2.0
            for j in range(len(pt_bins) - 1):
                pt_center = (pt_bins[j] + pt_bins[j+1]) / 2.0
                
                # [FIX] t_nb를 int로 명시적 전달
                sf = sf_tool.evaluate(int(t_nb), float(t_eta), float(ht_center), float(pt_center))
                row.append(sf)
            data_matrix.append(row)

        make_pretty_plot(data_matrix, ht_bins, pt_bins, f"SF_{label}_fromJSON", f"SF Check ({label})")

        # 2. Extended Range Plot
        ext_ht_bins = [0.0] + ht_bins + [ht_bins[-1] + 1000.0]
        ext_pt_bins = [0.0] + pt_bins + [pt_bins[-1] + 100.0]

        ext_data_matrix = []
        for i in range(len(ext_ht_bins) - 1):
            row = []
            ht_val = (ext_ht_bins[i] + ext_ht_bins[i+1]) / 2.0
            for j in range(len(ext_pt_bins) - 1):
                pt_val = (ext_pt_bins[j] + ext_pt_bins[j+1]) / 2.0
                
                # [FIX] t_nb를 int로 명시적 전달
                sf = sf_tool.evaluate(int(t_nb), float(t_eta), float(ht_val), float(pt_val))
                row.append(sf)
            ext_data_matrix.append(row)

        make_pretty_plot(ext_data_matrix, ext_ht_bins, ext_pt_bins, f"SF_{label}_Extended", f"SF Check Extended ({label})")

if __name__ == "__main__": main()
