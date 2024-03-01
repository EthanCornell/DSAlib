import matplotlib.pyplot as plt
import re
import subprocess
import os

# Ensure the output directory exists
output_dir = "performance_result"
os.makedirs(output_dir, exist_ok=True)

# Compile the C++ program
compile_command = "g++ -pg -std=c++17 -o pT performanceTest.cpp -lpthread -O3"
subprocess.run(compile_command, shell=True, check=True)

# Run the compiled program and capture its output
run_command = "./pT"
result = subprocess.run(run_command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

# Parse the output
data = {}
lines = result.stdout.split('\n')
current_list = ""
for line in lines:
    # Check for the linked list type
    if "Testing :" in line:
        current_list = line.split(":")[1].strip()
        data[current_list] = {"threads": [], "ops/ms": []}
    # Extract the number of threads and Ops/ms
    elif "Threads:" in line:
        parts = re.findall(r'\d+', line)
        if len(parts) >= 3:  # Ensure there are enough parts to avoid IndexError
            threads = int(parts[0])
            ops_per_ms = float(parts[1] + "." + parts[2])
            data[current_list]["threads"].append(threads)
            data[current_list]["ops/ms"].append(ops_per_ms)

# Determine the max number of threads to adjust figure size
max_threads = max(len(metrics["threads"]) for metrics in data.values())
fig_width = max(10, 1.5*max_threads)  # Base width of 10, increase by 1.5 for each thread beyond 8

# Plotting with dynamic size based on data points
plt.figure(figsize=(fig_width, 8))
for list_name, metrics in data.items():
    plt.plot(metrics["threads"], metrics["ops/ms"], marker='o', label=list_name)

plt.title('Performance Comparison of Concurrent Linked Lists')
plt.xlabel('Number of Threads')
plt.ylabel('Operations per ms (Ops/ms)')
# plt.xscale('log', base=2)
plt.xticks(metrics["threads"], labels=metrics["threads"])
plt.legend()
plt.grid(True)

# Save the plot to the specified directory
plot_path = os.path.join(output_dir, 'performance_comparison.png')
plt.savefig(plot_path)
plt.close()  # Close the plot to avoid displaying it in the notebook

print("Plot saved to {}".format(plot_path))
