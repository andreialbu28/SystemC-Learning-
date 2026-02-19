#!/bin/bash
# Script to activate the Python virtual environment for this project

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$(dirname "${BASH_SOURCE[0]}")" && pwd )"
PROJECT_ROOT="$( realpath "$SCRIPT_DIR/../.." )"

VENV_DIR="$PROJECT_ROOT/venv"

# Check if venv exists
if [ ! -f "$VENV_DIR/bin/activate" ]; then
    echo "[ERROR] Virtual environment not found at $VENV_DIR"
    echo "Please run 'scripts/menu/01_setup_venv.sh' first to create it."
    return 1 2>/dev/null || exit 1
fi

# Activate the virtual environment
source "$VENV_DIR/bin/activate"

echo ""
echo "[SUCCESS] Virtual environment activated!"
echo "Python: $(which python)"
echo "Pip: $(which pip)"
echo ""
echo "To deactivate, run: deactivate"
echo ""