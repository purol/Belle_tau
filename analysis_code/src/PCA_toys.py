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

# Function to plot correlation matrices
def plot_correlation_matrix(corr_matrix, title):
    plt.figure(figsize=(6,5))
    sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', fmt=".2f", linewidths=0.5)
    plt.title(title)
    plt.show()

# Plot correlation matrices
plot_correlation_matrix(corr_before, "Correlation Matrix Before PCA")
plot_correlation_matrix(corr_after, "Correlation Matrix After PCA")

# Print explained variance ratio
print("Explained Variance Ratio:", pca.explained_variance_ratio_)
