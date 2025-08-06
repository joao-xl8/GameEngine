#!/bin/bash

# Generate compile_commands.json for LSP support
echo "Generating compile_commands.json..."

# Clean and build with Bear to capture compile commands
bear -- make clean
bear -- make

echo "✅ compile_commands.json generated!"
echo "Your LSP should now work properly with:"
echo "  - Code completion"
echo "  - Go to definition"
echo "  - Error highlighting"
echo "  - Telescope live grep"
