#!/bin/bash

# Define the project directory and output directories
PROJECT_DIR=$(pwd)
OUTPUT_DIR="$PROJECT_DIR/docs"
DOXYFILE="$PROJECT_DIR/Doxyfile"

# Function to install necessary packages if not installed
install_package_if_needed() {
    local pkg_name="$1"
    if ! dpkg -s "$pkg_name" >/dev/null 2>&1; then
        echo "$pkg_name is not installed. Installing..."
        sudo apt-get update -y && sudo apt-get install -y "$pkg_name"
    else
        echo "$pkg_name is already installed."
    fi
}

# Function to generate the Doxyfile if it doesn't exist
generate_doxyfile() {
    if [ ! -f "$DOXYFILE" ]; then
        echo "Generating Doxygen configuration file..."
        doxygen -g "$DOXYFILE"
    else
        echo "Doxygen configuration file already exists."
    fi
}

# Function to configure the Doxyfile
configure_doxyfile() {
    echo "Configuring Doxygen..."

    sed -i -e "s|^PROJECT_NAME.*|PROJECT_NAME = \"My C++ Project\"|" \
           -e "s|^OUTPUT_DIRECTORY.*|OUTPUT_DIRECTORY = \"$OUTPUT_DIR\"|" \
           -e "s|^INPUT.*|INPUT = \"$PROJECT_DIR/includes\"|" \
           -e "s|^RECURSIVE.*|RECURSIVE = YES|" \
           -e "s|^EXTRACT_ALL.*|EXTRACT_ALL = YES|" \
           -e "s|^HAVE_DOT.*|HAVE_DOT = YES|" \
           -e "s|^CALL_GRAPH.*|CALL_GRAPH = YES|" \
           -e "s|^CALLER_GRAPH.*|CALLER_GRAPH = YES|" \
           -e "s|^UML_LOOK.*|UML_LOOK = YES|" \
           -e "s|^CLASS_DIAGRAMS.*|CLASS_DIAGRAMS = YES|" \
           -e "s|^INCLUDE_GRAPH.*|INCLUDE_GRAPH = YES|" \
           -e "s|^INCLUDED_BY_GRAPH.*|INCLUDED_BY_GRAPH = YES|" \
           -e "s|^DOT_IMAGE_FORMAT.*|DOT_IMAGE_FORMAT = svg|" \
           -e "s|^GENERATE_LATEX.*|GENERATE_LATEX = NO|" \
           -e "s|^GENERATE_XML.*|GENERATE_XML = YES|" \
           -e "s|^HAVE_DOT.*|HAVE_DOT = YES|" \
           -e "s|^DOT_CLEANUP.*|DOT_CLEANUP = YES|" \
           "$DOXYFILE"
}

# Function to run Doxygen to generate the documentation
run_doxygen() {
    echo "Running Doxygen..."
    doxygen "$DOXYFILE"
}

# Function to check and install dependencies
check_and_install_dependencies() {
    install_package_if_needed "doxygen"
    install_package_if_needed "graphviz"
}

# Main function to coordinate the steps
main() {
    echo "Starting documentation generation process..."

    # Check and install necessary tools
    check_and_install_dependencies

    # Generate and configure the Doxyfile
    generate_doxyfile
    configure_doxyfile

    # Create the output directory if it doesn't exist
    mkdir -p "$OUTPUT_DIR"

    # Run Doxygen to generate the documentation
    run_doxygen

    echo "Documentation generation completed. Check the $OUTPUT_DIR/html directory for the output."
}

# Execute the main function
main
