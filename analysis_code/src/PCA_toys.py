#!/usr/bin/env python3

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA

# Load CSV file
df = pd.read_csv("your_file.csv", header=None)  # No header in your data

# Compute correlation matrix before PCA
corr_before = df.corr()

# Apply PCA without standardization
pca = PCA(n_components=4)  # Keep all components to analyze correlation
X_pca = pca.fit_transform(df)

# Compute correlation matrix after PCA
corr_after = pd.DataFrame(X_pca).corr()

# Function to plot and save correlation matrices
def save_correlation_matrix(corr_matrix, title, filename):
    plt.figure(figsize=(6, 5))
    sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', fmt=".2f", linewidths=0.5)
    plt.title(title)
    plt.savefig(filename, dpi=300, bbox_inches='tight')  # Save as PNG
    plt.close()  # Close the figure to free memory

# Save correlation matrices as PNG
save_correlation_matrix(corr_before, "Correlation Matrix Before PCA", "corr_before_pca.png")
save_correlation_matrix(corr_after, "Correlation Matrix After PCA", "corr_after_pca.png")

# Print explained variance ratio
print("Explained Variance Ratio:", pca.explained_variance_ratio_)

print("Saved 'corr_before_pca.png' and 'corr_after_pca.png'")
