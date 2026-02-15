import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read CSVs
dijkstra = pd.read_csv("dijkstra_results.csv")
prim     = pd.read_csv("prim_results.csv")

# Metrics to plot
metrics = [
    ("total_mean", "total_sd", "Total Runtime (s)"),
    ("decrease_mean", "decrease_sd", "DecreaseKey Time (s)"),
    ("extract_mean", "extract_sd", "ExtractMin Time (s)"),
    ("ops_mean", "ops_sd", "Heap Operations")
]

algorithms = [("Dijkstra", dijkstra), ("Prim", prim)]
heaps = ["Fibonacci", "Pairing"]
graph_types = ["Sparse", "Dense"]

line_styles = ['-', '--', ':', '-.', '-', '--', ':', '-.']
markers = ['o', 's', '^', 'v', 'o', 's', '^', 'v']
colors = ['tab:blue', 'tab:orange', 'tab:green', 'tab:red', 
          'tab:purple', 'tab:brown', 'tab:pink', 'tab:gray']

marker_sizes = {"Sparse": 6, "Dense": 10}
line_widths = {"Sparse": 1.5, "Dense": 2.5}
alphas = {"Sparse": 0.6, "Dense": 1.0}

n_values = sorted(dijkstra["n"].unique())
x_positions = np.arange(len(n_values))

for mean_col, sd_col, title in metrics:
    plt.figure(figsize=(10,6))
    line_idx = 0
    end_points = []  # store tuples (y_value, color, label, alpha)
    
    # First pass: plot lines and store final y-values
    for algo_name, df_algo in algorithms:
        for heap in heaps:
            for gtype in graph_types:
                df = df_algo[(df_algo["heap_type"] == heap) & (df_algo["graph_type"] == gtype)]
                if df.empty:
                    continue
                y = df[mean_col].values
                label = f"{algo_name} / {heap} / {gtype}"
                
                plt.plot(
                    x_positions, y,
                    color=colors[line_idx],
                    marker=markers[line_idx],
                    linestyle=line_styles[line_idx],
                    markersize=marker_sizes[gtype],
                    linewidth=line_widths[gtype],
                    alpha=alphas[gtype]
                )
                
                end_points.append({
                    "y": y[-1],
                    "color": colors[line_idx],
                    "label": label,
                    "alpha": alphas[gtype]
                })
                
                line_idx += 1

    # Adjust label positions to prevent overlap
    # Sort by y (log scale)
    end_points = sorted(end_points, key=lambda d: np.log10(d["y"]))
    min_sep = 0.13  # minimal log10 separation between labels
    adjusted_positions = []
    
    for i, pt in enumerate(end_points):
        y_log = np.log10(pt["y"])
        if i == 0:
            new_log = y_log
        else:
            prev_log = np.log10(adjusted_positions[-1])
            if y_log - prev_log < min_sep:
                new_log = prev_log + min_sep
            else:
                new_log = y_log
        adjusted_positions.append(10**new_log)
    
    # Draw labels with adjusted Y
    for i, pt in enumerate(end_points):
        plt.text(
            x_positions[-1] + 0.1,  # small horizontal offset
            adjusted_positions[i],
            pt["label"],
            color=pt["color"],
            fontsize=10,
            va='center',
            alpha=pt["alpha"]
        )
    
    plt.xticks(x_positions, n_values)
    plt.yscale('log')
    
    plt.xlabel("Graph size n")
    plt.ylabel(title)
    plt.title(title + " vs Graph Size")
    plt.grid(True, which="both", ls="--", alpha=0.5)
    plt.tight_layout()
    
    filename = title.replace(" ", "_").replace("(", "").replace(")", "").replace("/", "_") + ".png"
    plt.savefig(filename, bbox_inches='tight')
    plt.close()

print("All plots saved as PNG files.")
