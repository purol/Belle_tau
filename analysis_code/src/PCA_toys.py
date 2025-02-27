#!/usr/bin/env python3

import numpy as np
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA

# Function to plot and save correlation matrices
def save_correlation_matrix(corr_matrix, title, filename):
    plt.figure(figsize=(6, 5))
    sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', fmt=".2f", linewidths=0.5, vmin=-1.0, vmax=1.0)
    plt.title(title)
    plt.savefig(filename, dpi=300, bbox_inches='tight')  # Save as PNG
    plt.close()  # Close the figure to free memory

def covariance_to_correlation(cov_matrix):
    std_devs = np.sqrt(np.diag(cov_matrix))  # Get standard deviations
    correlation_matrix = cov_matrix / np.outer(std_devs, std_devs)  # Normalize
    return correlation_matrix    

# Load CSV file
df = pd.read_csv("muonID_toys.csv", header=None)  # No header in your data

# Compute correlation matrix before PCA
corr_before = df.corr()

# Apply PCA without standardization
pca = PCA(n_components=4)  # Keep all components to analyze correlation
X_pca = pca.fit_transform(df)

# Eigenvalues (variance explained by each principal component)
eigenvalues = pca.explained_variance_

# Eigenvectors (principal component directions)
eigenvectors = pca.components_

Lambda = np.diag(eigenvalues)  # Shape: (4,4)

# Reconstruct matrix
cov_after = eigenvectors.T @ Lambda @ eigenvectors
corr_after = covariance_to_correlation(cov_after)

# Save correlation matrices as PNG
save_correlation_matrix(corr_before, "Correlation Matrix Before PCA", "corr_before_pca.png")
save_correlation_matrix(corr_after, "Correlation Matrix After PCA", "corr_after_pca.png")

# Print explained variance ratio
print("Explained Variance Ratio:", pca.explained_variance_ratio_)

print("Saved 'corr_before_pca.png' and 'corr_after_pca.png'")
