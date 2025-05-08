#!/bin/bash

set -e

BASE_DIR=$(dirname "$0")
PARENT_DIR=$(dirname "$BASE_DIR")
TOOLS_DIR="$BASE_DIR/test-tools"
REPORT_DIR="$BASE_DIR/test-reports"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
REPORT_FILE="$REPORT_DIR/test_report_$TIMESTAMP.json"

mkdir -p "$REPORT_DIR"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;36m'
WHITE='\033[1;37m'
BOLD='\033[1m'
RESET='\033[0m'

show_help() {
    echo "ESD.OS TEST UTILITY"
    echo "==================="
    echo "Usage: ./manager.sh [COMMAND] [OPTIONS]"
    echo ""
    echo "Commands:"
    echo "  compile               Compile all test files"
    echo "  run [options]         Run tests"
    echo "    all                 Run all tests"
    echo "    menu                Show interactive menu (default)"
    echo "    <path>              Run tests in specific path"
    echo "  report [options]      Show test reports"
    echo "    latest              Show latest test report"
    echo "    <filename>          Show specific report file"
    echo "  visualize [options]   Show fancy test results visualization"
    echo "    latest              Visualize latest test report (default)"
    echo "    <filename>          Visualize specific report file"
    echo "  html-report           Generate HTML visualization"
    echo "  help                  Show this help message"
    echo ""
    echo "Examples:"
    echo "  ./manager.sh compile                    Compile all tests"
    echo "  ./manager.sh run all                    Run all tests"
    echo "  ./manager.sh visualize                  Show fancy visualization of latest results"
    echo "  ./manager.sh html-report                Generate HTML report"
    echo ""
}

CFLAGS="-m32 -g -Wall -Wextra -I$TOOLS_DIR"

compile_tests() {
    echo "Compiling test files..."
    
    find "$BASE_DIR" -name "unit_tests.c" | while read -r test_file; do
        test_dir=$(dirname "$test_file")
        out_file="${test_dir}/unit_tests"
        
        echo "Compiling: $test_file -> $out_file"
        
        if ! grep -q "#include <stdio.h>" "$test_file"; then
            sed -i '1i\#include <stdio.h>' "$test_file"
        fi
        
        gcc $CFLAGS -o "$out_file" "$test_file"
        
        if [ $? -ne 0 ]; then
            echo "Error: Compilation failed for $test_file"
        else
            echo "Successfully compiled $test_file"
            chmod +x "$out_file"
        fi
    done
    
    echo "All test files compiled."
}

run_test() {
    local test_exec="$1"
    local test_name="${test_exec#$BASE_DIR/}"
    
    echo "======================================================"
    echo "RUNNING TEST: $test_name"
    echo "======================================================"
    
    "$test_exec"
    local exit_code=$?
    
    echo "------------------------------------------------------"
    echo "TEST COMPLETED WITH EXIT CODE: $exit_code"
    echo "======================================================"
    echo ""
    
    return $exit_code
}

