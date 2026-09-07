import matplotlib.pyplot as plt

# Data
labels = ["uubar", "ddbar", "ssbar", "ccbar", "mumu", "eetautau", "mumutautau"]
sizes = [7.216, 2.466, 2.648, 2.100, 2.192, 0.365, 2.557]

# Custom function to show absolute values
def absolute_value(val):
    total = sum(sizes)
    absolute = val * total / 100
    return f'{absolute:.3f}'

# Create the pie chart
plt.pie(
    sizes,
    labels=labels,
    autopct=absolute_value,
    startangle=90
)

plt.axis('equal')  # Keep pie chart circular
plt.title("background composition", fontsize=14)
plt.show()
