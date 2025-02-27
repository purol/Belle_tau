#!/usr/bin/env python3

import numpy as np
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA

import argparse

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

parser = argparse.ArgumentParser()
parser.add_argument('--input_file', required=True, type=str, help='input path + input file name')
parser.add_argument('--output_file', required=True, type=str, help='output path + output file name')
args = parser.parse_args()

# Load CSV file
df = pd.read_csv(args.input_file, header=None)  # No header in your data
num_samples, num_features = df.shape

# Compute correlation matrix before PCA
corr_before = df.corr()
cov_before = df.cov()

# Apply PCA without standardization
pca = PCA()
X_pca = pca.fit_transform(df)

# select the num of dim
cumsum = np.cumsum(pca.explained_variance_ratio_)
dim = np.argmax(cumsum >= 0.95) + 1
print(dim, "components are selected")

# Eigenvalues (variance explained by each principal component)
eigenvalues = pca.explained_variance_

# Eigenvectors (principal component directions)
eigenvectors = pca.components_

# Select only the first `dim` components
eigenvalues_selected = eigenvalues[:dim]
eigenvectors_selected = eigenvectors[:dim, :]

# Construct diagonal matrix of selected eigenvalues
Lambda_selected = np.diag(eigenvalues_selected)

# Reconstruct matrix
cov_after = eigenvectors_selected.T @ Lambda_selected @ eigenvectors_selected
corr_after = covariance_to_correlation(cov_after)

# Save correlation matrices as PNG
save_correlation_matrix(corr_before, "Correlation Matrix Before PCA", args.output_file + "_corr_before_pca.png")
save_correlation_matrix(corr_after, "Correlation Matrix After PCA", args.output_file + "_corr_after_pca.png")

# Print explained variance ratio
print("Explained Variance Ratio:", pca.explained_variance_ratio_)

print("Saved 'corr_before_pca.png' and 'corr_after_pca.png'")

print(eigenvalues_selected)
print(eigenvectors_selected)

# get remaining component
cov_diff = cov_before - cov_after
print("cov diff:", cov_diff)

# save in file
with open(args.output_file, "w") as file:
    file.write("%d,%d\n" % (num_features, dim))
    for i in range(dim):
        file.write("%f\n" % np.sqrt(eigenvalues_selected[i]))
        for j in range(num_features):
            file.write("%f\n" % eigenvectors_selected[i][j])

with open(args.output_file + "_remain", "w") as file:
    file.write("%d\n" % (num_features))
    for i in range(num_features):
        if (cov_diff[i][i] > 0):
            file.write("%f\n" % np.sqrt(cov_diff[i][i]))
        else:
            file.write("0.0\n")
        