run_all_tests() {
    local test_files=("$@")
    local passed=0
    local failed=0
    local total=${#test_files[@]}
    
    echo "{" > "$REPORT_FILE"
    echo "  \"timestamp\": \"$(date)\","  >> "$REPORT_FILE"
    echo "  \"tests\": [" >> "$REPORT_FILE"
    
    echo "Running $total tests..."
    for test_exec in "${test_files[@]}"; do
        if [ -x "$test_exec" ]; then
            output=$(run_test "$test_exec")
            exit_code=$?
            
            if [ $exit_code -eq 0 ]; then
                passed=$((passed+1))
                status="passed"
            else
                failed=$((failed+1))
                status="failed"
            fi
            
            echo "  {" >> "$REPORT_FILE"
            echo "    \"name\": \"${test_exec#$BASE_DIR/}\"," >> "$REPORT_FILE"
            echo "    \"status\": \"$status\"," >> "$REPORT_FILE"
            sanitized_output=$(echo "$output" | sed -e 's/"/\\"/g' -e ':a;N;$!ba;s/\n/\\n/g')
            echo "    \"output\": \"$sanitized_output\"" >> "$REPORT_FILE"
            echo "  }," >> "$REPORT_FILE"
        fi
    done
    
    sed -i '$ s/,$//' "$REPORT_FILE"
    echo "  ]," >> "$REPORT_FILE"
    echo "  \"summary\": {" >> "$REPORT_FILE"
    echo "    \"total\": $total," >> "$REPORT_FILE"
    echo "    \"passed\": $passed," >> "$REPORT_FILE"
    echo "    \"failed\": $failed" >> "$REPORT_FILE"
    echo "  }" >> "$REPORT_FILE"
    echo "}" >> "$REPORT_FILE"
    
    mkdir -p "$REPORT_DIR"
    
    if [ -f "$REPORT_FILE" ]; then
        rm -f "$REPORT_DIR/latest_report.json"
        ln -sf "$(realpath "$REPORT_FILE")" "$REPORT_DIR/latest_report.json"
        echo "Report saved to: $REPORT_FILE"
        echo "Latest report link: $REPORT_DIR/latest_report.json -> $(readlink -f "$REPORT_DIR/latest_report.json")"
    else
        echo "Error: Failed to create report file!"
    fi
    
    echo ""
    echo "TEST SUMMARY"
    echo "==========="
    echo "Total: $total"
    echo "Passed: $passed"
    echo "Failed: $failed"
    echo ""
}

find_test_executables() {
    find "$BASE_DIR" -name "unit_tests" -type f -executable
}

visualize_report() {
    local report_file=""
    
    if [ "$1" = "latest" ] || [ -z "$1" ]; then
        report_file="$REPORT_DIR/latest_report.json"
    else
        report_file="$1"
        if [[ ! "$report_file" == /* ]]; then
            report_file="$REPORT_DIR/$report_file"
        fi
    fi
    
    if [ ! -f "$report_file" ]; then
        echo "Error: Report file not found: $report_file"
        echo "Available reports:"
        find "$REPORT_DIR" -name "test_report_*.json" -type f | sort
        exit 1
    fi
    
    local timestamp=$(grep -o '"timestamp": "[^"]*"' "$report_file" | cut -d '"' -f 4)
    local total=$(grep -o '"total": [0-9]*' "$report_file" | grep -o '[0-9]*')
    local passed=$(grep -o '"passed": [0-9]*' "$report_file" | grep -o '[0-9]*')
    local failed=$(grep -o '"failed": [0-9]*' "$report_file" | grep -o '[0-9]*')
    local pass_percent=$((passed * 100 / total))
    local fail_percent=$((failed * 100 / total))

    cat << "EOF"
                            /~\
                            C oo
                            _( ^)
                            /   ~\
                OOGA BOOGA! MONKE TEST RESULTS!
EOF

    echo -e "\n${WHITE}Report: ${RESET}$(basename "$report_file") - ${BLUE}$timestamp${RESET}\n"

    echo -e "${WHITE}╔════════════════════════════════════════════════════════════════╗${RESET}"
    echo -e "${WHITE}║                      TEST RESULTS SUMMARY                      ║${RESET}"
    echo -e "${WHITE}╠════════════════════════════════════════════════════════════════╣${RESET}"
    echo -e "${WHITE}║${RESET} Total Tests: $total"
    printf "${WHITE}║${RESET} %-13s %-5s ${GREEN}%-22s${RESET} %5s %3d%% ${RESET}${WHITE}║${RESET}\n" "Passed:" "$passed" "$(printf '%*s' $((pass_percent/2)) | tr ' ' '█')" "" "$pass_percent"
    printf "${WHITE}║${RESET} %-13s %-5s ${RED}%-22s${RESET} %5s %3d%% ${RESET}${WHITE}║${RESET}\n" "Failed:" "$failed" "$(printf '%*s' $((fail_percent/2)) | tr ' ' '█')" "" "$fail_percent"
    echo -e "${WHITE}╚════════════════════════════════════════════════════════════════╝${RESET}"

    echo -e "\n${WHITE}╔════════════════════════════════════════════════════════════════╗${RESET}"
    echo -e "${WHITE}║                       INDIVIDUAL RESULTS                       ║${RESET}"
    echo -e "${WHITE}╠════════════════════════════════════════════════════════════════╣${RESET}"

    grep -E '"name"|"status"' "$report_file" | tr -d ' ' | tr '\n' ' ' | sed 's/"name":"/"name":"/g' | sed 's/"status":"/"status":"/g' |
    grep -o '"name":"[^"]*","status":"[^"]*"' | while read -r line; do
        NAME=$(echo "$line" | grep -o '"name":"[^"]*"' | cut -d':' -f2- | tr -d '"')
        STATUS=$(echo "$line" | grep -o '"status":"[^"]*"' | cut -d':' -f2- | tr -d '"')
        
        if [ "$STATUS" == "passed" ]; then
            echo -e "${WHITE}║${RESET} ${GREEN}✓${RESET} ${BLUE}$(printf "%-60s" "$NAME")${RESET} ${WHITE}║${RESET}"
        else
            echo -e "${WHITE}║${RESET} ${RED}✗${RESET} ${BLUE}$(printf "%-60s" "$NAME")${RESET} ${WHITE}║${RESET}"
        fi
    done

    echo -e "${WHITE}╚════════════════════════════════════════════════════════════════╝${RESET}"

    if [ "$failed" -eq 0 ]; then
        echo -e "\n${GREEN}MONKE HAPPY! ALL TESTS PASS! OOGA BOOGA!${RESET} 🦍 🍌"
    else
        echo -e "\n${RED}MONKE ANGRY! TESTS FAIL! SYSTEM GO BONK!${RESET} 🦍 💥"
    fi

    echo -e "\n${YELLOW}Generate HTML report? (y/n)${RESET}"
    read -n 1 -r REPLY
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        generate_html_report
    fi
}

show_report() {
    local report_file=""
    
    if [ "$1" = "latest" ] || [ -z "$1" ]; then
        report_file="$REPORT_DIR/latest_report.json"
    else
        report_file="$1"
        if [[ ! "$report_file" == /* ]]; then
            report_file="$REPORT_DIR/$report_file"
        fi
    fi
    
    if [ ! -f "$report_file" ]; then
        echo "Error: Report file not found: $report_file"
        echo "Available reports:"
        find "$REPORT_DIR" -name "test_report_*.json" -type f | sort
        exit 1
    fi
    
    local timestamp=$(grep -o '"timestamp": "[^"]*"' "$report_file" | cut -d '"' -f 4)
    local total=$(grep -o '"total": [0-9]*' "$report_file" | grep -o '[0-9]*')
    local passed=$(grep -o '"passed": [0-9]*' "$report_file" | grep -o '[0-9]*')
    local failed=$(grep -o '"failed": [0-9]*' "$report_file" | grep -o '[0-9]*')
    
    local RED='\033[0;31m'
    local GREEN='\033[0;32m'
    local YELLOW='\033[0;33m'
    local BLUE='\033[0;34m'
    local PURPLE='\033[0;35m'
    local CYAN='\033[0;36m'
    local BOLD='\033[1m'
    local RESET='\033[0m'
    
    echo -e "${BOLD}${PURPLE}======================================================${RESET}"
    echo -e "${BOLD}${PURPLE}=                ESD.OS TEST REPORT                 =${RESET}"
    echo -e "${BOLD}${PURPLE}======================================================${RESET}"
    echo
    echo -e "${BOLD}${BLUE}Report:${RESET} $(basename "$report_file")"
    echo -e "${BOLD}${BLUE}Time:${RESET}   $timestamp"
    echo
    
    local pass_percent=$((passed * 100 / total))
    local fail_percent=$((failed * 100 / total))
    
    echo -e "${BOLD}SUMMARY:${RESET}"
    echo -e "  ${BOLD}Total:${RESET}  $total tests"
    echo -e "  ${BOLD}${GREEN}Passed:${RESET} $passed tests ($pass_percent%)"
    echo -e "  ${BOLD}${RED}Failed:${RESET} $failed tests ($fail_percent%)"
    
    local bar_width=50
    local pass_width=$((pass_percent * bar_width / 100))
    local fail_width=$((fail_percent * bar_width / 100))
    
    echo -n "  ["
    for ((i=0; i<pass_width; i++)); do
        echo -ne "${GREEN}#${RESET}"
    done
    for ((i=0; i<fail_width; i++)); do
        echo -ne "${RED}#${RESET}"
    done
    for ((i=pass_width+fail_width; i<bar_width; i++)); do
        echo -ne " "
    done
    echo "]"
    echo
    
    echo -e "${BOLD}DETAILED RESULTS:${RESET}"
    echo
    
    local test_names=$(grep -o '"name": "[^"]*"' "$report_file" | cut -d '"' -f 4)
    local test_statuses=$(grep -o '"status": "[^"]*"' "$report_file" | cut -d '"' -f 4)
    
    paste <(echo "$test_names") <(echo "$test_statuses") | while read -r name status; do
        if [ "$status" == "passed" ]; then
            echo -e "  ${GREEN}✓${RESET} ${CYAN}$name${RESET}: ${GREEN}PASSED${RESET}"
        else
            echo -e "  ${RED}✗${RESET} ${CYAN}$name${RESET}: ${RED}FAILED${RESET}"
            
            local test_output=$(grep -A1 "\"name\": \"$name\"" "$report_file" | grep -o '"output": "[^"]*"' | cut -d '"' -f 4)
            echo "$test_output" | grep -i fail | sed 's/\\n/\n/g' | while read -r line; do
                echo -e "     ${RED}→ $line${RESET}"
            done
        fi
    done
    
    echo
    echo -e "${BOLD}${PURPLE}======================================================${RESET}"
    
    echo
    echo -e "${YELLOW}Want a graphical report?${RESET}"
    echo -e "Run: ${BOLD}./manager.sh html-report${RESET} to generate an HTML visualization"
}

generate_html_report() {
    local report_file="$REPORT_DIR/latest_report.json"
    local html_file="$REPORT_DIR/report.html"
    
    if [ ! -f "$report_file" ]; then
        echo "Error: No test report found. Run tests first."
        exit 1
    fi
    
    echo "Generating HTML report from $report_file..."
    
    local timestamp=$(grep -o '"timestamp": "[^"]*"' "$report_file" | cut -d '"' -f 4)
    local total=$(grep -o '"total": [0-9]*' "$report_file" | grep -o '[0-9]*')
    local passed=$(grep -o '"passed": [0-9]*' "$report_file" | grep -o '[0-9]*')
    local failed=$(grep -o '"failed": [0-9]*' "$report_file" | grep -o '[0-9]*')
    
    cat > "$html_file" << EOF
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESD.OS Test Report</title>
    <style>
        body {
            font-family: 'Courier New', monospace;
            max-width: 1000px;
            margin: 0 auto;
            padding: 20px;
            background-color: #1e1e1e;
            color: #e0e0e0;
        }
        header {
            text-align: center;
            margin-bottom: 30px;
            border-bottom: 1px solid #444;
            padding-bottom: 10px;
        }
        .summary {
            display: flex;
            justify-content: space-around;
            margin: 30px 0.
        }
        .stat-box {
            text-align: center;
            padding: 15px;
            border-radius: 5px;
            min-width: 150px.
        }
        .passed { background-color: #143d14; }
        .failed { background-color: #4d1919; }
        .total { background-color: #1a3b5c; }
        
        .progress-bar {
            height: 30px;
            background-color: #333;
            border-radius: 15px;
            margin: 20px 0;
            overflow: hidden.
        }
        .progress-passed {
            height: 100%;
            background-color: #2d882d;
            float: left.
        }
        .progress-failed {
            height: 100%;
            background-color: #aa3939;
            float: left.
        }
        
        table {
            width: 100%;
            border-collapse: collapse;
            margin: 30px 0.
        }
        th, td {
            padding: 12px;
            text-align: left;
            border-bottom: 1px solid #444.
        }
        th {
            background-color: #333.
        }
        tr.pass { background-color: rgba(45, 136, 45, 0.2); }
        tr.fail { background-color: rgba(170, 57, 57, 0.2); }
        
        .test-output {
            margin: 10px 0;
            padding: 10px;
            background-color: #252525;
            border-radius: 5px;
            white-space: pre-wrap;
            font-size: 12px;
            overflow: auto;
            max-height: 200px.
        }
        
        h2 {
            margin-top: 30px;
            border-bottom: 1px solid #444;
            padding-bottom: 5px.
        }
        
        .pass-label { color: #2d882d; }
        .fail-label { color: #aa3939; }
        
        footer {
            margin-top: 50px;
            text-align: center;
            font-size: 12px;
            color: #777.
        }
        
        .monke {
            font-size: 24px;
            text-align: center;
            margin: 40px 0;
            color: #f0c674.
        }
    </style>
</head>
<body>
    <header>
        <h1>ESD.OS Test Report</h1>
        <p>$timestamp</p>
    </header>
    
    <div class="monke">🦍 MONKE TEST RESULTS 🦍</div>
    
    <div class="summary">
        <div class="stat-box total">
            <h3>Total Tests</h3>
            <div style="font-size: 32px">$total</div>
        </div>
        <div class="stat-box passed">
            <h3>Passed</h3>
            <div style="font-size: 32px">$passed</div>
        </div>
        <div class="stat-box failed">
            <h3>Failed</h3>
            <div style="font-size: 32px">$failed</div>
        </div>
    </div>
    
    <div class="progress-bar">
        <div class="progress-passed" style="width: $((passed * 100 / total))%;"></div>
        <div class="progress-failed" style="width: $((failed * 100 / total))%;"></div>
    </div>
    
    <h2>Test Results</h2>
    <table>
        <tr>
            <th>Test</th>
            <th>Status</th>
        </tr>
EOF
    
    paste <(grep -o '"name": "[^"]*"' "$report_file" | cut -d '"' -f 4) <(grep -o '"status": "[^"]*"' "$report_file" | cut -d '"' -f 4) | 
    while read -r name status; do
        if [ "$status" == "passed" ]; then
            echo "<tr class=\"pass\">" >> "$html_file"
            echo "  <td>$name</td>" >> "$html_file"
            echo "  <td><span class=\"pass-label\">✓ PASSED</span></td>" >> "$html_file"
        else
            echo "<tr class=\"fail\">" >> "$html_file"
            echo "  <td>$name</td>" >> "$html_file"
            echo "  <td><span class=\"fail-label\">✗ FAILED</span></td>" >> "$html_file"
        fi
        echo "</tr>" >> "$html_file"
    done
    
    echo "<h2>Test Details</h2>" >> "$html_file"
    
    grep -A 3 '"name":' "$report_file" | grep -v "status" | paste - - | 
    while read -r line; do
        name=$(echo "$line" | grep -o '"name": "[^"]*"' | cut -d '"' -f 4)
        output=$(echo "$line" | grep -o '"output": "[^"]*"' | cut -d '"' -f 4 | sed 's/\\n/\n/g')
        status=$(grep -A 1 "\"name\": \"$name\"" "$report_file" | grep -o '"status": "[^"]*"' | cut -d '"' -f 4)
        
        if [ "$status" == "passed" ]; then
            echo "<h3><span class=\"pass-label\">✓</span> $name</h3>" >> "$html_file"
        else
            echo "<h3><span class=\"fail-label\">✗</span> $name</h3>" >> "$html_file"
        fi
        
        echo "<div class=\"test-output\">$output</div>" >> "$html_file"
    done
    
    cat >> "$html_file" << EOF
    <footer>
        <p>Generated by ESD.OS Test Manager</p>
        <p>Remember: In ESD.OS, only handler is BIG RED BUTTON (RESET)!</p>
    </footer>
</body>
</html>
EOF
    
    echo "HTML report generated: $html_file"
    echo "Open in browser with: xdg-open $html_file"
}

case "$1" in
    compile)
        compile_tests
        ;;
    run)
        shift
        case "$1" in
            all)
                test_files=($(find_test_executables))
                if [ ${#test_files[@]} -eq 0 ]; then
                    echo "No test executables found. Try compiling first with: ./manager.sh compile"
                    exit 1
                fi
                run_all_tests "${test_files[@]}"
                ;;
            "")
                echo "Please specify which tests to run:"
                echo "  ./manager.sh run all        - Run all tests"
                echo "  ./manager.sh run <path>     - Run a specific test"
                ;;
            *)
                if [ -f "$1" ] && [ -x "$1" ]; then
                    run_all_tests "$1"
                elif [ -d "$1" ]; then
                    test_files=($(find "$1" -name "unit_tests" -type f -executable))
                    if [ ${#test_files[@]} -eq 0 ]; then
                        echo "No test executables found in $1"
                        exit 1
                    fi
                    run_all_tests "${test_files[@]}"
                else
                    echo "Error: $1 is not a valid test file or directory"
                    exit 1
                fi
                ;;
        esac
        ;;
    report)
        shift
        show_report "$@"
        ;;
    visualize)
        shift
        visualize_report "$@"
        ;;
    html-report)
        generate_html_report
        ;;
    help|"")
        show_help
        ;;
    *)
        echo "Unknown command: $1"
        echo "Run './manager.sh help' for usage information."
        exit 1
        ;;
esac

exit 0
