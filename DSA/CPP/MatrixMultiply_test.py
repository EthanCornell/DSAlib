import subprocess
import re
import matplotlib.pyplot as plt
import os
import sys

# Function to compile and run C++ program, then parse output
def compile_and_run(executable, cpp_source):
    # Compile the C++ code
    compile_command = f'g++ -fopenmp -o {executable} {cpp_source} -O3'
    subprocess.run(compile_command, shell=True, check=True)

    # Run the compiled executable and capture its output
    run_command = f'./{executable}' if not is_windows else executable
    result = subprocess.run(run_command, shell=True, check=True, stdout=subprocess.PIPE, text=True)
    return result.stdout

# Check the platform
is_windows = sys.platform.startswith('win')

# Path to the C++ source code
cpp_source = 'MatrixMultiply.cpp'
# Output executable name
executable = 'MatrixMultiply.out' if not is_windows else 'MatrixMultiply.exe'

output = compile_and_run(executable, cpp_source)

# Regular expressions for extracting data
size_pattern = r"Matrix size: (\d+)x\d+"
method_patterns = {
    "Basic": r"Basic: (\d+) ms",
    "Cache Friendly": r"Cache Friendly: (\d+) ms",
    "Strassen": r"Strassen: (\d+) ms",
    "Parallel": r"Parallel: (\d+) ms",
    "Block-wise": r"Block-wise: (\d+) ms",
    "Fork-join model in shared-memory": r"Fork-join model in shared-memory: (\d+) ms",
}

# Initialize data structure to hold execution times
execution_times = {method: [] for method in method_patterns}
sizes = []

# Parse output
current_size = None
for line in output.splitlines():
    size_match = re.search(size_pattern, line)
    if size_match:
        current_size = int(size_match.group(1))
        sizes.append(current_size)
    else:
        for method, pattern in method_patterns.items():
            time_match = re.search(pattern, line)
            if time_match:
                execution_times[method].append(int(time_match.group(1)))

# Plotting
plt.figure(figsize=(12, 8))
for method, times in execution_times.items():
    plt.plot(sizes, times, label=method, marker='o')

plt.xlabel('Matrix Size (N x N)')
plt.ylabel('Execution Time (ms)')
plt.title('Matrix Multiplication Methods Performance')
plt.legend()
plt.grid(True)
plt.xticks(sizes)
plt.tight_layout()
plt.show()
