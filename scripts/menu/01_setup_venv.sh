# !/usr/bin/env bash

# This script sets up a Python virtual environment and installs the required dependencies for the project 
set -e 

SCRIPT_DIR="$( cd "$(dirname "${BASH_SOURCE[0]}")" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")/.."

VENV_DIR="$PROJECT_ROOT/venv"
INSTALL_PACKAGES=0 # Flag to track if we need to install packages

echo "--------------------------------------------------------"
echo "[01_setup_venv] Setting up Python virtual environment..."
echo "--------------------------------------------------------"
echo ""

# -----------------------------------------------------------------------------
# 1. Check Python installation
# -----------------------------------------------------------------------------
echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
echo "1. Checking if Python is installed ..."
echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

if ! command -v python3 &> /dev/null; then
    echo "[ERROR] Python3 is not installed or not in PATH!"
    return 1 2>/dev/null || exit 1
else
    echo "[INFO] Python3 is installed: $(python3 --version)"
fi
echo ""

# -----------------------------------------------------------------------------
# 2. Check/Create virtual environment
# -----------------------------------------------------------------------------
echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
echo "2. Setting up the virtual environment ..."
echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

if [ -f "$VENV_DIR/bin/activate" ]; then
    echo "[INFO] Virtual environment already exists at $VENV_DIR"
    read -p "Do you want to recreate it? (Y/N): " recreate_choice
    if [[ "$recreate_choice" =~ ^[Yy]$ ]]; then
        echo "[INFO] Removing existing virtual environment ..."
        rm -rf "$VENV_DIR"
        INSTALL_PACKAGES=1
    else
        echo "[INFO] Keeping existing virtual environment"
    fi
else
    echo "[INFO] Virtual environment does not exist. Creating a new one..."
    INSTALL_PACKAGES=1
fi

if [ "$INSTALL_PACKAGES" -eq 1 ]; then
    echo "[INFO] Creating virtual environment..."
    python3 -m venv "$VENV_DIR"
    echo "[INFO] Virtual environment created at $VENV_DIR"
fi
echo ""

# -----------------------------------------------------------------------------
# 3. Activate virtual environment in the current shell
# -----------------------------------------------------------------------------
echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
echo "3. Activating virtual environment ..."
echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

# shellcheck disable=SC1090
source "$VENV_DIR/bin/activate"

if [ -z "$VIRTUAL_ENV" ]; then
    echo "[ERROR] Failed to activate virtual environment!"
    return 1 2>/dev/null || exit 1
else
    echo "[INFO] Virtual environment activated: $VIRTUAL_ENV"
fi
echo ""

# -----------------------------------------------------------------------------
# 4. Upgrade pip and install packages
# -----------------------------------------------------------------------------
if [ "$INSTALL_PACKAGES" -eq 1 ]; then
    echo "[INFO] Upgrading pip..."
    python3 -m pip install --upgrade pip

    echo "[INFO] Installing clang-format==18.1.0 ..."
    pip install clang-format==18.1.0

    REQ_FILE="$PROJECT_ROOT/config/requirements.txt"
    if [ -f "$REQ_FILE" ]; then
        echo "[INFO] Installing packages from $REQ_FILE ..."
        pip install --upgrade --upgrade-strategy only-if-needed -r "$REQ_FILE"
    else
        echo "[WARNING] requirements.txt not found at $REQ_FILE"
    fi
else
    echo "[INFO] Skipping package installation (venv already exists)"
fi

echo ""
echo "[SUCCESS] Environment setup complete. Your shell is now inside the virtual environment."
echo "To deactivate, run: deactivate"