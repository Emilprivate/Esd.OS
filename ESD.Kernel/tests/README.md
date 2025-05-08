# ESD.OS Tests

MONKE TEST! Test first, crash later. This directory contains tests for all ESD.OS components.

## Structure

Tests are organized to mirror the development structure:

```
tests/
├── ESD.Kernel/                    # Main kernel component
│   └── ESD.Kernel-0.0.1/          # Tests grouped by kernel version  
│       ├── 01-simple-boot/        # Tests for each prototype
│       │   └── unit_tests.c
│       ├── 02-improved-console/
│       │   └── unit_tests.c
│       └── 03-keyboard-input/
│           └── unit_tests.c
├── test-tools/                    # Shared testing utilities
│   └── test-framework.h           # Basic test macros and functions
├── test-reports/                  # JSON test reports storage
│   └── report.html                # HTML visualization of test results
└── manager.sh                     # All-in-one test utility
```

## Test Types

Each prototype directory contains these test types:

1. **Unit Tests**: Test individual functions in isolation
2. **Integration Tests**: Test interactions between components
3. **System Tests**: Test the entire prototype as a whole

## Using the Test Manager

### Overview

The `manager.sh` script provides all testing functionality in a single command:

```bash
./manager.sh [COMMAND] [OPTIONS]
```

Make sure to make it executable first:
```bash
chmod +x manager.sh
```

### Commands

1. **Compile Tests**:
   ```bash
   ./manager.sh compile
   ```
   This compiles all test files into executable binaries.

2. **Run Tests**:
   ```bash
   ./manager.sh run all           # Run all tests
   ./manager.sh run menu          # Interactive menu (default)
   ./manager.sh run path/to/test  # Run specific test
   ```

3. **View Test Results**:
   ```bash
   ./manager.sh report latest     # Basic report of latest run
   ./manager.sh visualize         # Fancy terminal visualization
   ./manager.sh html-report       # Generate HTML report
   ```

4. **Show Help**:
   ```bash
   ./manager.sh help
   ```

### Visualizations

The test manager provides three visualization options:

1. **Report View** - Basic text display of test results:
   ```bash
   ./manager.sh report latest
   ```

2. **Terminal Visualization** - Colorful ASCII art with progress bars:
   ```bash
   ./manager.sh visualize
   ```
   
3. **HTML Report** - Interactive web view with expandable test details:
   ```bash
   ./manager.sh html-report
   ```
   Open the generated HTML file from the test-reports directory.

### Typical Workflow

For the full MONKE testing experience:

1. Compile the tests:
   ```bash
   ./manager.sh compile
   ```

2. Run all tests:
   ```bash
   ./manager.sh run all
   ```

3. See fancy MONKE visualization:
   ```bash
   ./manager.sh visualize
   ```

4. Generate HTML report if needed:
   ```bash
   ./manager.sh html-report
   ```

## Troubleshooting

### Compilation Issues

If you encounter compilation errors related to 32-bit support, install the required libraries:

```bash
# Ubuntu/Debian
sudo apt-get install gcc-multilib libc6-dev-i386

# Fedora/RHEL
sudo dnf install glibc-devel.i686 libstdc++-devel.i686
```

### No Test Output

If tests run but don't produce output:
1. Make sure they compiled successfully with `./manager.sh compile`
2. Check if test executables exist in each test directory
3. Run a test directly to see if it produces output:
   ```bash
   ./ESD.Kernel/ESD.Kernel-0.0.1/01-simple-boot/unit_tests
   ```

### MONKE PHILOSOPHY

Remember: In ESD.OS, only handler is BIG RED BUTTON (RESET)! 🦍
Tests may fail - that's the MONKE way. OOGA BOOGA!
