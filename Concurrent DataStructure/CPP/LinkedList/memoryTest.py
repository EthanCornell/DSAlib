import matplotlib.pyplot as plt
import re
import subprocess
import os

# Ensure the output directory exists
output_dir = "memory_test_results"
os.makedirs(output_dir, exist_ok=True)

# Compile and run your C++ program
compile_command = "g++ -pg -std=c++17 -o memoryTest memoryTest.cpp -lpthread -O3"
subprocess.run(compile_command, shell=True, check=True)

run_command = "./memoryTest"
result = subprocess.run(run_command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

# Parse the output for memory usage
data = {}
lines = result.stdout.split('\n')
for line in lines:
    if "Memory Testing:" in line:
        current_list = line.split(":")[1].strip()
        data[current_list] = {"threads": [], "delta_memory": []}
    elif "Threads:" in line:
        parts = re.findall(r'\d+', line)
        if len(parts) >= 2:  # Ensure there are enough parts
            threads = int(parts[0])
            delta_memory = float(line.split(",")[-1].split(":")[1].strip().split(" ")[0])  # Extract delta memory in MB
            data[current_list]["threads"].append(threads)
            data[current_list]["delta_memory"].append(delta_memory)

# Plotting the memory usage
plt.figure(figsize=(12, 8))
for list_name, metrics in data.items():
    plt.plot(metrics["threads"], metrics["delta_memory"], marker='o', label=list_name)

plt.title('Memory Usage Comparison of Concurrent Linked Lists')
plt.xlabel('Number of Threads')
plt.ylabel('Delta Memory Usage (MB)')
plt.xscale('log', base=2)
plt.xticks(metrics["threads"], labels=metrics["threads"])
plt.legend()
plt.grid(True)

# Save the plot
plot_path = os.path.join(output_dir, 'memory_usage_comparison.png')
plt.savefig(plot_path)
plt.close()  # Close the plot

print("Plot saved to {}".format(plot_path))
