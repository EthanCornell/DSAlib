#  Copyright (c) Cornell University.
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  any later version.

#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.

#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#  Author: I-Hsuan (Ethan) Huang
#  Email: ih246@cornell.edu

import subprocess
import re
import matplotlib.pyplot as plt
import os
import sys

def compile_and_run(executable, cpp_source):
    # Check the platform
    is_windows = sys.platform.startswith('win')
    
    # Adjust the compile and run commands for compatibility
    compile_command = 'g++ -fopenmp -o {} {} -O3'.format(executable, cpp_source)
    subprocess.run(compile_command, shell=True, check=True)

    run_command = executable if is_windows else './{}'.format(executable)
    result = subprocess.run(run_command, shell=True, check=True, stdout=subprocess.PIPE, text=True)
    return result.stdout

# Path to the C++ source code and the executable
cpp_source = 'MatrixMultiply.cpp'
executable = 'MatrixMultiply.exe' if sys.platform.startswith('win') else 'MatrixMultiply.out'

output = compile_and_run(executable, cpp_source)

# Regular expressions for extracting data
patterns = {
    "Basic": r"Basic: (\d+) ms",
    "Cache Friendly": r"Cache Friendly: (\d+) ms",
    "Strassen": r"Strassen: (\d+) ms",
    "Parallel": r"Parallel: (\d+) ms",
    "Block-wise": r"Block-wise: (\d+) ms",
    "Fork-join model in shared-memory": r"Fork-join model in shared-memory: (\d+) ms",
}

# Initialize data structure to hold execution times
execution_times = {method: [] for method in patterns}
sizes = []

# Parse output to extract matrix sizes and execution times
for line in output.splitlines():
    size_match = re.search(r"Matrix size: (\d+)x\d+", line)
    if size_match:
        current_size = int(size_match.group(1))
        if current_size not in sizes:
            sizes.append(current_size)
    
    for method, pattern in patterns.items():
        time_match = re.search(pattern, line)
        if time_match:
            execution_times[method].append(int(time_match.group(1)))

# Create a directory for the output graph if it doesn't exist
output_dir = "output_graphs"
if not os.path.exists(output_dir):
    os.makedirs(output_dir)

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

# Add the operating system information at the bottom left corner
os_info = "OS:Windows" if sys.platform.startswith('win') else "OS:Linux"
plt.text(0.01, 0.01, os_info, transform=plt.gca().transAxes, fontsize=9, verticalalignment='bottom')


# Save the plot to a file
plt.savefig(os.path.join(output_dir, "matrix_multiplication_performance.png"))

# Optionally display the plot
# plt.show()
