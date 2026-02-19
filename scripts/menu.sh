#!/bin/bash

set -e

# Change to project root directory (parent of scripts folder)
cd "$(dirname "$0")/.."

menu() {
    # Don't clear screen automatically - let user see previous output
    echo
    echo "====================================================================="
    echo "               SystemC Learning - Menu                               "
    echo "====================================================================="
    echo
    echo "1. Setup Virtual Environment and Install Dependencies"
    echo "2. Install SystemC"
    echo "3. Install SystemC-AMS"
    echo "4. "
    echo "5. "
    echo "6. Exit"
    echo
    read -p "Select an option (1-6): " choice

    case $choice in
        1)
            source "scripts/menu/01_setup_venv.sh"
            ;;
        2)
            bash "scripts/menu/02_activate_venv.sh"
            ;;
        3)
            bash "scripts/menu/03_generate_fm.sh"
            ;;
        4)
            bash "scripts/menu/04_build_fmu.sh"
            ;;
        5)
            bash "scripts/linux_menu/05_simulate_fmu.sh"
            ;;
        6)
            exit_script
            ;;
        *)
            echo "Invalid option. Please try again."
            sleep 2
            menu
            ;;
    esac
    
    # After executing an option, show menu again (unless exited)
    menu
}

exit_script() {
    echo "Exiting the management script. Goodbye!"
    exit 0
}

# Start the menu
menu